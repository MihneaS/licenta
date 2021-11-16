#include "Laborator4.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include "Transform3D.h"

#include <math.h>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <stdio.h>
#include <set>
#include <tuple>

using namespace std;
using glm::vec3;
using glm::vec2;
using glm::distance;
using glm::distance2;
using glm::normalize;
using glm::dot;
using glm::cross;
using glm::length;

Laborator4::Laborator4()
{
}

Laborator4::~Laborator4()
{
}

void Laborator4::Init()
{
	fp_err = 0.000001;
	lungime_bat = 2.5;
	dy = 1.25;
	n = 4; // nodes per side
	width = 0.2;
	height = 0.08;
	raza_joc = 0.1;
	raza_joc2 = raza_joc * raza_joc;
	min_angle_vertical_deg = 165;
	min_angle_vertical = min_angle_vertical_deg * AI_MATH_PI_F / 180;
	min_angle_ortho_deg = 15;
	min_angle_ortho = min_angle_ortho_deg * AI_MATH_PI_F / 180;
	max_h_from_c = 0.5 / tan(min_angle_vertical);
	joints = InitJoints(n, dy, lungime_bat);
	lines = CreateLinesFromJoints(joints, "struct", dy, n);
	mesh3D = Create3DFromLines(lines, "mesh3D", width, height);
	locked_v = alloc_mat<bvec4>(n);
	locked_nodes = alloc_mat<bool>(n);
	tmp_joints = alloc_mat<Joint>(n);
	ang_coef = AI_MATH_PI_F / 2;

	miscarea_aia = vec2(0,0.01);
	le_f = 0.25;
	
}

void Laborator4::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator4::Update(float deltaTimeSeconds)
{

	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	modelMatrix = glm::mat4(1);
	
	//UpdateLinesFromJoints(lines, joints, n);
	//Update3DFromLines(lines, mesh3D, width, height);
	delete lines;
	delete mesh3D;
	lines = CreateLinesFromJoints(joints, "structura", dy, n);
	mesh3D = Create3DFromLines(lines, "mesh3d", width, height);


	RenderMesh(lines, shaders["VertexNormal"], modelMatrix);
}

void Laborator4::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator4::OnInputUpdate(float deltaTime, int mods)
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
}

void Laborator4::OnKeyPress(int key, int mods)
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
		Export3D2Obj("structura", lines); //TODO MAGIC STRING
	}
	if (key == GLFW_KEY_M)
	{
		for (int i = 0; i < le_f; i++)
		MoveJoint(0, 0, miscarea_aia / le_f);
	}
}

void Laborator4::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator4::OnWindowResize(int width, int height)
{
}

Joint** Laborator4::InitJoints(int n, float dy, float lungime_bat) {
	Joint** joints = alloc_mat<Joint>(n);

	// Pt dy = lungime_bat/2,  vom avea romburi cu unghiuri de 120 de grade, 60 de grade;
	if (dy == -1) {
		dy = lungime_bat / 2;
	}
	// TODO CLAMP INPUT
	float y1 = -dy/2;
	float y2 = dy/2;
	//float dx = sqrt(lungime_bat * lungime_bat - dy * dy) + raza_joc * 2;
	float dx = sqrt(lungime_bat * lungime_bat - dy * dy);	
	float dz = dx;

	float x0 = ((n - 1.0)/ 2) * dx * -1;
	float z0 = ((n - 1.0)/ 2) * dz;

	float x = x0;
	float z = z0;
	for (int i = 0; i < n; i++) {
		x = x0;
		for (int j = 0; j < n; j++) {
			joints[i][j] = { vec2(x,z), vec2(x, z + raza_joc), vec2(x + raza_joc, z), vec2(x, z - raza_joc), vec2(x - raza_joc, z), i, j};
			x += dx;
		}
		z -= dz;
	}

	return joints;
}

