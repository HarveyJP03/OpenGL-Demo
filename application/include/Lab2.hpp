#pragma once
#include <DemonRenderer.hpp>
class Lab2 : public Layer
{
public:
	Lab2(GLFWWindowImpl& win);
private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:
	std::shared_ptr<Scene> m_mainScene;

	Renderer m_mainRenderer;
	// Actor positions for ease of use and to avoid additonal magic numbers
	size_t modelIdx;
	size_t cameraIdx;
	size_t floorIdx;
	size_t vampireIdx;
	size_t skyBoxIdx;

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
	bool m_wireFrame = false;
};