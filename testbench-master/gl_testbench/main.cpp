#include <string>
#include <SDL_keyboard.h>
#include <SDL_events.h>
#include <type_traits> 

#include "Renderer.h"
#include "Mesh.h"
#include "Texture2D.h"
#include <math.h>
#include <CommandConsole\Include\ConsoleThread.h>
using namespace std;
Renderer* renderer;

// flat scene at the application level...we don't care about this here.
// do what ever you want in your renderer backend.
// all these objects are loosely coupled, creation and destruction is responsibility
// of the testbench, not of the container objects
vector<Mesh*> scene;
vector<Material*> materials;
vector<Technique*> techniques;
vector<Texture2D*> textures;
vector<Sampler2D*> samplers;

// forward decls
void updateScene();
void renderScene();

// lissajous points
float xt[4*360], yt[4*360];

typedef union { 
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
} float4;

typedef union { 
	struct { float x, y; };
	struct { float u, v; };
} float2;


void run() {

	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
		}
		updateScene();
		renderScene();
	}
}

/*
 update positions of triangles in the screen changing a translation only
*/
void updateScene()
{
	/*
	    For each mesh in scene list, update their position 
	*/
	float translation[4] = { 0.0,0.0,0.0,0.0 };
	static int shift = 0;

	//float scale = 359.0 / scene.size();
	float scale = 1.0;
	// fatboy slim is a special case. outside the loop
	translation[0] = xt[(0+shift) % (4*360)];
	translation[1] = yt[(0+shift) % (4*360)];
	translation[2] = -0.1;

	Mesh* m0 = scene[0];
	m0->txBuffer->setData( translation, sizeof(translation), m0->technique->getMaterial(), TRANSLATION);
	translation[2] = 0.0;


	for (int i = 1; i < scene.size(); i++)
	{
		translation[0] = xt[(i+shift) % (4*360)];
		translation[1] = yt[(i+shift) % (4*360)];

		// updates the buffer data (whenever the implementation decides...)
		Mesh* mn = scene[i];
		mn->txBuffer->setData( translation, sizeof(translation), mn->technique->getMaterial(), TRANSLATION);
	}
	shift++;
	return;
};


void renderScene()
{
	renderer->clearBuffer(CLEAR_BUFFER_FLAGS::COLOR | CLEAR_BUFFER_FLAGS::DEPTH);
	for (auto m : scene)
	{
		renderer->submit(m);
	}
	renderer->frame();
	renderer->present();
}