Mesh* Laborator4::CreateLinesFromJoints(Joint** joints, string name, float dy, int n) {
	Mesh* mesh = new Mesh(name);
	mesh->SetDrawMode(GL_LINES);

	vector<VertexFormat> vertices;
	vector<unsigned int> indices;

	float y1 = -dy / 2;
	float y2 = dy / 2;

	for (int i = 0, vi = 0; i < n; i++) {
		for (int j = 0; j < n; j++, vi += vs_in_node) {
			int k = 0;
			for (k = 0; k < vs_in_node / 2; k++) {
				//vertices.push_back(VertexFormat(vec3(joints[i][j].v[k].x, y1, joints[i][j].v[k].y)));
				vertices.push_back(VertexFormat(vec3(joints[i][j].center.x, y1, joints[i][j].center.y)));
			}
			for (k = 0; k < vs_in_node / 2; k++) {
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
				indices.push_back(vi - vs_in_node * n + 6);

				indices.push_back(vi + 4);
				indices.push_back(vi - vs_in_node * n + 2);
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

Mesh* Laborator4::UpdateLinesFromJoints(Mesh* lines, Joint** joints, int n) {

	for (int i = 0, vi = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < vs_in_node; k++) {
				lines->vertices[i + k + j * vs_in_node * n].position.x = joints[i][j].v[k % (vs_in_node / 2)].x;
				lines->vertices[i + k + j * vs_in_node * n].position.z = joints[i][j].v[k % (vs_in_node / 2)].y;
				//lines->vertices[i + k + j * vs_in_node * n].position.x = joints[i][j].center.x;
				//lines->vertices[i + k + j * vs_in_node * n].position.z = joints[i][j].center.y;
			}
		}
	}

	return lines;
}

Mesh* Laborator4::Create3DFromLines(Mesh* lines, std::string name, float width, float height) {
	Mesh* result = new Mesh(name);
	result->SetDrawMode(GL_TRIANGLES);

	vector<VertexFormat> vertices;
	vector<unsigned int> indices;
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

Mesh* Laborator4::Update3DFromLines(Mesh* lines, Mesh* mesh3D, float width, float height) {

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

void Laborator4::ExportLines2Obj(std::string file_name, Mesh* mesh) {
	FILE* export_file = fopen((file_name + ".OBJ").c_str(), "w");

	for (auto v : mesh->vertices) {
		fprintf(export_file, "v %f %f %f\n", v.position.x, v.position.y, v.position.z);
	}

	fprintf(export_file, "\n");

	fprintf(export_file, "o Struct001\n");
	fprintf(export_file, "g Struct001\n");
	for (int i = 0; i < mesh->indices.size(); i+=2) {
		fprintf(export_file, "l %d %d \n", mesh->indices[i]+1, mesh->indices[i+1]+1);
	}

	fclose(export_file);
}

void Laborator4::Export3D2Obj(std::string file_name, Mesh* mesh) {
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

void Laborator4::Remesh() {
	delete[] joints[0];
	delete[] joints;
	delete lines;
	delete mesh3D;
	joints = InitJoints(n, dy, lungime_bat);
	lines = CreateLinesFromJoints(joints, "struct", dy, n);
	mesh3D = Create3DFromLines(lines, "mesh3D", width, height);
}



Joint** Laborator4::MoveJoint(int i, int j, vec2 delta) {
	PrintJoints("Begin", false);

	set<ivec2, ivec2_cmp>* moved_joints = new set<ivec2, ivec2_cmp>;
	set<ivec2, ivec2_cmp>* joints_moving = new set<ivec2, ivec2_cmp>;
	set<ivec2, ivec2_cmp>* neighbours = new set<ivec2, ivec2_cmp>;
	set<ivec2, ivec2_cmp>* aux_set;
	//static vec2 last_delta = { 0,0 }; // used for cacheing

	//if (delta != last_delta) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				locked_nodes[i][j] = false;
			}
		}
	//}
	locked_nodes[i][j] = true;
	//last_delta = delta;

	bool redo = false;
	do {
		redo = false;

		moved_joints->clear();
		joints_moving->clear();
		neighbours->clear();

		// fill tmp_joints
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				tmp_joints[i][j] = joints[i][j];
			}
		}

		//compute locked_v and joints_moving
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				for (int k = 0; k < 4; k++) {
					locked_v[i][j][k] = false;
				}
			}
		}
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (locked_nodes[i][j]) {
					joints_moving->insert(ivec2(i,j));
					if (i - 1 >= 0 && locked_nodes[i - 1][j]) {
						locked_v[i][j][3] = true;
						locked_v[i - 1][j][1] = true;
					}
					if (j + 1 < n && locked_nodes[i][j + 1]) {
						locked_v[i][j][2] = true;
						locked_v[i][j + 1][0] = true;
					}
					if (i + 1 < n && locked_nodes[i + 1][j]) {
						locked_v[i][j][1] = true;
						locked_v[i + 1][j][3] = true;
					}
					if (j - 1 >= 0 && locked_nodes[i][j - 1]) {
						locked_v[i][j][0] = true;
						locked_v[i][j - 1][2] = true;
					}
				}
			}
		}

		while (!redo && !joints_moving->empty()) {
			for (auto pair : *joints_moving) {
				int i = pair[0];
				int j = pair[1];
				int moved_neighbours = 0;
				if (i - 1 >= 0 && contains(moved_joints, ivec2(i - 1,j))) {
					moved_neighbours++;
				}
				if (j + 1 < n && contains(moved_joints, ivec2(i, j + 1))) {
					moved_neighbours++;
				}
				if (i + 1 < n && contains(moved_joints, ivec2(i + 1, j))) {
					moved_neighbours++;
				}
				if (j - 1 >= 0 && contains(moved_joints, ivec2(i, j - 1))) {
					moved_neighbours++;
				}
				bool blocked = false;
				switch (moved_neighbours) {
					case 0:
 						MoveJoint0(joints[i][j], tmp_joints[i][j], delta, locked_v[i][j]);
						break;
					case 1:
						MoveJoint1(joints[i][j], tmp_joints[i][j]);
						break;
					case 2:
						blocked = MoveJoint2(joints[i][j], tmp_joints[i][j]);
						break;
					case 3:
						blocked = MoveJoint3(joints[i][j], tmp_joints[i][j]);
						break;
					case 4:
						blocked = MoveJoint4(joints[i][j], tmp_joints[i][j]);
						break;
				}
				if (blocked) {
					if (locked_nodes[i][j]) {
						ScuzeMan(__FILE__, __LINE__, "Cacat... nu trebuia sa se ajunga aici\nProbabil s-a blocat de tot");
					}
					bool found_locked = false;
					for (int ii = i - 1; ii <= i + 1; ii++) {
						for (int jj = j - 1; jj <= j + 1; jj++) {
							if (ii >= 0 && ii < n && jj >= 0 && jj < n && contains(moved_joints, ivec2(ii, jj))) {
								if (locked_nodes[ii][jj]) {
									found_locked = true;
								}
								locked_nodes[ii][jj] = true;
							}
						}
					}
					locked_nodes[i][j] = true;
					if (!found_locked) {
						ScuzeMan(__FILE__, __LINE__, "Asa cum am gandit simularea, de aici incolo va fi ireala\nNu stiu daca se va observa");
					}
					redo = true;
					break;
				}
			}

			// compute neighbours
			for (auto pair : *joints_moving) {
				int i = pair[0];
				int j = pair[1];
				if (i - 1 >= 0) {
					ivec2 tmp(i-1,j);
					if (!contains(joints_moving, tmp) && !contains(moved_joints, tmp)) {
						neighbours->insert(tmp);
					}
				}
				if (i + 1 < n) {
					ivec2 tmp(i + 1, j);
					if (!contains(joints_moving, tmp) && !contains(moved_joints, tmp)) {
						neighbours->insert(tmp);
					}
				}
				if (j - 1 >= 0) {
					ivec2 tmp(i, j - 1);
					if (!contains(joints_moving, tmp) && !contains(moved_joints, tmp)) {
						neighbours->insert(tmp);
					}
				}
				if (j + 1 < n) {
					ivec2 tmp(i, j + 1);
					if (!contains(joints_moving, tmp) && !contains(moved_joints, tmp)) {
						neighbours->insert(tmp);
					}
				}
			}

			// adjust neighbours
			for (auto pair : *joints_moving) {
				AdjustJointsNeighbours(tmp_joints, pair[0], pair[1], neighbours);
			}

			moved_joints->insert(joints_moving->begin(), joints_moving->end());
			aux_set = joints_moving;
			joints_moving = neighbours;
			neighbours = aux_set;
			neighbours->clear();
		}
		if (!redo) {
			Joint** aux = tmp_joints;
			tmp_joints = joints;
			joints = aux;
		}
	} while (redo);

	//Joint** aux = tmp_joints;
	//tmp_joints = joints;
	//joints = aux;

	PrintJoints("end", false);

	return joints;
}

