#pragma once

#include <Component/SimpleScene.h>

#include <string>

using glm::vec2;
using glm::vec3;
using glm::bvec2;

using std::string;

struct Joint {
	vec2 center;
	vec2 v[4];
	int id_i;
	int id_j;
	float h;
	float l;
};

class Laborator1 : public SimpleScene
{
	public:
		Laborator1();
		~Laborator1();

		void Init() override;


		float h_func(float x, float z);
		float h_x_f(float x);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		Joint** InitJoints(int axe_verticale_per_latura, float lungime_bat);
		Mesh* CreateLinesFromJoints(Joint** joints, string name, int n);
		Mesh* UpdateLinesFromJoints(Mesh* lines, Joint** joints, int n);
		void ExportLines2Obj(std::string file_name, Mesh* lines);
		Mesh* Create3DFromLines(Mesh* lines, string name, float width, float height);
		Mesh* Update3DFromLines(Mesh* lines, Mesh* mesh3D, float width, float height);
		void Export3D2Obj(std::string file_name, Mesh* mesh);
		void Export3D2Obj2(string file_name, Mesh* lines, float width, float height);
		void Remesh();
		// return -1 too to the left, 0 is good, 1 too to the right

		int CheckAngleVert(Joint** joints, Joint& this_j);
		//return sum of cadrane wrong.
/*
	4	|	2
	____|____
		|
	8	|	1
*/
		unsigned int CheckAngleOrtho(Joint** joints, Joint& this_joint);

		Joint** Repair(Joint** joints, float speed);
		vec2 Repair_RepairAnglesDir(vec2 p1, vec2 p2, vec2 pc, vec2 old_repair_dir);
		vec2 Repair_RepairAnglesDir(vec2 p1, vec2 p2, vec2 pc, vec2 old_repair_dir, bool to_c);
		vec2 GetR(Joint& me, Joint& other, float speed); 



protected:
		float dy;
		float lungime_bat;
		float width;
		float height;
		int n; // nodes per side
		Mesh* lines;
		Mesh* mesh3D;
		Joint** joints;
		bvec2** locked;
		float raza_joc;
		float move_fractioner;
		float perf_l;
		float repair_speed;
		float repair_speed_angels;
		float min_angle_vertical;
		float min_angle_vertical_deg;
		float min_angle_ortho;
		float min_angle_ortho_deg;
		float max_angle_ortho;
		float max_angle_ortho_deg;
		bool show_mesh;
		vec2 rot_45deg;
};

float GetAngle(vec2 p1, vec2 p2, vec2 center);
float GetAngleDiff(vec2 p1, vec2 p2, vec2 center);
template <typename T> int sgn(T val);
template <typename T> T** alloc_mat(int n);
template <typename T> void free_mat(T** mat);
vec2 middle(vec2 a, vec2 b);
vec2 complex_mul(vec2 a, vec2 b);

