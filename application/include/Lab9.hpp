#pragma once
#include <DemonRenderer.hpp>
class Lab9 : public Layer
{
public:
	Lab9(GLFWWindowImpl& win);
private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:

	void SetUpPPMaterial(std::string fragPath, std::shared_ptr<Material>& mat, std::shared_ptr<Texture> inputTex); //Create shader and add to material passed in, set u_inputTexture
	std::shared_ptr<Scene> m_mainScene;
	std::shared_ptr<Scene> m_screenScene; //Contains a single quad, which is used to draw the colour attachment from the main pass onto the screen
	std::shared_ptr<Scene> skyBoxOnlyScene;

	Timer m_deltaTimer;

	std::shared_ptr<Scene> m_forwardRenderScene;

	std::shared_ptr<Material> lightPassMaterial;
	std::shared_ptr<Material> tiltShiftMaterial;

	std::shared_ptr<Material> noiseMaterial;

	int m_numPointLights = 1;

	//In header, as needs to be accessed from update() to set the colour through ImGui
	float m_edgeStrength = 1.0f;
	int m_fogType = -1;

	int m_previousRenderPassIndex = -1; //To keep track of the index of the last added render pass
	int m_blurPassIndex = -1;
	int m_tintPassIndex = -1;
	int m_edgeDetectionPassIndex = -1;
	int m_fogPassIndex = -1;
	int m_dofPassIndex = -1;
	int m_sepiaPassIndex = -1;
	float m_tiltShiftPassIndex = -1;

	//To create view and projection matrices for camrea at light position
	struct ShadowMapVars
	{
		glm::vec3 centre; //Centre of scene, where light is looking (dosen't have to always be centre, can be any point)
		float distanceAlongLightVector;
		float orthoSize; //Size of the orthographic projection (size of cube that light camera can see
		const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		ShadowMapVars(glm::vec3 c = glm::vec3(0.0), float dist = 60.0f) : centre(c), distanceAlongLightVector(dist)
		{
			orthoSize = distanceAlongLightVector * 1.5f;
		};
	};

	ShadowMapVars m_shadowMapVars;
	std::shared_ptr<Material> vampireMaterial;
	std::shared_ptr<Material> floorMaterial;

	Renderer m_mainRenderer; //Runs every frame
	Renderer m_initRenderer; //Runs once at start
	Renderer m_forwardRenderer;

	// Actor positions for ease of use and to avoid additonal magic numbers
	size_t modelIdx;
	size_t cameraIdx;
	size_t floorIdx;
	size_t vampireIdx;
	size_t skyBoxIdx;
	size_t screenIdx;

	std::vector<float> skyboxVertices = {
		// positions          
		-200.f,  200.f, -200.f,
		-200.f, -200.f, -200.f,
		200.f, -200.f, -200.f,
		200.f, -200.f, -200.f,
		200.f,  200.f, -200.f,
		-200.f,  200.f, -200.f,

		-200.f, -200.f,  200.f,
		-200.f, -200.f, -200.f,
		-200.f,  200.f, -200.f,
		-200.f,  200.f, -200.f,
		-200.f,  200.f,  200.f,
		-200.f, -200.f,  200.f,

		200.f, -200.f, -200.f,
		200.f, -200.f,  200.f,
		200.f,  200.f,  200.f,
		200.f,  200.f,  200.f,
		200.f,  200.f, -200.f,
		200.f, -200.f, -200.f,

		-200.f, -200.f,  200.f,
		-200.f,  200.f,  200.f,
		200.f,  200.f,  200.f,
		200.f,  200.f,  200.f,
		200.f, -200.f,  200.f,
		-200.f, -200.f,  200.f,

		-200.f,  200.f, -200.f,
		200.f,  200.f, -200.f,
		200.f,  200.f,  200.f,
		200.f,  200.f,  200.f,
		-200.f,  200.f,  200.f,
		-200.f,  200.f, -200.f,

		-200.f, -200.f, -200.f,
		-200.f, -200.f,  200.f,
		200.f, -200.f, -200.f,
		200.f, -200.f, -200.f,
		-200.f, -200.f,  200.f,
		200.f, -200.f,  200.f
	};
	//Gui
	glm::vec3 m_vampireColour{ 1.0 };
	glm::vec3 m_tintColour{ 0.0 };
	float m_blurRadius = 1.0f;
	bool m_wireFrame = false;
	float m_focusDistance = 0.25f;
	glm::vec3 m_lightDirection{ glm::vec3(-0.2, -1.0, -0.5) };

	float m_remapRange = 20.0f;
	bool m_geoNormal = false;
	float m_tiltIntensity = 0.125f;

	//noise
	float m_frequency = 5.0f;
	float m_amplitude = 1.5f;
	float m_lacunarity = 2.0f;
	float m_persistence = 0.5f;
	int m_noiseType = 1;
	bool m_invertWorley = false;
	bool m_animateWorley = false;

	//particles
	float m_speed = 1.0f;
	bool m_attachToCube = true;
	glm::vec3 m_particleColour = glm::vec3(1.0f, 1.0f, 0.0f);
	glm::vec3 m_emitterLocation = glm::vec3(0.0f, 10.0f, -9.0f);
	glm::vec2 m_velocityRangeX = glm::vec2(-1.0f, 1.0f);
	glm::vec2 m_velocityRangeY = glm::vec2(-1.0f, 1.0f);
	glm::vec2 m_velocityRangeZ = glm::vec2(-1.0f, 1.0f);

	bool m_sepia = false;
	int m_blurType = 0.0f;
};