void Laborator4::MoveJoint0(const Joint& old, Joint& next, vec2 dist, bvec4 locked)
{
	next.center = old.center + dist;
	for (int i = 0; i < 4; i++) {
		if (!locked[i]) {
			float h = GetDistFromPointToLine(old.center, dist, old.v[i]);
			float deltaAngle = ang_coef * h / raza_joc;
			float oldAngle = atan2(old.v[i].y - old.center.y, old.v[i].x - old.center.x);
			float nextAngle = oldAngle + deltaAngle;
			next.v[i] = next.center + vec2(cos(nextAngle), sin(nextAngle)) * raza_joc;
		}
		else {
			next.v[i] = old.v[i] + dist;
		}
	}
	bool new_lock = false;
	do {
		new_lock = false;
		if (!locked[0] || !locked[2]) {
			if (GetAngle(next.v[0], next.v[2], next.center) < min_angle_vertical) {
				locked[0] = true;
				locked[2] = true;
				new_lock = true;
				next.v[0] = old.v[0] + dist;
				next.v[2] = old.v[2] + dist;
			}
		}
		if (!locked[0] || !locked[1]) {
			if (GetAngle(next.v[0], next.v[1], next.center) < min_angle_ortho) {
				locked[0] = true;
				locked[1] = true;
				new_lock = true;
				next.v[0] = old.v[0] + dist;
				next.v[1] = old.v[1] + dist;
			}
		}if (!locked[0] || !locked[3]) {
			if (GetAngle(next.v[0], next.v[3], next.center) < min_angle_ortho) {
				locked[0] = true;
				locked[3] = true;
				new_lock = true;
				next.v[0] = old.v[0] + dist;
				next.v[3] = old.v[3] + dist;
			}
		}if (!locked[2] || !locked[1]) {
			if (GetAngle(next.v[2], next.v[1], next.center) < min_angle_ortho) {
				locked[2] = true;
				locked[1] = true;
				new_lock = true;
				next.v[2] = old.v[2] + dist;
				next.v[1] = old.v[1] + dist;
			}
		}if (!locked[2] || !locked[3]) {
			if (GetAngle(next.v[2], next.v[3], next.center) < min_angle_ortho) {
				locked[2] = true;
				locked[3] = true;
				new_lock = true;
				next.v[2] = old.v[2] + dist;
				next.v[3] = old.v[3] + dist;
			}
		}
	} while (new_lock);
}

