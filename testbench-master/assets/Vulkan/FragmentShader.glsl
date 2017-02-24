layout(location = 0) in vec2 texC;
layout(location = 0) out vec4 color;

layout(set = 0, binding = 6) uniform Tint
{
	vec4 g_Tint;
};

layout(set = 1, binding = 0) uniform texture2D tex;
layout(set = 1, binding = 1) uniform sampler sam;
void main()
{
#ifdef DIFFUSE_SLOT
	vec2 temp = texC;
	vec4 col = texture(sampler2D(tex, sam), temp);
#else
	vec4 col = vec4(1.0, 1.0, 0.0, 1.0);
#endif

	color = col*vec4(g_Tint.rgb, 1.0);
}
