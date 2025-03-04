#include "SysRenderingMeshRenderers.h"
#include "CmpTransformLocalToWorld.h"
#include "CmpMeshRenderer.h"

SysRenderingMeshRenderers::SysRenderingMeshRenderers(Scene& scene) : SceneSystem(scene) {

}

void SysRenderingMeshRenderers::OnEvtReceived(SysRenderingPipeline::EvtRenderPre& evt) {
	//TODO: Construct acceleration structure to support instanced rendering and frustum culling.
}
void SysRenderingMeshRenderers::OnEvtReceived(SysRenderingPipeline::EvtRenderShadows& evt) {
	//TODO: Render shadow casting geometry.
}
void SysRenderingMeshRenderers::OnEvtReceived(SysRenderingPipeline::EvtRenderGeometryOpaque& evt) {
	auto entity = scene.entities.First();
	while (entity.index) {
		auto renderer = scene.entities.GetComponent<CmpMeshRendererOpaque>(entity);
		auto localToWorld = scene.entities.GetComponent<CmpTransformLocalToWorld>(entity);
		if (renderer && localToWorld) {
			if (renderer->layers & evt.layers) {
				if (evt.frustum.contains(localToWorld->value.getTranslation(), renderer->mesh->GetRadius())) {
					auto tess = renderer->material->Use(evt.projection, evt.view, localToWorld->value);
					renderer->mesh->Draw(tess);
				}
			}
		}
		entity = scene.entities.Next(entity);
	}
}
void SysRenderingMeshRenderers::OnEvtReceived(SysRenderingPipeline::EvtRenderGeometryTransparent& evt) {
	//TODO: Add support for lighting to transparent geometry.
	auto entity = scene.entities.First();
	while (entity.index) {
		auto renderer = scene.entities.GetComponent<CmpMeshRendererTransparent>(entity);
		auto localToWorld = scene.entities.GetComponent<CmpTransformLocalToWorld>(entity);
		if (renderer && localToWorld) {
			if (renderer->layers & evt.layers) {
				if (evt.frustum.contains(localToWorld->value.getTranslation(), renderer->mesh->GetRadius())) {
					auto tess = renderer->material->Use(evt.projection, evt.view, localToWorld->value);
					renderer->mesh->Draw(tess);
				}
			}
		}
		entity = scene.entities.Next(entity);
	}
}