void Laborator4::MoveJoint1(const Joint& old, Joint& next)
{
	int i1 = -1;
	bvec4 locked = { false, false, false, false };

	for (int i = 0; i < 4; i++) {
		if (old.v[i] != next.v[i]) {
				i1 = i;
				break;
		}
	}
	//if (i1 == -1) {
	//	return;
	//}

	vec2 c2v1 = normalize(next.v[i1] - old.center);
	Joint tmp = old;
	tmp.v[i1] = tmp.center + c2v1 * raza_joc;

	int delta_angle_sign = sgn(GetAngleDiff(tmp.v[i1], old.v[i1], old.center));
	AdjustRotatingVs(tmp, i1, delta_angle_sign);
	
	locked[i1] = true;
	vec2 dist = next.v[i1] - old.v[i1];
	MoveJoint0(tmp, next, dist, locked);
}

void Laborator4::AdjustRotatingVs(Joint& joint, int start_i, int delta_angle_sign)
{
	if (delta_angle_sign != 0) {
		int i = start_i;
		for (int j = 0; j < 3; j++) {
			int next_i = (i + delta_angle_sign + 4) % 4;
			AdjustRotatingVs_Adjust2Vertices(joint, i, next_i, min_angle_ortho, delta_angle_sign);
			if ((i == 0 || i == 2) && j < 2) {
				int i_opus = (i + 2) % 4;
				AdjustRotatingVs_Adjust2Vertices(joint, i, i_opus, min_angle_vertical, delta_angle_sign);
			}
			i = next_i;
		}
	}
}

void Laborator4::AdjustRotatingVs_Adjust2Vertices(Joint& joint, int i_static, int i_movable, float min_angle, int delta_angle_sign)
{
	if (GetAngle(joint.v[i_static], joint.v[i_movable], joint.center) < min_angle) {
		vec2 complex_angle = vec2(cos(min_angle_vertical * delta_angle_sign),
			sin(min_angle_vertical * delta_angle_sign));
		joint.v[i_static] = joint.center + complex_mul(joint.v[i_static] - joint.center, complex_angle);
	}
}

