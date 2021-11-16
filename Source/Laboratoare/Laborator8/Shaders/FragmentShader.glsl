#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 tmp = light_position - world_position;
	float d = length(tmp);
	vec3 L = tmp/d; // = nomalize(tmp)
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L+V);

	// TODO: define ambient light component
	float ambient_light = 0.25;

	// TODO: compute diffuse light component
	float diffuse_light = max(dot(world_normal,L),0);

	// TODO: compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0) {
		specular_light = pow(max(dot(world_normal,H),0),material_shininess);
	}

	// TODO: compute light
	float kc = 0.1, kl = 0.1, kq = 0.1;
	float light = material_kd*ambient_light + (material_kd*diffuse_light + material_ks*specular_light) / (kc + kl*d + kq*d*d);

	// TODO: write pixel out color
	out_color = vec4(light * object_color, 1);
}