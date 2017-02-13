#version 450

layout(set = 0, binding = 0, rgba32f) uniform imageBuffer g_Positions;
layout(set = 0, binding = 5) uniform PositionConstants
{
	vec4 g_Translation;
};

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(imageLoad(g_Positions, gl_VertexIndex).xyz, 1.0f) + g_Translation;
	gl_Position.z += 1.0f; // Fransisco uses -0.1 for translation, which in OpenGL means slightly before middle (range -1 to 1), but in Vulkan NDC z is 0 to 1.
	gl_Position.z *= 0.5f;
}
