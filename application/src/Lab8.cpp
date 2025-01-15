#include "Lab8.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include "Grid.hpp"


Lab8::Lab8(GLFWWindowImpl& win) : Layer(win)
{

	m_mainScene.reset(new Scene); //Scene holds everything in the scene; actors + lights

	VBOLayout depthLayout = { {GL_FLOAT, 3} };
	//Set Up Depth Only Pass Material
	ShaderDescription depthShaderDesc;
	depthShaderDesc.type = ShaderType::rasterization;
	depthShaderDesc.vertexSrcPath = "./assets/shaders/Lab4/ShadowVert.glsl";
	depthShaderDesc.fragmentSrcPath = "./assets/shaders/Lab4/ShadowFrag.glsl";
	std::shared_ptr<Shader> depthShader = std::make_shared<Shader>(depthShaderDesc);
	std::shared_ptr<Material> depthPassMaterial = std::make_shared<Material>(depthShader);

	ShaderDescription phongShaderDesc; //Path to source files and shader type, used to load the shader.
	phongShaderDesc.type = ShaderType::rasterization;
	phongShaderDesc.vertexSrcPath = "./assets/shaders/PhongVert.glsl";
	phongShaderDesc.fragmentSrcPath = "./assets/shaders/Lab5/PhongFrag.glsl";

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

	Actor cube;
	cube.geometry = cubeVAO;
	cube.material = cubeMaterial;
	cube.translation = glm::vec3(0.f, 5.f, -9.f);
	cube.scale = glm::vec3(0.25f);
	cube.recalc();
	modelIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(cube);


	//** Creating Floor
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	ShaderDescription floorShaderDesc; //Path to source files and shader type, used to load the shader.
	floorShaderDesc.type = ShaderType::tessellationAndGeometry;
	floorShaderDesc.vertexSrcPath = "./assets/shaders/Lab5/FloorVert.glsl";
	floorShaderDesc.controlSrcPath = "./assets/shaders/Lab7/FloorTCS.glsl";
	floorShaderDesc.evaluationSrcPath = "./assets/shaders/Lab7/FloorTES.glsl";
	floorShaderDesc.geometrySrcPath = "./assets/shaders/Lab6/FloorGeo.glsl";
	floorShaderDesc.fragmentSrcPath = "./assets/shaders/Lab5/FloorFrag.glsl";

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

	std::shared_ptr<VAO> floorDepthVAO;
	floorDepthVAO = std::make_shared<VAO>(grid->getIndices());
	floorDepthVAO->addVertexBuffer(grid->getVertexPositions(), depthLayout);

	floorMaterial = std::make_shared<Material>(floorShader);

	std::shared_ptr<Texture> floorHeightMap;
	floorHeightMap = std::make_shared<Texture>("./assets/textures/HeightMaps/map2.png", GL_CLAMP_TO_EDGE);
	floorMaterial->setValue("u_heightMap", floorHeightMap);

	std::shared_ptr<Texture> floorTexture;
	floorTexture = std::make_shared<Texture>("./assets/textures/HeightMaps/mountain-rock.png");
	floorMaterial->setValue("u_terrainTexture", floorTexture);

	std::shared_ptr<Texture> floorGrassTexture;
	floorGrassTexture = std::make_shared<Texture>("./assets/textures/HeightMaps/seamless-grasspng.png");
	floorMaterial->setValue("u_terrainGrassTexture", floorGrassTexture);

	floorMaterial->setPrimitive(GL_PATCHES);

	Actor floor;
	floor.geometry = gridVAO;
	floor.material = floorMaterial;
	floor.translation = glm::vec3(-50.0f, -20.f, 0.0f);
	floor.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);

	Actor floor2;
	floor2.geometry = gridVAO;
	floor2.material = floorMaterial;
	floor2.translation = glm::vec3(-50.0f, -5.f, -100.0f);
	floor2.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor2);
	//** Floor Created


	//**Creating Vampire
	ShaderDescription vampireShaderDesc; //Path to source files and shader type, used to load the shader.
	vampireShaderDesc.type = ShaderType::rasterization;
	vampireShaderDesc.vertexSrcPath = "./assets/shaders/Lab5/VampireVert.glsl";
	vampireShaderDesc.fragmentSrcPath = "./assets/shaders/Lab5/VampireFrag.glsl";
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

	std::shared_ptr<VAO> vampireDepthVAO;
	vampireDepthVAO = std::make_shared<VAO>(vampireModel.m_meshes[0].indices);
	vampireDepthVAO->addVertexBuffer(vampireModel.m_meshes[0].positions, depthLayout);

	std::shared_ptr<Texture> vampireSpecular;
	vampireSpecular = std::make_shared<Texture>("./assets/models/Vampire/textures/specular.png");

	std::shared_ptr<Texture> vampireAlbedo;
	vampireAlbedo = std::make_shared<Texture>("./assets/models/Vampire/textures/diffuse.png");

	std::shared_ptr<Texture> vampireNormal;
	vampireNormal = std::make_shared<Texture>("./assets/models/Vampire/textures/normal.png");


	vampireMaterial = std::make_shared<Material>(vampireShader);
	vampireMaterial->setValue("u_specularMap", vampireSpecular);
	vampireMaterial->setValue("u_albedoMap", vampireAlbedo);
	vampireMaterial->setValue("u_normalMap", vampireNormal);

	vampireMaterial->m_shader = vampireShader;

	Actor vampire;
	vampire.geometry = vampireVAO;
	vampire.depthGeometry = vampireDepthVAO;
	vampire.material = vampireMaterial;
	vampire.depthMaterial = depthPassMaterial;
	vampire.translation = glm::vec3(0.0f, 5.0f, -11.0f);
	vampire.scale = glm::vec3(5.0f, 5.0f, 5.0f);
	vampire.recalc();
	vampireIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(vampire);
	//**Vampire Created


	for (int i = 0; i < 10; i++)
	{
		Actor vamp;
		vamp.geometry = vampireVAO;
		vamp.depthGeometry = vampireDepthVAO;
		vamp.material = vampireMaterial;
		vamp.depthMaterial = depthPassMaterial;
		vamp.translation = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), 2.5f, Randomiser::uniformFloatBetween(-12.0, -60.0));
		vamp.scale = glm::vec3(5.0f, 5.0f, 5.0f);
		vamp.recalc();
		m_mainScene->m_actors.push_back(vamp);
	}

	DirectionalLight dl;
	dl.direction = glm::normalize(m_lightDirection);
	dl.colour = glm::vec3(0.25f);
	m_mainScene->m_directionalLights.push_back(dl);

	PointLight pointLight;
	m_numPointLights = 60;


	for (int i = 0; i < m_numPointLights; i++)
	{
		if (i == 0)
		{
			pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
			pointLight.position = glm::vec3(0.0f, 5.f, -9.0f);
			pointLight.constants = glm::vec3(1.0f, 0.22f, 0.2f);
		}

		else
		{
			pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
			pointLight.position = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -4.0f, Randomiser::uniformFloatBetween(0.0, -80.0));
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
	//Created SkyBox

	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	camera.translation = glm::vec3(0.0f, 10.0f, 0.0f);
	m_mainScene->m_actors.push_back(camera);

	std::vector<float> billboardPositions; //GEO shader for billboards will create all billboards in scene, pass it all positions and for each position it will create a billboard

	std::shared_ptr<VAO> billboardVAO;
	std::vector<uint32_t> billboardIndices = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	for (int i = 0; i < 10; i++)
	{
		billboardPositions.push_back(Randomiser::uniformFloatBetween(-50.0f, 50.0f)); //x
		billboardPositions.push_back(Randomiser::uniformFloatBetween(16.0f, 16.0f)); //y
		billboardPositions.push_back(Randomiser::uniformFloatBetween(-80.0, -100.0f)); //z
	}
	billboardVAO = std::make_shared<VAO>(billboardIndices);
	billboardVAO->addVertexBuffer(billboardPositions, { {GL_FLOAT, 3} });

	ShaderDescription billboardShaderDesc; //Path to source files and shader type, used to load the shader.
	billboardShaderDesc.type = ShaderType::geometry;
	billboardShaderDesc.vertexSrcPath = "./assets/shaders/Lab6/BillboardVert.glsl";
	billboardShaderDesc.geometrySrcPath = "./assets/shaders/Lab6/BillboardGeo.glsl";
	billboardShaderDesc.fragmentSrcPath = "./assets/shaders/Lab6/BillboardFrag.glsl";
	std::shared_ptr<Shader> billboardShader;
	billboardShader = std::make_shared<Shader>(billboardShaderDesc);

	std::shared_ptr<Material> billBoardMaterial;
	billBoardMaterial = std::make_shared<Material>(billboardShader);
	billBoardMaterial->setPrimitive(GL_POINTS);

	std::shared_ptr<Texture> billboardTexture;
	billboardTexture = std::make_shared<Texture>("./assets/textures/Billboards/bigtree2.png");
	billBoardMaterial->setValue("u_billboardImage", billboardTexture);

	Actor billboard;
	billboard.geometry = billboardVAO;
	billboard.material = billBoardMaterial;

	ShaderDescription billboardDepthShaderDesc;
	billboardDepthShaderDesc.type = ShaderType::geometry;
	billboardDepthShaderDesc.vertexSrcPath = "./assets/shaders/Lab6/BillboardDepthVert.glsl";
	billboardDepthShaderDesc.geometrySrcPath = "./assets/shaders/Lab6/BillboardDepthGeo.glsl";
	billboardDepthShaderDesc.fragmentSrcPath = "./assets/shaders/Lab6/BillboardDepthFrag.glsl";
	std::shared_ptr<Shader> billboardDepthShader = std::make_shared<Shader>(billboardDepthShaderDesc);
	std::shared_ptr<Material> billboardDepthPassMaterial = std::make_shared<Material>(billboardDepthShader);

	billboardDepthPassMaterial->setPrimitive(GL_POINTS);
	billboardDepthPassMaterial->setValue("u_billboardImage", billboardTexture);
	billboard.depthMaterial = billboardDepthPassMaterial;
	billboard.depthGeometry = billboardVAO;

	m_mainScene->m_actors.push_back(billboard);

	//Depth Only Pass
	FBOLayout prePassLayout =
	{
		{ AttachmentType::Depth, true }
	};

	FBOLayout GBufferLayout =
	{
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}
	};

	//Shadow pass
	DepthPass shadowPass;
	shadowPass.scene = m_mainScene;
	shadowPass.parseScene();
	shadowPass.target = std::make_shared<FBO>(glm::ivec2(4096, 4096), prePassLayout);
	shadowPass.viewPort = { 0, 0, 4096, 4096 }; //Larger viewport, more fragments availabe to store depth info, more accurate show map

	/* Get view direction from light to center, and then place light camera some distance along this vector */
	glm::vec3 lightPosition = (m_shadowMapVars.centre - glm::normalize(m_mainScene->m_directionalLights.at(0).direction)) * m_shadowMapVars.distanceAlongLightVector;
	glm::mat4 shadowView = glm::lookAt(lightPosition, m_shadowMapVars.centre, m_shadowMapVars.up);
	shadowPass.camera.view = shadowView;
	shadowPass.camera.projection = glm::ortho(-m_shadowMapVars.orthoSize, //Left			Vertical Clipping PLanes
		m_shadowMapVars.orthoSize, //Right
		-m_shadowMapVars.orthoSize, //Bottom        Horizontal Clipping Planes
		m_shadowMapVars.orthoSize, //Top
		-m_shadowMapVars.orthoSize / 2, //Near
		m_shadowMapVars.orthoSize * 2);//Far


	shadowPass.UBOmanager.setCachedValue("b_lightCamera", "u_view", shadowPass.camera.view);
	shadowPass.UBOmanager.setCachedValue("b_lightCamera", "u_projection", shadowPass.camera.projection);

	m_mainRenderer.addDepthPass(shadowPass);
	m_previousRenderPassIndex++;
	//ShadowPass setup


	FBOLayout colourLayout =
	{
		{AttachmentType::ColourHDR, true}, //bool = can be sampled?
		{AttachmentType::Depth, false}
	};

	FBOLayout colourAndDepthLayout =
	{
		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}
	};


	//Gpass
	RenderPass GPass;
	GPass.scene = m_mainScene;
	GPass.parseScene();
	GPass.target = std::make_shared<FBO>(m_winRef.getSize(), GBufferLayout); //Target is custom FBO
	GPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.0f);
	GPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	GPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	GPass.UBOmanager.setCachedValue("b_camera", "u_view", GPass.camera.view);
	GPass.UBOmanager.setCachedValue("b_camera", "u_projection", GPass.camera.projection);
	GPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	m_mainRenderer.addRenderPass(GPass);
	m_previousRenderPassIndex++;
	//Gpass setup

	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(GPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f) * 2.0f, 2.5f);
	m_mainScene->m_actors.at(modelIdx).attachScript<RotationScript>(GPass.scene->m_actors.at(modelIdx), glm::vec3(0.3f, 0.6f, 0.9f), GLFW_KEY_1);

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
	Actor screen;
	//std::shared_ptr<Actor> screen = std::make_shared<Actor>(quad);
	screen.geometry = screenQuadVAO;

	//For Post processing render passes, use screen (quad) as input texture
	m_screenScene.reset(new Scene);


	//Skybox Only Pass
	skyBoxOnlyScene.reset(new Scene);
	skyBoxOnlyScene->m_actors.push_back(skyBox);

	RenderPass skyBoxPass;
	skyBoxPass.scene = skyBoxOnlyScene;
	skyBoxPass.parseScene();
	skyBoxPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	skyBoxPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.0f);
	skyBoxPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	skyBoxPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_view", skyBoxPass.camera.view);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_projection", skyBoxPass.camera.projection);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	m_mainRenderer.addRenderPass(skyBoxPass);
	m_previousRenderPassIndex++;
	//Skybox pass set up


	//Light Pass
	ShaderDescription lightPassShaderDesc; //Path to source files and shader type, used to load the shader.
	lightPassShaderDesc.type = ShaderType::rasterization;
	lightPassShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	lightPassShaderDesc.fragmentSrcPath = "./assets/shaders/Lab5/LightPassFrag.glsl";
	std::shared_ptr<Shader> lightPassShader;
	lightPassShader = std::make_shared<Shader>(lightPassShaderDesc);

	lightPassMaterial = std::make_shared<Material>(lightPassShader);
	lightPassMaterial->setValue("u_positionTexture", GPass.target->getTarget(0));
	lightPassMaterial->setValue("u_normalTexture", GPass.target->getTarget(1));
	lightPassMaterial->setValue("u_albedoTexture", GPass.target->getTarget(2));
	lightPassMaterial->setValue("u_specularTexture", GPass.target->getTarget(3));
	lightPassMaterial->setValue("u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	lightPassMaterial->setValue("u_depthTexture", GPass.target->getTarget(4));
	lightPassMaterial->setValue("u_skyBoxTexture", skyBoxPass.target->getTarget(0));
	lightPassMaterial->setValue("u_shadowMap", shadowPass.target->getTarget(0));
	lightPassMaterial->setValue("u_lightSpaceTransform", m_mainRenderer.getDepthPass(0).camera.projection * m_mainRenderer.getDepthPass(0).camera.view);

	screen.material = lightPassMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass LightPass;
	LightPass.scene = m_screenScene;
	LightPass.parseScene(); //sorts UBOs for each actor
	LightPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	LightPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	LightPass.camera.projection = glm::ortho(0.f, width, height, 0.f);

	LightPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	LightPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < m_numPointLights; i++)
	{
		bool passed = LightPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		passed = LightPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		passed = LightPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	m_mainRenderer.addRenderPass(LightPass);
	m_previousRenderPassIndex++;
	m_screenScene.reset(new Scene);
	//Light Pass Setup


	////Depth Render Pass
	//std::shared_ptr<Material> depthRenderMaterial;
	//SetUpPPMaterial("./assets/shaders/Lab3/RenderDepthFrag.glsl", depthRenderMaterial, shadowPass.target->getTarget(0)); //0 = Colour attatchment, 1 = depth (unless there's more colour attatchments)
	//screen.material = depthRenderMaterial;
	//m_screenScene->m_actors.push_back(screen);
	//
	//RenderPass depthRenderPass;
	//depthRenderPass.scene = m_screenScene;
	//depthRenderPass.parseScene(); //sorts UBOs for each actor
	//depthRenderPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourAndDepthLayout); //Target is custom FBO
	//depthRenderPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	//depthRenderPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	//depthRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_view", depthRenderPass.camera.view);
	//depthRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", depthRenderPass.camera.projection);

	//m_mainRenderer.addRenderPass(depthRenderPass);
	//m_previousRenderPassIndex++;
	//m_screenScene.reset(new Scene);
	////Depth Render Pass Setup


	//Fog Render Pass
	std::shared_ptr<Material> fogRenderMaterial;
	SetUpPPMaterial("./assets/shaders/Lab3/FogFrag.glsl", fogRenderMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0)); //0 = Colour attatchment, 1 = depth (unless there's more colour attatchments)
	fogRenderMaterial->setValue("u_depthTexture", GPass.target->getTarget(4));
	fogRenderMaterial->setValue("u_expSquared", 4.f);
	screen.material = fogRenderMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass fogRenderPass;
	fogRenderPass.scene = m_screenScene;
	fogRenderPass.parseScene(); //sorts UBOs for each actor
	fogRenderPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	fogRenderPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	fogRenderPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	fogRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_view", fogRenderPass.camera.view);
	fogRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", fogRenderPass.camera.projection);

	m_mainRenderer.addRenderPass(fogRenderPass);
	m_previousRenderPassIndex++;
	m_fogPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Fog Render Pass Setup


	//Edge Detection Pass : Need to comment setting of u_edgeStrength in onUpdate if commenting this out
	std::shared_ptr<Material> edgeDetectionMaterial;
	SetUpPPMaterial("./assets/shaders/Lab3/EdgeDetectionFrag.glsl", edgeDetectionMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	edgeDetectionMaterial->setValue("u_screenSize", glm::vec2(width, height));
	edgeDetectionMaterial->setValue("u_edgeStrength", m_edgeStrength);
	screen.material = edgeDetectionMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass edgeDetectionPass;
	edgeDetectionPass.scene = m_screenScene;
	edgeDetectionPass.parseScene(); //sorts UBOs for each actor
	edgeDetectionPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout);
	edgeDetectionPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	edgeDetectionPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	edgeDetectionPass.UBOmanager.setCachedValue("b_camera2D", "u_view", edgeDetectionPass.camera.view);
	edgeDetectionPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", edgeDetectionPass.camera.projection);

	m_mainRenderer.addRenderPass(edgeDetectionPass);
	m_previousRenderPassIndex++;
	m_edgeDetectionPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Edge Detection Pass completed


	//Tint pass : Need to comment setting of u_tintColour in onUpdate if commenting this out
	std::shared_ptr<Material> tintMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/TintFrag.glsl", tintMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	screen.material = tintMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass tintPass;
	tintPass.scene = m_screenScene;
	tintPass.parseScene(); //sorts UBOs for each actor
	tintPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	tintPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	tintPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_view", tintPass.camera.view);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", tintPass.camera.projection);

	m_mainRenderer.addRenderPass(tintPass);
	m_previousRenderPassIndex++;
	m_tintPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Tint Pass setup


	////Sepia Pass
	//std::shared_ptr<Material> sepiaMaterial;
	//SetUpPPMaterial("./assets/shaders/Lab2/SepiaFrag.glsl", sepiaMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	//screen.material = sepiaMaterial;
	//m_screenScene->m_actors.push_back(screen);
	//
	//RenderPass sepiaPass;
	//sepiaPass.scene = m_screenScene;
	//sepiaPass.parseScene(); //sorts UBOs for each actor
	//sepiaPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	//sepiaPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	//sepiaPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	//sepiaPass.UBOmanager.setCachedValue("b_camera2D", "u_view", sepiaPass.camera.view);
	//sepiaPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", sepiaPass.camera.projection);

	//m_mainRenderer.addRenderPass(sepiaPass);
	//m_previousRenderPassIndex++;
	//m_screenScene.reset(new Scene);
	////Sepia Pass Setup


	//Blur Pass : Need to comment setting of uBlurRadius in onUpdate if commenting this out
	std::shared_ptr<Material> blurMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/BlurFrag.glsl", blurMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	blurMaterial->setValue("u_screenSize", glm::vec2(width, height));
	blurMaterial->setValue("u_blurRadius", m_blurRadius);
	screen.material = blurMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass blurPass;
	blurPass.scene = m_screenScene;
	blurPass.parseScene(); //sorts UBOs for each actor
	blurPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	blurPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	blurPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_view", blurPass.camera.view);
	blurPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", blurPass.camera.projection);

	m_mainRenderer.addRenderPass(blurPass);
	m_previousRenderPassIndex++;
	m_blurPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Blur Pass Completed


	//Depth of Field Pass
	std::shared_ptr<Material> dofMaterial;
	SetUpPPMaterial("./assets/shaders/Lab3/DepthOfFieldFrag.glsl", dofMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex - 1).target->getTarget(0)); // Pre blur texture
	dofMaterial->setValue("u_blurTexture", m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	dofMaterial->setValue("u_depthTexture", GPass.target->getTarget(4)); //Main pass has depth buffer attached to target(1).
	dofMaterial->setValue("u_focusDistance", 0.9f); //Main pass hasdepth buffer attached to target(1).
	screen.material = dofMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass dofPass;
	dofPass.scene = m_screenScene;
	dofPass.parseScene(); //sorts UBOs for each actor
	dofPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); //Target is custom FBO
	dofPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	dofPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_view", dofPass.camera.view);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", dofPass.camera.projection);

	m_mainRenderer.addRenderPass(dofPass);
	m_previousRenderPassIndex++;
	m_dofPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Depth of Field pass completed


	//Vignette Pass
	std::shared_ptr<Material> vignetteMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/VignetteFrag.glsl", vignetteMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	screen.material = vignetteMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass vignettePass;
	vignettePass.scene = m_screenScene;
	vignettePass.parseScene(); //sorts UBOs for each actor
	vignettePass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout);
	vignettePass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	vignettePass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	vignettePass.UBOmanager.setCachedValue("b_camera2D", "u_view", vignettePass.camera.view);
	vignettePass.UBOmanager.setCachedValue("b_camera2D", "u_projection", vignettePass.camera.projection);

	m_mainRenderer.addRenderPass(vignettePass);
	m_previousRenderPassIndex++;
	m_screenScene.reset(new Scene);
	//Vignette Pass completed


	//Screen Pass
	std::shared_ptr<Material> screenQuadMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/ScreenFrag.glsl", screenQuadMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
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

	//Compute Pass
	TextureDescription textDesc;
	textDesc.width = 512;
	textDesc.height = 512;
	textDesc.channels = 4;
	textDesc.isHDR = false;
	std::shared_ptr<Texture>emptyTexture = std::make_shared<Texture>(textDesc); //empty texture to write to with compute

	ShaderDescription textureComputeDesc;
	textureComputeDesc.type = ShaderType::compute;
	textureComputeDesc.computeSrcPath = "./assets/shaders/Lab8/compute_textureTest.glsl";

	std::shared_ptr<Shader> computeTestShader = std::make_shared<Shader>(textureComputeDesc);
	std::shared_ptr<Material> computeTestMaterial = std::make_shared<Material>(computeTestShader);
	std::shared_ptr<Texture> computeImageIn;
	computeImageIn = std::make_shared<Texture>("./assets/textures/compute/city.png");
	computeTestMaterial->setValue("u_image", computeImageIn);
	computeTestMaterial->setValue("u_redTint", m_redTint);


	ComputePass textureComputePass;
	textureComputePass.material = computeTestMaterial;
	textureComputePass.workgroups = { 32, 32, 1 }; //Workgroup = block containing threads
	/*Memory accesses using shader image load, store, and atomic built - in functions issued after the barrier will reflect data written by shaders
	prior to the barrier.Additionally, image stores and atomics issued after the barrier will not execute until all memory accesses
	(e.g., loads, stores, texture fetches, vertex fetches) initiated prior to the barrier complete.*/
	textureComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image image;
	image.mipLevel = 0;
	image.layered = false;
	image.texture = emptyTexture;
	image.imageUnit = textureComputePass.material->m_shader->m_imageBindingPoints["outputImg"];
	image.access = TextureAccess::WriteOnly;

	textureComputePass.images.push_back(image);
	m_previousRenderPassIndex++;
	m_mainRenderer.addComputePass(textureComputePass);
	//ComputePass setup
}

