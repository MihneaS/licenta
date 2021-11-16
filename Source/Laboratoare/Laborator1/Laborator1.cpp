#include "Laborator1.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

using namespace std;
using glm::length;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/World.cpp

Laborator1::Laborator1()
{
}

Laborator1::~Laborator1()
{
}

void Laborator1::Init()
{
	lungime_bat = 2.5;
	dy = 1;
	n = 22;//24; // nodes per side
	width = 0.2;
	height = 0.08;
	raza_joc = 0.1;
	move_fractioner = 10;
	repair_speed = 2;
	repair_speed_angels = 1.5;
	perf_l = sqrt(lungime_bat * lungime_bat - (lungime_bat / 2 * lungime_bat / 2));

	min_angle_vertical_deg = 165;
	min_angle_vertical = min_angle_vertical_deg * AI_MATH_PI_F / 180;
	min_angle_ortho_deg = 15;
	min_angle_ortho = min_angle_ortho_deg * AI_MATH_PI_F / 180;
	max_angle_ortho_deg = 100;
	max_angle_ortho = max_angle_ortho_deg * AI_MATH_PI_F / 180;


	rot_45deg = vec2(sqrt(2)/2, sqrt(2)/2);

	show_mesh = false;

	locked = alloc_mat<bvec2>(n);
	joints = InitJoints(n, lungime_bat);
	lines = CreateLinesFromJoints(joints, "struct", n);
	mesh3D = Create3DFromLines(lines, "mesh3D", width, height);


	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			locked[i][j] = bvec2(false,false);
			if (j == n - 1 || j == 0) {
			//locked[i][j].x = true;
			}
			if (i == n - 1 || i == 0) {
			//locked[i][j].y = true;
			}
		}
	}

}

float Laborator1::h_func(float x, float z)
{
	//return fabs(z + 16) / 7;// +sgn(z) / (x + 0.5);
	//return 2.25f;
	vec2 tmp = vec2(x, z);
	tmp = complex_mul(tmp, rot_45deg);
	x = tmp.x;
	z = tmp.y;
	/*
	float c = 2;
	float b = 0.01;
	float d = 5;
	float f = 0.75;
	return c / sqrt(b * (z - d) * (z - d) + 1) + c / sqrt(b * (x - d) * (x - d) + 1) + f;
	*/
	float d = 0.7;


	return h_x_f(x) + d;
}

float Laborator1::h_x_f(float x)
{
	float u = 0.1;
	float p = 7.4;
	float t = 10;
	float w = 0.14;
	float v = -3;
	float a = -1.2;
	float b = 5.3;
	float f = -3.7;

	float sin_ceva = sin(w*x + f);
	float sin_ceva4 = sin_ceva * sin_ceva * sin_ceva * sin_ceva;
	float cos_ceva = cos(w*x + f + v);
	float cos_ceva2 = cos_ceva * cos_ceva;

	return (t - x / p) / sqrt(a * sin_ceva4 + 1 / sqrt(x * x + 0.1) * cos_ceva2 + 2);
}


void Laborator1::FrameStart()
{

}

void Laborator1::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->props.resolution;

	// sets the clear color for the color buffer
	glClearColor(0, 0, 0, 1);

	// clears the color buffer (using the previously set color) and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);

	delete lines;
	delete mesh3D;
	lines = CreateLinesFromJoints(joints, "structura", n);
	mesh3D = Create3DFromLines(lines, "mesh3d", width, height);

	if (show_mesh) {
		RenderMesh(mesh3D, shaders["VertexNormal"], vec3(0.0f, 0.5f, 0.0f));
	}
	else {
		RenderMesh(lines, shaders["VertexNormal"], vec3(0.0f, 0.5f, 0.0f));
	}
	
}

void Laborator1::FrameEnd()
{
	DrawCoordinatSystem();
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator1::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 0.4;
	if (window->KeyHold(GLFW_KEY_P)) {
		lungime_bat += speed * deltaTime;
		Remesh();
	}
	if (window->KeyHold(GLFW_KEY_O)) {
		lungime_bat -= speed * deltaTime;
		Remesh();
	}
	if (window->KeyHold(GLFW_KEY_L)) {
		dy += speed * deltaTime;
		Remesh();
	}
	if (window->KeyHold(GLFW_KEY_K)) {
		dy -= speed * deltaTime;
		Remesh();
	}
	if (window->KeyHold(GLFW_KEY_M)) {
		joints = Repair(joints, deltaTime * repair_speed);
	}
};