int initialiseTestbench()
{
	std::string definePos = "#define POSITION " + std::to_string(POSITION) + "\n";
	std::string defineNor = "#define NORMAL " + std::to_string(NORMAL) + "\n";
	std::string defineUV = "#define TEXTCOORD " + std::to_string(TEXTCOORD) + "\n";

	std::string defineTX = "#define TRANSLATION " + std::to_string(TRANSLATION) + "\n";

	std::string defineTXName = "#define TRANSLATION_NAME " + std::string(TRANSLATION_NAME) + "\n";
	
	std::string defineDiffCol = "#define DIFFUSE_TINT " + std::to_string(DIFFUSE_TINT) + "\n";
	std::string defineDiffColName = "#define DIFFUSE_TINT_NAME " + std::string(DIFFUSE_TINT_NAME) + "\n";

	std::string defineDiffuse = "#define DIFFUSE_SLOT " + std::to_string(DIFFUSE_SLOT) + "\n";


	std::vector<std::vector<std::string>> materialDefs = {
		// vertex shader, fragment shader, defines
		// shader extension must be asked to the renderer
		// these strings should be constructed from the IA.h file!!!

		{ "VertexShader", "FragmentShader", definePos + defineNor + defineUV + defineTX + 
		   defineTXName + defineDiffCol + defineDiffColName }, 

		{ "VertexShader", "FragmentShader", definePos + defineNor + defineUV + defineTX + 
		   defineTXName + defineDiffCol + defineDiffColName }, 

		{ "VertexShader", "FragmentShader", definePos + defineNor + defineUV + defineTX + 
		   defineTXName + defineDiffCol + defineDiffColName + defineDiffuse	}
	};

	float degToRad = M_PI / 180.0;
	for (int a = 0; a < 4*360; a++)
	{
		xt[a] = 0.8f * cosf(degToRad * (a/4.0) * 3.0);
		yt[a] = 0.8f * sinf(degToRad * (a/4.0) * 2.0);
	};

	// triangle geometry:
	float4 triPos[3] = { { 0.0f,  0.05, 0.0f, 1.0f },{ 0.05, -0.05, 0.0f, 1.0f },{ -0.05, -0.05, 0.0f, 1.0f } };
	float4 triNor[3] = { { 0.0f,  0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f } };
	float2 triUV[3] =  { { 0.5f,  -0.99f },{ 1.49f, 1.1f },{ -0.51, 1.1f } };

	// load Materials.
	std::string shaderPath = renderer->getShaderPath();
	std::string shaderExtension = renderer->getShaderExtension();
	float diffuse[3][4] = {
		0.0,0.0,1.0,1.0,
		0.0,1.0,0.0,1.0,
		1.0,1.0,1.0,1.0 
	};

	for (int i = 0; i < materialDefs.size(); i++)
	{
		// set material name from text file?
		Material* m = renderer->makeMaterial();
		m->setShader(shaderPath + materialDefs[i][0] + shaderExtension, Material::ShaderType::VS);
		m->setShader(shaderPath + materialDefs[i][1] + shaderExtension, Material::ShaderType::PS);

		m->addDefine(materialDefs[i][2], Material::ShaderType::VS);
		m->addDefine(materialDefs[i][2], Material::ShaderType::PS);

		std::string err;
		m->compileMaterial(err);

		// add a constant buffer to the material, to tint every triangle using this material
		m->addConstantBuffer(DIFFUSE_TINT_NAME, DIFFUSE_TINT);
		// no need to update anymore
		// when material is bound, this buffer should be also bound for access.
		m->updateConstantBuffer(diffuse[i], 4 * sizeof(float), DIFFUSE_TINT);
		
		materials.push_back(m);
	}

	// one technique with wireframe
	RenderState* renderState1 = renderer->makeRenderState();
	renderState1->setWireFrame(true);

	// basic technique
	techniques.push_back(renderer->makeTechnique(materials[0], renderState1));
	techniques.push_back(renderer->makeTechnique(materials[1], renderer->makeRenderState()));
	techniques.push_back(renderer->makeTechnique(materials[2], renderer->makeRenderState()));

	// create texture
	Texture2D* fatboy = renderer->makeTexture2D();
	fatboy->loadFromFile("../assets/textures/fatboy.png");
	Sampler2D* sampler = renderer->makeSampler2D();
	sampler->setWrap(WRAPPING::REPEAT, WRAPPING::REPEAT);
	fatboy->sampler = sampler;

	textures.push_back(fatboy);
	samplers.push_back(sampler);

	// Create a mesh array with 3 basic vertex buffers.
	for (int i = 0; i < 2000; i++) {

		Mesh* m = renderer->makeMesh();

		constexpr auto numberOfElements = std::extent<decltype(triPos)>::value;

		VertexBuffer* pos = renderer->makeVertexBuffer();
		pos->setData(triPos, sizeof(triPos), VertexBuffer::STATIC);
		pos->bind(0, sizeof(triPos), POSITION);
		m->addIAVertexBufferBinding(pos, 0, numberOfElements, POSITION);

		VertexBuffer* nor = renderer->makeVertexBuffer();
		nor->setData(triNor, sizeof(triNor), VertexBuffer::STATIC);
		nor->bind(0, sizeof(triNor), NORMAL);
		m->addIAVertexBufferBinding(nor, 0, numberOfElements, NORMAL);

		VertexBuffer* uvs = renderer->makeVertexBuffer();
		uvs->setData(triUV, sizeof(triUV), VertexBuffer::STATIC);
		uvs->bind(0, sizeof(triUV), TEXTCOORD);
		m->addIAVertexBufferBinding(uvs, 0, numberOfElements, TEXTCOORD);

		// we can create a constant buffer outside the material, for example as part of the Mesh.
		m->txBuffer = renderer->makeConstantBuffer(std::string(TRANSLATION_NAME), TRANSLATION);
		
		if (i == 0) {
			m->technique = techniques[2];
			m->addTexture(textures[0], DIFFUSE_SLOT);
			
		}
		else 
			m->technique = techniques[ i % 2];

		scene.push_back(m);
	}
	return 0;
}

void shutdown() {
	// shutdown.
	// delete dynamic objects
	for (auto m : materials)
	{
		delete(m);
	}
	for (auto t : techniques)
	{
		delete(t);
	}
	for (auto m : scene)
	{
		for (auto g : m->geometryBuffers)
		{
			delete g.second.buffer;
		}
		delete(m);
	}
	
	for (auto s : samplers)
	{
		delete s;
	}

	for (auto t : textures)
	{
		delete t;
	}

	renderer->shutdown();
};

int main(int argc, char *argv[])
{
	DebugUtils::DebugConsole::Command_Structure def =
	{
		nullptr,
		[](void* userData, int argc, char** argv) {printf("Command not found: %s\n\n", argv[0]);},
		[](void* userData, int argc, char** argv) {},
		"",
		""
	};

	DebugUtils::ConsoleThread::Init(&def);

	DebugUtils::DebugConsole::Command_Structure exitCommand =
	{
		nullptr,
		[](void* userData, int argc, char** argv) {
		SDL_Event user_event;

		user_event.type = SDL_QUIT;
		user_event.user.code = 0;
		user_event.user.data1 = NULL;
		user_event.user.data2 = NULL;
		SDL_PushEvent(&user_event);
	},
		[](void* userData, int argc, char** argv) {},
		"exit",
		"Terminates the program."
	};

	DebugUtils::ConsoleThread::AddCommand(&exitCommand);


	DebugUtils::ConsoleThread::ShowConsole();


	renderer = Renderer::makeRenderer(Renderer::BACKEND::VULKAN);
	renderer->initialize();
	renderer->setClearColor(0.5, 0.1, 0.1, 1.0);
	initialiseTestbench();
	run();
	shutdown();

	DebugUtils::ConsoleThread::Shutdown();
	return 0;
};
