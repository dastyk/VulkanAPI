#pragma once
#include <GL/glew.h>
#include "VertexBuffer.h"

class VertexBufferGL :
	public VertexBuffer
{
public:
	
	VertexBufferGL();
	~VertexBufferGL();
	
	void setData(const void* data, size_t size, DATA_USAGE usage );
	void bind(size_t offset, size_t size, unsigned int location);
	void unbind();
	size_t getSize();

	static GLuint usageMapping[3];

private:
	size_t totalSize;
	GLuint _handle;
};

