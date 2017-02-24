#pragma once
#include <vector>
#include "ResourceBinding.h"
#include "Material.h"
#include "RenderState.h"

class Renderer;

class Technique
{
public:
	Technique(Material* m, RenderState* r) : material(m), renderState(r) {};
	virtual ~Technique();

	Material* getMaterial() { return material; };
	RenderState* getRenderState() { return renderState; };

	//void setBindings(std::vector<ResourceBinding*>* rb);
	virtual void enable(Renderer* renderer);

protected:
	Material* material = nullptr;
	RenderState* renderState = nullptr;

	// zero size vector
	std::vector<ResourceBinding*> bindings;
};