void Laborator1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_EQUAL)
	{
		n++;
		Remesh();
	}
	if (key == GLFW_KEY_MINUS && n > 2)
	{
		n--;
		Remesh();
	}
	if (key == GLFW_KEY_V)
	{
		printf("Exporting...\n");
		Export3D2Obj("structura", mesh3D); //TODO MAGIC STRING
		//Export3D2Obj2("structura", lines, width, height);
		printf("Export finished\n");
	}
	if (key == GLFW_KEY_M)
	{
		//joints = Repair(joints);
	}
	if (key == GLFW_KEY_B)
	{
		show_mesh = !show_mesh;
	}
};

void Laborator1::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator1::OnWindowResize(int width, int height)
{
	// treat window resize event
}


Joint** Laborator1::InitJoints(int n, float lungime_bat) {
	Joint** joints = alloc_mat<Joint>(n);

	// Pt dy = lungime_bat/2,  vom avea romburi cu unghiuri de 120 de grade, 60 de grade;
	//if (dy == -1) {
	dy = lungime_bat / 2;
	//}
	// TODO CLAMP INPUT
	//float dx = sqrt(lungime_bat * lungime_bat - dy * dy) + raza_joc * 2;
	float dx = sqrt(lungime_bat * lungime_bat - dy * dy);
	float dz = dx;

	float x0 = ((n - 1.0) / 2) * dx * -1;
	float z0 = ((n - 1.0) / 2) * dz;

	float x = x0;
	float z = z0;
	for (int i = 0; i < n; i++) {
		x = x0;
		for (int j = 0; j < n; j++) {
			joints[i][j].center = vec2(x, z);
			joints[i][j].v[0] = vec2(x, z + raza_joc);
			joints[i][j].v[1] = vec2(x + raza_joc, z);
			joints[i][j].v[2] = vec2(x, z - raza_joc);
			joints[i][j].v[2] = vec2(x - raza_joc, z);
			joints[i][j].id_i = i;
			joints[i][j].id_j = j;
			float h = h_func(x, z);
			joints[i][j].h = h;
			joints[i][j].l = sqrt(lungime_bat * lungime_bat - h * h)/2;

			x += dx;
		}
		z -= dz;
	}

	return joints;
}

Mesh* Laborator1::CreateLinesFromJoints(Joint** joints, string name, int n) {
	Mesh* mesh = new Mesh(name);
	mesh->SetDrawMode(GL_LINES);

	vector<VertexFormat> vertices;
	vector<unsigned int> indices;

	for (int i = 0, vi = 0; i < n; i++) {
		for (int j = 0; j < n; j++, vi += 8) {
			float y1 = -joints[i][j].h / 2;
			float y2 = joints[i][j].h / 2;
			int k = 0;
			for (k = 0; k < 4; k++) {
				//vertices.push_back(VertexFormat(vec3(joints[i][j].v[k].x, y1, joints[i][j].v[k].y)));
				vertices.push_back(VertexFormat(vec3(joints[i][j].center.x, y1, joints[i][j].center.y)));
			}
			for (k = 0; k < 4; k++) {
				//vertices.push_back(VertexFormat(vec3(joints[i][j].v[k].x, y2, joints[i][j].v[k].y)));
				vertices.push_back(VertexFormat(vec3(joints[i][j].center.x, y2, joints[i][j].center.y)));
			}

			if (j != 0) {
				indices.push_back(vi + 3);
				indices.push_back(vi - 3);

				indices.push_back(vi + 7);
				indices.push_back(vi - 7);
			}
			if (i != 0) {
				indices.push_back(vi);
				indices.push_back(vi - 8 * n + 6);

				indices.push_back(vi + 4);
				indices.push_back(vi - 8 * n + 2);
			}
		}
	}

	mesh->InitFromData(vertices, indices);

	return mesh;
}

/*	Lines
 *		6
 *	  7	  5
 *		4
 *
 *		2
 *	  3	  1
 *		0
 */

