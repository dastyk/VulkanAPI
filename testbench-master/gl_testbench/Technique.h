#pragma once
#include <vector>
#include "ResourceBinding.h"
#include "Material.h"
#include "RenderState.h"

class Renderer;

class Technique
{
public:
	Technique();
	~Technique();
	//void setBindings(std::vector<ResourceBinding*>* rb);

	void enable(Renderer* renderer);

	Material* material = nullptr;
	RenderState* renderState = nullptr;

	// zero size vector
	std::vector<ResourceBinding*> bindings;
};

