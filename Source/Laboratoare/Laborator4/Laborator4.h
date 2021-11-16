#pragma once
#include <Component/SimpleScene.h>

#include <string>
#include <set>

using std::string;
using std::set;
using glm::vec2;
using glm::ivec2;
using glm::bvec4;

struct Joint {
	glm::vec2 center;
	glm::vec2 v[4];
	int id_i;
	int id_j;
};

constexpr auto vs_in_node = 8;

class Laborator4 : public SimpleScene
{
	public:
		Laborator4();
		~Laborator4();

		void Init() override;

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

		Joint** InitJoints(int axe_verticale_per_latura, float dy, float lungime_bat);
		Mesh* CreateLinesFromJoints(Joint** joints, string name, float dy, int n);
		Mesh* UpdateLinesFromJoints(Mesh* lines, Joint** joints, int n);
		void ExportLines2Obj(std::string file_name, Mesh* lines);
		Mesh* Create3DFromLines(Mesh* lines, string name, float width, float height);
		Mesh* Update3DFromLines(Mesh* lines, Mesh* mesh3D, float width, float height);
		void Export3D2Obj(std::string file_name, Mesh* mesh);
		void Remesh();
		void AdjustRotatingVs(Joint& joint, int start_i, int delta_angle_sign);
		Joint** MoveJoint(int i, int j, vec2 delta);
		void MoveJoint0(const Joint& old, Joint& next, vec2 dist, bvec4 locked);
		void MoveJoint1(const Joint& old, Joint& next);
		void AdjustRotatingVs_Adjust2Vertices(Joint& joint, int i_static, int i_movable, float min_angle, int delta_angle_sign);
		int MoveJoint2(const Joint& old, Joint& next);
		void MoveJoint2_FindCircles(vec2 next_v1, vec2 next_v2, vec2& out1, vec2& out2);
		int MoveJoint3(const Joint& old, Joint& next);
		int MoveJoint4(const Joint& old, Joint& next);
		bool CheckAngleOrtho(float angle);
		//bool CheckAngleVert(float angle);
		void AdjustVerticies(Joint& joint, vec2 delta);
		void AdjustOpposingPair(Joint& joint, int i, int ip2, bool locked); // subfunction of AdjustVerticies. DO NOT USE OUTSIDE
		template <typename T>
		void AdjustJointsNeighbours(Joint** joints, int i, int j, set<ivec2, T>* neighbours);
		void AdjustJointsNeighbour(vec2 v, vec2& neighbour);
		float GetAngle(vec2 p1, vec2 p2, vec2 center);
		float GetAngleDiff(vec2 p1, vec2 p2, vec2 center);
		float GetDistFromPointToLine(vec2 linePoint, vec2 lineDirection, vec2 otherPoint);

		void PrintJoints(char* msg, bool stop);

	protected:
		glm::mat4 modelMatrix;
		float translateX, translateY, translateZ;
		float scaleX, scaleY, scaleZ;
		float angularStepOX, angularStepOY, angularStepOZ;
		GLenum polygonMode;
		float dy;
		float lungime_bat;
		float width;
		float height;
		int n; // nodes per side
		Mesh* lines;
		Mesh* mesh3D;
		Joint** joints;
		bvec4** locked_v;
		Joint** tmp_joints;
		bool** locked_nodes;
		float raza_joc;
		float raza_joc2;
		float min_angle_vertical;
		float min_angle_vertical_deg;
		float min_angle_ortho;
		float min_angle_ortho_deg;
		float max_h_from_c;
		float fp_err;// = 0.000001f;
		vec2 miscarea_aia;
		float le_f;
		float ang_coef;
};

template <typename T> int sgn(T val);
vec2 complex_mul(vec2 a, vec2 b);
vec2 middle(vec2 a, vec2 b);
template <typename T> T** alloc_mat(int n);
template <class T1, typename T2> bool contains(T1* container, T2 item);

void ScuzeMan(char* file, int line, char* details);
struct ivec2_cmp {
	bool operator() (const ivec2& lhs, const ivec2& rhs) const;
};