#include <GL/glew.h>
#include "RenderStateGL.h"

RenderStateGL::RenderStateGL()
{
	_wireframe = false;
}

RenderStateGL::~RenderStateGL()
{
}

void RenderStateGL::set()
{
	// have we really changed the wireframe mode?
	if (*globalWireFrame == _wireframe)
		return;
	else
		*globalWireFrame = _wireframe;

	if (_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderStateGL::setGlobalWireFrame(bool* global)
{
	this->globalWireFrame = global;
}

void RenderStateGL::setWireFrame(bool wireframe) {
	_wireframe = wireframe;

}