Mesh* Laborator1::UpdateLinesFromJoints(Mesh* lines, Joint** joints, int n) {

	for (int i = 0, vi = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < 8; k++) {
				lines->vertices[i + k + j * 8 * n].position.x = joints[i][j].v[k % (8 / 2)].x;
				lines->vertices[i + k + j * 8 * n].position.z = joints[i][j].v[k % (8 / 2)].y;
				//lines->vertices[i + k + j * 8 * n].position.x = joints[i][j].center.x;
				//lines->vertices[i + k + j * 8 * n].position.z = joints[i][j].center.y;
			}
		}
	}

	return lines;
}

Mesh* Laborator1::Create3DFromLines(Mesh* lines, std::string name, float width, float height) {
	Mesh* result = new Mesh(name);
	result->SetDrawMode(GL_TRIANGLES);

	vector<VertexFormat> vertices;
	//vector<unsigned int> indices;
	vector<unsigned int> indices;
	int vi = 0;

	//int wtf_n = (n - 1) * (n - 1) * 8 + 4 * n * 6 + 4 * 4;

	for (int i = 0; i < lines->indices.size(); i += 2) {
	//for (int i = 0; i < wtf_n; i += 2) {

		vec3 p1 = lines->vertices[lines->indices[i]].position;
		vec3 p2 = lines->vertices[lines->indices[i + 1]].position;

		vec3 tmp1 = vec3(p1.x, p2.y, p1.z);
		/*
			A=p1
			|\
			| \
			|  \c
		  b |   \D=tmp2
			| h_/\
			|/____\
	   tmp1=C  a  B=p2

			h = inaltimea
		*/


		float a2 = glm::distance2(p1, tmp1);
		float b2 = glm::distance2(tmp1, p2);
		float c2 = glm::distance2(p1, p2);
		float h2 = a2 * b2 / c2;
		float AD = sqrt(a2 - h2);
		float c = sqrt(c2);

		vec3 tmp2 = p1 + (p2 - p1) * AD / c2;

		vec3 front = glm::normalize(p2 - p1);
		vec3 up = glm::normalize(tmp2 - tmp1);
		vec3 right = glm::cross(front, up);


		vertices.push_back(VertexFormat(p1 - up * (height / 2) - right * (width / 2)));
		vertices.push_back(VertexFormat(p1 - up * (height / 2) + right * (width / 2)));
		vertices.push_back(VertexFormat(p1 + up * (height / 2) + right * (width / 2)));
		vertices.push_back(VertexFormat(p1 + up * (height / 2) - right * (width / 2)));

		vertices.push_back(VertexFormat(p2 - up * (height / 2) - right * (width / 2)));
		vertices.push_back(VertexFormat(p2 - up * (height / 2) + right * (width / 2)));
		vertices.push_back(VertexFormat(p2 + up * (height / 2) + right * (width / 2)));
		vertices.push_back(VertexFormat(p2 + up * (height / 2) - right * (width / 2)));

		// back
		indices.push_back(vi);
		indices.push_back(vi + 1);
		indices.push_back(vi + 3);
		indices.push_back(vi + 1);
		indices.push_back(vi + 3);
		indices.push_back(vi + 2);

		// bottom
		indices.push_back(vi);
		indices.push_back(vi + 1);
		indices.push_back(vi + 4);
		indices.push_back(vi + 1);
		indices.push_back(vi + 4);
		indices.push_back(vi + 5);

		// right
		indices.push_back(vi + 1);
		indices.push_back(vi + 2);
		indices.push_back(vi + 5);
		indices.push_back(vi + 2);
		indices.push_back(vi + 5);
		indices.push_back(vi + 6);

		// top
		indices.push_back(vi + 3);
		indices.push_back(vi + 2);
		indices.push_back(vi + 7);
		indices.push_back(vi + 2);
		indices.push_back(vi + 7);
		indices.push_back(vi + 6);

		// left
		indices.push_back(vi);
		indices.push_back(vi + 3);
		indices.push_back(vi + 4);
		indices.push_back(vi + 3);
		indices.push_back(vi + 4);
		indices.push_back(vi + 7);

		// front
		indices.push_back(vi + 4);
		indices.push_back(vi + 7);
		indices.push_back(vi + 5);
		indices.push_back(vi + 7);
		indices.push_back(vi + 5);
		indices.push_back(vi + 6);

		vi += 8;
	}

	result->InitFromData(vertices, indices);

	return result;
}

