#version 450

const vec3 c_positions[] = {
	vec3(0.0f, -0.7f, 0.0f),
	vec3(0.7f, 0.7f, 0.0f),
	vec3(-0.7f, 0.7f, 0.0f)
};

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(c_positions[gl_VertexIndex], 1.0f);
}
