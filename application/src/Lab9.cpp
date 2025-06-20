#include "Lab9.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/rotation.hpp"
#include <numeric> // For std::iota
#include <glm/gtc/matrix_transform.hpp>
#include "Grid.hpp"


Lab9::Lab9(GLFWWindowImpl& win) : Layer(win)
{
	m_mainScene.reset(new Scene); //Scene holds everything in the scene; actors + lights
	m_forwardRenderScene.reset(new Scene); //Seperate Scene for forward rendering

	//Depth only material for shadow mapping
	VBOLayout depthLayout = { {GL_FLOAT, 3} };
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
	//Add to forward rendering scene later on


	//** Creating Floor
	std::shared_ptr<Grid> grid = std::make_shared<Grid>();
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	ShaderDescription floorShaderDesc;
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

	std::shared_ptr<Texture> floorTexture;
	floorTexture = std::make_shared<Texture>("./assets/textures/HeightMaps/mountain-rock.png", GL_REPEAT);
	floorMaterial->setValue("u_terrainTexture", floorTexture);

	std::shared_ptr<Texture> floorGrassTexture;
	floorGrassTexture = std::make_shared<Texture>("./assets/textures/HeightMaps/seamless-grasspng.png", GL_REPEAT);
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
	ShaderDescription vampireShaderDesc;
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
	//**Vampires Created

	DirectionalLight dl;
	dl.direction = glm::normalize(m_lightDirection);
	dl.colour = glm::vec3(0.25f);
	m_mainScene->m_directionalLights.push_back(dl);
	m_forwardRenderScene->m_directionalLights.push_back(dl);

	PointLight pointLight;
	m_numPointLights = 1;


	//No need to iterate over point lights here, as I only have 1 in the scene
	pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
	pointLight.position = glm::vec3(0.0f, 5.f, -9.0f);
	pointLight.constants = glm::vec3(1.0f, 0.22f, 0.2f);

	m_mainScene->m_pointLights.push_back(pointLight);


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

	ShaderDescription skyboxShaderDesc;
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

	//Tree billboards
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

	ShaderDescription billboardShaderDesc;
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
	billboardTexture = std::make_shared<Texture>("./assets/textures/Billboards/bigtree2.png", GL_CLAMP_TO_EDGE);
	billBoardMaterial->setValue("u_billboardImage", billboardTexture);

	Actor billboard;
	billboard.geometry = billboardVAO;
	billboard.material = billBoardMaterial;

	//Repeat same process to turn vertex into billboard, but in shadow pass for tree shadows
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
	//Tree Billboards set up


	//Set up particle SSBO and initiailse	
	struct Particle
	{
		glm::vec4 position; //z = particle age
		glm::vec4 velocity;
	};

	int numParts = 256;
	std::shared_ptr<SSBO>particleSSBO = std::make_shared<SSBO>(sizeof(Particle) * numParts, numParts); //Size of SSBO, element count

	ShaderDescription initParticlesComputeDesc;
	initParticlesComputeDesc.type = ShaderType::compute;
	initParticlesComputeDesc.computeSrcPath = "./assets/shaders/Lab10/compute_initParticles.glsl";
	std::shared_ptr<Shader> computeInitParticlesShader = std::make_shared<Shader>(initParticlesComputeDesc);

	std::shared_ptr<Material>initParticlesMaterial = std::make_shared<Material>(computeInitParticlesShader);

	ComputePass initParticlesComputePass;
	initParticlesComputePass.material = initParticlesMaterial;
	initParticlesComputePass.workgroups = { 32, 1, 1 }; //256 particles, work groups are 8x1
	initParticlesComputePass.barrier = MemoryBarrier::ShaderStorageAccess;
	initParticlesComputePass.ssbo = particleSSBO;

	m_initRenderer.addComputePass(initParticlesComputePass);
	m_initRenderer.render();
	//Particle initialisation set up

	//Depth Only Pass
	FBOLayout prePassLayout =
	{
		{ AttachmentType::Depth, true }
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

	FBOLayout GBufferLayout =
	{
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}
	};

	//G Pass
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
	//G Pass setup

	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(GPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(1.6f, 0.6f, 2.f) * 2.0f, 2.5f);

	//Screen Quad
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
	screen.geometry = screenQuadVAO;

	m_screenScene.reset(new Scene); 	//For Post processing render passes, use screen (quad) as input texture
	//Screen Quad set up


	//Skybox Only Pass
	skyBoxOnlyScene.reset(new Scene);
	skyBoxOnlyScene->m_actors.push_back(skyBox);

	RenderPass skyBoxPass;
	skyBoxPass.scene = skyBoxOnlyScene;
	skyBoxPass.parseScene();
	skyBoxPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout);
	skyBoxPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.0f);
	skyBoxPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	skyBoxPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_view", skyBoxPass.camera.view);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_projection", skyBoxPass.camera.projection);
	skyBoxPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	m_mainRenderer.addRenderPass(skyBoxPass);
	m_previousRenderPassIndex++;
	//Skybox only pass set up


	//Light Pass
	ShaderDescription lightPassShaderDesc;
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
	LightPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
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


	//Create particles
	ShaderDescription particleShaderDesc;
	particleShaderDesc.type = ShaderType::geometry;
	particleShaderDesc.vertexSrcPath = "./assets/shaders/Lab10/particleVert.glsl";
	particleShaderDesc.geometrySrcPath = "./assets/shaders/Lab10/particleGeo.glsl";
	particleShaderDesc.fragmentSrcPath = "./assets/shaders/Lab10/particleFrag.glsl";
	std::shared_ptr<Shader> particleShader;
	particleShader = std::make_shared<Shader>(particleShaderDesc);

	std::shared_ptr<Material> particleMaterial;
	particleMaterial = std::make_shared<Material>(particleShader);
	particleMaterial->setPrimitive(GL_POINTS);

	std::shared_ptr<Texture> particleTexture;
	particleTexture = std::make_shared<Texture>("./assets/textures/Billboards/particle.png", GL_CLAMP_TO_EDGE);
	particleMaterial->setValue("u_particleImage", particleTexture);
	
	Actor particle;
	particle.material = particleMaterial;
	particle.SSBOgeometry = particleSSBO;
	//Particles Created

	//Pass for forward rendering
	m_forwardRenderScene->m_actors.push_back(cube);
	m_forwardRenderScene->m_actors.push_back(particle);

	RenderPass forwardRenderPass;
	forwardRenderPass.scene = m_forwardRenderScene;
	forwardRenderPass.parseScene();
	forwardRenderPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourAndDepthLayout);
	forwardRenderPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.0f);
	forwardRenderPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	forwardRenderPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	forwardRenderPass.UBOmanager.setCachedValue("b_camera", "u_view", forwardRenderPass.camera.view);
	forwardRenderPass.UBOmanager.setCachedValue("b_camera", "u_projection", forwardRenderPass.camera.projection);
	forwardRenderPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);

	forwardRenderPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	forwardRenderPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < m_numPointLights; i++)
	{
		bool passed = forwardRenderPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		passed = forwardRenderPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		passed = forwardRenderPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	m_forwardRenderer.addRenderPass(forwardRenderPass);
	m_forwardRenderScene->m_actors.at(0).attachScript<RotationScript>(forwardRenderPass.scene->m_actors.at(0), glm::vec3(0.3f, 0.6f, 0.9f), GLFW_KEY_1);
	//forward render pass setup



	//Mix Forward Render Scene and Deffered Render Scene based off of depth here
	m_screenScene.reset(new Scene);
	std::shared_ptr<Material> FDscreenQuadMaterial;
	SetUpPPMaterial("./assets/shaders/Lab10/FDmixFrag.glsl", FDscreenQuadMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	FDscreenQuadMaterial->setValue("u_forwardTexture", m_forwardRenderer.getRenderPass(0).target->getTarget(0));

	FDscreenQuadMaterial->setValue("u_inputDepthTexture", GPass.target->getTarget(4));
	FDscreenQuadMaterial->setValue("u_forwardDepthTexture", m_forwardRenderer.getRenderPass(0).target->getTarget(1));
	screen.material = FDscreenQuadMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass FMmixScreenPass;
	FMmixScreenPass.scene = m_screenScene;
	FMmixScreenPass.parseScene();
	FMmixScreenPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourAndDepthLayout);
	FMmixScreenPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	FMmixScreenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	FMmixScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_view", FMmixScreenPass.camera.view);
	FMmixScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", FMmixScreenPass.camera.projection);

	m_previousRenderPassIndex++;
	m_mainRenderer.addRenderPass(FMmixScreenPass);
	m_screenScene.reset(new Scene);
	//Forward and Deferred Rendering mix done


	////Linearise and render depth (redundant)
	////Depth Render Pass
	//std::shared_ptr<Material> depthRenderMaterial;
	//SetUpPPMaterial("./assets/shaders/Lab3/RenderDepthFrag.glsl", depthRenderMaterial, shadowPass.target->getTarget(0)); //0 = Colour attatchment, 1 = depth (unless there's more colour attatchments)
	//screen.material = depthRenderMaterial;
	//m_screenScene->m_actors.push_back(screen);
	//
	//RenderPass depthRenderPass;
	//depthRenderPass.scene = m_screenScene;
	//depthRenderPass.parseScene();
	//depthRenderPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourAndDepthLayout); 
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
	fogRenderMaterial->setValue("u_fogType", 4.f);
	screen.material = fogRenderMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass fogRenderPass;
	fogRenderPass.scene = m_screenScene;
	fogRenderPass.parseScene();
	fogRenderPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
	fogRenderPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	fogRenderPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	fogRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_view", fogRenderPass.camera.view);
	fogRenderPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", fogRenderPass.camera.projection);

	m_mainRenderer.addRenderPass(fogRenderPass);
	m_previousRenderPassIndex++;
	m_fogPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Fog Render Pass Setup


	//Edge Detection Pass (Outlines)
	std::shared_ptr<Material> edgeDetectionMaterial;
	SetUpPPMaterial("./assets/shaders/Lab3/EdgeDetectionFrag.glsl", edgeDetectionMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	edgeDetectionMaterial->setValue("u_screenSize", glm::vec2(width, height));
	edgeDetectionMaterial->setValue("u_edgeStrength", m_edgeStrength);
	screen.material = edgeDetectionMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass edgeDetectionPass;
	edgeDetectionPass.scene = m_screenScene;
	edgeDetectionPass.parseScene();
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


	//Tint pass (black == no tint)
	std::shared_ptr<Material> tintMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/TintFrag.glsl", tintMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	screen.material = tintMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass tintPass;
	tintPass.scene = m_screenScene;
	tintPass.parseScene(); 
	tintPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
	tintPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	tintPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_view", tintPass.camera.view);
	tintPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", tintPass.camera.projection);

	m_mainRenderer.addRenderPass(tintPass);
	m_previousRenderPassIndex++;
	m_tintPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Tint Pass setup


	//Sepia Pass
	std::shared_ptr<Material> sepiaMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/SepiaFrag.glsl", sepiaMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	screen.material = sepiaMaterial;
	m_screenScene->m_actors.push_back(screen);
	
	RenderPass sepiaPass;
	sepiaPass.scene = m_screenScene;
	sepiaPass.parseScene(); 
	sepiaPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
	sepiaPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	sepiaPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	sepiaPass.UBOmanager.setCachedValue("b_camera2D", "u_view", sepiaPass.camera.view);
	sepiaPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", sepiaPass.camera.projection);

	m_mainRenderer.addRenderPass(sepiaPass);
	m_previousRenderPassIndex++;
	m_sepiaPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Sepia Pass Setup


	//Blur Pass (for tilt shift and depth of field)
	std::shared_ptr<Material> blurMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/BlurFrag.glsl", blurMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	blurMaterial->setValue("u_screenSize", glm::vec2(width, height));
	blurMaterial->setValue("u_blurRadius", m_blurRadius);
	screen.material = blurMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass blurPass;
	blurPass.scene = m_screenScene;
	blurPass.parseScene();
	blurPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
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
	dofMaterial->setValue("u_blurTexture", m_mainRenderer.getRenderPass(m_blurPassIndex).target->getTarget(0));
	dofMaterial->setValue("u_depthTexture", GPass.target->getTarget(4));
	dofMaterial->setValue("u_focusDistance", 0.9f);
	screen.material = dofMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass dofPass;
	dofPass.scene = m_screenScene;
	dofPass.parseScene(); 
	dofPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
	dofPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	dofPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_view", dofPass.camera.view);
	dofPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", dofPass.camera.projection);

	m_mainRenderer.addRenderPass(dofPass);
	m_previousRenderPassIndex++;
	m_dofPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Depth of Field pass completed


	//Tilt Shift Pass
	SetUpPPMaterial("./assets/shaders/Lab8/tiltShiftFrag.glsl", tiltShiftMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	tiltShiftMaterial->setValue("u_blurTexture", m_mainRenderer.getRenderPass(m_blurPassIndex).target->getTarget(0));
	screen.material = tiltShiftMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass tiltShiftPass;
	tiltShiftPass.scene = m_screenScene;
	tiltShiftPass.parseScene(); 
	tiltShiftPass.target = std::make_shared<FBO>(m_winRef.getSize(), colourLayout); 
	tiltShiftPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	tiltShiftPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	tiltShiftPass.UBOmanager.setCachedValue("b_camera2D", "u_view", tiltShiftPass.camera.view);
	tiltShiftPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", tiltShiftPass.camera.projection);

	m_mainRenderer.addRenderPass(tiltShiftPass);
	m_previousRenderPassIndex++;
	m_tiltShiftPassIndex = m_previousRenderPassIndex;
	m_screenScene.reset(new Scene);
	//Tilt Shift Pass Completed


	//Vignette Pass
	std::shared_ptr<Material> vignetteMaterial;
	SetUpPPMaterial("./assets/shaders/Lab2/VignetteFrag.glsl", vignetteMaterial, m_mainRenderer.getRenderPass(m_previousRenderPassIndex).target->getTarget(0));
	screen.material = vignetteMaterial;
	m_screenScene->m_actors.push_back(screen);

	RenderPass vignettePass;
	vignettePass.scene = m_screenScene;
	vignettePass.parseScene(); 
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
	screenPass.parseScene(); 
	screenPass.target = std::make_shared<FBO>(); //Default FBO, the screenPass is where we draw to the screen
	screenPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };

	screenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_view", screenPass.camera.view);
	screenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection", screenPass.camera.projection);

	m_previousRenderPassIndex++;
	m_mainRenderer.addRenderPass(screenPass);
	//Screen Pass Set up


	//Compute to create height map
	TextureDescription computeNoiseTextureDesc;
	computeNoiseTextureDesc.width = 1024;
	computeNoiseTextureDesc.height = 1024;
	computeNoiseTextureDesc.channels = 4;
	computeNoiseTextureDesc.isHDR = false;
	std::shared_ptr<Texture>heightMapTexture = std::make_shared<Texture>(computeNoiseTextureDesc); //empty texture to write to with compute

	ShaderDescription computeNoiseDesc;
	computeNoiseDesc.type = ShaderType::compute;
	computeNoiseDesc.computeSrcPath = "./assets/shaders/Lab9/compute_noise.glsl";

	std::shared_ptr<Shader> computeNoiseShader = std::make_shared<Shader>(computeNoiseDesc);
	noiseMaterial = std::make_shared<Material>(computeNoiseShader);

	ComputePass computeNoisePass;
	computeNoisePass.material = noiseMaterial;
	computeNoisePass.workgroups = { 32, 32, 1 }; //Workgroup = block containing threads
	computeNoisePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image image;
	image.mipLevel = 0;
	image.layered = false;
	image.texture = heightMapTexture;
	image.imageUnit = computeNoisePass.material->m_shader->m_imageBindingPoints["outputImg"];
	image.access = TextureAccess::WriteOnly;

	computeNoisePass.images.push_back(image);
	m_previousRenderPassIndex++;
	m_mainRenderer.addComputePass(computeNoisePass); //In main renderer rather than init renderer, so you can change noise values during run time
	//Height Map Compute Passs set up


	//Compute Pass, create normal map for height map
	TextureDescription CDMcomputeNoiseTextureDesc;
	CDMcomputeNoiseTextureDesc.width = 1024;
	CDMcomputeNoiseTextureDesc.height = 1024;
	CDMcomputeNoiseTextureDesc.channels = 4;
	CDMcomputeNoiseTextureDesc.isHDR = true;
	std::shared_ptr<Texture>CDMterrainNormalsTexture = std::make_shared<Texture>(CDMcomputeNoiseTextureDesc); //empty texture to write to with compute

	ShaderDescription textureCDMComputeDesc;
	textureCDMComputeDesc.type = ShaderType::compute;
	textureCDMComputeDesc.computeSrcPath = "./assets/shaders/Lab8/compute_CDMnorms.glsl";

	std::shared_ptr<Shader> computeCDMShader = std::make_shared<Shader>(textureCDMComputeDesc);
	std::shared_ptr<Material> computeCDMMaterial = std::make_shared<Material>(computeCDMShader);
	std::shared_ptr<Texture> computeHeightMapIn;
	computeHeightMapIn = std::make_shared<Texture>("./assets/textures/HeightMaps/map2.png", GL_CLAMP_TO_EDGE);
	computeCDMMaterial->setValue("u_heightMap", heightMapTexture);


	ComputePass CDMComputePass;
	CDMComputePass.material = computeCDMMaterial;
	CDMComputePass.workgroups = { 64, 64, 1 }; //Workgroup = block containing threads
	CDMComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image CDM_mapimage;
	CDM_mapimage.mipLevel = 0;
	CDM_mapimage.layered = false;
	CDM_mapimage.texture = CDMterrainNormalsTexture;
	CDM_mapimage.imageUnit = CDMComputePass.material->m_shader->m_imageBindingPoints["outputImg"];
	CDM_mapimage.access = TextureAccess::WriteOnly;

	CDMComputePass.images.push_back(CDM_mapimage);
	m_previousRenderPassIndex++;
	m_mainRenderer.addComputePass(CDMComputePass);
	//CDM ComputePass setup

	floor.material->setValue("u_normalHeightMap", CDMterrainNormalsTexture);
	
	//Compute pass to update particles per frame
	ShaderDescription updateParticlesComputeDesc;
	updateParticlesComputeDesc.type = ShaderType::compute;
	updateParticlesComputeDesc.computeSrcPath = "./assets/shaders/Lab10/compute_updateParticles.glsl";
	std::shared_ptr<Shader> computeUpdateParticlesShader = std::make_shared<Shader>(updateParticlesComputeDesc);

	std::shared_ptr<Material>updateParticlesMaterial = std::make_shared<Material>(computeUpdateParticlesShader);

	ComputePass updateParticlesComputePass;
	updateParticlesComputePass.material = updateParticlesMaterial;
	updateParticlesComputePass.workgroups = { 32, 1, 1 }; //64 particles, workgroup is 8 x 1
	updateParticlesComputePass.barrier = MemoryBarrier::ShaderStorageAccess;
	updateParticlesComputePass.ssbo = particleSSBO;

	m_mainRenderer.addComputePass(updateParticlesComputePass);
	//Update particles pass complete
}

void Lab9::onRender() const
{
	m_forwardRenderer.render();
	m_mainRenderer.render();
}

void Lab9::onUpdate(float timestep)
{
	// per frame uniforms

	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	floorMat->setValue("u_remapRange", m_remapRange);

	//Convert from bool to float, as passing in a bool as uniform causes a crash (same with an int too?)
	if (m_geoNormal) floorMat->setValue("u_geoNormal", 0.0f);
	else floorMat->setValue("u_geoNormal", 1.0f);

	//directional lights
	m_mainScene->m_directionalLights.at(0).direction = glm::normalize(m_lightDirection);
	m_mainRenderer.getRenderPass(3).UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);
	m_forwardRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < m_numPointLights; i++)
	{
		m_mainScene->m_pointLights.at(0).position = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4 + 10.0f, -9.0f);
		
		m_forwardRenderScene->m_actors.at(0).translation = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4 + 10.0f, -9.0f);
		if(m_attachToCube)m_emitterLocation = glm::vec3(cos(glfwGetTime()), sin(glfwGetTime()) * 4 + 10.0f, -9.0f);

		m_forwardRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_particleColour);
		m_forwardRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		m_forwardRenderer.getRenderPass(0).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	if (m_attachToCube) m_speed = 0.2f; //If particle system is attahced to cube position, lock speed as it looks really good with lower speed

	m_mainRenderer.getRenderPass(0).drawInWireFrame = m_wireFrame;

	auto& skyBox = skyBoxOnlyScene->m_actors.at(0);
	skyBox.material->setValue("u_skyBoxView", glm::mat4(glm::mat3(m_mainRenderer.getRenderPass(0).camera.view)));

	//These only show up in the GUI if the pass exists (commented out = not in GUI)
	if (m_tintPassIndex != -1) m_mainRenderer.getRenderPass(m_tintPassIndex).scene->m_actors.at(0).material->setValue("u_tintColour", m_tintColour);
	if (m_blurPassIndex != -1) m_mainRenderer.getRenderPass(m_blurPassIndex).scene->m_actors.at(0).material->setValue("u_blurRadius", m_blurRadius);
	if (m_edgeDetectionPassIndex != -1) m_mainRenderer.getRenderPass(m_edgeDetectionPassIndex).scene->m_actors.at(0).material->setValue("u_edgeStrength", m_edgeStrength);
	if (m_fogPassIndex != -1) m_mainRenderer.getRenderPass(m_fogPassIndex).scene->m_actors.at(0).material->setValue("u_fogType", (float)m_fogType);
	if (m_dofPassIndex != -1)
	{
		m_mainRenderer.getRenderPass(m_dofPassIndex).scene->m_actors.at(0).material->setValue("u_focusDistance", m_focusDistance);
		m_mainRenderer.getRenderPass(m_dofPassIndex).scene->m_actors.at(0).material->setValue("u_active", (float)m_blurType);
	}


	if(m_sepia) m_mainRenderer.getRenderPass(m_sepiaPassIndex).scene->m_actors.at(0).material->setValue("u_active", 1.0f);
	else m_mainRenderer.getRenderPass(m_sepiaPassIndex).scene->m_actors.at(0).material->setValue("u_active", 0.0f);


	//Update scripts
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onUpdate(timestep);
	}

	//Update scripts
	for (auto it = m_forwardRenderScene->m_actors.begin(); it != m_forwardRenderScene->m_actors.end(); ++it)
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

	// Update camera  and its position in UBO
	auto& forwardPass = m_forwardRenderer.getRenderPass(0);
	forwardPass.camera.updateView(camera.transform);
	forwardPass.UBOmanager.setCachedValue("b_camera", "u_view", forwardPass.camera.view);
	forwardPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", camera.translation);

	lightPassMaterial->setValue("u_viewPos", camera.translation);

	glm::vec3 lightPosition = (m_shadowMapVars.centre - m_mainScene->m_directionalLights.at(0).direction) * m_shadowMapVars.distanceAlongLightVector;
	glm::mat4 shadowView = glm::lookAt(lightPosition, m_shadowMapVars.centre, m_shadowMapVars.up);
	m_mainRenderer.getDepthPass(0).camera.view = shadowView;
	m_mainRenderer.getDepthPass(0).UBOmanager.setCachedValue("b_lightCamera", "u_view", m_mainRenderer.getDepthPass(0).camera.view);

	lightPassMaterial->setValue("u_shadowMap", m_mainRenderer.getDepthPass(0).target->getTarget(0));
	lightPassMaterial->setValue("u_lightSpaceTransform", m_mainRenderer.getDepthPass(0).camera.projection * m_mainRenderer.getDepthPass(0).camera.view);

	tiltShiftMaterial->setValue("u_intensity", m_tiltIntensity);

	noiseMaterial->setValue("u_frequency", m_frequency);
	noiseMaterial->setValue("u_amplitude", m_amplitude);
	noiseMaterial->setValue("u_lacunarity", m_lacunarity);
	noiseMaterial->setValue("u_persistence", m_persistence);

	noiseMaterial->setValue("u_noiseType", (float)m_noiseType);
	noiseMaterial->setValue("u_time", (float)glfwGetTime());

	if (m_invertWorley) noiseMaterial->setValue("u_invertWorley", 1.0f);
	else noiseMaterial->setValue("u_invertWorley", 0.0f);

	if (m_animateWorley) noiseMaterial->setValue("u_animateWorley", 1.0f);
	else noiseMaterial->setValue("u_animateWorley", 0.0f);

	//per frame particle uniforms
	m_mainRenderer.getComputePass(3).material->setValue("u_deltaTime", m_deltaTimer.reset());
	m_mainRenderer.getComputePass(3).material->setValue("u_speed", m_speed);
	m_mainRenderer.getComputePass(3).material->setValue("u_emitterLocation", m_emitterLocation);
	m_mainRenderer.getComputePass(3).material->setValue("u_velocityRangeX", m_velocityRangeX);
	m_mainRenderer.getComputePass(3).material->setValue("u_velocityRangeY", m_velocityRangeY);
	m_mainRenderer.getComputePass(3).material->setValue("u_velocityRangeZ", m_velocityRangeZ);

	m_forwardRenderScene->m_actors.at(1).material->setValue("u_particleColour", m_particleColour);
	m_forwardRenderScene->m_actors.at(0).material->setValue("u_albedo", m_particleColour);

}