Mesh* Laborator1::Update3DFromLines(Mesh* lines, Mesh* mesh3D, float width, float height) {

	int vi = 0;
	for (int i = 0; i < lines->indices.size(); i += 2) {

		vec3 p1 = lines->vertices[lines->indices[i]].position;
		vec3 p2 = lines->vertices[lines->indices[i + 1]].position;

		vec3 tmp1 = vec3(p1.x, p2.y, p1.z);
		/*
			A=p1
			|\
			| \
			|  \c
		  b |   \D=tmp2
			| h_/\
			|/____\
	   tmp1=C  a  B=p2

			h = inaltimea
		*/


		float a2 = glm::distance2(p1, tmp1);
		float b2 = glm::distance2(tmp1, p2);
		float c2 = glm::distance2(p1, p2);
		float h2 = a2 * b2 / c2;
		float AD = sqrt(a2 - h2);
		float c = sqrt(c2);

		vec3 tmp2 = p1 + (p2 - p1) * AD / c2;

		vec3 front = glm::normalize(p2 - p1);
		vec3 up = glm::normalize(tmp2 - tmp1);
		vec3 right = glm::cross(front, up);


		mesh3D->vertices[vi].position = p1 - up * (height / 2) - right * (width / 2);
		mesh3D->vertices[vi + 1].position = p1 - up * (height / 2) + right * (width / 2);
		mesh3D->vertices[vi + 2].position = p1 + up * (height / 2) + right * (width / 2);
		mesh3D->vertices[vi + 3].position = p1 + up * (height / 2) - right * (width / 2);

		mesh3D->vertices[vi + 4].position = p2 - up * (height / 2) - right * (width / 2);
		mesh3D->vertices[vi + 5].position = p2 - up * (height / 2) + right * (width / 2);
		mesh3D->vertices[vi + 6].position = p2 + up * (height / 2) + right * (width / 2);
		mesh3D->vertices[vi + 7].position = p2 + up * (height / 2) - right * (width / 2);
	}

	return mesh3D;
}

void Laborator1::ExportLines2Obj(std::string file_name, Mesh* mesh) {
	FILE* export_file = fopen((file_name + ".OBJ").c_str(), "w");

	for (auto v : mesh->vertices) {
		fprintf(export_file, "v %f %f %f\n", v.position.x, v.position.y, v.position.z);
	}

	fprintf(export_file, "\n");

	fprintf(export_file, "o Struct001\n");
	fprintf(export_file, "g Struct001\n");
	for (int i = 0; i < mesh->indices.size(); i += 2) {
		fprintf(export_file, "l %d %d \n", mesh->indices[i] + 1, mesh->indices[i + 1] + 1);
	}

	fclose(export_file);
}

void Laborator1::Export3D2Obj(std::string file_name, Mesh* mesh) {
	FILE* export_file = fopen((file_name + ".OBJ").c_str(), "w");

	for (auto v : mesh->vertices) {
		fprintf(export_file, "v %f %f %f\n", v.position.x, v.position.y, v.position.z);
	}

	fprintf(export_file, "\n");

	fprintf(export_file, "o Struct001\n");
	fprintf(export_file, "g Struct001\n");
	for (int i = 0; i < mesh->indices.size(); i += 6) {
		fprintf(export_file, "f %d %d %d %d\n", mesh->indices[i] + 1, mesh->indices[i + 1] + 1, mesh->indices[i + 2] + 1, mesh->indices[i + 5] + 1);
	}

	fclose(export_file);
}

