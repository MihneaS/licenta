#version 330
in vec3 frag_normal;

uniform sampler2D u_texture_0;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 tmp = (frag_normal - abs(frag_normal)) / 4.0f;
	out_color = vec4(abs(frag_normal) + tmp, 1);
}