#include "OpenGLRenderer.h"
#include "Renderer.h"
#include "VulkanRenderer.h"

Renderer* Renderer::makeRenderer(BACKEND option)
{
	if (option == BACKEND::GL45)
		return new OpenGLRenderer();
	else if (option == BACKEND::VULKAN)
		return new VulkanRenderer();
}

	
	/*
	int newId = 0;
	while (techniques.count(newId) != 0)
		newId++;
	techniques[newId] = new Technique();

	return techniques[newId];
}

Technique* Renderer::getTechnique(unsigned int techniqueId) {
	if (techniques.count(techniqueId) == 1)
		return techniques[techniqueId];
	else
		return nullptr;
}
*/