void Laborator1::Export3D2Obj2(string file_name, Mesh* lines, float width, float height) {
	FILE* export_file = fopen((file_name + ".OBJ").c_str(), "w");

	vector<VertexFormat> vertices;
	vector<unsigned int> indices;
	int vi = 1;

	for (int i = 0; i < lines->indices.size(); i += 2) {

		vec3 p1 = lines->vertices[lines->indices[i]].position;
		vec3 p2 = lines->vertices[lines->indices[i + 1]].position;

		vec3 tmp1 = vec3(p1.x, p2.y, p1.z);
		/*
			A=p1
			|\
			| \
			|  \c
		  b |   \D=tmp2
			| h_/\
			|/____\
	   tmp1=C  a  B=p2

			h = inaltimea
		*/


		float a2 = glm::distance2(p1, tmp1);
		float b2 = glm::distance2(tmp1, p2);
		float c2 = glm::distance2(p1, p2);
		float h2 = a2 * b2 / c2;
		float AD = sqrt(a2 - h2);
		float c = sqrt(c2);

		vec3 tmp2 = p1 + (p2 - p1) * AD / c2;

		vec3 front = glm::normalize(p2 - p1);
		vec3 up = glm::normalize(tmp2 - tmp1);
		vec3 right = glm::cross(front, up);

		vec3 tmp;
		tmp = p1 - up * (height / 2) - right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p1 - up * (height / 2) + right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p1 + up * (height / 2) + right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p1 + up * (height / 2) - right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);

		tmp = p2 - up * (height / 2) - right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p2 - up * (height / 2) + right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p2 + up * (height / 2) + right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
		tmp = p2 + up * (height / 2) - right * (width / 2);
		fprintf(export_file, "v %f %f %f\n", tmp.x, tmp.y, tmp.z);
	}

	fprintf(export_file, "\n");

	fprintf(export_file, "o Struct001\n");
	fprintf(export_file, "g Struct001\n");

	for (int i = 0; i < lines->indices.size(); i += 2) {

		// back
		fprintf(export_file, "f %d %d %d\n", vi, vi + 1, vi + 3);
		fprintf(export_file, "f %d %d %d\n", vi + 1, vi + 3, vi + 2);

		// bottom
		fprintf(export_file, "f %d %d %d\n", vi, vi + 1, vi + 4);
		fprintf(export_file, "f %d %d %d\n", vi + 1, vi + 4, vi + 5);

		// right
		fprintf(export_file, "f %d %d %d\n", vi + 1, vi + 2, vi + 5);
		fprintf(export_file, "f %d %d %d\n", vi + 2, vi + 5, vi + 6);

		// top
		fprintf(export_file, "f %d %d %d\n", vi + 3, vi + 2, vi + 7);
		fprintf(export_file, "f %d %d %d\n", vi + 2, vi + 7, vi + 8);

		// left
		fprintf(export_file, "f %d %d %d\n", vi, vi + 3, vi + 4);
		fprintf(export_file, "f %d %d %d\n", vi + 3, vi + 4, vi + 7);

		// front
		fprintf(export_file, "f %d %d %d\n", vi + 4, vi + 7, vi + 5);
		fprintf(export_file, "f %d %d %d\n", vi + 7, vi + 5, vi + 6);

		vi += 8;
	}

	fclose(export_file);
}

void Laborator1::Remesh() {
	delete[] joints[0];
	delete[] joints;
	delete lines;
	delete mesh3D;
	joints = InitJoints(n, lungime_bat);
	lines = CreateLinesFromJoints(joints, "struct", n);
	mesh3D = Create3DFromLines(lines, "mesh3D", width, height);
}

int Laborator1::CheckAngleVert(Joint** joints, Joint& this_joint)
{
	int i = this_joint.id_i;
	if (!(i - 1 >= 0 && i + 1 < n)) {
		return 0;
	}
	int j = this_joint.id_j;
	vec2 p1 = joints[i - 1][j].center;
	vec2 pc = joints[i][j].center;
	vec2 p2 = joints[i + 1][j].center;
	float vert_angle = GetAngle(p1, p2, pc);
	if (vert_angle < min_angle_vertical) {
		return 1; // angle is not ok
	}
	return 0; // angle is not ok
	// ignora ce e mai jos
	if (p1.y == pc.y) {
		if (p2.y < pc.y) {
			return -1; // e mai in sens trigonometric
		}
		else {
			return +1; // e mai in sens orar
		}
	}
	if (p1.x == pc.x) {
		if (p2.x > pc.x) {
			return -1; // e mai in sens trigonometric
		}
		else {
			return +1; // e mai in sens orar
		}
	}
	// a*x1 + b = y1;
	// a*xc + b = yc;
	float a = (p1.x - pc.x) / (p1.y - pc.y);
	float b = p1.y - a * p1.x;
	if (p2.x * a + b > p2.y) {
		return -1; // e mai in sens trigonometric
	}
	else {
		return 1; // e mai in sens orar
	}
}

