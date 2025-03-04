#pragma once
#include <memory>
#include "float4x4.h"
#include "Framebuffer.h"
#include "CmpTransformLocalToWorld.h"
#include "EntityComponent.h"
#include "EntityRegistry.h"

struct CmpCamera : public EntityComponent {
	std::shared_ptr<Framebuffer> framebuffer;
	float clipPlaneNear, clipPlaneFar;
	float fieldOfView;
	bool orthographic;

	uint64_t layers;
	int64_t order;

	CmpCamera(float clipPlaneNear, float clipPlaneFar, float fieldOfView, bool orthographic = false, uint64_t layers = ~0U, int64_t order = 0, std::shared_ptr<Framebuffer> framebuffer = nullptr)
		: clipPlaneNear(clipPlaneNear), clipPlaneFar(clipPlaneFar)
		, fieldOfView(fieldOfView), orthographic(orthographic)
		, layers(layers), order(order), framebuffer(framebuffer) {}

	/// <summary>
	/// Returns the framebuffer for the given camera.
	/// </summary>
	/// <param name="registry">The entity registry.</param>
	/// <param name="entity">The camera entity.</param>
	/// <returns>The framebuffer for the given camera.</returns>
	static std::shared_ptr<Framebuffer> GetFramebuffer(EntityRegistry& registry, EntityId entity) {
		CmpCamera* cam = registry.GetComponent<CmpCamera>(entity);
		if (cam) {
			return cam->framebuffer;
		}
		else {
			throw "The entity does not have a camera component.";
		}
	}
	/// <summary>
	/// Returns the projection matrix for the given camera.
	/// </summary>
	/// <param name="registry">The entity registry.</param>
	/// <param name="entity">The camera entity.</param>
	/// <returns>The projection matrix for the given camera.</returns>
	static float4x4 GetProjection(EntityRegistry& registry, EntityId entity, float width, float height) {
		CmpCamera* cam = registry.GetComponent<CmpCamera>(entity);
		if (cam) {
			if (cam->orthographic)
				return float4x4::orthographic(width / height, cam->fieldOfView, cam->clipPlaneNear, cam->clipPlaneFar);
			else
				return float4x4::perspective(width / height, cam->fieldOfView, cam->clipPlaneNear, cam->clipPlaneFar);
		}
		else {
			throw "The entity does not have a camera component.";
		}
	}
	/// <summary>
	/// Returns the view matrix for the given camera.
	/// </summary>
	/// <param name="registry">The entity registry.</param>
	/// <param name="entity">The camera entity.</param>
	/// <returns>The view matrix for the given camera.</returns>
	static float4x4 GetView(EntityRegistry& registry, EntityId entity) {
		CmpCamera* cam = registry.GetComponent<CmpCamera>(entity);
		if (cam) {
			CmpTransformLocalToWorld* ltw = registry.GetComponent<CmpTransformLocalToWorld>(entity);
			if (ltw) {
				return ltw->value.inverse();
			}
			else {
				throw "The entity does not have a transform local to world component.";
			}
		}
		else {
			throw "The entity does not have a camera component.";
		}
	}
	/// <summary>
	/// Returns the rendering layers for the given camera.
	/// </summary>
	/// <param name="registry">The entity registry.</param>
	/// <param name="entity">The camera entity.</param>
	/// <returns>The rendering layers for the given camera.</returns>
	static uint64_t GetLayers(EntityRegistry& registry, EntityId entity) {
		CmpCamera* cam = registry.GetComponent<CmpCamera>(entity);
		if (cam) {
			return cam->layers;
		}
		else {
			throw "The entity does not have a camera component.";
		}
	}
};
