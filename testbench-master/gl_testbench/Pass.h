#pragma once
#include <memory>
#include "Material.h"

class Pass
{
public:
	Pass();
	~Pass();
	std::shared_ptr<Material> material = nullptr;
};