unsigned int Laborator1::CheckAngleOrtho(Joint** joints, Joint& this_joint)
{
	bool exist[4];
	int i = this_joint.id_i;
	int j = this_joint.id_j;
	unsigned int result = 0;
	exist[0] = i - 1 >= 0;
	exist[1] = j + 1 < n;
	exist[2] = i + 1 < n;
	exist[3] = j - 1 >= 0;
	vec2 p[4];
	vec2 pc = this_joint.center;
	if (exist[0]) {
		p[0] = joints[i - 1][j].center;
	}
	if (exist[1]) {
		p[1] = joints[i][j + 1].center;
	}
	if (exist[2]) {
		p[2] = joints[i + 1][j].center;
	}
	if (exist[3]) {
		p[3] = joints[i][j - 1].center;
	}
	if (exist[0] && exist[1]) {
		float angle = GetAngle(p[0], p[1], pc);
		if (angle < min_angle_ortho) {
			result |= 1 << 1;
		}
		if (angle > max_angle_ortho) {
			result |= 1 << 2;
		}
	}
	if (exist[1] && exist[2]) {
		float angle = GetAngle(p[1], p[2], pc);
		if (angle < min_angle_ortho) {
			result |= 1 << 3;
		}
		if (angle > max_angle_ortho) {
			result |= 1 << 4;
		}
	}
	if (exist[2] && exist[3]) {
		float angle = GetAngle(p[2], p[3], pc);
		if (angle < min_angle_ortho) {
			result |= 1 << 5;
		}
		if (angle > max_angle_ortho) {
			result |= 1 << 6;
		}
	}
	if (exist[3] && exist[0]) {
		float angle = GetAngle(p[3], p[0], pc);
		if (angle < min_angle_ortho) {
			result |= 1 << 7;
		}
		if (angle > max_angle_ortho) {
			result |= 1 << 8;
		}
	}
	return result;
}

Joint** Laborator1::Repair(Joint** joints, float speed = 1)
{
	vec2** repair_mat = alloc_mat<vec2>(n);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vec2 repair = { 0, 0 };

			// dist corection based on h
			if (i - 1 >= 0) {
				repair += GetR(joints[i][j], joints[i - 1][j], speed);
			}
			if (i + 1 < n) {
				repair += GetR(joints[i][j], joints[i + 1][j], speed);
			}
			if (j - 1 >= 0) {
				repair += GetR(joints[i][j], joints[i][j - 1], speed);
			}
			if (j + 1 < n) {
				repair += GetR(joints[i][j], joints[i][j + 1], speed);
			}

			// angle based corection;
			int vert_problem = CheckAngleVert(joints, joints[i][j]);
			vec2 repair_dir = { 0,0 };
			if (vert_problem == -1) {// e prea in sesn trigonometric 
				repair_dir = -Repair_RepairAnglesDir(joints[i - 1][j].center, joints[i + 1][j].center,
						joints[i][j].center, repair_dir);
			}

			int ortho_problem = CheckAngleOrtho(joints, joints[i][j]);
			if (ortho_problem & 1<<1) {
				repair_dir = Repair_RepairAnglesDir(joints[i - 1][j].center, joints[i][j + 1].center, joints[i][j].center, repair_dir);
			}
			if (ortho_problem & 1 << 2) {
				repair_dir = Repair_RepairAnglesDir(joints[i - 1][j].center, joints[i][j + 1].center, joints[i][j].center, repair_dir, false);
			}
			if (ortho_problem & 1<<3) {
				repair_dir = Repair_RepairAnglesDir(joints[i + 1][j].center, joints[i][j + 1].center, joints[i][j].center, repair_dir);
			}
			if (ortho_problem & 1 << 4) {
				repair_dir = Repair_RepairAnglesDir(joints[i + 1][j].center, joints[i][j + 1].center, joints[i][j].center, repair_dir, false);
			}
			if (ortho_problem & 1<<5) {
				repair_dir = Repair_RepairAnglesDir(joints[i + 1][j].center, joints[i][j - 1].center, joints[i][j].center, repair_dir);
			}
			if (ortho_problem & 1 << 6) {
				repair_dir = Repair_RepairAnglesDir(joints[i + 1][j].center, joints[i][j - 1].center, joints[i][j].center, repair_dir, false);
			}
			if (ortho_problem & 1<<7) {
				repair_dir = Repair_RepairAnglesDir(joints[i - 1][j].center, joints[i][j - 1].center, joints[i][j].center, repair_dir);
			}
			if (ortho_problem & 1 << 8) {
				repair_dir = Repair_RepairAnglesDir(joints[i - 1][j].center, joints[i][j - 1].center, joints[i][j].center, repair_dir, false);
			}

			//repair -= repair_dir * repair_speed_angels * length(repair);// *repair_speed_angels* speed;

			repair_mat[i][j] = repair;
		}
	}

	// angle based correction


	// apply corection
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (!locked[i][j].x) {
				joints[i][j].center.x += repair_mat[i][j].x;
			}
			if (!locked[i][j].y) {
				joints[i][j].center.y += repair_mat[i][j].y;
			}
		}
	}

	free_mat(repair_mat);
	return joints;
}


