#pragma once
#include <string>
#include <vector>
#include <list>
#include <functional>

#include <include/gl.h>

#define MAX_2D_TEXTURES		16
#define INVALID_LOC			-1

namespace migine {
	enum class Shader_id {
		color,
		vertex_normal,
		vertex_color,
		simple,
		lab8,
		sphere
	};
}

class Shader
{
	public:
		Shader(migine::Shader_id id);
		~Shader();

		migine::Shader_id get_id() const;
		GLuint GetProgramID() const;

		void Use() const;
		unsigned int Reload();

		void AddShader(const std::string &shaderFile, GLenum shaderType);
		void ClearShaders();
		unsigned int CreateAndLink();

		void BindTexturesUnits();
		GLint GetUniformLocation(const char * uniformName) const;

		void OnLoad(std::function<void()> onLoad);

	private:
		void GetUniforms();
		static unsigned int CreateShader(const std::string &shaderFile, GLenum shaderType);
		static unsigned int CreateProgram(const std::vector<unsigned int> &shaderObjects);

	public:
		GLuint program;

		// Textures
		GLint loc_textures[MAX_2D_TEXTURES];

		// MVP
		GLint loc_model_matrix;
		GLint loc_view_matrix;
		GLint loc_projection_matrix;

		// Shadow
		GLint loc_light_pos;
		GLint loc_light_color;
		GLint loc_light_radius;
		GLint loc_light_direction;

		// Camera
		GLint loc_eye_pos;
		GLint loc_eye_forward;
		GLint loc_z_far;
		GLint loc_z_near;

		// Sphere
		GLint loc_sphere_center;
		GLint loc_sphere_radius;

		// General
		GLint loc_resolution;
		
		// Text
		GLint text_color;

	private:

		bool compileErrors;

		struct ShaderFile
		{
			std::string file;
			GLenum type;
		};

		migine::Shader_id id;
		std::vector<ShaderFile> shaderFiles;
		std::list<std::function<void()>> loadObservers;
};
