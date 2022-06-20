#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 sphere_center;
uniform float sphere_radius;

out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

void main()
{
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	vec3 pos_tmp;
	pos_tmp.x = gl_Position.x;
	pos_tmp.y = gl_Position.y;
	pos_tmp.z = gl_Position.z;
	
	vec3 c_to_pos = pos_tmp - sphere_center;
	float len = length(c_to_pos);
	
	frag_normal = c_to_pos / len;
	frag_color = frag_normal;
	tex_coord = v_texture_coord;
	pos_tmp = sphere_center + frag_normal * sphere_radius;
}
