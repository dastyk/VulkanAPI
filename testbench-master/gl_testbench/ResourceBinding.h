#pragma once
class ResourceBinding
{
	/* Mapping of buffers to names defined in Shaders.
	   Including:
	   Matrices, Vectors, Textures, Samplers, etc.
	   The mapping is defined by Name (in shader) and Resource.
	 */
public:
	ResourceBinding();
	~ResourceBinding();

	// update by hand whenever you want
	// id is a unique Id of a resource on the shader.
	template<typename T, typename V> void updateValue(T id, V& value) = 0;
	// can assume that ptr will be updated every frame.
	template<typename T, typename V> void mapBufferToAddr(T name, V ptr) = 0;
};

