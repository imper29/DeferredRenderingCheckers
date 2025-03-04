#include "SysCameraFreemode.h"
#include "InputManager.h"

#include "CmpCameraFreecam.h"
#include "CmpPosition.h"
#include "CmpRotation.h"
#include "CmpTransformLocalToWorld.h"

static float2 euler = float2(0.0f);

SysCameraFreemode::SysCameraFreemode(Scene& scene) : SceneSystem(scene) {

}

void SysCameraFreemode::OnEvtReceived(SDL_Event& evt) {
	if (evt.type == SDL_EventType::SDL_MOUSEWHEEL) {
		auto entity = scene.entities.First();
		while (entity.index) {
			auto camera = scene.entities.GetComponent<CmpCameraFreecam>(entity);
			if (camera) {
				camera->speedLinear += camera->speedLinear * evt.wheel.preciseY * 0.1f;
				camera->speedAngular += camera->speedAngular * evt.wheel.preciseX * 0.1f;
			}
			entity = scene.entities.Next(entity);
		}
	}
}
void SysCameraFreemode::OnEvtReceived(Scene::EvtUpdate& evt) {
	auto entity = scene.entities.First();
	while (entity.index) {
		auto camera = scene.entities.GetComponent<CmpCameraFreecam>(entity);
		auto position = scene.entities.GetComponent<CmpPosition>(entity);
		auto rotation = scene.entities.GetComponent<CmpRotation>(entity);
		auto localToWorld = scene.entities.GetComponent<CmpTransformLocalToWorld>(entity);
		if (camera && position && rotation && localToWorld) {
			//Linear controls.
			auto& keyboard = InputManager::GetKeyboard(0);
			if ((keyboard.GetMods() & SDL_Keymod::KMOD_CTRL) == 0) {
				if (keyboard.IsDown(SDL_KeyCode::SDLK_d))
					position->value = position->value + localToWorld->value * float4(1, 0, 0, 0) * camera->speedLinear * evt.timestep;
				if (keyboard.IsDown(SDL_KeyCode::SDLK_a))
					position->value = position->value - localToWorld->value * float4(1, 0, 0, 0) * camera->speedLinear * evt.timestep;
				if (keyboard.IsDown(SDL_KeyCode::SDLK_s))
					position->value = position->value + localToWorld->value * float4(0, 0, 1, 0) * camera->speedLinear * evt.timestep;
				if (keyboard.IsDown(SDL_KeyCode::SDLK_w))
					position->value = position->value - localToWorld->value * float4(0, 0, 1, 0) * camera->speedLinear * evt.timestep;
				if (keyboard.IsDown(SDL_KeyCode::SDLK_LSHIFT))
					position->value = position->value - localToWorld->value * float4(0, 1, 0, 0) * camera->speedLinear * evt.timestep;
				if (keyboard.IsDown(SDL_KeyCode::SDLK_SPACE))
					position->value = position->value + localToWorld->value * float4(0, 1, 0, 0) * camera->speedLinear * evt.timestep;
			}
			//Angular controls.
			auto& mouse = InputManager::GetMouse(0);
			if (mouse.GetButtons() & SDL_BUTTON_RMASK) {
				euler.x -= mouse.GetMousePosDeltaY();
				euler.y -= mouse.GetMousePosDeltaX();
				quaternion rot = quaternion::fromEulerAngle(euler * camera->speedAngular * evt.timestep * math::DegToRad<float>);
				rotation->value = rot;
			}
		}
		entity = scene.entities.Next(entity);
	}
}