int Laborator4::MoveJoint2(const Joint& old, Joint& next) 
{
	int i1 = -1, i2 = -1;
	for (int i = 0; i < 4; i++) {
		if (old.v[i] != next.v[i]) {
			if (i1 == -1) {
				i1 = i;
			}
			else {
				i2 = i;
			}
		}
	}
	//if (i2 == -1) {
	//	MoveJoint1(old, next);
	//	return 0; // no error, circle not locked
	//}

	if (distance2(next.v[i1], next.v[i2]) > 4 * raza_joc2) {
		return 1; // circle is locked
		// LOCK cicle
	}
	vec2 c1, c2;
	MoveJoint2_FindCircles(next.v[i1], next.v[i2], c1, c2);
	float i1c1i2_angle = GetAngle(next.v[i1], next.v[i2], c1);
	if ((i1 + 2) % 4 != i2) {
		if (!CheckAngleOrtho(i1c1i2_angle)) {
			return 1; // circle is locked
			// TODO if angle too big, lock i1 i2 + vertical (else only i1, i2)
		}
	}
	else {
		if (i1c1i2_angle < min_angle_vertical) {
			return 1; // circle is locked
			// LOCK cicle
		}
	}

	//decide what is the best p
	vec2 mid_old = middle(old.v[i2], old.v[i1]);
	vec2 mid_next = middle(next.v[i1], next.v[i2]);
	vec2 c2m_old = mid_old - old.center;
	vec2 c2m_p1 = mid_next - c1;
	float decide_factor = dot(c2m_old, c2m_p1);
	vec2 next_c;
	if (decide_factor > 0) {
		next_c = c1;
	}
	else if (decide_factor < 0) {
		next_c = c2;
	}
	else {
		vec2 mid1 = middle(old.v[i1], next.v[i1]);
		vec2 mid2 = middle(old.v[i2], next.v[i2]);
		vec2 mp1, mp2;
		MoveJoint2_FindCircles(mid1, mid2, mp1, mp2);
		vec2 midmid = middle(mid1, mid2);
		vec2 decide_vec;
		if (dot(c2m_old, midmid - mp1) > 0) {
			decide_vec = midmid - mp1;
		}
		else {
			decide_vec = midmid - mp2;
		}
		if (dot(decide_vec, c2m_p1) > 0) {
			next_c = c1;
		}
		else {
			next_c = c2;
		}
	}

	int das1 = sgn(GetAngleDiff(next.v[i1], old.v[i1], next_c));
	int das2 = sgn(GetAngleDiff(next.v[i2], old.v[i2], next_c));
	next.center = next_c;
	Joint tmp = old;
	for (int i = 0; i < 4; i++) {
		if (i == i1 || i == i2) {
			tmp.v[i] = next.v[i] - next.center + tmp.center;
		}
	}

	if (das1 == 0) {
		AdjustRotatingVs(tmp, i2, das2);
	}
	else if (das2 == 0) {
		AdjustRotatingVs(tmp, i1, das1);
	}
	else if (das1 == das2) {
		int ni1 = (i1 + das1 + 4) % 4;
		if (i2 == ni1) {
			AdjustRotatingVs(tmp, i1, das1);
		}
		else {
			AdjustRotatingVs(tmp, i2, das2);
		}
	}
	else {
		if ((i1 + 2) % 4 == i2) {
			int other_i = (i1 + das1) % 4;
			AdjustRotatingVs_Adjust2Vertices(tmp, i1, other_i, min_angle_ortho, das1);
			AdjustRotatingVs_Adjust2Vertices(tmp, i2, other_i, min_angle_ortho, das2);
		}
		else {
			int ni1 = (i1 + das1 + 4) % 4;
			if (i2 == ni1) {
				int this_i;
				int i_opus;
				int das;
				if ((i1 == 0 || i1 == 2)) {
					this_i = i1;
					i_opus = (i1 + 2) % 4;
					das = das1;
				}
				else {
					this_i = i2;
					i_opus = (i2 + 2) % 4;
					das = das2;
				}
				AdjustRotatingVs_Adjust2Vertices(tmp, this_i, i_opus, min_angle_vertical, das);
				int last_i = (i_opus + das + 4) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, i_opus, last_i, min_angle_ortho, das);
			}
			else {
				int next_i = (i1 + das1 + 4) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, i1, next_i, min_angle_ortho, das1);
				int next_next_i = (next_i + das1 + 4) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, next_i, next_next_i, min_angle_ortho, das1);


				next_i = (i2 + das2 + 4) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, i2, next_i, min_angle_ortho, das2);
				next_next_i = (next_i + das2 + 4) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, next_i, next_next_i, min_angle_ortho, das2);

				int i_vert;
				int das_vert;
				if (i1 == 0 || i1 == 2) {
					i_vert = i1;
					das_vert = das1;
				}
				else {
					i_vert = i2;
					das_vert = das2;
				}
				int i_opus = (i_vert + 2) % 4;
				AdjustRotatingVs_Adjust2Vertices(tmp, i_vert, i_opus, min_angle_vertical, das_vert);
			}
		}
	}

	bvec4 locked;
	locked[i1] = true;
	locked[i2] = true;
	vec2 dist = next.center - old.center;
	MoveJoint0(tmp, next, dist, locked);

	return 0; // no error, circle not locked
}

void Laborator4::MoveJoint2_FindCircles(vec2 next_v1, vec2 next_v2, vec2& out1, vec2& out2)
{
	/*	p1, p2 sunt intersectiile cercurilor de raza R cu centrul in v1 si v2
		tmp e lungimea de la m la p1
				p1
			   /|\
			  / | \
			 /  |  \ R = raza_joc
			/   |tmp\
		   /    |    \
		v1/_____m_____\ v2
		  \     |     /
		   \    |    /
			\   |   /
			 \  |  /
			  \ | /
			   \|/
				p2
	*/

	vec2 v1_v2 = next_v2 - next_v1;
	float len_v1_v2 = length(v1_v2);
	float tmp = sqrt(raza_joc2 - len_v1_v2 * len_v1_v2 / 4);
	v1_v2 = normalize(v1_v2);
	// m2p1 = (v1_v2 * sqrt(-1)) * tmp
	vec2 m2p1 = vec2(-v1_v2.y, v1_v2.x) * tmp;
	vec2 mid = next_v2 + next_v1;
	mid.x /= 2;
	mid.y /= 2;
	out1 = mid + m2p1;
	out2 = mid - m2p1;
}

