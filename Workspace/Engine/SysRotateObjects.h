#pragma once
#include "Scene.h"
#include "SceneSystem.h"
#include "SceneSystemListener.h"

class SysRotateObjects : public SceneSystem, public SceneSystemListener<Scene::EvtUpdate> {
public:
	SysRotateObjects(Scene& scene);

	void OnEvtReceived(Scene::EvtUpdate& evt) override;
};
