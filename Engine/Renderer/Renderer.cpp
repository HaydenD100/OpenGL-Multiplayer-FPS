#include "Renderer.h"
#include "Scene/SceneManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Core/DecalInstance.h"
#include "Engine/Renderer/Bloom.h"
#include "Engine/Renderer/Particle.h"
#include "Engine/Renderer/Raycaster.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/UI/Text2D.h" 
#include "Loaders/stb_image.h"
#include "Engine/Pathfinding/Pathfinding.h"

#include "Engine/Core/Scene/World.h"

#include <random>
#include <memory>

#include "glm/gtx/norm.hpp"

SkyBox::SkyBox() = default;

SkyBox::SkyBox(std::vector<std::string> faces) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

unsigned int SkyBox::GetTextureID() {
	return textureID;
}

unsigned int SkyBox::GetSkyBoxVAO() {
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return skyboxVAO;
}


namespace Renderer
{
	//deffered rendering stuff
	GLuint quad_vertexbuffer;
	GLuint depthTexture;
	GLuint cubeVBO;
	GLuint pointVBO, pointVAO;
	GLuint lineVAO, lineVBO;

	//Water
	Texture GaussianNoise;
	Texture waterVecOut;
	Texture waterHeightMap;
	std::vector<glm::vec3> _randomdir;
	const int waveCount = 32;

	//ssao
	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;
	unsigned int noiseTexture;
	unsigned int ssaoColorBuffer;

	GLuint FinalFrameFBO = 0;
	GLuint FinalFrameTexture = 0;


	GLuint testFBO = 0;
	GLuint testTexture;
	GLuint testDepth;
	GLenum DrawBuffersTest[1] = { GL_COLOR_ATTACHMENT0 };


	GLuint bloomFBO = 0;
	GLuint bloomTexture;

	//Shaders
	Shader s_skybox;
	Shader s_geomerty;
	Shader s_ssao;
	Shader s_screen;
	Shader s_transparent;
	Shader s_shadow;
	Shader s_decal;
	Shader s_final;
	Shader s_SSR;
	Shader s_voxel;
	Shader s_voxel_display;
	Shader s_probe;
	Shader s_probeDeffered;
	Shader s_probeRender;
	Shader s_probeirradiance;
	Shader s_SolidColor;
	Shader s_fxaa;
	Shader s_water;
	Shader s_textShader;
	Shader s_downScale;
	Shader s_upScale;
	Shader s_drawPoint;
	Shader s_drawLine;
	Shader s_particle;
	Shader s_spriteSheet;


	//ComputeShaders
	ComputeShader cs_lighting;
	ComputeShader cs_GI;
	ComputeShader cs_post;
	ComputeShader cs_ssao;
	ComputeShader cs_water_vec;
	ComputeShader cs_water_height_fft_col;
	ComputeShader cs_water_height_fft_row;
	ComputeShader cs_sim_particle;
	ComputeShader cs_probeIrradiance;
	ComputeShader cs_Raycaster;

	BloomRenderer emmisiveRenderer;

	StorageBuffer SHBuffer;

	Texture3D probeTextureX;
	Texture3D probeTextureY;
	Texture3D probeTextureZ;
	Texture3D probeTextureW;

	Texture3D voxelizedScene;

	GIBuffer GIbuffer;

	GBuffer gbuffer;
	BufferSSAO ssaoBuffer;
	BufferSSR ssrBuffer;
	BufferSSR fxaaBuffer;
	BufferLighting lightingBuffer;
	BufferLighting postBuffer;
	BufferTransparent transparentBuffer;

	ProbeGrid probeGrid;

	//some objects will be withheld from rendering in the gemoetry render
	std::vector<GameObject*> g_overlay;

	//Generate all the probes on another thread while the assets are loading
	std::thread ProbeGridThread;

	//state stuff for enabling/disabling indirect lighting for showcase
	int lightingState = 0;
	int DebugState = 0;
	int WaterWireFrame = 1;

	int frameCount = 0;


