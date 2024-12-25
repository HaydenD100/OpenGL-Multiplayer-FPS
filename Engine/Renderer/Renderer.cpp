#include "Renderer.h"
#include "Scene/SceneManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Core/DecalInstance.h"

#include <random>





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
	Shader s_lighting;
	Shader s_skybox;
	Shader s_geomerty;
	Shader s_ssao;
	Shader s_screen;
	Shader s_transparent;
	Shader s_shadow;
	Shader s_decal;
	Shader s_final;
	Shader s_SSR;
	Shader s_Post;
	Shader s_voxel;
	Shader s_voxel_display;
	Shader s_probe;
	Shader s_probeRender;
	Shader s_probeirradiance;

	Shader s_downScale;
	Shader s_upScale;


	StorageBuffer SHBuffer;
	Texture3D probeTexture;



	GBuffer gbuffer;


	BufferLighting lightingBuffer;
	BufferSSAO ssaoBuffer;
	BufferSSR ssrBuffer;

	ProbeGrid probeGrid;


	

	//some objects will be withheld from rendering in the gemoetry render
	std::vector<GameObject*> NeedRendering;



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
		s_Post.Load("Assets/Shaders/PostProccess/post.vert", "Assets/Shaders/PostProccess/post.frag");
		s_probe.Load("Assets/Shaders/GI/probe.vert", "Assets/Shaders/GI/probe.frag");
		s_probeRender.Load("Assets/Shaders/GI/renderProbes.vert", "Assets/Shaders/GI/renderProbes.frag");
		s_probeirradiance.Load("Assets/Shaders/GI/irradiance.vert", "Assets/Shaders/GI/irradiance.frag");
		s_lighting.Load("Assets/Shaders/Lighting/lighting.vert", "Assets/Shaders/Lighting/lighting.frag");
		s_downScale.Load("Assets/Shaders/Bloom/bloom.vert", "Assets/Shaders/Bloom/downscale.frag");
		s_upScale.Load("Assets/Shaders/Bloom/bloom.vert", "Assets/Shaders/Bloom/upscale.frag");





		
		s_lighting.Use();


		for (int i = 0; i < 26; i++) {
			s_lighting.SetInt("depthMap[" + std::to_string(i) + "]", 8 + i);
		}



		s_skybox.Use();
		s_skybox.SetInt("skybox", 0);

		s_transparent.Use();
		s_transparent.SetInt("DiffuseTextureSampler", 0);
		s_transparent.SetInt("NormalTextureSampler", 1);
		s_transparent.SetInt("RoughnessTextureSampler", 2);
		s_transparent.SetInt("MetalicTextureSampler", 3);
		for (int i = 0; i < 26; i++) {
			s_transparent.SetInt("depthMap[" + std::to_string(i) + "]", 5 + i);
		}

		s_geomerty.Use();
		s_geomerty.SetInt("DiffuseTextureSampler", 0);
		s_geomerty.SetInt("NormalTextureSampler", 1);
		s_geomerty.SetInt("RoughnessTextureSampler", 2);
		s_geomerty.SetInt("MetalicTextureSampler", 3);
		s_geomerty.SetInt("DefaultNormal", 4);


		s_ssao.Use();
		for (size_t i = 0; i < ssaoKernel.size(); i++) {
			s_ssao.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		s_ssao.SetFloat("ScreenWidth", Backend::GetWidth());
		s_ssao.SetFloat("ScreenHeight", Backend::GetHeight());
		s_ssao.SetInt("gPosition", 0);
		s_ssao.SetInt("gNormal", 1);
		s_ssao.SetInt("texNoise", 2);

		s_decal.Use();
		s_decal.SetInt("decalTexture", 1);
		s_decal.SetInt("gDepth", 3);
		s_decal.SetVec2("resolution", glm::vec2(Backend::GetWidth(), Backend::GetHeight()));

		s_Post.Use();
		s_Post.SetInt("gLighting", 0);
		s_Post.SetInt("gSSR", 1);

		s_final.Use();
		s_final.SetInt("gFinal", 0);

		s_SSR.Use();
		s_SSR.SetInt("gPostion", 0);
		s_SSR.SetInt("gNormal", 1);
		s_SSR.SetInt("gAlbedo", 2); 
		s_SSR.SetInt("gFinal", 3);   
		s_SSR.SetInt("gRMA", 4);    

		s_probe.Use();
		for (int i = 0; i < 26; i++) {
			s_probe.SetInt("depthMap[" + std::to_string(i) + "]", 4 + i);
		}
		s_probe.SetInt("diffuse", 0);
		s_probe.SetInt("normal", 1);
		s_probe.SetInt("roughness", 2);
		s_probe.SetInt("metalic", 3);


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


		gbuffer.Configure();
		ssaoBuffer.Configure();
		ssrBuffer.Configure(Backend::GetWidth(), Backend::GetHeight());
		lightingBuffer.Configure();

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


		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



		//TODO :: I hate this i wish i could just get it work in the gbuffer but ive spent to long trying to fix it 
		glGenFramebuffers(1, &FinalFrameFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FinalFrameFBO);
		glGenTextures(1, &FinalFrameTexture);
		glBindTexture(GL_TEXTURE_2D, FinalFrameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Backend::GetWidth(), Backend::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FinalFrameTexture, 0);

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);


		//this is the projection to voxlize the scene from orgin (0,0,0) while the other one is the view of the camera
		//voxel_orth = glm::ortho(-((float)voxelize_scene_albedo.GetWidth() / 2.0f), (float)voxelize_scene_albedo.GetWidth() / 2.0f, (float)voxelize_scene_albedo.GetWidth() / 2.0f, -((float)voxelize_scene_albedo.GetWidth() / 2.0f), -(float)voxelize_scene_albedo.GetWidth()/2.0f, (float)voxelize_scene_albedo.GetWidth()/2.0f);
		float spacing = 1;
		probeTexture.Create(glm::ceil(25 * 1/ spacing), glm::ceil(5 * 1 / spacing), glm::ceil(30 * 1 / spacing));
		probeGrid.Configure(25,5,30, spacing, glm::vec3(-6,1,-14));
		SHBuffer.Configure(probeGrid.probes.size() * sizeof(glm::mat3) * 3 + probeGrid.probes.size() * 256 * sizeof(float));

		return 0;
	}

	
	void Renderer::SetLights(std::vector<Light> lights) {
		// Upload lights data to the GPU
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightDirection;

		std::vector<glm::vec3> lightColors;
		std::vector<float> LightLinears;
		std::vector<float> LightQuadratics;
		std::vector<float> LightRadius;
		std::vector<float> LightCutoff;
		std::vector<float> LightOuterCutOff;


		for (const auto& light : lights) {
			lightPositions.push_back(light.position);
			lightDirection.push_back(light.direction);
			lightColors.push_back(light.colour);
			LightLinears.push_back(light.linear);
			LightQuadratics.push_back(light.quadratic);
			LightRadius.push_back(light.radius);
			LightCutoff.push_back(light.cutoff);
			LightOuterCutOff.push_back(light.outercutoff);
		}


		s_lighting.SetVec3Array("LightPositions_worldspace", lightPositions);
		s_lighting.SetVec3Array("Lightdirection", lightDirection);
		s_lighting.SetVec3Array("LightColors", lightColors);
		s_lighting.SetFloatArray("LightLinears", LightLinears);
		s_lighting.SetFloatArray("LightQuadratics", LightQuadratics);
		s_lighting.SetFloatArray("LightRadius", LightRadius);
		s_lighting.SetFloatArray("LightCutOff", LightCutoff);
		s_lighting.SetFloatArray("LightOuterCutOff", LightOuterCutOff);

		// Upload depth maps (cubemap for shadow mapping)
		for (int i = 0; i < lights.size(); i++) {
			glActiveTexture(GL_TEXTURE8 + i); // Activate texture unit i
			glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
		}


	}
	
	void Renderer::ClearScreen() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void Renderer::RenderAllObjects(Shader& shader) {
		NeedRendering.clear();
		glm::mat4 ViewMatrix = Camera::getViewMatrix();
		for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
			GameObject* gameobjectRender = AssetManager::GetGameObject(i);

			if (!gameobjectRender->ShouldRender())
				continue;
			if (gameobjectRender->GetShaderType() != "Default") {
				//make guns render on top;
				NeedRendering.push_back(gameobjectRender);
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

	void Renderer::RenderScene() {



		//--------------------------------------------PROBE-------------------------------------------	
		//Renderer::probeGrid.Bake(SceneManager::GetCurrentScene()->getLights());

		//-------------------------------------------GBUFFER-----------------------------------------

		glEnable(GL_DEPTH_TEST);
		gbuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RendererSkyBox(Camera::getViewMatrix(), Camera::getProjectionMatrix(), SceneManager::GetCurrentScene()->GetSkyBox());

		s_geomerty.Use();
		s_geomerty.SetMat4("P", Camera::getProjectionMatrix());
		s_geomerty.SetMat4("V", Camera::getViewMatrix());

		glBindTextureUnit(4, AssetManager::GetTexture("normal")->GetTextureNormal());


		NeedRendering.clear();
		glm::mat4 ViewMatrix = Camera::getViewMatrix();
		for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
			GameObject* gameobjectRender = AssetManager::GetGameObject(i);

			if (!gameobjectRender->ShouldRender())
				continue;
			if (gameobjectRender->GetShaderType() != "Default") {
				//make guns render on top;
				NeedRendering.push_back(gameobjectRender);
				continue;
			}
			if (!gameobjectRender->GetModel()->GetAABB()->isOnFrustum(Camera::GetFrustum(), gameobjectRender->getTransform()) && !gameobjectRender->DontCull())
				continue;

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



		if(Input::KeyDown('g'))
			probeGrid.ShowProbes();

		//-----------------------------------------Decal---------------------------------------
		glEnable(GL_BLEND);
		s_decal.Use();
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gbuffer.Depth);
		s_decal.SetMat4("P", Camera::getProjectionMatrix());
		s_decal.SetMat4("V", Camera::getViewMatrix());
		s_decal.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		s_decal.SetMat4("inverseP", glm::inverse(Camera::getProjectionMatrix()));

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
		s_transparent.Use();

		//SetLights(SceneManager::GetCurrentScene()->getLights());


		s_transparent.SetMat4("P", Camera::getProjectionMatrix());
		s_transparent.SetMat4("V", Camera::getViewMatrix());
		s_transparent.SetVec3("viewPos", Camera::GetPosition());
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glm::vec3 cameraPosition = Camera::GetPosition(); // Camera position

		std::sort(NeedRendering.begin(), NeedRendering.end(),
			[&cameraPosition](const GameObject* a, const GameObject* b) {
				float distanceA = glm::length(a->GetPosition() - cameraPosition);
				float distanceB = glm::length(b->GetPosition() - cameraPosition);

				return distanceA > distanceB; // Sort by descending distance (farthest first)
			});

		std::vector<GameObject*> overlay; 
		for (int i = 0; i < NeedRendering.size(); i++) {
			if (NeedRendering[i]->GetShaderType() == "Overlay") {
				overlay.push_back(NeedRendering[i]);
				continue;
			}
			auto transforms = NeedRendering[i]->GetFinalBoneMatricies();
			if (transforms[0] != glm::mat4(1)) {
				s_transparent.SetBool("animated", true);
				for (int i = 0; i < transforms.size(); ++i) {
					std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
					s_transparent.SetMat4(pos.c_str(), transforms[i]);
				}
			}
			else {
				s_transparent.SetBool("animated", false);
			}
			glm::mat4 ModelMatrix = NeedRendering[i]->GetModelMatrix();
			glm::mat4 modelViewMatrix = Camera::getViewMatrix() * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			s_transparent.SetMat3("normalMatrix3", normalMatrix);
			s_transparent.SetMat4("M", ModelMatrix);
			NeedRendering[i]->RenderObject(s_transparent.GetShaderID());
		}


		glDisable(GL_BLEND);



		//---------------------------------------------------Overlay-------------------------------------
		s_geomerty.Use();

		glClear(GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < overlay.size(); i++) {
			glm::mat4 ModelMatrix = overlay[i]->GetModelMatrix();
			glm::mat4 modelViewMatrix = Camera::getViewMatrix() * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			//auto transforms = SceneManager::GetCurrentScene()->GetAnimator()->GetFinalBoneMatrices(overlay[i]->GetName());
			auto transforms = overlay[i]->GetFinalBoneMatricies();

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

			overlay[i]->RenderObject(s_geomerty.GetShaderID());
		}


		//---------------------------------------------------SSAO-------------------------------------
		ssaoBuffer.Bind();
		glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_ssao.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		s_ssao.SetMat4("projection", Camera::getProjectionMatrix());		




		RenderPlane();
		//---------------------------------------------------Lighting-------------------------------------

		lightingBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_lighting.Use();
		SetLights(SceneManager::GetCurrentScene()->getLights());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gRMA);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ssaoBuffer.gSSAO);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gTrueNormal);

		SHBuffer.Bind(7);
		probeTexture.Bind(6);

		
		s_lighting.SetVec3("viewPos", Camera::GetPosition());
		s_lighting.SetMat4("inverseV", glm::inverse(Camera::getViewMatrix()));
		s_lighting.SetMat4("V", Camera::getViewMatrix());
		s_lighting.SetBool("isDead", Player::IsDead());
		s_lighting.SetVec3("gridWorldPos", probeGrid.postion);
		s_lighting.SetVec3("volume", probeGrid.volume);
		s_lighting.SetFloat("spacing", probeGrid.spacing);


		RenderPlane();

		//-------------------------------------------------SSR-------------------------
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




		//---------------------------------------------------Post-------------------------------------
		

		glBindFramebuffer(GL_FRAMEBUFFER, FinalFrameFBO);
		glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		s_Post.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightingBuffer.gLighting);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ssrBuffer.gSSR);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.gEmission);


		RenderPlane();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--------------------------------------------------Final-------------------------------------

		s_final.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FinalFrameTexture); //FinalFrameTexture);

		RenderPlane();

		glDisable(GL_DEPTH_TEST);

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
	}

	GLuint Renderer::GetCurrentProgramID() {
		GLint currentProgramID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramID);
		return currentProgramID;
	}

	void RenderPlane() {
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
}
