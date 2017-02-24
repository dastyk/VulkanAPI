
#ifdef POSITION
layout(set = 0, binding = POSITION) buffer b {
	vec4 g_Positions[3];
};
#endif
#ifdef TEXTCOORD
layout(set = 0, binding = TEXTCOORD)  buffer b2 {
	vec2 g_TexCoords[3];
};
#endif
layout(set = 0, binding = 5) uniform PositionConstants
{
	vec4 g_Translation;
};

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec2 outTex;

void main()
{
#ifdef POSITION
	gl_Position = vec4(g_Positions[gl_VertexIndex]) + g_Translation;
#endif
	gl_Position.z += 1.0f; // Fransisco uses -0.1 for translation, which in OpenGL means slightly before middle (range -1 to 1), but in Vulkan NDC z is 0 to 1.
	gl_Position.z *= 0.5f;
#ifdef TEXTCOORD
	outTex = g_TexCoords[gl_VertexIndex];
#endif
}
