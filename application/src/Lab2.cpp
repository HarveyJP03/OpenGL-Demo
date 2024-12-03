#include "Lab2.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include "Grid.hpp"
#include "Lab2.hpp"


Lab2::Lab2(GLFWWindowImpl& win) : Layer(win)
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
	cube.translation = glm::vec3(0.f, -5.f, -9.f);
	cube.scale = glm::vec3(0.25f);
	cube.recalc();
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(cube);


	//** Creating Floor
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	ShaderDescription floorShaderDesc; //Path to source files and shader type, used to load the shader.
	floorShaderDesc.type = ShaderType::rasterization;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/FloorVert.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/FloorFrag.glsl";

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
	floorMaterial->setValue("u_albedo", m_floorColour);

	Actor floor;
	floor.geometry = gridVAO;
	floor.material = floorMaterial;
	floor.translation = glm::vec3(-50.0f, -5.f, -50.0f);
	floor.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);
	//** Floor Created


	//**Creating Vampire
	ShaderDescription vampireShaderDesc; //Path to source files and shader type, used to load the shader.
	vampireShaderDesc.type = ShaderType::rasterization;
	vampireShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/VampireVert.glsl";
	vampireShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/VampireFrag.glsl";
	std::shared_ptr<Shader> vampireShader;
	vampireShader = std::make_shared<Shader>(vampireShaderDesc);

	VBOLayout vampireLayout =
	{
		{GL_FLOAT, 3}, //pos
		{GL_FLOAT, 3}, //normal
		{GL_FLOAT, 2}, //UV
		{GL_FLOAT, 3}  //Tangent ( normals fron normal map )
	};

	uint32_t attributeTypes =
		Model::VertexFlags::positions |
		Model::VertexFlags::normals |
		Model::VertexFlags::uvs |
		Model::VertexFlags::tangents;

	Model vampireModel("./assets/models/Vampire/vampire.obj", attributeTypes);

	std::shared_ptr<VAO> vampireVAO;
	vampireVAO = std::make_shared<VAO>(vampireModel.m_meshes[0].indices); //Init with IBO

	vampireVAO->addVertexBuffer(vampireModel.m_meshes[0].vertices, vampireLayout); //Add VBO

	std::shared_ptr<Texture> vampireSpecular;
	vampireSpecular = std::make_shared<Texture>("./assets/models/Vampire/textures/specular.png");

	std::shared_ptr<Texture> vampireAlbedo;
	vampireAlbedo = std::make_shared<Texture>("./assets/models/Vampire/textures/diffuse.png");

	std::shared_ptr<Texture> vampireNormal;
	vampireNormal = std::make_shared<Texture>("./assets/models/Vampire/textures/normal.png");

	std::shared_ptr<Material> vampireMaterial;
	vampireMaterial = std::make_shared<Material>(vampireShader);
	vampireMaterial->setValue("u_albedo", m_colour);
	vampireMaterial->setValue("u_specularMap", vampireSpecular);
	vampireMaterial->setValue("u_albedoMap", vampireAlbedo);
	vampireMaterial->setValue("u_normalMap", vampireNormal);

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
	dl.direction = glm::normalize(glm::vec3(0.2, -1.0, -0.5));
	dl.colour = glm::vec3(0.25f);
	m_mainScene->m_directionalLights.push_back(dl);

	PointLight pointLight;
	uint32_t numPointLights = 6;

	glm::vec3 positions[2] = { glm::vec3(0.0f, -5.f, 10.0f), glm::vec3(0.0f, -5.f, -9.0f) };
	glm::vec3 colours[2] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.f, 1.0f) };
	for (int i = 0; i < numPointLights; i++)
	{
		if (i == 0)
		{
			pointLight.colour = colours[i];
			pointLight.position = glm::vec3(0.0f, -5.f, -9.0f);
			pointLight.constants = glm::vec3(1.0f, 0.22f, 0.2f);
		}

		else
		{
			pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
			pointLight.position = glm::vec3(Randomiser::uniformFloatBetween(-10.0, 10.0) , -4.0f, Randomiser::uniformFloatBetween(-10.0, 5.0));
			pointLight.constants = glm::vec3(1.0f, 0.14f, 0.07f);
		}

		m_mainScene->m_pointLights.push_back(pointLight);
	}


	//Creating Skybox**
	std::vector<uint32_t> skyboxIndices(skyboxVertices.size() / 3);
	std::iota(skyboxIndices.begin(), skyboxIndices.end(), 0);

	std::shared_ptr<VAO> skyBoxVAO;
	skyBoxVAO = std::make_shared<VAO>(skyboxIndices);
	skyBoxVAO->addVertexBuffer(skyboxVertices, { { GL_FLOAT, 3 } });

	std::array<const char*, 6> cubeMapPaths =
	{
		"./assets/textures/Skybox/right.png",
		"./assets/textures/Skybox/left.png",
		"./assets/textures/Skybox/top.png",
		"./assets/textures/Skybox/bottom.png",
		"./assets/textures/Skybox/back.png",
		"./assets/textures/Skybox/front.png",
	};

	std::shared_ptr<CubeMap> cubeMap;
	cubeMap = std::make_shared<CubeMap>(cubeMapPaths, false, false);

	ShaderDescription skyboxShaderDesc; //Path to source files and shader type, used to load the shader.
	skyboxShaderDesc.type = ShaderType::rasterization;
	skyboxShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/SkyBoxVert.glsl";
	skyboxShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/SkyBoxFrag.glsl";
	std::shared_ptr<Shader> skyboxShader;
	skyboxShader = std::make_shared<Shader>(skyboxShaderDesc);

	std::shared_ptr<Material> skyBoxMaterial;
	skyBoxMaterial = std::make_shared<Material>(skyboxShader);

	Actor skyBox;
	skyBox.geometry = skyBoxVAO;
	skyBox.material = skyBoxMaterial;
	skyBoxMaterial->setValue("u_skyBoxView", glm::inverse(skyBox.transform));
	skyBoxMaterial->setValue("u_skyBox", cubeMap);
	skyBoxIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(skyBox);

	//Created SkyBox

	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(camera);



	/*************************
	*  Main Render Pass
	**************************/

	FBOLayout typicalLayout =
	{
		{AttachmentType::ColourHDR, true}, //bool = can be sampled?
		{AttachmentType::Depth, false}
	};
	
	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	//mainPass.target = std::make_shared<FBO>(); ;
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), typicalLayout); //Target is custom FBO
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < numPointLights; i++)
	{
		bool passed = mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		passed = mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		passed = mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f), 0.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(modelIdx), glm::vec3(0.3f, 0.6f, 0.9f), GLFW_KEY_1);

	m_mainRenderer.addRenderPass(mainPass);

	float width = m_winRef.getWidthf();
	float height = m_winRef.getHeightf();
	const std::vector<float> screenVertices =
	{
		//position					UV
		0.0f,  0.0f,   0.0f,   0.0f, 1.0f, //bleft
		width, 0.0f,   0.0f,   1.0f, 1.0f, //bright
		width, height, 0.0f,   1.0f, 0.0f, //trigh
		0.0f,  height, 0.0f,   0.0f, 0.0f  //tleft
	};

	//Setup VAO
	VBOLayout screenQuadLayout = { {GL_FLOAT, 3}, { GL_FLOAT, 2} };
	const std::vector<uint32_t> screenIndices = { 0, 1, 2, 2, 3, 0 };
	std::shared_ptr<VAO> screenQuadVAO;
	screenQuadVAO = std::make_shared<VAO>(screenIndices);
	screenQuadVAO->addVertexBuffer(screenVertices, screenQuadLayout);


	//For Post processing render passes, use screen (quad) as input texture
	m_screenScene.reset(new Scene);
	Actor screen;

	////Setup Shaders
	ShaderDescription tintShaderDesc; //Path to source files and shader type, used to load the shader.
	tintShaderDesc.type = ShaderType::rasterization;
	tintShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	tintShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/TintFrag.glsl";
	std::shared_ptr<Shader> tintShader;
	tintShader = std::make_shared<Shader>(tintShaderDesc);

	tintMaterial = std::make_shared<Material>(tintShader);;
	tintMaterial->setValue("u_inputTexture", mainPass.target->getTarget(0)); //simply reads from the texture, which is the FBO we set the main pass to draw to

	screen.geometry = screenQuadVAO;
	screen.material = tintMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass tintPass;
	tintPass.scene = m_screenScene;
	tintPass.parseScene(); //sorts UBOs for each actor
	tintPass.target = std::make_shared<FBO>(m_winRef.getSize(), typicalLayout); //Target is custom FBO
	tintPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	tintPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_view", tintPass.camera.view);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", tintPass.camera.projection);

	m_mainRenderer.addRenderPass(tintPass);
	m_screenScene.reset(new Scene);

	//Setup Shaders
	ShaderDescription blurShaderDesc; //Path to source files and shader type, used to load the shader.
	blurShaderDesc.type = ShaderType::rasterization;
	blurShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	blurShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/BlurFrag.glsl";
	std::shared_ptr<Shader> blurShader;
	blurShader = std::make_shared<Shader>(blurShaderDesc);

	blurMaterial = std::make_shared<Material>(blurShader);
	blurMaterial->setValue("u_inputTexture", tintPass.target->getTarget(0)); //simply reads from the texture, which is the FBO we set the main pass to draw to
	glm::vec2 screenSize = glm::vec2(width, height);
	blurMaterial->setValue("u_screenSize", screenSize); //simply reads from the texture, which is the FBO we set the main pass to draw to
	blurMaterial->setValue("u_blurRadius", 0.75f); //simply reads from the texture, which is the FBO we set the main pass to draw to
	
	screen.geometry = screenQuadVAO;
	screen.material = blurMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass blurPass;
	blurPass.scene = m_screenScene;
	blurPass.parseScene(); //sorts UBOs for each actor
	blurPass.target = std::make_shared<FBO>(m_winRef.getSize(), typicalLayout); //Target is custom FBO
	blurPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	blurPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_view", blurPass.camera.view);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", blurPass.camera.projection);

	m_mainRenderer.addRenderPass(blurPass);
	m_screenScene.reset(new Scene);


	//Setup Shaders
	ShaderDescription screenShaderDesc; //Path to source files and shader type, used to load the shader.
	screenShaderDesc.type = ShaderType::rasterization;
	screenShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	screenShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/ScreenFrag.glsl";
	std::shared_ptr<Shader> screenShader;
	screenShader = std::make_shared<Shader>(screenShaderDesc);

	std::shared_ptr<Material> screenQuadMaterial;
	screenQuadMaterial = std::make_shared<Material>(screenShader);
	screenQuadMaterial->setValue("u_inputTexture", blurPass.target->getTarget(0)); //ScreenQuad shader simply reads from the texture, which is the FBO we set the main pass to draw to
	
	screen.geometry = screenQuadVAO;
	screen.material = screenQuadMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass screenPass;
	screenPass.scene = m_screenScene;
	screenPass.parseScene(); //sorts UBOs for each actor
	screenPass.target = std::make_shared<FBO>(); //Default FBO, the screenPass is where we draw to the screen
	screenPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	screenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_view", screenPass.camera.view);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", screenPass.camera.projection);

	m_mainRenderer.addRenderPass(screenPass);
	//Screen Pass Set up
}

