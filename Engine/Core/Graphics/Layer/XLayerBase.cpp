#include "common_pch.h"
#include "XLayerBase.h"

#include "Core/Utils/Logger.h"

XLayerBase::XLayerBase(ELayerType LayerType)
	: mLayerType(LayerType) {}

XLayerBase::~XLayerBase() = default;

void XLayerBase::Render()
{
	for (auto& object : mRenderObjects)
	{
		object->PreRender();

		object->Render();

		object->PostRender();
	}
}

void XLayerBase::AddLayerObject(IRenderable* RenderableObj)
{
	if (RenderableObj->GetLayerType() == mLayerType)
	{
		mRenderObjects.emplace_back(RenderableObj);
	}
	else
	{
		LOG_CORE_ERROR("ItemType and LayerType MisMatch... %s", __FUNCTION__);
	}
}

void XLayerBase::DeleteLayerObject(IRenderable* RenderableObj)
{
	auto it = std::ranges::find(
								mRenderObjects, RenderableObj);
	if (it != mRenderObjects.end())
	{
		mRenderObjects.erase(it);
	}
}
