#include "Lab1.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include "Grid.hpp"
#include "Lab1.hpp"


Lab1::Lab1(GLFWWindowImpl& win) : Layer(win)
{

	m_mainScene.reset(new Scene); //Scene holds everything in the scene; actors + lights
	ShaderDescription phongShaderDesc; //Path to source files and shader type, used to load the shader.
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/PhongFrag.glsl";

	std::shared_ptr<Shader> phongShader;
	phongShader = std::make_shared<Shader>(phongShaderDesc);
	VBOLayout cubeLayout = 
	{
		{GL_FLOAT, 3},  // position,3 for 3 values for attribute (vec3)
		{GL_FLOAT, 3},  // normal
		{GL_FLOAT, 2}   // UV, 2 for 2 values (vec2)
	};

	Model cubeModel("./assets/models/whiteCube/letterCube.obj"); //model has vertices, position and indices
	std::shared_ptr<VAO> cubeVAO;
	cubeVAO = std::make_shared<VAO>(cubeModel.m_meshes[0].indices);  //initialise with IBO
	cubeVAO->addVertexBuffer(cubeModel.m_meshes[0].vertices, cubeLayout); //add VBO after

	std::shared_ptr<Texture> cubeTexture;
	cubeTexture = std::make_shared<Texture>("./assets/models/whiteCube/letterCube.png");

	std::shared_ptr<Material> cubeMaterial;
	cubeMaterial = std::make_shared<Material>(phongShader);
	cubeMaterial->setValue("u_albedoMap", cubeTexture);
	cubeMaterial->setValue("u_albedo", m_colour);

	Actor cube;
	cube.geometry = cubeVAO;
	cube.material = cubeMaterial;
	cube.translation = glm::vec3(3.f, 1.f, -6.f);
	cube.recalc();
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(cube);

	//** Creating Floor
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	ShaderDescription floorShaderDesc; //Path to source files and shader type, used to load the shader.
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/FloorVert.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/FloorFrag.glsl";

	std::shared_ptr<Shader> floorShader;
	floorShader = std::make_shared<Shader>(floorShaderDesc);
	
	VBOLayout gridLayout =
	{
		{GL_FLOAT, 3}, //pos
		{GL_FLOAT, 2}  //uv      no normal since we just hard code that in FloorFrag.glsl
	};

	std::shared_ptr<VAO> gridVAO;
	gridVAO = std::make_shared<VAO>(floor_indices);
	gridVAO->addVertexBuffer(floor_vertices, gridLayout);

	std::shared_ptr<Material> floorMaterial;
	floorMaterial = std::make_shared<Material>(floorShader);
	floorMaterial->setValue("u_albedoMap", cubeTexture);
	floorMaterial->setValue("u_albedo", m_floorColour);

	Actor floor;
	floor.geometry = gridVAO;
	floor.material = floorMaterial;
	floor.translation = glm::vec3(-50.0f, -15.f, -50.0f);
	floor.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);
	//** Floor Created

	//**Creating Vampire
	ShaderDescription vampireShaderDesc; //Path to source files and shader type, used to load the shader.
	vampireShaderDesc.type = ShaderType::rasterization;
	vampireShaderDesc.vertexSrcPath = "./assets/shaders/Lab1/VampireVert.glsl";
	vampireShaderDesc.fragmentSrcPath = "./assets/shaders/Lab1/VampireFrag.glsl";
	std::shared_ptr<Shader> vampireShader;
	vampireShader = std::make_shared<Shader>(vampireShaderDesc);
	Model vampireModel("./assets/models/Vampire/vampire.obj");
	
	std::shared_ptr<VAO> vampireVAO;
	vampireVAO = std::make_shared<VAO>(vampireModel.m_meshes[0].indices); //Init with IBO
	VBOLayout vampireLayout =
	{
		{GL_FLOAT, 3}, //pos
		{GL_FLOAT, 3}, //normal
		{GL_FLOAT, 2}  //UV
	};
	vampireVAO->addVertexBuffer(vampireModel.m_meshes[0].vertices, vampireLayout); //Add VBO

	std::shared_ptr<Texture> vampireSpecular;
	vampireSpecular = std::make_shared<Texture>("./assets/models/Vampire/textures/specular.png");

	std::shared_ptr<Texture> vampireAlbedo;
	vampireAlbedo = std::make_shared<Texture>("./assets/models/Vampire/textures/diffuse.png");

	std::shared_ptr<Material> vampireMaterial;
	vampireMaterial = std::make_shared<Material>(vampireShader);
	vampireMaterial->setValue("u_albedo", m_colour);
	vampireMaterial->setValue("u_specularMap", vampireSpecular);
	vampireMaterial->setValue("u_albedoMap", vampireAlbedo);

	Actor vampire;
	vampire.geometry = vampireVAO;
	vampire.material = vampireMaterial;
	vampire.translation = glm::vec3(0.0f, -5.0f, -11.0f);
	vampire.scale = glm::vec3(5.0f, 5.0f, 5.0f);
	vampire.recalc();
	vampireIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(vampire);
	//**Vampire Created

	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(-0.2, -1.0, -0.5));
	m_mainScene->m_directionalLights.push_back(dl);

	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(camera);


	/*************************
	*  Main Render PAss
	**************************/
	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(); ;
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);


	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(modelIdx), glm::vec3(0.3f, 0.6f, 0.9f), GLFW_KEY_1);


	m_mainRenderer.addRenderPass(mainPass);

}

void Lab1::onRender() const
{
	m_mainRenderer.render();
}
void Lab1::onUpdate(float timestep)
{
	// per frame uniforms
	auto cubeMat = m_mainScene->m_actors.at(modelIdx).material;
	cubeMat->setValue("u_albedo", m_colour);

	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_albedo", m_floorColour);

	auto& directionalLight = m_mainScene->m_directionalLights.at(0);
	directionalLight.direction = glm::normalize(glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()), 0.0));

	m_mainRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	m_mainRenderer.getRenderPass(0).drawInWireFrame = m_wireFrame;

	// Update scripts
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onUpdate(timestep);
	}

	// Update camera  and its position in UBO
	auto& camera = m_mainScene->m_actors.at(cameraIdx);
	auto& pass = m_mainRenderer.getRenderPass(0);
	pass.camera.updateView(camera.transform);
	pass.UBOmanager.setCachedValue("b_camera", "u_view", pass.camera.view);
	pass.UBOmanager.setCachedValue("b_camera", "u_viewPos", camera.translation);


}


void Lab1::onImGUIRender()
{
	float ms = 1000.0f / ImGui::GetIO().Framerate; ;  //milisecconds
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("GAMR3521");
	ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);  // display FPS and ms
	ImGui::ColorEdit3("Cube Colour", (float*)&m_colour);
	ImGui::ColorEdit3("Floor Colour", (float*)&m_floorColour);
	ImGui::Checkbox("WireFrame", &m_wireFrame);
	ImGui::End();
	ImGui::Render();
}

void Lab1::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}