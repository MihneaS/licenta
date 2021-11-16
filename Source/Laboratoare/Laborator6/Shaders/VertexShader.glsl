#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 1) in vec3 v_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// TODO: output values to fragment shader

out vec3 culoarea;
out vec3 frag_normal;
void main()
{
	culoarea = v_color;
	frag_normal = v_normal;
	// TODO: send output to fragment shader
    gl_Position = Projection * View * Model * ;

	// TODO: compute gl_Position
}