void Lab9::onImGUIRender()
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
		if (ImGui::BeginTabItem("Read Me"))
		{
			ImGui::TextWrapped("Here's what I have covered with this coursework:");
			ImGui::TextWrapped(" -Skybox");
			ImGui::TextWrapped(" -FrameBuffers (Colour + Depth, post processing)");
			ImGui::TextWrapped(" -Shadow Mapping (with billboards too)");
			ImGui::TextWrapped(" -Deffered Rendering");
			ImGui::TextWrapped(" -Geometry Shaders (billboarding + surface normal option in terrain tab)");
			ImGui::TextWrapped(" -Tesselation Shaders (distance based)");
			ImGui::TextWrapped(" -Compute Shaders (writing noise texture, CDM normal texture for noise texture and particle system)");
			ImGui::TextWrapped(" -SSBO (Particle System)");
			ImGui::NewLine();
			ImGui::TextWrapped("One thing to note, is that I am using a mixture of forward and deffered rendering (cube + particles forward). There is a render pass that mixes the two textures together based off of depth");
			ImGui::NewLine();
			ImGui::TextWrapped("IMPORTANT: Going back to previous layers will not work, as I unfortunately changed some shaders to work with future weeks without creating a new version of them in a new folder, so only the most recent layer will work(sorry for any inconvenience!)");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("G Buffer"))
		{
			ImGui::Text("Position");
			GLuint positionTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(0)->getID();
			ImVec2 imageSize = ImVec2(256, 256);
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)positionTextureID, imageSize, uv0, uv1);

			ImGui::Text("Normals");
			GLuint normalsTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(1)->getID();
			ImGui::Image((void*)(intptr_t)normalsTextureID, imageSize, uv0, uv1);

			ImGui::Text("Albedo");
			GLuint albedoTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(2)->getID();
			ImGui::Image((void*)(intptr_t)albedoTextureID, imageSize, uv0, uv1);

			ImGui::Text("Specular");
			GLuint specularTextureID = m_mainRenderer.getRenderPass(1).target->getTarget(3)->getID();
			ImGui::Image((void*)(intptr_t)specularTextureID, imageSize, uv0, uv1);


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Lighting"))
		{
			ImGui::DragFloat3("Light Direction", (float*)&m_lightDirection, 0.001f, -1.0f, 1.0f);

			ImGui::Text("Shadow Map");
			GLuint textureID = m_mainRenderer.getDepthPass(0).target->getTarget(0)->getID();
			ImVec2 imageSize = ImVec2(512, 512);
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Post Processing"))
		{

			ImGui::Checkbox("Sepia", &m_sepia);
			if (m_tintPassIndex != -1) ImGui::ColorEdit3("Tint Colour (Black = No Tint)", (float*)&m_tintColour);
			if (m_fogPassIndex != -1)
			{
				ImGui::RadioButton("No Fog", &m_fogType, -1);
				ImGui::SameLine();
				ImGui::RadioButton("Linear Fog", &m_fogType, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Exponential Fog", &m_fogType, 1);
				ImGui::SameLine();
				ImGui::RadioButton("Exponential Squared Fog", &m_fogType, 2);
			}
			ImGui::TextWrapped("Outline strength, 1.0 = no effect, 0.0 = maximum effect");
			if (m_edgeDetectionPassIndex != -1)ImGui::DragFloat("Outline Strength", (float*)&m_edgeStrength, 0.002f, 0.f, 1.0f);
			ImGui::RadioButton("Tilt Shift", &m_blurType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Depth Of Field + Tilt Shift", &m_blurType, 1);
			ImGui::TextWrapped("Blur Radius, for tilt shift and depth of field blur level");
			if (m_blurPassIndex != -1)ImGui::DragFloat("Blur Radius", (float*)&m_blurRadius, 0.025f, 0.0f, 5.0f);
			if (m_dofPassIndex != -1 && m_blurType == 1)ImGui::TextWrapped("Focus Distance, what distance depth of field will not blur");
			if (m_dofPassIndex != -1 && m_blurType == 1)ImGui::DragFloat("Focus Distance", (float*)&m_focusDistance, 0.002f, 0.f, 1.0f);
			ImGui::TextWrapped("Rate of change for blur in tilt shift areas (top and bottom of screen");
			ImGui::DragFloat("Tilt Intensity", (float*)&m_tiltIntensity, 0.001f, 0.0f, 0.24f);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Terrain"))
		{
			ImGui::TextWrapped("Remap terrain texture UV's to look good, since tri plannar does not keep UV's in [0,1]");
			ImGui::DragFloat("Texture Remap Range", (float*)&m_remapRange, 0.2f, 0.f, 1000.0f);
			ImGui::Checkbox("Geometry Shader Surface Normals", &m_geoNormal);
			ImGui::NewLine();
			ImGui::TextWrapped("I've added a second floor behond the camera with exaggerated distance based tesselation for demonstration");

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Noise"))
		{
			if (m_noiseType != 5)ImGui::DragFloat("Frequency", (float*)&m_frequency, 0.5f, 0.0f, 200.f);
			ImGui::DragFloat("Amplitude", (float*)&m_amplitude, 0.01f, 0.0f, 200.f);
			if (m_noiseType != 0 && m_noiseType != 5)ImGui::DragFloat("Lacunarity", (float*)&m_lacunarity, 0.02f, 0.0f, 10.f);
			if (m_noiseType != 0 && m_noiseType != 5) ImGui::DragFloat("Persistence", (float*)&m_persistence, 0.02f, 0.0f, 10.0f);

			ImGui::TextWrapped("You will need to turn down the amplitude to see rigidFBM");
			ImGui::RadioButton("Gradient", &m_noiseType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("FBM", &m_noiseType, 1);
			ImGui::SameLine();
			ImGui::RadioButton("RigidFBM", &m_noiseType, 2);
			ImGui::SameLine();
			ImGui::RadioButton("TurbulentFBM", &m_noiseType, 3);
			ImGui::SameLine();
			ImGui::RadioButton("Combined", &m_noiseType, 4);
			ImGui::SameLine();
			ImGui::RadioButton("Worley", &m_noiseType, 5);

			if(m_noiseType == 5) ImGui::Checkbox("Invert", &m_invertWorley);
			if (m_noiseType == 5) ImGui::Checkbox("Animate", &m_animateWorley);
			
			ImGui::TextWrapped("Height Map (Noise)");
			GLuint textureID = m_mainRenderer.getComputePass(0).images[0].texture->getID();
			ImVec2 imageSize = ImVec2(512, 512);
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

			ImGui::TextWrapped("Normal Map (CDM)");
			textureID = m_mainRenderer.getComputePass(2).images[0].texture->getID();
			imageSize = ImVec2(512, 512);
			uv0 = ImVec2(0.0f, 1.0f);
	        uv1 = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Particle System"))
		{
			ImGui::DragFloat("Speed", (float*)&m_speed, 0.01f, 0.0f, 5.0f);
			ImGui::TextWrapped("Cube Trail attaches particle system to cube and reduces speed");
			ImGui::Checkbox("Cube Trail", &m_attachToCube);
			ImGui::ColorEdit3("Particle Colour", (float*)&m_particleColour);
			if (!m_attachToCube)ImGui::DragFloat3("Emitter Location", (float*)&m_emitterLocation, 0.001f);

			ImGui::TextWrapped("Remap Velocity to range");
			ImGui::DragFloat2("Velocity Range X", (float*)&m_velocityRangeX, 0.05f, -1.0f, 1.0f);
			ImGui::DragFloat2("Velocity Range Y", (float*)&m_velocityRangeY, 0.05f, -1.0f, 1.0f);
			ImGui::DragFloat2("Velocity Range Z", (float*)&m_velocityRangeZ, 0.05f, -1.0f, 1.0f);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::Render();
}

//Screate shader with vertex shader for quad and set input texture, this code is repeated alot so just use this instead
void Lab9::SetUpPPMaterial(std::string fragPath, std::shared_ptr<Material>& mat, std::shared_ptr<Texture> inputTex)
{
	ShaderDescription shaderDesc;
	shaderDesc.type = ShaderType::rasterization;
	shaderDesc.vertexSrcPath = "./assets/shaders/Lab2/ScreenVert.glsl";
	shaderDesc.fragmentSrcPath = fragPath;
	std::shared_ptr<Shader> shader;
	shader = std::make_shared<Shader>(shaderDesc);

	mat = std::make_shared<Material>(shader);
	mat->setValue("u_inputTexture", inputTex); //simply reads from the texture, which is the FBO we set the main pass to draw to
}

void Lab9::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}