void Lab8::onRender() const
{
	m_mainRenderer.render();
}
void Lab8::onUpdate(float timestep)
{
	// per frame uniforms
	auto cubeMat = m_mainScene->m_actors.at(modelIdx).material;

	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_remapRange", m_remapRange);
	
	//Convert from bool to float, as passing in a boolas uniform causes a crash (same with an int too?)
	if (m_geoNormal) floorMat->setValue("u_geoNormal", 0.0f);
	else floorMat->setValue("u_geoNormal", 1.0f);


	m_mainScene->m_directionalLights.at(0).direction = glm::normalize(m_lightDirection);
	m_mainRenderer.getRenderPass(3).UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < m_numPointLights; i++)
	{
		m_mainScene->m_pointLights.at(0).position = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4 + 10.0f, -9.0f);
		m_mainScene->m_actors.at(modelIdx).translation = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4 + 10.0f, -9.0f);

		m_mainRenderer.getRenderPass(3).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		m_mainRenderer.getRenderPass(3).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		m_mainRenderer.getRenderPass(3).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	m_mainRenderer.getRenderPass(0).drawInWireFrame = m_wireFrame;

	auto& skyBox = skyBoxOnlyScene->m_actors.at(0);
	skyBox.material->setValue("u_skyBoxView", glm::mat4(glm::mat3(m_mainRenderer.getRenderPass(0).camera.view)));

	if (m_tintPassIndex != -1) m_mainRenderer.getRenderPass(m_tintPassIndex).scene->m_actors.at(0).material->setValue("u_tintColour", m_tintColour);
	if (m_blurPassIndex != -1) m_mainRenderer.getRenderPass(m_blurPassIndex).scene->m_actors.at(0).material->setValue("u_blurRadius", m_blurRadius);
	if (m_edgeDetectionPassIndex != -1) m_mainRenderer.getRenderPass(m_edgeDetectionPassIndex).scene->m_actors.at(0).material->setValue("u_edgeStrength", m_edgeStrength);
	if (m_fogPassIndex != -1) m_mainRenderer.getRenderPass(m_fogPassIndex).scene->m_actors.at(0).material->setValue("u_expSquared", m_fogType);
	if (m_dofPassIndex != -1) m_mainRenderer.getRenderPass(m_dofPassIndex).scene->m_actors.at(0).material->setValue("u_focusDistance", m_focusDistance);


	//Update scripts
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

	// Update camera  and its position in UBO
	auto& skyBoxpass = m_mainRenderer.getRenderPass(2);
	skyBoxpass.camera.updateView(camera.transform);
	skyBoxpass.UBOmanager.setCachedValue("b_camera", "u_view", skyBoxpass.camera.view);
	skyBoxpass.UBOmanager.setCachedValue("b_camera", "u_viewPos", camera.translation);

	lightPassMaterial->setValue("u_viewPos", camera.translation);

	glm::vec3 lightPosition = (m_shadowMapVars.centre - m_mainScene->m_directionalLights.at(0).direction) * m_shadowMapVars.distanceAlongLightVector;
	glm::mat4 shadowView = glm::lookAt(lightPosition, m_shadowMapVars.centre, m_shadowMapVars.up);
	m_mainRenderer.getDepthPass(0).camera.view = shadowView;
	m_mainRenderer.getDepthPass(0).UBOmanager.setCachedValue("b_lightCamera", "u_view", m_mainRenderer.getDepthPass(0).camera.view);

	lightPassMaterial->setValue("u_shadowMap", m_mainRenderer.getDepthPass(0).target->getTarget(0));
	lightPassMaterial->setValue("u_lightSpaceTransform", m_mainRenderer.getDepthPass(0).camera.projection * m_mainRenderer.getDepthPass(0).camera.view);

	m_mainRenderer.getComputePass(0).material->setValue("u_redTint", m_redTint);
}


