#pragma once
#include "Sampler2D.h"
class VulkanSampler2D : public Sampler2D
{
public:
	VulkanSampler2D();
	virtual ~VulkanSampler2D();
	virtual void setMagFilter(FILTER filter);
	virtual void setMinFilter(FILTER filter);
	virtual void setWrap(WRAPPING s, WRAPPING t);
private:

};