int Laborator4::MoveJoint3(const Joint& old, Joint& next)
{
	int i1 = -1, i2 = -1, i3 = -1, i_liber = -1;
	bvec4 liber = { true, true, true, true };
	for (int i = 0; i < 4; i++) {
		if (old.v[i] != next.v[i]) {
			liber[i] = false;
			if (i1 == -1) {
				i1 = i;
			}
			else  if (i2 == -1) {
				i2 = i;
			}
			else {
				i3 = i;
			}
		}
		else {
			i_liber = i;
		}
	}
	//if (i_liber == -1) {
	//	return MoveJoint4(old, next);
	//}
	//if (i3 == -1) {
	//	return MoveJoint2(old, next);
	//}

	vec2 c1, c2;
	MoveJoint2_FindCircles(next.v[i1], next.v[i2], c1, c2);
	float i1c1i2_angle = GetAngle(next.v[i1], next.v[i2], c1);
	if (abs(distance2(c1, next.v[i3]) - raza_joc2) < fp_err) {
		next.center = c1;
	}
	else if (abs(distance2(c2, next.v[i3]) - raza_joc2) < fp_err) {
		next.center = c2;
	}
	else {
		return 1; // circle is locked
	}
	
	Joint tmp = old;
	for (int i = 0; i < 4; i++) {
		if (!liber[i]) {
			tmp.v[i] = next.v[i] - next.center + tmp.center;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (!liber[i]) {
			int dasi = sgn(GetAngleDiff(next.v[i], old.v[i], next.center));
			if (i == 0 || i == 2) {
				int i_opus = (i + 2) % 4;
				float ang = GetAngle(next.v[i], next.v[i_opus], next.center);
				if (ang < min_angle_vertical) {
					if (!liber[i_opus]) {
						return 1; // circle is locked
						// LOCK NODE
					}
					else {
						AdjustRotatingVs_Adjust2Vertices(tmp, i, i_opus, min_angle_vertical, dasi);
					}
				}
			}
			int next_i = (i + dasi + 4) % 4;
			if (liber[next_i]) {
				AdjustRotatingVs_Adjust2Vertices(tmp, i, next_i, min_angle_ortho, dasi);
			}
			else if (!CheckAngleOrtho(GetAngle(tmp.v[i], tmp.v[next_i], tmp.center))) {
				return 1; // circle is locked
				// LOCK NODE
			}
		}
	}
	bvec4 locked;
	for (int i = 0; i < 4; i++) {
		locked[i] = !liber[i];
	}
	vec2 dist = next.center - tmp.center;
	MoveJoint0(tmp, next, dist, locked);
	return 0; // no error, circle not locked
}

int Laborator4::MoveJoint4(const Joint& old, Joint& next)
{
	vec2 c1, c2;
	MoveJoint2_FindCircles(next.v[0], next.v[1], c1, c2);
	if (abs(distance2(next.v[2], c1) - raza_joc2) < fp_err) {
		if (abs(distance2(next.v[3], c1) - raza_joc2) < fp_err) {
			next.center = c1;
		} else {
			return 1; // circle is locked
			// LOCK circle
		}
	}
	else if (abs(distance2(next.v[2], c2) - raza_joc2) < fp_err) {
		if (abs(distance2(next.v[3], c2) - raza_joc2) < fp_err) {
			next.center = c2;
		}
		else {
			return 1; // circle is locked
			// LOCK circle
		}
	}

	float vert_angle = GetAngle(next.v[0], next.v[1], next.center);
	if (vert_angle < min_angle_vertical) {
		return 1; // circle is locked
		// LOCK circle;
	}
	for (int i = 0; i < 4; i++) {
		int next_i = (i + 1) % 4;
		float angle = GetAngle(next.v[i], next.v[next_i], next.center);
		if (angle < min_angle_ortho) {
			return 1; // circle is locked
			//Lock circle
		}
	}
	return 0; // no error, circle not locked
}

bool Laborator4::CheckAngleOrtho(float angle)
{
	return angle > min_angle_ortho || angle < 2 * AI_MATH_PI_F - min_angle_ortho - min_angle_vertical;
}

//obsolete
void  Laborator4::AdjustVerticies(Joint& joint, vec2 delta) {
/*	int no_locked_vertices = 0;
	bool outside[4] = {false};
	if (delta != vec2(0,0)) {
		joint.center += delta;
	}
	else {
		bool outside[4];
		for (int i = 0; i < 4; i++) {
			outside[i] = distance2(joint.center, joint.v[i]) > raza_joc2;
		}
		int vs_out = outside[0] + outside[1] + outside[2] + outside[3];
		no_locked_vertices = vs_out;
		if (vs_out == 0) {
			return;
		}
		if (vs_out == 1) {
			int i;
			for (i = 0; i < 4; i++) {
				if (outside[i]) {
					break;
				}
			}
			vec2 v2c = normalize(joint.center - joint.v[i]);
			joint.center = joint.v[i] + v2c * raza_joc;
			// TODO calcule cu 1 locked
		}
		else if (vs_out == 2) {
			vec2 v1, v2;
			bool first = true;
			for (int i = 0; i < 4; i++) {
				if (outside[i]) {
					if (first) {
						v1 = joint.v[i];
						first = false;
					}
					else {
						v2 = joint.v[i];
					}
				}
			}
			if (distance2(v1, v2) > raza_joc2) {
				// TODO this joint is locked
				// TODO return something
			}

			// bring the center to the mid of the two points outside, until both are inside <=>
			// <=> the furthest away is on the limit of the circle
			float d1 = distance(v1, joint.center);
			float d2 = distance(v2, joint.center);
			float dmax = d1 > d2 ? d1 : d2;
			vec2 mid = v1 + v2;
			mid.x /= 2;
			mid.y /= 2;
			vec2 c2mid = normalize (mid - joint.center);
			float d_to_move = distance(joint.center, mid) * dmax / raza_joc;
			joint.center += c2mid * d_to_move;


			// TODO calcule cu 2 locked
			// se paote ca acest joint sa fie blocat
		}
		else {
			// TODO acest joint e locked
			// TODO WARNING, this is not supposed to happen
			// return something
		}
	}
	bool locked_vertices[4] = { outside[0], outside[1], outside[2], outside[3] };
	
	// bring all vertices back into the joint (the joint is a circle)
	for (int i = 0; i < 4; i++) {
		if (distance(joint.v[i], joint.center) > raza_joc) {
			vec2 cv = normalize(joint.v[i] - joint.center);
			joint.v[i] = joint.center + cv * raza_joc;
		}
	}
	// if they all have the same coordinates (they start like this) no further adjusments are needed
	if (joint.v[0] == joint.v[1] && joint.v[1] == joint.v[2] && joint.v[2] == joint.v[3]
			|| joint.v[0] == joint.v[2] && joint.v[1] == joint.v[3]) {
		joint.v[0] = joint.v[1] = joint.v[2] = joint.v[3] = joint.center;
		return;
	}
	// if (read the condition below) move the vertices so that
	// their mid will be the the center of the joint
	// or the center will be their mid
	if (joint.v[0] == joint.v[1] && joint.v[2] == joint.v[3]
			|| joint.v[0] == joint.v[3] && joint.v[1] == joint.v[2]) {
		vec2 mid = (joint.v[0] + joint.v[2]);
		mid.x /= 2;
		mid.y /= 2;
		if (locked_vertices == 0) {
			vec2 mid2c = joint.center - mid;
			for (int i = 0; i < 4; i++) {
				joint.v[i] += mid2c;
			}
		}
		else {
			joint.center = mid;
		}
		return;
	}
	// else, for each opposing pair
	if (no_locked_vertices == 0) {
		AdjustOpposingPair(joint, 0, 2, false);
		AdjustOpposingPair(joint, 1, 3, false);
	}
	else if (no_locked_vertices == 1 // assert(no_locked_vertices == 1 sau 2)
		|| locked_vertices[0] && locked_vertices[2]
		|| locked_vertices[1] && locked_vertices[3]) {
		if (locked_vertices[0] || locked_vertices[2]) {
			AdjustOpposingPair(joint, 0, 2, true);
			AdjustOpposingPair(joint, 1, 3, false);
		}
		else {
			AdjustOpposingPair(joint, 1, 3, true);
			AdjustOpposingPair(joint, 0, 2, false);

		}

	}
	else {
		// TODO this one is locked;
		// return something
	}
	*/
}

// obsolete
void Laborator4::AdjustOpposingPair(Joint& joint, int i, int ip2, bool locked) {// subfunction of AdjustVerticies. DO NOT USE OUTSIDE
/*	if (joint.v[i] == joint.v[ip2]) {
		if (locked) {
			joint.center = joint.v[i];
		}
		else {
			joint.v[i] = joint.v[ip2] = joint.center;
		}
	}
	// else 
	else {
		// move the vertices/center so that they will form un triunghi isoscel with the center (CV0 = CV2)
		vec2 mid = (joint.v[i] + joint.v[ip2]);
		mid.x /= 2;
		mid.y /= 2;
		vec2 mid2c = joint.center - mid;
		vec2 v2_v0 = normalize(joint.v[i] - joint.v[ip2]);
		vec2 mid2v0 = dot(mid2c, v2_v0) * v2_v0;
		if (locked) {
			joint.center -= mid2v0;
		}
		else {
			joint.v[i] += mid2v0;
			joint.v[ip2] += mid2v0;
		}
		// if the height of the new triangle is  to high (pt ca unghiul e prea ascutit), move the vertices
		// closer to the center in the direction of the height (or the center to the vertices
		float max_h_from_c_here = max_h_from_c * distance(joint.v[i], joint.v[ip2]);
		float h = distance(mid, joint.center);
		if (h > max_h_from_c_here) {
			mid2c = normalize(mid2c) * (h - max_h_fom_c_here);
			if (locked) {
				joint.center -= mid2c;
			}
			else {
				joint.v[i] += mid2c;
				joint.v[ip2] += mid2c;
			}
		}
	}
	*/
}
//TODO AdjustJointCenter(Joint& joint)


template <typename T>
void Laborator4::AdjustJointsNeighbours(Joint** joints, int i, int j, set<ivec2, T>* neighbours) {
	if (i - 1 >= 0 && contains(neighbours, ivec2(i-1,j))) {
		AdjustJointsNeighbour(joints[i][j].v[0], joints[i - 1][j].v[2]);
	}
	if (j + 1 < n && contains(neighbours, ivec2(i, j + 1))) {
		AdjustJointsNeighbour(joints[i][j].v[1], joints[i][j + 1].v[3]);
	}
	if (i + 1 < n && contains(neighbours, ivec2(i + 1, j))) {
		AdjustJointsNeighbour(joints[i][j].v[2], joints[i + 1][j].v[0]);
	}
	if (j - 1 >= 0 && contains(neighbours, ivec2(i, j - 1))) {
		AdjustJointsNeighbour(joints[i][j].v[3], joints[i][j - 1].v[1]);
	}
}

void Laborator4::AdjustJointsNeighbour(vec2 v, vec2& neighbour) {
	vec2 v2neig = normalize(neighbour - v);
	neighbour = v + v2neig * lungime_bat;
}

float Laborator4::GetAngle(vec2 p1, vec2 p2, vec2 center)
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

float Laborator4::GetAngleDiff(vec2 p1, vec2 p2, vec2 center) {
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

float Laborator4::GetDistFromPointToLine(vec2 linePoint, vec2 lineDirection, vec2 otherPoint)
{
	vec3 lp = { linePoint.x, linePoint.y, 0 };
	vec3 ld = { lineDirection.x, lineDirection.y, 0 };
	vec3 op = { otherPoint.x, otherPoint.y, 0 };

	return length(cross(op-lp,ld))/length(lineDirection);
}

void Laborator4::PrintJoints(char* msg, bool stop)
{
	printf("%s\n", msg);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("[%d %d]\n", i, j);
			printf("center x:%.2f y:%.2f\n", joints[i][j].center.x, joints[i][j].center.y);
			printf("  v[0] x:%.2f y:%.2f\n", joints[i][j].v[0].x, joints[i][j].v[0].y);
			printf("  v[1] x:%.2f y:%.2f\n", joints[i][j].v[1].x, joints[i][j].v[1].y);
			printf("  v[2] x:%.2f y:%.2f\n", joints[i][j].v[2].x, joints[i][j].v[2].y);
			printf("  v[3] x:%.2f y:%.2f\n", joints[i][j].v[3].x, joints[i][j].v[3].y);
			printf("\n");
		}
	}
	if (stop) {
		char c;
		scanf("%c", &c);
	}

	printf("\n");
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

template <typename T1, typename T2 > bool contains(T1* container, T2 item) {
	return container->find(item) != container->end();
}

void ScuzeMan(char* file, int line, char* details) {
	static char a;
	fprintf(stderr,"[%s:%d]:\nScuze Man, sunt chestii de care mi-am dat seama pe parcurs\n", file, line);
	fprintf(stderr,"%s\n", details);
	if (a != 'y' && a != 'Y') {
		fprintf(stderr,"Introdu o litera sa continui. Daca introduci Y nu se mai opri programul cand apare acest mesaj in timpul acestei rulari\n");
		fprintf(stderr,"P.S. dar se poate sa se opreasca simularea daca pogramul intra intr-un loop infinit\n");
		scanf("%c", &a);
	}
}

bool ivec2_cmp::operator()(const ivec2& lhs, const ivec2& rhs) const
{
	if (lhs[0] < rhs[0]) {
		return true;
	}
	if (lhs[0] > rhs[0]) {
		return false;
	}
	if (lhs[1] < rhs[1]) {
		return true;
	}
	return false;
}