void Lab8::onImGUIRender()
{
	float ms = 1000.0f / ImGui::GetIO().Framerate; ;  //milliseconds
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("GAMR3521");
	ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);  // display FPS and ms
	ImGui::Checkbox("WireFrame", &m_wireFrame);
	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("G Buffer"))
		{
			//Display pre tone mapped + gamma corrected FBO texture in GUI for comparision
			GLuint positionTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(0)->getID();
			ImVec2 imageSize = ImVec2(256, 256);
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)positionTextureID, imageSize, uv0, uv1);

			GLuint normalsTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(1)->getID();
			ImGui::Image((void*)(intptr_t)normalsTextureID, imageSize, uv0, uv1);

			GLuint albedoTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(2)->getID();
			ImGui::Image((void*)(intptr_t)albedoTextureID, imageSize, uv0, uv1);

			GLuint specularTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(3)->getID();
			ImGui::Image((void*)(intptr_t)specularTextureID, imageSize, uv0, uv1);


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Lighting"))
		{
			ImGui::DragFloat3("Light Direction", (float*)&m_lightDirection, 0.001f, -1.0f, 1.0f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Post Processing"))
		{
			if (m_tintPassIndex != -1) ImGui::ColorEdit3("Tint Colour", (float*)&m_tintColour);
			if (m_fogPassIndex != -1)ImGui::DragFloat("Fog Type", (float*)&m_fogType, 0.025f, -1.f, 2.0f, "%1.0f");
			if (m_edgeDetectionPassIndex != -1)ImGui::DragFloat("Edge Strength", (float*)&m_edgeStrength, 0.002f, 0.f, 1.0f);
			if (m_blurPassIndex != -1)ImGui::DragFloat("Blur Radius", (float*)&m_blurRadius, 0.025f, 0.0f, 5.0f);
			if (m_dofPassIndex != -1)ImGui::DragFloat("Focus Distance", (float*)&m_focusDistance, 0.002f, 0.f, 1.0f);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Terrain"))
		{
			ImGui::DragFloat("Texture Remap Range", (float*)&m_remapRange, 0.2f, 0.f, 1000.0f);
			ImGui::Checkbox("Geo Normals", &m_geoNormal);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Compute Pass"))
		{
			//Display pre tone mapped + gamma corrected FBO texture in GUI for comparision
			GLuint textureID = m_mainRenderer.getComputePass(0).images[0].texture->getID();
			ImVec2 imageSize = ImVec2(512, 512);
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

			ImGui::DragFloat("Red tint", (float*)&m_redTint, 0.01f, 0.f, 1.0f);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::Render();
}

void Lab8::SetUpPPMaterial(std::string fragPath, std::shared_ptr<Material>& mat, std::shared_ptr<Texture> inputTex)
{
	ShaderDescription shaderDesc; //Path to source files and shader type, used to load the shader.
	shaderDesc.type = ShaderType::rasterization;
	shaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	shaderDesc.fragmentSrcPath = fragPath;
	std::shared_ptr<Shader> shader;
	shader = std::make_shared<Shader>(shaderDesc);

	mat = std::make_shared<Material>(shader);
	mat->setValue("u_inputTexture", inputTex); //simply reads from the texture, which is the FBO we set the main pass to draw to
}

void Lab8::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}