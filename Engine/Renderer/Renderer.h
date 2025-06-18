#pragma once
#include "Engine/Core/Common/RenderCommon.h"



#include "Engine/Renderer/Texture.h"
#include "Engine/Animation/SkinnedAnimatior.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Gbuffer.h"
#include "Engine/Renderer/BufferLighting.h"
#include "Engine/Renderer/BufferTransparent.h"

#include "Engine/Renderer/BufferSSAO.h"
#include "Engine/Renderer/BufferSSR.h"
#include "Engine/Renderer/Texture3D.h"
#include "Engine/Renderer/Probe.h"
#include "Engine/Renderer/StorageBuffer.h"
#include "Engine/Core/Lights/Light.h"

#include <thread>
#include <mutex>





enum RenderDebugStates {
	NoGUi = 1,
	ShowProbes = 2,
	ShowTrigger = 4
};

class SkyBox
{
public:
    SkyBox();
    SkyBox(std::vector<std::string> faces);
    unsigned int GetTextureID();
    unsigned int GetSkyBoxVAO();
private:
	float skyboxVertices[108] =  {
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

    unsigned int textureID;
    unsigned int skyboxVAO, skyboxVBO;
};
struct EnviromentLighting {
	SkyBox sky = SkyBox();
	glm::vec3 indirectLight = glm::vec3(0, 0, 0);
};


namespace Renderer
{
    
	extern ProbeGrid probeGrid;

	extern StorageBuffer SHBuffer;
	extern Texture3D probeTexture;

	extern Shader s_downScale;
	extern Shader s_upScale;
	extern Shader s_SolidColor;
	extern Shader s_textShader;
	extern Shader s_drawPoint;
	extern Shader s_drawLine;
	extern Shader s_skybox;
	extern Shader s_geomerty;
	extern Shader s_ssao;
	extern Shader s_screen;
	extern Shader s_transparent;
	extern Shader s_shadow;
	extern Shader s_decal;
	extern Shader s_probe;
	extern Shader s_probeDeffered;
	extern Shader s_probeRender;
	extern Shader s_probeirradiance;
	extern Shader s_particle;

	extern ComputeShader cs_Raycaster;
	extern ComputeShader cs_probeIrradiance;
	extern ComputeShader cs_sim_particle;

	extern GLuint quad_vertexbuffer;

	extern StorageBuffer SHBuffer;

	extern int DebugState;



    int init();
    void LoadAllShaders();
	void ConfigureFrameBuffers();
	void DeleteFrameBuffers();
    void ClearScreen();
    void SwapBuffers(GLFWwindow* window);

    GLuint GetCurrentProgramID();

    // Text Rendering
    void RenderText(const char* text, int x, int y, int size);
    void RendererSkyBox(glm::mat4 view, glm::mat4 projection, SkyBox skybox);
    void RenderAllObjects(Shader& shader);
	void RenderWater();
	//anything that needs to be ran before the gameLoop;
	void BeforeRender();


    // Shader
    void SetLights(Shader* shader);
	void SetLights(ComputeShader* shader);
	void SetLightsAllShaders(std::vector<Light> lights);
    void RenderScene(float dt);

	void DrawPoint(glm::vec3 position, glm::vec3 colour);
	void DrawLine(glm::vec3 position1, glm::vec3 position2, glm::vec3 colour);

	void RenderCube();
    void RenderPlane();
	void CheckDebugState();

}