	void Renderer::LoadAllShaders() {
		

		s_skybox.Load("Assets/Shaders/SkyBoxShader.vert", "Assets/Shaders/SkyBoxShader.frag");
		s_geomerty.Load("Assets/Shaders/Deffered/geomerty.vert", "Assets/Shaders/Deffered/geomerty.frag");
		s_ssao.Load("Assets/Shaders/SSAO/ssao.vert", "Assets/Shaders/SSAO/ssao.frag");
		s_screen.Load("Assets/Shaders/Texture_Render/Texture_Render.vert", "Assets/Shaders/Texture_Render/Texture_Render.frag");
		s_transparent.Load("Assets/Shaders/Transparent/transparent.vert", "Assets/Shaders/Transparent/transparent.frag");
		s_shadow.Load("Assets/Shaders/Shadow/depth.vert", "Assets/Shaders/Shadow/depth.frag", "Assets/Shaders/Shadow/depth.geom");
		s_decal.Load("Assets/Shaders/Decal/decal.vert", "Assets/Shaders/Decal/decal.frag");
		s_final.Load("Assets/Shaders/Final/final.vert", "Assets/Shaders/Final/final.frag");
		s_SSR.Load("Assets/Shaders/SSR/SSR.vert", "Assets/Shaders/SSR/SSR.frag");
		s_probeDeffered.Load("Assets/Shaders/GI/probeGeom.vert", "Assets/Shaders/GI/probeGeom.frag");
		s_probeRender.Load("Assets/Shaders/GI/renderProbes.vert", "Assets/Shaders/GI/renderProbes.frag");
		s_downScale.Load("Assets/Shaders/Bloom/bloom.vert", "Assets/Shaders/Bloom/downscale.frag");
		s_upScale.Load("Assets/Shaders/Bloom/bloom.vert", "Assets/Shaders/Bloom/upscale.frag");
		s_SolidColor.Load("Assets/Shaders/SolidColour/solidColour.vert", "Assets/Shaders/SolidColour/solidColour.frag");
		s_fxaa.Load("Assets/Shaders/fxaa/fxaa.vert", "Assets/Shaders/fxaa/fxaa.frag");
		s_water.Load("Assets/Shaders/Water/water.vert", "Assets/Shaders/Water/water.frag","Assets/Shaders/Water/water.tese","Assets/Shaders/Water/water.tesc");
		s_textShader.Load("Assets/Shaders/textShader.vert", "Assets/Shaders/textShader.frag");
		s_drawPoint.Load("Assets/Shaders/Debug/point.vert", "Assets/Shaders/Debug/point.frag");
		s_particle.Load("Assets/Shaders/Particles/particle.vert", "Assets/Shaders/Particles/particle.frag");
		s_spriteSheet.Load("Assets/Shaders/spriteSheet.vert", "Assets/Shaders/spriteSheet.frag");

		cs_probeIrradiance.Load("Assets/Shaders/GI/irradiance.comp");
		cs_Raycaster.Load("Assets/Shaders/GI/triangleIntersection.comp");
		cs_lighting.Load("Assets/Shaders/Lighting/lighting.comp");
		cs_post.Load("Assets/Shaders/PostProccess/post.comp");
		cs_ssao.Load("Assets/Shaders/SSAO/ssao.comp");
		cs_water_vec.Load("Assets/Shaders/Water/wave_vec.comp");
		cs_water_height_fft_col.Load("Assets/Shaders/Water/water_height_col.comp");
		cs_sim_particle.Load("Assets/Shaders/Particles/simParticle.comp");
		cs_GI.Load("Assets/Shaders/Lighting/GI.comp");
		//TODO :: change this so the texture bindings are defined in the GLSL shader
		
		cs_lighting.Use();
		for (int i = 0; i < 26; i++) {
			cs_lighting.SetInt("lights[" + std::to_string(i) + "].depthMap", 8 + i);
		}

		s_water.Use();
		for (int i = 0; i < 26; i++) {
			s_water.SetInt("lights[" + std::to_string(i) + "].depthMap", 8 + i);
		}


		s_skybox.Use();
		s_skybox.SetInt("skybox", 0);

		s_transparent.Use();

		for (int i = 0; i < 26; i++) {
			s_transparent.SetInt("lights[" + std::to_string(i) + "].depthMap", 8 + i);
		}
		s_geomerty.Use();
		s_geomerty.SetInt("DiffuseTextureSampler", 0);
		s_geomerty.SetInt("NormalTextureSampler", 1);
		s_geomerty.SetInt("RoughnessTextureSampler", 2);
		s_geomerty.SetInt("MetalicTextureSampler", 3);
		s_geomerty.SetInt("DefaultNormal", 4);

		s_SolidColor.Use();
		s_SolidColor.SetInt("DiffuseTextureSampler", 0);
		s_SolidColor.SetInt("NormalTextureSampler", 1);
		s_SolidColor.SetInt("RoughnessTextureSampler", 2);
		s_SolidColor.SetInt("MetalicTextureSampler", 3);
		s_SolidColor.SetInt("DefaultNormal", 4);


		cs_ssao.Use();
		for (size_t i = 0; i < ssaoKernel.size(); i++) {
			cs_ssao.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		cs_ssao.SetInt("gPosition", 0);
		cs_ssao.SetInt("gNormal", 1);
		cs_ssao.SetInt("texNoise", 2);

		s_decal.Use();
		s_decal.SetInt("decalTexture", 1);
		s_decal.SetInt("gDepth", 3);

		s_final.Use();
		s_final.SetInt("gFinal", 0);

		s_SSR.Use();
		s_SSR.SetInt("gPostion", 0);
		s_SSR.SetInt("gNormal", 1);
		s_SSR.SetInt("gAlbedo", 2); 
		s_SSR.SetInt("gFinal", 3);   
		s_SSR.SetInt("gRMA", 4);    

		cs_probeIrradiance.Use();
		for (int i = 0; i < 17; i++) {
			cs_probeIrradiance.SetInt("lights[" + std::to_string(i) + "].depthMap", 5 + i);
		}

		std::cout << "Done loading shaders \n";
	}

	
	int Renderer::init() {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);  // Cull back-facing triangles
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//SSAO
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0f;

			// scale samples s.t. they're more aligned to center of kernel
			scale = 0.1f + (scale * scale) * (0.1f - 0.1f);

			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}



