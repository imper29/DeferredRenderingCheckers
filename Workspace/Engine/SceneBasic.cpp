#include "SceneBasic.h"
#include "SysTransformCalculator.h"

#include "SysCameraFreemode.h"
#include "SysRenderingPipeline.h"
#include "SysRenderingMeshRenderers.h"
#include "SysRenderingLighting.h"

SceneBasic::SceneBasic() {
	GetOrAddSystem<SysTransformCalculator>();

	GetOrAddSystem<SysCameraFreemode>();
	GetOrAddSystem<SysRenderingPipeline>();
	GetOrAddSystem<SysRenderingMeshRenderers>();
	//GetOrAddSystem<SysRenderingLighting>();
}
