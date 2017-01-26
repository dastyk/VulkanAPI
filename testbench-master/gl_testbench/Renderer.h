#pragma once

#include <unordered_map>
#include <Windows.h>

#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderState.h"
#include "DrawInfo.h"
#include "Technique.h"
#include "ConstantBuffer.h"

class Mesh;
class VertexBuffer;
class Texture2D;
class Sampler2D;

//CRITICAL_SECTION protectHere;
//#define LOCK EnterCriticalSection(&protectHere)
//#define UNLOCK LeaveCriticalSection(&protectHere)

namespace CLEAR_BUFFER_FLAGS {
	static const int COLOR = 1;
	static const int DEPTH = 2;
	static const int STENCIL = 4;
};

class Renderer {
public:
	enum class BACKEND { GL45, VULKAN, DX11, DX12 };

	/*
	Return concrete objects of the BACKEND
	*/
	static Renderer* makeRenderer(BACKEND backend);
	virtual Material* makeMaterial() = 0;
	virtual Mesh* makeMesh() = 0;
	virtual VertexBuffer* makeVertexBuffer() = 0;
	virtual Texture2D* makeTexture2D() = 0;
	virtual Sampler2D* makeSampler2D() = 0;
	virtual ResourceBinding* makeResourceBinding() = 0;
	virtual RenderState* makeRenderState() = 0;
	virtual std::string getShaderPath() = 0;
	virtual std::string getShaderExtension() = 0;
	virtual ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location) = 0;

	Renderer() { /*InitializeCriticalSection(&protectHere);*/ };
	virtual int initialize(unsigned int width = 800, unsigned int height = 600) = 0;
	virtual void present() = 0;
	virtual int shutdown() = 0;

	virtual void setClearColor(float, float, float, float) = 0;
	virtual void clearBuffer(unsigned int) = 0;
	virtual void setRenderTarget(RenderTarget* rt) = 0; // complete parameters
	// can be partially overriden by a specific Technique.
	virtual void setRenderState(RenderState* ps) = 0;
	// submit work (to render) to the renderer.
	virtual void submit(Mesh* mesh) = 0;
	virtual void frame() = 0;
	
	BACKEND IMPL;
};
