#pragma once
#include "RenderState.h"
class VulkanRenderState :
	public RenderState
{
public:
	VulkanRenderState();
	~VulkanRenderState();

	bool isWireFrame() { return _wireframe; }

	// Overrides
	void setWireFrame(bool);
	void set();

private:
	bool _wireframe = false;
};

