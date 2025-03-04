#include "SceneEnginesCheckers.h"

#include "Texture2D.h"
#include "SysTransformCalculator.h"
#include "TransformComponents.h"

#include "SysCameraFreemode.h"
#include "SysRenderingPipeline.h"
#include "SysRenderingMeshRenderers.h"
#include "SysRenderingLighting.h"
#include "SysRenderingSkybox.h"
#include "SysRotateObjects.h"

#include "TextureCube.h"
#include "MaterialPbr.h"
#include "CmpCamera.h"
#include "CmpCameraFreecam.h"
#include "CmpLightPoint.h"
#include "CmpLightAmbient.h"
#include "CmpLightDirectional.h"
#include "CmpMeshRenderer.h"
#include "CmpRotationVelocity.h"

SceneEnginesCheckers::SceneEnginesCheckers() {
	//Register systems.
	GetOrAddSystem<SysTransformCalculator>();

	GetOrAddSystem<SysCameraFreemode>();
	GetOrAddSystem<SysRenderingPipeline>();
	GetOrAddSystem<SysRenderingMeshRenderers>();
	GetOrAddSystem<SysRenderingLighting>();
	GetOrAddSystem<SysRenderingSkybox>();
	GetOrAddSystem<SysRotateObjects>();

	//Load meshes.
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>("Assets/meshes/cubeChamfered.obj");
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>("Assets/meshes/sphere.obj");
	std::shared_ptr<Mesh> checkerPieceMesh = std::make_shared<Mesh>("Assets/meshes/CheckerPiece.obj");
	std::shared_ptr<Mesh> checkerBoardMesh = std::make_shared<Mesh>("Assets/meshes/plane_100x100.obj");
	//Load shaders.
	std::shared_ptr<Shader> opaquePbr = std::make_shared<Shader>(std::initializer_list{
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_vert.glsl", ShaderStage::Vert),
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_frag.glsl", ShaderStage::Frag) });
	std::shared_ptr<Shader> opaquePbrTess = std::make_shared<Shader>(std::initializer_list{
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_tess_vert.glsl", ShaderStage::Vert),
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_tess_frag.glsl", ShaderStage::Frag),
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_tess_tess_ctrl.glsl", ShaderStage::TessCtrl),
		ShaderSource::FromFile("Assets/shaders/opaque_pbr_tess_tess_eval.glsl", ShaderStage::TessEval) });
	std::shared_ptr<Shader> opaqueUnlitReflectRefract = std::make_shared<Shader>(std::initializer_list{
		ShaderSource::FromFile("Assets/shaders/opaque_unlit_reflect_refract_vert.glsl", ShaderStage::Vert),
		ShaderSource::FromFile("Assets/shaders/opaque_unlit_reflect_refract_frag.glsl", ShaderStage::Frag) });
	std::shared_ptr<Shader> transparentUnlitReflect = std::make_shared<Shader>(std::initializer_list{
		ShaderSource::FromFile("Assets/shaders/transparent_unlit_reflect_vert.glsl", ShaderStage::Vert),
		ShaderSource::FromFile("Assets/shaders/transparent_unlit_reflect_frag.glsl", ShaderStage::Frag) });

	//Load materials.
	std::shared_ptr<Texture> skyboxTexture;
	{
		skyboxTexture = std::make_shared<TextureCube>(
			Bitmap("Assets/textures/torontoSkyBox/negx.jpg"),
			Bitmap("Assets/textures/torontoSkyBox/posx.jpg"),
			Bitmap("Assets/textures/torontoSkyBox/negy.jpg"),
			Bitmap("Assets/textures/torontoSkyBox/posy.jpg"),
			Bitmap("Assets/textures/torontoSkyBox/negz.jpg"),
			Bitmap("Assets/textures/torontoSkyBox/posz.jpg"));
		if (HasSystem<SysRenderingSkybox>()) {
			auto skyboxShader = std::make_shared<Shader>(std::initializer_list{
				ShaderSource::FromFile("Assets/shaders/skyboxes/skybox_frag.glsl", ShaderStage::Frag),
				ShaderSource::FromFile("Assets/shaders/skyboxes/skybox_vert.glsl", ShaderStage::Vert) });
			auto& skyboxRenderer = GetSystem<SysRenderingSkybox>();
			skyboxRenderer.SetSkyboxTexture(skyboxTexture);
			skyboxRenderer.SetSkyboxShader(skyboxShader);
		}
	}
	std::shared_ptr<Material> boardMaterial;
	{
		Bitmap roughness = Bitmap("Assets/textures/checkers/board_roughness.png");
		Bitmap metalness = Bitmap("Assets/textures/checkers/board_metalness.png");
		Bitmap orm = Bitmap(roughness.width, roughness.height);
		orm.evaluate([&roughness, &metalness](const int2& pos) { return Color32(255, roughness[pos].r, metalness[pos].r, 255); });
		boardMaterial = std::make_shared<MaterialPbr>(opaquePbr,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/checkers/board_albedo.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/checkers/board_emission.png")),
			std::make_shared<Texture2D>(orm),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			skyboxTexture,
			1.0f);
	}
	std::shared_ptr<Material> dirtMaterial;
	{
		Bitmap ambientOcclusion = Bitmap("Assets/textures/megascan_forest/pftlS0_8K_AO.png");
		Bitmap roughness = Bitmap("Assets/textures/megascan_forest/pftlS0_8K_AO.png");
		Bitmap orm = Bitmap(ambientOcclusion.width, ambientOcclusion.height);
		orm.evaluate([&ambientOcclusion, &roughness](const int2& pos) { return Color32(ambientOcclusion[pos].r, roughness[pos].r, 0, 255); });
		dirtMaterial = std::make_shared<MaterialPbr>(opaquePbrTess,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/megascan_forest/pftlS0_8K_Albedo.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/megascan_forest/pftlS0_8K_Normal.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/black.png")),
			std::make_shared<Texture2D>(orm),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/megascan_forest/pftlS0_8K_Displacement.png")),
			skyboxTexture,
			0.2f);
	}
	std::shared_ptr<Material> whiteMaterial;
	{
		Bitmap orm = Bitmap(1, 1);
		orm.evaluate([](const int2& pos) { return Color32((uint8_t)255, 255 - 32, 0, 255); });
		whiteMaterial = std::make_shared<MaterialPbr>(opaquePbr,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/white.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/black.png")),
			std::make_shared<Texture2D>(orm),
			nullptr,
			skyboxTexture,
			0.0f);
	}
	std::shared_ptr<Material> mirrorMaterial;
	{
		Bitmap orm = Bitmap(1, 1);
		orm.evaluate([](const int2& pos) { return Color32((uint8_t)255, 0, 255, 255); });
		mirrorMaterial = std::make_shared<MaterialPbr>(opaquePbr,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/white.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/black.png")),
			std::make_shared<Texture2D>(orm),
			nullptr,
			skyboxTexture,
			0.0f);
	}
	std::shared_ptr<Material> reflectRefractMaterial;
	{
		Bitmap orm = Bitmap(1, 1);
		orm.evaluate([](const int2& pos) { return Color32((uint8_t)255, 0, 255, 255); });
		reflectRefractMaterial = std::make_shared<MaterialPbr>(opaqueUnlitReflectRefract,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/white.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			nullptr,
			std::make_shared<Texture2D>(orm),
			nullptr,
			skyboxTexture,
			0.0f);
	}
	std::shared_ptr<Material> transparentReflectMaterial;
	{
		Bitmap orm = Bitmap(1, 1);
		orm.evaluate([](const int2& pos) { return Color32((uint8_t)255, 0, 255, 255); });
		transparentReflectMaterial = std::make_shared<MaterialPbr>(transparentUnlitReflect,
			std::make_shared<Texture2D>(Bitmap("Assets/textures/white.png")),
			std::make_shared<Texture2D>(Bitmap("Assets/textures/flat.png")),
			nullptr,
			std::make_shared<Texture2D>(orm),
			nullptr,
			skyboxTexture,
			0.0f);
	}

	//Create camera.
	if (true) {
		auto camera = entities.Create();
		entities.GetOrAddComponent<CmpTransformLocalToWorld>(camera);
		entities.GetOrAddComponent<CmpCamera>(camera, 1.0f, 100.0f, 45.0f * math::DegToRad<>, false, 1, 1);
		entities.GetOrAddComponent<CmpCameraFreecam>(camera);
		entities.GetOrAddComponent<CmpRotation>(camera, quaternion::fromEulerAngle(float3(-50, 160, 0)* math::DegToRad<float>));
		entities.GetOrAddComponent<CmpPosition>(camera, float3(9, 15, -22));
	}
	//Create board.
	EntityId board{};
	if (true) {
		board = entities.Create();
		entities.GetOrAddComponent<CmpMeshRendererOpaque>(board, boardMaterial, checkerBoardMesh);
		entities.GetOrAddComponent<CmpTransformLocalToWorld>(board);
		entities.GetOrAddComponent<CmpRotationVelocity>(board, float3(0.0f, 30.0f, 0.0f)* math::DegToRad<float>);
		entities.GetOrAddComponent<CmpScale>(board, 16.0f);
		entities.GetOrAddComponent<CmpRotation>(board);
	}
	//Create white pieces.
	if (true) {
		for (size_t i = 0; i < 4; ++i) {
			auto a = entities.Create();
			auto b = entities.Create();
			auto c = entities.Create();

			entities.GetOrAddComponent<CmpMeshRendererOpaque>(a, whiteMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(b, whiteMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(c, whiteMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(a);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(b);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(c);
			entities.GetOrAddComponent<CmpParent>(a, board);
			entities.GetOrAddComponent<CmpParent>(b, board);
			entities.GetOrAddComponent<CmpParent>(c, board);
			entities.GetOrAddComponent<CmpScale>(a, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpScale>(b, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpScale>(c, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpRotation>(a, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpRotation>(b, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpRotation>(c, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpPosition>(a, float3((i * 2 - 3.5f) / 4.0f, 1.0f / 32.0f, 3.5f / 4.0f));
			entities.GetOrAddComponent<CmpPosition>(b, float3((i * 2 - 2.5f) / 4.0f, 1.0f / 32.0f, 2.5f / 4.0f));
			entities.GetOrAddComponent<CmpPosition>(c, float3((i * 2 - 3.5f) / 4.0f, 1.0f / 32.0f, 1.5f / 4.0f));
		}
	}
	//Create black pieces.
	if (true) {
		for (size_t i = 0; i < 4; ++i) {
			auto a = entities.Create();
			auto b = entities.Create();
			auto c = entities.Create();

			entities.GetOrAddComponent<CmpMeshRendererTransparent>(a, transparentReflectMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpMeshRendererTransparent>(b, transparentReflectMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpMeshRendererTransparent>(c, transparentReflectMaterial, checkerPieceMesh);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(a);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(b);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(c);
			entities.GetOrAddComponent<CmpParent>(a, board);
			entities.GetOrAddComponent<CmpParent>(b, board);
			entities.GetOrAddComponent<CmpParent>(c, board);
			entities.GetOrAddComponent<CmpScale>(a, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpScale>(b, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpScale>(c, float3(1.0f / 32.0f));
			entities.GetOrAddComponent<CmpRotation>(a, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpRotation>(b, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpRotation>(c, quaternion::fromAxisAngle(float3(1, 0, 0), -90.0f * math::DegToRad<float>));
			entities.GetOrAddComponent<CmpPosition>(a, float3((i * 2 - 2.5f) / 4.0f, 1.0f / 32.0f, -3.5f / 4.0f));
			entities.GetOrAddComponent<CmpPosition>(b, float3((i * 2 - 3.5f) / 4.0f, 1.0f / 32.0f, -2.5f / 4.0f));
			entities.GetOrAddComponent<CmpPosition>(c, float3((i * 2 - 2.5f) / 4.0f, 1.0f / 32.0f, -1.5f / 4.0f));
		}
	}
	//Create mirror cubes.
	if (false) {
		for (size_t i = 0; i < 10; ++i) {
			auto cube = entities.Create();
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(cube, mirrorMaterial, cubeMesh);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(cube);
			entities.GetOrAddComponent<CmpPosition>(cube, math::random(float3(-15.0f, -5.0f, -15.0f), float3(10.0f)));
			entities.GetOrAddComponent<CmpRotation>(cube, quaternion::fromAxisAngle(math::random<float3>(-1.0f, 1.0f).normalized(), math::random(0.0f, 360.0f) * math::DegToRad<>));
		}
	}
	//Create dirt spheres.
	if (false) {
		for (size_t i = 0; i < 10; ++i) {
			auto cube = entities.Create();
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(cube, dirtMaterial, sphereMesh);
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(cube);
			entities.GetOrAddComponent<CmpPosition>(cube, math::random(float3(-25, -4, -25), float3(25, 12, 25)));
			entities.GetOrAddComponent<CmpRotation>(cube, quaternion::fromAxisAngle(math::random<float3>(-1.0f, 1.0f).normalized(), math::random(0.0f, 360.0f) * math::DegToRad<>));
		}
	}
	//Create reflection and refraction cubes and spheres.
	if (false) {
		for (size_t i = 0; i < 10; ++i) {
			auto cube = entities.Create();
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(cube, CmpMeshRendererOpaque(reflectRefractMaterial, math::random(0.0f, 1.0f) >= 0.5f ? cubeMesh : checkerPieceMesh));
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(cube);
			entities.GetOrAddComponent<CmpPosition>(cube, math::random(float3(-15.0f, -5.0f, -15.0f), float3(10.0f)));
			entities.GetOrAddComponent<CmpRotation>(cube, quaternion::fromAxisAngle(math::random<float3>(-1.0f, 1.0f).normalized(), math::random(0.0f, 360.0f) * math::DegToRad<>));
		}
	}
	//Create transparent cubes and spheres.
	if (false) {
		for (size_t i = 0; i < 10; ++i) {
			auto cube = entities.Create();
			entities.GetOrAddComponent<CmpMeshRendererTransparent>(cube, CmpMeshRendererTransparent(transparentReflectMaterial, math::random(0.0f, 1.0f) >= 0.5f ? cubeMesh : checkerPieceMesh));
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(cube);
			entities.GetOrAddComponent<CmpPosition>(cube, math::random(float3(-15.0f, -5.0f, -15.0f), float3(10.0f)));
			entities.GetOrAddComponent<CmpRotation>(cube, quaternion::fromAxisAngle(math::random<float3>(-1.0f, 1.0f).normalized(), math::random(0.0f, 360.0f) * math::DegToRad<>));
		}
	}
	//Create ambient light.
	if (false) {
		auto light = entities.Create();
		entities.GetOrAddComponent<CmpLightAmbient>(light, float3(0.05f, 0.05f, 0.05f));
	}
	//Create directional light.
	if (true) {
		auto light = entities.Create();
		entities.GetOrAddComponent<CmpLightDirectional>(light, float3(238, 175, 97) / 255.0f);
		entities.GetOrAddComponent<CmpTransformLocalToWorld>(light, float4x4::rotate(quaternion::fromAxisAngle(float3(1.0f, 0.0f, 0.0f), -30.0f * math::DegToRad<>)));
	}
	//Create point lights
	if (true) {
		for (size_t i = 0; i < 3; ++i) {
			auto light = entities.Create();
			entities.GetOrAddComponent<CmpMeshRendererOpaque>(light, whiteMaterial, sphereMesh);
			entities.GetOrAddComponent<CmpLightPoint>(light, math::random<float3>(0.3f, 1.0f), float2(4, 25));
			entities.GetOrAddComponent<CmpTransformLocalToWorld>(light);
			entities.GetOrAddComponent<CmpPosition>(light, math::random(float3(-16, 4.0f, -16), float3(16, 8, 16)));
			entities.GetOrAddComponent<CmpScale>(light, 0.3f);
		}
	}
}