vec2 Laborator1::Repair_RepairAnglesDir(vec2 p1, vec2 p2, vec2 pc, vec2 old_repair_dir) {
	return Repair_RepairAnglesDir(p1, p2, pc, old_repair_dir, true);
}
vec2 Laborator1::Repair_RepairAnglesDir(vec2 p1, vec2 p2, vec2 pc,vec2 old_repair_dir, bool to_c)
{
	vec2 mid = middle(p1, p2);
	vec2 dir = normalize(pc - mid); // from mid to joints[i][j].center
	if (to_c == false) {
		dir = -dir;
	}
	if (old_repair_dir == vec2(0, 0)) {
		old_repair_dir = dir;
	}
	else {
		old_repair_dir = complex_mul(old_repair_dir, dir);
	}
	return old_repair_dir;
}

vec2 Laborator1::GetR(Joint& me, Joint& other, float speed)
{
	vec2 r = other.center - me.center;
	vec2 nr = normalize(r);
	float len_r = length(r);
	float real_h = (other.h + me.h) / 2;
	float real_l = sqrt(len_r * len_r + real_h * real_h);
	vec2 r1 = nr * (real_l - lungime_bat - 1) / move_fractioner;
	return r1 * speed;
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template <typename T> T** alloc_mat(int n) {
	T** mat = new T * [n];
	mat[0] = new T[n * n];
	for (int i = 1; i < n; i++) {
		mat[i] = mat[i - 1] + n;
	}
	return mat;
}

float GetAngle(vec2 p1, vec2 p2, vec2 center)
{
	/*
	 * In unghiA, unghiB se retine unghiul dintre OA respectiv OB si o dreapta
	 * paralela cu abscisa ce trece prin O. unghiul OAB este diferenta acestor
	 * unghiuri
	 */
	float unghiFinal;

	/*
	unghiA = atan2(p1.y - center.y, p1.x - center.x);
	unghiB = atan2(p2.y - center.y, p2.x - center.x);
	unghiFinal = unghiA - unghiB;
	*/

	unghiFinal = GetAngleDiff(p1, p2, center);
	if (unghiFinal < 0) { /* unghiul este translatat intr-o valoare pozitiva */
		unghiFinal += 2 * AI_MATH_PI_F;
	}
	if (unghiFinal > AI_MATH_PI_F) {             /* unghiurile de peste 180 de grade */
		unghiFinal = 2 * AI_MATH_PI_F - unghiFinal;  /* sunt luate in sens orar*/
	}
	return unghiFinal;
}

float GetAngleDiff(vec2 p1, vec2 p2, vec2 center) {
	/*
	 * In unghiA, unghiB se retine unghiul dintre OA respectiv OB si o dreapta
	 * paralela cu abscisa ce trece prin O. unghiul OAB este diferenta acestor
	 * unghiuri
	 */
	float unghiA, unghiB;

	unghiA = atan2(p1.y - center.y, p1.x - center.x);
	unghiB = atan2(p2.y - center.y, p2.x - center.x);
	return unghiA - unghiB;

}

template <typename T>
void free_mat(T** mat)
{
	delete[] mat[0];
	delete[] mat;
}

vec2 middle(vec2 a, vec2 b) {
	vec2 c = a + b;
	c.x /= 2;
	c.y /= 2;
	return c;
}

vec2 complex_mul(vec2 a, vec2 b) {
	return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}