void Lab2::onRender() const
{
	m_mainRenderer.render();
}
void Lab2::onUpdate(float timestep)
{
	// per frame uniforms
	auto cubeMat = m_mainScene->m_actors.at(modelIdx).material;
	cubeMat->setValue("u_albedo", m_colour);

	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_albedo", m_floorColour);

	for (int i = 0; i < 6; i++)
	{
		m_mainScene->m_pointLights.at(0).position = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4, -9.0f);
		m_mainScene->m_actors.at(modelIdx).translation = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4, -9.0f);

		m_mainRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		m_mainRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		m_mainRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}


	m_mainRenderer.getRenderPass(0).drawInWireFrame = m_wireFrame;

	auto& skyBox = m_mainScene->m_actors.at(skyBoxIdx);
	skyBox.material->setValue("u_skyBoxView", glm::mat4(glm::mat3(m_mainRenderer.getRenderPass(0).camera.view)));

	tintMaterial->setValue("u_tintColour", m_tintColour);
	//blurMaterial->setValue("u_blurRadius", cos(glfwGetTime()));

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


void Lab2::onImGUIRender()
{
	float ms = 1000.0f / ImGui::GetIO().Framerate; ;  //milisecconds
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("GAMR3521");
	ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);  // display FPS and ms
	ImGui::ColorEdit3("Cube Colour", (float*)&m_colour);
	ImGui::ColorEdit3("Floor Colour", (float*)&m_floorColour);
	ImGui::ColorEdit3("Tint Colour", (float*)&m_tintColour);
	ImGui::Checkbox("WireFrame", &m_wireFrame);

	//Display pre tone mapped + gamma corrected FBO texture in GUI for comparision
	GLuint textureID = m_mainRenderer.getRenderPass(0).target->getTarget(0)->getID();
	ImVec2 imageSize = ImVec2(256, 256);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

	ImGui::End();
	ImGui::Render();
}

void Lab2::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}