		LoadAllShaders();
		ConfigureFrameBuffers();

		static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		float skyboxVertices[108] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &pointVAO);
		glGenBuffers(1, &pointVBO);

		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);


		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		
		//GaussianNoise = Texture(generateGaussianNoise(512, 512), 512, 512);
		//waterVecOut = Texture(generateGaussianNoise(512, 512), 512, 512);
		//waterHeightMap = Texture(generateGaussianNoise(512, 512), 512, 512);

		//Raycaster::Init();

		glm::vec3 spacing = glm::vec3(4,4,4);
		glm::vec3 propgationGridSize = glm::vec3(60, 30, 60);
		//glm::vec3 spacing = glm::vec3(1, 1, 1);
		//glm::vec3 propgationGridSize = glm::vec3(1, 3, 1);

		glm::vec3 gridPos = glm::vec3(propgationGridSize.x/-2, -3, propgationGridSize.z/-2);

		probeTextureX.Create(glm::ceil(propgationGridSize.x / spacing.x), glm::ceil(propgationGridSize.y / spacing.y), glm::ceil(propgationGridSize.z / spacing.z));
		probeTextureY.Create(glm::ceil(propgationGridSize.x / spacing.x), glm::ceil(propgationGridSize.y / spacing.y), glm::ceil(propgationGridSize.z / spacing.z));
		probeTextureZ.Create(glm::ceil(propgationGridSize.x / spacing.x), glm::ceil(propgationGridSize.y / spacing.y), glm::ceil(propgationGridSize.z / spacing.z));
		probeTextureW.Create(glm::ceil(propgationGridSize.x / spacing.x), glm::ceil(propgationGridSize.y / spacing.y), glm::ceil(propgationGridSize.z / spacing.z));

		probeGrid.Configure(propgationGridSize.x, propgationGridSize.y, propgationGridSize.z, spacing, gridPos);

		_randomdir = generate_random_directions(waveCount);

		return 0;
	}
	void ConfigureFrameBuffers() {
		std::cout << "Config buffers \n";
		gbuffer.Configure();
		ssaoBuffer.Configure();
		ssrBuffer.Configure(Backend::GetWidth(), Backend::GetHeight());
		lightingBuffer.Configure();
		postBuffer.Configure();
		fxaaBuffer.Configure(Backend::GetWidth(), Backend::GetHeight());
		emmisiveRenderer.Init(Backend::GetWidth(), Backend::GetHeight());
		transparentBuffer.Configure();
		GIbuffer.Configure(Backend::GetWidth(), Backend::GetHeight());

		glGenFramebuffers(1, &FinalFrameFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FinalFrameFBO);
		glGenTextures(1, &FinalFrameTexture);
		glBindTexture(GL_TEXTURE_2D, FinalFrameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Backend::GetWidth(), Backend::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FinalFrameTexture, 0);

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);


	}
	void DeleteFrameBuffers() {
		gbuffer.Destroy();
		ssaoBuffer.Destroy();
		ssrBuffer.Destroy();
		lightingBuffer.Destroy();
		postBuffer.Destroy();
		fxaaBuffer.Destroy();
		transparentBuffer.Destroy();
		//emmisiveRenderer.Destroy();
	}



	void Renderer::BeforeRender() {

		Renderer::probeGrid.Bake();
		ParticleSystem::init();
		//Raycaster::FillBuffers();
		int gridX = 10;
		int gridY = 10;
		int lenght = 15;

		//Raycaster::queueRay(glm::vec3(0, 0, 1), glm::vec3(-1.97, 10.7, -0.8),20);
		//Renderer::cs_Raycaster.Use();
		//SHBuffer.Bind(7);
		//Raycaster::Bind();
		//Raycaster::Compute();
	}


	
	void Renderer::SetLights(Shader* shader) {
		// Upload lights data to the GPU
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightDirection;

		std::vector<glm::vec3> lightColors;
		std::vector<float> LightLinears;
		std::vector<float> LightQuadratics;
		std::vector<float> LightRadius;
		std::vector<float> LightCutoff;
		std::vector<float> LightOuterCutOff;

		for (int i = 0; i < World::g_lights.size(); i++) {
	
			shader->SetVec3("lights[" + std::to_string(i) + "].position",World::g_lights[i].position);
			shader->SetVec3("lights[" + std::to_string(i) + "].color", World::g_lights[i].colour);
			shader->SetFloat("lights[" + std::to_string(i) + "].strength", World::g_lights[i].strength);
			shader->SetFloat("lights[" + std::to_string(i) + "].radius", World::g_lights[i].radius);

			glActiveTexture(GL_TEXTURE8 + i); // Activate texture unit i
			glBindTexture(GL_TEXTURE_CUBE_MAP, World::g_lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
		}
	}

	void Renderer::SetLights(ComputeShader* shader) {
		// Upload lights data to the GPU
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightDirection;

		std::vector<glm::vec3> lightColors;
		std::vector<float> LightLinears;
		std::vector<float> LightQuadratics;
		std::vector<float> LightRadius;
		std::vector<float> LightCutoff;
		std::vector<float> LightOuterCutOff;


		for (int i = 0; i < World::g_lights.size(); i++) {

			shader->SetVec3("lights[" + std::to_string(i) + "].position", World::g_lights[i].position);
			shader->SetVec3("lights[" + std::to_string(i) + "].color", World::g_lights[i].colour);
			shader->SetFloat("lights[" + std::to_string(i) + "].strength", World::g_lights[i].strength);
			shader->SetFloat("lights[" + std::to_string(i) + "].radius", World::g_lights[i].radius);

			glActiveTexture(GL_TEXTURE8 + i); // Activate texture unit i
			glBindTexture(GL_TEXTURE_CUBE_MAP, World::g_lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
		}
	}

	
	void Renderer::ClearScreen() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void Renderer::RenderAllObjects(Shader& shader) {
		g_overlay.clear();
		glm::mat4 ViewMatrix = Camera::getViewMatrix();
		for (int i = 0; i < World::g_lights.size(); i++) {
			GameObject* gameobjectRender = World::g_objects[i].get();

			if (!gameobjectRender->ShouldRender())
				continue;
			if (gameobjectRender->GetShaderType() != "Default") {
				//make guns render on top;
				g_overlay.push_back(gameobjectRender);
				continue;
			}

			//auto transforms = animatior.GetFinalBoneMatrices(gameobjectRender->GetName());
			auto transforms = gameobjectRender->GetFinalBoneMatricies();
			if (transforms[0] != glm::mat4(1)) {
				shader.SetBool("animated", true);

				for (int i = 0; i < transforms.size(); ++i) {
					std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
					shader.SetMat4(pos.c_str(), transforms[i]);
				}
			}
			else
				shader.SetBool("animated", false);


			glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
			shader.SetMat4("M", ModelMatrix);

			gameobjectRender->RenderObject(shader.GetShaderID());

		}
	}
	void Renderer::RenderText(const char* text,int x, int y, int size) {
		glUseProgram(Text2D::GetProgramID());
		Text2D::printText2D(text, x, y, size);
	}

	void Renderer::RenderScene(float dt) {
		frameCount++;
		//--------------------------------------------PROBE-------------------------------------------	
		Renderer::CheckDebugState();

		//Renderer::probeGrid.Bake();

		Renderer::probeGrid.ReLight(1);

		//ParticleSystem::Simulate(dt);
		

		//cs_water_height_fft_col.SetBool("uHorizontalPass", false);

		//glDispatchCompute(512 / 32, 512 / 32, 1); // 32 workgroups for 512 columns
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//cs_water_height_fft_row	.Use();

		//glBindImageTexture(0, waterVecOut.GetTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		//glBindImageTexture(1, waterHeightMap.GetTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		//glDispatchCompute(512 / 32, 1, 1); // 32 workgroups for 512 rows
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);
		//-------------------------------------------GBUFFER-----------------------------------------

		g_overlay.clear();

		glEnable(GL_DEPTH_TEST);
		gbuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RendererSkyBox(Camera::getViewMatrix(), Camera::getProjectionMatrix(), World::GetEnviromentLighting().sky);

		s_geomerty.Use();
		s_geomerty.SetMat4("P", Camera::getProjectionMatrix());
		s_geomerty.SetMat4("V", Camera::getViewMatrix());

		glm::mat4 ViewMatrix = Camera::getViewMatrix();
		for (int i = 0; i < World::g_objects.size(); i++) {
			GameObject* gameobjectRender = World::g_objects[i].get();

			if (!gameobjectRender->ShouldRender())
				continue;
			if (!gameobjectRender->GetModel()->GetAABB()->isOnFrustum(Camera::GetFrustum(), gameobjectRender->getTransform()) && !gameobjectRender->DontCull())
				continue;
			if (gameobjectRender->GetShaderType() == "Overlay") {
				g_overlay.push_back(gameobjectRender);
				continue;
			}

			auto transforms = gameobjectRender->GetFinalBoneMatricies();
			if (transforms[0] != glm::mat4(1)) {
				s_geomerty.SetBool("animated", true);
				for (int i = 0; i < transforms.size(); ++i) {
					std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
					s_geomerty.SetMat4(pos.c_str(), transforms[i]);
				}
			}
			else
				s_geomerty.SetBool("animated", false);


			glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
			glm::mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			s_geomerty.SetMat3("normalMatrix3", normalMatrix);
			s_geomerty.SetMat4("M", ModelMatrix);
			gameobjectRender->RenderObject(s_geomerty.GetShaderID());
		}




		//ParticleSystem::RenderParticles();

		//This can be removed later but it just renders a cube thats glowing to show where point lights are
		s_SolidColor.Use();
		s_SolidColor.SetMat4("P", Camera::getProjectionMatrix());
		s_SolidColor.SetMat4("V", Camera::getViewMatrix());
		s_SolidColor.SetBool("animated", false);
		s_SolidColor.SetBool("IsEmissive", true);
		s_SolidColor.SetFloat("Rougness", 0.5);
		s_SolidColor.SetFloat("Metalic", 0);


		for (int i = 0; i < World::g_lights.size(); i++) {
			s_SolidColor.SetVec4("color", glm::vec4(World::g_lights[i].colour,1));
			glm::mat4 positionMatrix = glm::mat4(); // create an identity matrix;
			positionMatrix = glm::translate(positionMatrix, World::g_lights[i].position); //position is a vec3
			s_SolidColor.SetMat4("M", positionMatrix);
			AssetManager::GetModel("light_cube")->RenderModel(s_SolidColor.GetShaderID());
		}

		if ((DebugState & ShowTrigger) == ShowTrigger) {
			s_SolidColor.SetBool("IsEmissive", false);

			for (int i = 0; i < World::g_triggers.size(); i++) {
				s_SolidColor.SetVec4("color", glm::vec4(1, 0, 0, 0.5));
				glm::mat4 positionMatrix = glm::mat4(); // create an identity matrix;
				positionMatrix = glm::translate(positionMatrix, World::g_triggers[i].get()->m_pos);
				positionMatrix = glm::scale(positionMatrix, World::g_triggers[i].get()->m_scale * 0.5f); //0.5 becuase the model cube is 2 units wide

				s_SolidColor.SetMat4("M", positionMatrix);
				AssetManager::GetModel("cube")->RenderModel(s_SolidColor.GetShaderID());
			}
		}

		if((DebugState & ShowProbes) == ShowProbes)
			probeGrid.ShowProbes();

		//-----------------------------------------Decal---------------------------------------
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthMask(GL_FALSE);
		s_spriteSheet.Use();
		s_spriteSheet.SetMat4("P", Camera::getProjectionMatrix());
		s_spriteSheet.SetMat4("V", Camera::getViewMatrix());
		s_spriteSheet.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		s_spriteSheet.SetFloat("u_mixFactor", 0.1);
		
		//TODO this is pretty much hard coded for gun fire sprites
		for (int i = 0; i < World::g_sprites.size(); i++) {
			s_spriteSheet.SetInt("u_rowCount", World::g_sprites[i].rowcount);
			s_spriteSheet.SetInt("u_columnCount", World::g_sprites[i].columncount);
			s_spriteSheet.SetVec4("u_position", World::g_sprites[i].position);
			s_spriteSheet.SetVec4("u_rotation", World::g_sprites[i].rotation);
			s_spriteSheet.SetVec4("u_scale", World::g_sprites[i].scale);
			s_spriteSheet.SetInt("u_frameIndex", World::g_sprites[i].frameindex);
			s_spriteSheet.SetInt("u_frameNextIndex", World::g_sprites[i].frameindex + 1);
			glm::mat4 modelMatrix = World::GetGameObject(World::g_sprites[i].name)->GetModelMatrix(); // Your transformation matrix
			glm::vec3 worldPos = glm::vec3(modelMatrix[3]); // Extract x, y, z from the 4th column
			s_spriteSheet.SetVec4("u_position", glm::vec4(worldPos + Camera::GetRotation() * 2.0f, 1.0f));

			if(World::g_sprites[i].frameindex < 2)
				s_spriteSheet.SetBool("IsEmissive", true);
			else
				s_spriteSheet.SetBool("IsEmissive", false);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexture("flash")->GetTexture());
			AssetManager::GetModel("quad")->GetMesh(0)->UploadData();
			glDrawElements(
				GL_TRIANGLES,      // mode
				(GLsizei)AssetManager::GetModel("quad")->GetMesh(0)->indices.size(),    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);
		}

		glDepthMask(GL_TRUE);


		s_decal.Use();
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gbuffer.Depth);
		s_decal.SetMat4("P", Camera::getProjectionMatrix());
		s_decal.SetMat4("V", Camera::getViewMatrix());
		s_decal.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		s_decal.SetMat4("inverseP", glm::inverse(Camera::getProjectionMatrix()));
		s_decal.SetVec2("resolution", glm::vec2(Backend::GetWidth(), Backend::GetHeight()));

		std::vector<DecalInstance>* decals = AssetManager::GetAllDecalInstances();
		for (int i = 0; i < decals->size(); i++) {
			DecalInstance& decal = (*decals)[i];

			// Skip decals with null parents or those outside the camera frustum
			//getting rid of  || !decal.GetAABB()->isOnFrustum(Camera::GetFrustum(), decal.getTransform()) for now
			if (decal.CheckParentIsNull())
				continue;
			decal.GetDecal()->AddInstace(&decal);
		}
		std::vector<Decal>* decalsToBeRendered = AssetManager::GetAllDecals();
		for (int i = 0; i < decalsToBeRendered->size(); i++) {
			Decal& decal = (*decalsToBeRendered)[i];
			glm::vec3 size = decal.GetSize();

			s_decal.SetVec3("size", size);
			decal.RenderDecal(s_decal.GetShaderID());
		}

		//-----------------------------------------Transaprent stuff---------------------------------------
		transparentBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SetLights(&s_water);

		RenderWater();

		glDisablei(GL_BLEND, 1);
		glEnablei(GL_BLEND, 0);


		s_transparent.Use();
		s_transparent.SetMat4("P", Camera::getProjectionMatrix());
		s_transparent.SetMat4("V", Camera::getViewMatrix());
		s_transparent.SetVec3("viewPos", Camera::GetPosition());

		SetLights(&s_transparent);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, World::GetEnviromentLighting().sky.GetTextureID());
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glm::vec3 cameraPosition = Camera::GetPosition(); // Camera position
		
		auto& glassObjects = World::g_glass;
		std::sort(glassObjects.begin(), glassObjects.end(),
			[&cameraPosition](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b) {
				float distA = glm::length2(a->GetPosition() - cameraPosition);
				float distB = glm::length2(b->GetPosition() - cameraPosition);
				return distA > distB;
			});

		for (int i = 0; i < glassObjects.size(); i++) {
			
			glm::mat4 ModelMatrix = glassObjects[i]->GetModelMatrix();
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelMatrix)));

			s_transparent.SetMat3("normalMatrix3", normalMatrix);
			s_transparent.SetMat4("M", ModelMatrix);
			glassObjects[i]->RenderObject(s_transparent.GetShaderID());

		}
		glDisable(GL_BLEND);
		
		//---------------------------------------------------Overlay-------------------------------------
		gbuffer.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		s_geomerty.Use();
		s_geomerty.SetMat4("P", Camera::getProjectionMatrix());
		s_geomerty.SetMat4("V", Camera::getViewMatrix());



		for (int i = 0; i < g_overlay.size(); i++) {
			glm::mat4 ModelMatrix = g_overlay[i]->GetModelMatrix();
			glm::mat4 modelViewMatrix = Camera::getViewMatrix() * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			//auto transforms = World::GetAnimator()->GetFinalBoneMatrices(overlay[i]->GetName());
			auto transforms = g_overlay[i]->GetFinalBoneMatricies();

			if (transforms[0] != glm::mat4(1)) {
				s_geomerty.SetBool("animated", true);

				for (int i = 0; i < transforms.size(); ++i) {
					std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
					s_geomerty.SetMat4(pos.c_str(), transforms[i]);
				}
			}
			else
				s_geomerty.SetBool("animated", false);

			s_geomerty.SetMat3("normalMatrix3", normalMatrix);
			s_geomerty.SetMat4("M", ModelMatrix);

			g_overlay[i]->RenderObject(s_geomerty.GetShaderID());
		}
		
		//------------------------------------------------RAYCAST DEBUG--------------------------------
		/*
		s_SolidColor.Use();
		s_SolidColor.SetMat4("P", Camera::getProjectionMatrix());
		s_SolidColor.SetMat4("V", Camera::getViewMatrix());
		s_SolidColor.SetBool("animated", false);
		s_SolidColor.SetBool("IsEmissive", false);
		s_SolidColor.SetMat4("M", glm::mat4(1));
		s_SolidColor.SetVec3("color", glm::vec3(0.5, 0, 0.5));
		Raycaster::RenderVerticies();
		*/



		//---------------------------------------------------SSAO-------------------------------------
		//ssaoBuffer.Bind();
		//glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		cs_ssao.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		cs_ssao.SetMat4("projection", Camera::getProjectionMatrix());		
		cs_ssao.SetFloat("ScreenWidth", Backend::GetWidth());
		cs_ssao.SetFloat("ScreenHeight", Backend::GetHeight());

		glBindImageTexture(7, ssaoBuffer.gSSAO, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);

		glDispatchCompute(Backend::GetWidth() / 32 + 1, Backend::GetHeight() / 32 + 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//RenderPlane();
		//---------------------------------------------------LIGHTING-------------------------------------


		cs_GI.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gTrueNormal);

		probeTextureX.Bind(5);
		probeTextureY.Bind(6);
		probeTextureZ.Bind(7);
		probeTextureW.Bind(8);


		cs_GI.SetVec3("viewPos", Camera::GetPosition());
		cs_GI.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		cs_GI.SetMat4("V", Camera::getViewMatrix());
		cs_GI.SetVec3("gridWorldPos", probeGrid.postion);
		cs_GI.SetVec3("volume", probeGrid.volume);
		cs_GI.SetVec3("spacing", probeGrid.spacing);
		cs_GI.SetInt("lightingState", lightingState);
		cs_GI.SetVec2("screen", glm::vec2(Backend::GetWidth(), Backend::GetHeight()));
		cs_GI.SetVec3("envLighting", World::GetEnviromentLighting().indirectLight);

		glBindImageTexture(4, GIbuffer.gGI, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(Backend::GetWidth() / 32 + 1, Backend::GetHeight() / 32 + 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cs_lighting.Use();
		SetLights(&cs_lighting);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gRMA);
		//glActiveTexture(GL_TEXTURE4);
		//glBindTexture(GL_TEXTURE_2D, ssaoBuffer.gSSAO);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gTrueNormal);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, GIbuffer.gGI);

		cs_lighting.SetVec3("viewPos", Camera::GetPosition());
		cs_lighting.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		cs_lighting.SetMat4("V", Camera::getViewMatrix());
		cs_lighting.SetVec3("gridWorldPos", probeGrid.postion);
		cs_lighting.SetVec3("volume", probeGrid.volume);
		cs_lighting.SetVec3("spacing", probeGrid.spacing);
		cs_lighting.SetInt("lightingState", lightingState);
		cs_lighting.SetVec2("screen", glm::vec2(Backend::GetWidth(), Backend::GetHeight()));
		cs_lighting.SetVec3("envLighting", World::GetEnviromentLighting().indirectLight);

		glBindImageTexture(7, lightingBuffer.gLighting, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(Backend::GetWidth()/32 + 1, Backend::GetHeight()/32 + 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		

		//RenderPlane();

		//-------------------------------------------------SSR-------------------------
		//I have to optimze this or somthing becuase it tanks fps
		/*
		ssrBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		s_SSR.Use();
		s_SSR.SetMat4("P", Camera::getProjectionMatrix());
		s_SSR.SetMat4("V", Camera::getViewMatrix());
		s_SSR.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		s_SSR.SetMat4("inverseP", glm::inverse(Camera::getProjectionMatrix()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gRMA);
		RenderPlane();

		*/
		//-------------------------------------------------EMISSIVE-----------------------------------
		emmisiveRenderer.RenderBloomTexture(gbuffer.gEmission, 0.005f);


		//---------------------------------------------------Post-------------------------------------

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cs_post.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterHeightMap.GetTexture());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emmisiveRenderer.BloomTexture());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, transparentBuffer.gLighting);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, transparentBuffer.gData);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, transparentBuffer.gPosition);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);


		glBindImageTexture(7, postBuffer.gLighting, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		cs_post.SetVec2("screen", glm::vec2(Backend::GetWidth(), Backend::GetHeight()));
		cs_post.SetBool("isDead", Player::IsDead());
		cs_post.SetFloat("isSwimming", Player::m_headUnder);

		probeGrid.Bind(15);
		//2.23ms with plane and 0.17 with compute
		glDispatchCompute(Backend::GetWidth() / 32 + 1, Backend::GetHeight() / 32 + 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--------------------------------------------------Final-------------------------------------

		s_fxaa.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postBuffer.gLighting); //FinalFrameTexture);
		s_fxaa.SetFloat("viewportWidth", Backend::GetWidth());
		s_fxaa.SetFloat("viewportHeight", Backend::GetHeight());
		//2.79
		RenderPlane();

		glDisable(GL_DEPTH_TEST);
	}


	void Renderer::RenderWater() {

		if(Input::KeyDown('m'))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		s_water.Use();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDisable(GL_CULL_FACE);

		s_water.SetVec3("viewpos", Camera::GetPosition());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, World::GetEnviromentLighting().sky.GetTextureID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		s_water.SetMat4("P", Camera::getProjectionMatrix());
		s_water.SetMat4("V", Camera::getViewMatrix());
		s_water.SetVec3("cameraPosition", Camera::GetPosition());

		s_water.SetFloat("time", glfwGetTime());
		s_water.SetVec3Array("randomDir", _randomdir);


		for (int i = 0; i < World::g_water.size(); i++) {
			GameObject* water = World::g_water[i].get();
			//Upload the water plane data
			glm::mat4 ModelMatrix = water->GetModelMatrix();

			s_water.SetMat4("M", water->GetModelMatrix());
			water->GetModel()->GetMesh(0)->UploadData();
			glDrawElements(
				GL_PATCHES,      // mode
				(GLsizei)water->GetModel()->GetMesh(0)->indices.size(),    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);
		}

		glEnable(GL_CULL_FACE);

		if (Input::KeyDown('m'))
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	
	void Renderer::RendererSkyBox(glm::mat4 view, glm::mat4 projection, SkyBox skybox) {
		glDepthMask(GL_FALSE);
		s_skybox.Use();
		glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));
		s_skybox.SetMat4("projection", projection);
		s_skybox.SetMat4("view", viewWithoutTranslation);
		glBindVertexArray(skybox.GetSkyBoxVAO());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetTextureID());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}

	void Renderer::SwapBuffers(GLFWwindow* window) {
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwSwapInterval(0);  // Disable V-Sync
	}

	GLuint Renderer::GetCurrentProgramID() {
		GLint currentProgramID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramID);
		return currentProgramID;
	}

	void Renderer::RenderPlane() {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
	}
	void Renderer::RenderCube() {
		glDepthMask(GL_FALSE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisableVertexAttribArray(0);
		glDepthMask(GL_TRUE);


	}
	void Renderer::CheckDebugState() {
		if (Input::KeyPressed('f')) {
			lightingState++;
			if (lightingState > 5)
				lightingState = 0;
		}
		if (Input::KeyPressed(GUITOGGLE)) {
			if((DebugState & NoGUi) == NoGUi)
				DebugState = DebugState & !NoGUi;
			else
				DebugState = DebugState | NoGUi;
		}
		if (Input::KeyPressed(PROBETOGGLE)) {
			if ((DebugState & ShowProbes) == ShowProbes)
				DebugState = ShowProbes & !ShowProbes;
			else
				DebugState = ShowProbes | ShowProbes;
		}
		if (Input::KeyPressed(SHOWDEBUGTRIGGERS)) {
			if ((DebugState & ShowTrigger) == ShowTrigger)
				DebugState = ShowTrigger & !ShowTrigger;
			else
				DebugState = ShowTrigger | ShowTrigger;
		}
			
	}


	
	void Renderer::DrawPoint(glm::vec3 position, glm::vec3 colour) {
		s_drawPoint.Use();

		glBindVertexArray(pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &position, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		s_drawPoint.SetVec3("Colour", colour);
		s_drawPoint.SetMat4("V", Camera::getViewMatrix());
		s_drawPoint.SetMat4("P", Camera::getProjectionMatrix());
		glEnable(GL_PROGRAM_POINT_SIZE);

		glDrawArrays(GL_POINTS, 0, 1);
		glDisableVertexAttribArray(0);

	}
	void Renderer::DrawLine(glm::vec3 position1, glm::vec3 position2, glm::vec3 colour) {
		float lineVertices[] = {
		position1.x, position1.y, position1.z,
		position2.x, position2.y, position2.z
		};
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		s_drawLine.Use();
		s_drawLine.SetVec3("Colour", colour);
		s_drawLine.SetMat4("V", Camera::getViewMatrix());
		s_drawLine.SetMat4("P", Camera::getProjectionMatrix());
		glBindVertexArray(lineVAO);
		glDrawArrays(GL_LINES, 0, 2); 
		glDisableVertexAttribArray(0);
	}
}
