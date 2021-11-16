#include "Object2D.h"

#include <Core/Engine.h>

/*
Mesh* Object2D::CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color)
{
//	glm::vec3 center = center;
	int theta = 30;
	int steps = 13;

	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(center, color));
	for (int i = 0; i < steps; i++) {
		VertexFormat(center + glm::vec3(cos(i * theta), sin(i * theta), 0), color);
	}
	
	Mesh* circle = new Mesh(name);
	std::vector<unsigned int> indices;
	for (int i = 1; i < steps + 1; i++) {
		indices.push_back(i + 1);
		indices.push_back(0);
		indices.push_back(i);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}
*/

using std::vector;

Mesh* Object2D::CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color, bool fill)
{
	glm::vec3 corner = center;

	std::vector<VertexFormat> vertices;

	float tpi = glm::two_pi<float>();

	unsigned int cnt = 0;
	std::vector<unsigned int> indices;

	vertices.push_back(VertexFormat(center, color));
	for (float i = 0; i < tpi; i += tpi / 1000.0f) {
		vertices.push_back(VertexFormat(center + glm::vec3(radius * cos(i), radius * sin(i), 0), color));
	}

	Mesh* circle = new Mesh(name);

	if (!fill) {
		circle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		for (unsigned int i = 0; i < vertices.size() - 1; i++) {
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		indices.push_back(0);
		indices.push_back(vertices.size() - 1);
		indices.push_back(1);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}

Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* Object2D::CreatePower(std::string name, glm::vec3 center, float side, glm::vec3 color, bool fill) {

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(-side / 2, -side / 2, 0), color),
		VertexFormat(center + glm::vec3(side / 2, -side / 2, 0), color),
		VertexFormat(center + glm::vec3(side / 2, side / 2, 0), color),
		VertexFormat(center + glm::vec3(-side / 2, side / 2, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}
