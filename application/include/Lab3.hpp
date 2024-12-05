#pragma once
#include <DemonRenderer.hpp>
class Lab3 : public Layer
{
public:
	Lab3(GLFWWindowImpl& win);
private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:

	void SetUpPPMaterial(std::string fragPath, std::shared_ptr<Material>& mat, std::shared_ptr<Texture> inputTex); //Create shader and add to material passed in, set u_inputTexture
	std::shared_ptr<Scene> m_mainScene;
	std::shared_ptr<Scene> m_screenScene; //Contains a single quad, which is used to draw the colour attachment from the main pass onto the screen

	//In header, as needs to be accessed from update() to set the colour through ImGui
	float m_edgeStrength = 1.0f;
	float m_fogType = -1.0f;

	int m_previousRenderPassIndex = -1; //To keep track of the index of the last added render pass
	int m_blurPassIndex = -1;
	int m_tintPassIndex = -1;
	int m_edgeDetectionPassIndex = -1;
	int m_fogPassIndex = -1;

	Renderer m_mainRenderer;
	// Actor positions for ease of use and to avoid additonal magic numbers
	size_t modelIdx;
	size_t cameraIdx;
	size_t floorIdx;
	size_t vampireIdx;
	size_t skyBoxIdx;
	size_t screenIdx;

	std::vector<float> skyboxVertices = {
		// positions          
		-100.f,  100.f, -100.f,
		-100.f, -100.f, -100.f,
		100.f, -100.f, -100.f,
		100.f, -100.f, -100.f,
		100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f, -100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f,  100.f,
		-100.f, -100.f,  100.f,

		100.f, -100.f, -100.f,
		100.f, -100.f,  100.f,
		100.f,  100.f,  100.f,
		100.f,  100.f,  100.f,
		100.f,  100.f, -100.f,
		100.f, -100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f,  100.f,  100.f,
		100.f,  100.f,  100.f,
		100.f,  100.f,  100.f,
		100.f, -100.f,  100.f,
		-100.f, -100.f,  100.f,

		-100.f,  100.f, -100.f,
		100.f,  100.f, -100.f,
		100.f,  100.f,  100.f,
		100.f,  100.f,  100.f,
		-100.f,  100.f,  100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		100.f, -100.f, -100.f,
		100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		100.f, -100.f,  100.f
	};
	//Gui
	glm::vec3 m_colour{ 1.0 };
	glm::vec3 m_floorColour{ glm::vec3(0.2, 0.3, 0.4f) };
	glm::vec3 m_vampireColour{ 1.0 };
	glm::vec3 m_tintColour{ 0.0 };
	float m_blurRadius = 0.0f;
	bool m_wireFrame = false;
};