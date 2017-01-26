#include <stdio.h>
#include "OpenGLRenderer.h"
#include <GL/glew.h>

#include "MaterialGL.h"
#include "MeshGL.h"
#include "Technique.h"
#include "ResourceBindingGL.h"
#include "RenderStateGL.h"
#include "VertexBufferGL.h"
#include "ConstantBufferGL.h"
#include "Texture2DGL.h"

OpenGLRenderer::OpenGLRenderer()
{
}

OpenGLRenderer::~OpenGLRenderer()
{
}

int OpenGLRenderer::shutdown()
{
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

Mesh* OpenGLRenderer::makeMesh() { 
	return new MeshGL(); 
}

Texture2D* OpenGLRenderer::makeTexture2D()
{
	return (Texture2D*)new Texture2DGL();
}

Sampler2D* OpenGLRenderer::makeSampler2D()
{
	return (Sampler2D*)new Sampler2DGL();
}

ConstantBuffer* OpenGLRenderer::makeConstantBuffer(std::string NAME, unsigned int location) { 
	return new ConstantBufferGL(NAME, location); 
}

std::string OpenGLRenderer::getShaderPath() {
	return std::string("..\\assets\\GL45\\");
}

std::string OpenGLRenderer::getShaderExtension() {
	return std::string(".glsl");
}

VertexBuffer* OpenGLRenderer::makeVertexBuffer() { 
	return new VertexBufferGL(); 
};

Material* OpenGLRenderer::makeMaterial() { 
	return new MaterialGL(); 
}

ResourceBinding* OpenGLRenderer::makeResourceBinding() { 
	return new ResourceBindingGL(); 
}

RenderState* OpenGLRenderer::makeRenderState() { 
	RenderStateGL* newRS = new RenderStateGL();
	newRS->setGlobalWireFrame(&this->globalWireframeMode);
	newRS->setWireFrame(false);
	return (RenderState*)newRS;
}

int OpenGLRenderer::initialize(unsigned int width, unsigned int height) {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_GL_MakeCurrent(window, context);

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glViewport(0, 0, width, height);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
	}
	return 0;
}

/*
 Super simplified implementation of a work submission
 TODO.
*/
void OpenGLRenderer::submit(Mesh* mesh) 
{
	drawList.push_back(mesh);
};

/*
 Naive implementation, no re-ordering, checking for state changes, etc.
 TODO.
*/
void OpenGLRenderer::frame() 
{
	for (auto mesh : drawList)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		for (auto t : mesh->textures)
		{
			// we do not really know here if the sampler has been
			// defined in the shader.
			t.second->bind(t.first);
		}

		Technique* t = mesh->technique;
		t->enable(this);

		// bind buffers for this mesh.
		// this implementation only has buffers in the Vertex Shader!
		// bind them all before drawing.
		size_t numberElements = 3;
		for (auto element : mesh->geometryBuffers) {
			mesh->bindIAVertexBuffer(element.first);
			numberElements = element.second.numElements;
		}

		mesh->txBuffer->bind(mesh->technique->material);

		// everything is bound!
		// always 0 because we are just generating gl_VertexId
		glDrawArrays(GL_TRIANGLES, 0, numberElements);
	}
	drawList.clear();
};


void OpenGLRenderer::present()
{
	SDL_GL_SwapWindow(window);
};

void OpenGLRenderer::setClearColor(float r, float g, float b, float a)
{
	//clearColor[0] = r; clearColor[1] = g; clearColor[2] = b; clearColor[3] = a;
	glClearColor(r, g, b, a);
};

void OpenGLRenderer::clearBuffer(unsigned int flag) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return;
	// using is only valid inside the function!
	using namespace CLEAR_BUFFER_FLAGS;
	GLuint glFlags = BUFFER_MAP[flag & COLOR] | BUFFER_MAP[flag & DEPTH] | BUFFER_MAP[flag & STENCIL];
	glClear(glFlags);
};

void OpenGLRenderer::setRenderTarget(RenderTarget* rt) {};

void OpenGLRenderer::setRenderState(RenderState* ps)
{
	// naive implementation
	ps->set();
};
