#include "SysRotateObjects.h"
#include "CmpRotation.h"
#include "CmpRotationVelocity.h"
#include "quaternion.h"

SysRotateObjects::SysRotateObjects(Scene& scene) : SceneSystem(scene) {

}
void SysRotateObjects::OnEvtReceived(Scene::EvtUpdate& evt) {
	auto entity = scene.entities.First();
	while (entity.index) {
		auto rotation = scene.entities.GetComponent<CmpRotation>(entity);
		auto velocity = scene.entities.GetComponent<CmpRotationVelocity>(entity);
		if (rotation && velocity) {
			rotation->value = rotation->value * quaternion::fromEulerAngle(velocity->rotation * evt.timestep);
		}
		entity = scene.entities.Next(entity);
	}
}
