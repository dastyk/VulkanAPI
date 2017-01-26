#pragma once
#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class TransformGL :
	public Transform
{
public:
	TransformGL();
	~TransformGL();
	void translate(float x, float y, float z)
	{
		transform[3][0] += x;
		transform[3][1] += y;
		transform[3][2] += z;
	}

	void rotate(float radians, float x, float y, float z)
	{
		transform = glm::rotate(transform, radians, glm::vec3(x, y, z));
	}

	glm::mat4 transform;
};

