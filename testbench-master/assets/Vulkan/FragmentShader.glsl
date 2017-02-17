
layout(location = 0) out vec4 color;

layout(set = 0, binding = 6) uniform Tint
{
	vec4 g_Tint;
};


void main()
{
	vec4 col = vec4(1.0, 1.0, 0.0, 1.0);


	color = col * vec4(g_Tint.rgb, 1.0);
}
