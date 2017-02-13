#version 450

layout(set = 0, binding = 0, rgba32f) uniform imageBuffer g_Positions;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(imageLoad(g_Positions, gl_VertexIndex).xyz, 1.0f);
}
