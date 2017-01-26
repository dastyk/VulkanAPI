#include "VertexBufferGL.h"
#include "MeshGL.h"
#include <assert.h>

GLuint VertexBufferGL::usageMapping[3] = { GL_STATIC_COPY, GL_DYNAMIC_COPY, GL_DONT_CARE };

VertexBufferGL::VertexBufferGL() : _handle(0)
{
};

VertexBufferGL::~VertexBufferGL()
{
	glDeleteBuffers(1, &_handle);
}

void VertexBufferGL::setData(const void* data, size_t size, DATA_USAGE usage )
{
	totalSize = size;
	// delete if memory exists
	if (_handle > 0) {
		glDeleteBuffers(1, &_handle);
	}
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usageMapping[usage]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	_handle = newSSBO;
}

/*
 bind at "location", with offset "offset", "size" bytes 
 */
void VertexBufferGL::bind(size_t offset, size_t size, unsigned int location) {
	assert(offset + size <= totalSize);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _handle);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, location, _handle, offset, size);
}

void VertexBufferGL::unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

inline size_t VertexBufferGL::getSize() {
	return totalSize;
}
