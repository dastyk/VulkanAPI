#pragma once
#include "ConstantBuffer.h"

#include <string>

class ConstantBufferVk /*: public ConstantBuffer*/
{
public:
	ConstantBufferVk(std::string name, unsigned int location);
	~ConstantBufferVk();
};