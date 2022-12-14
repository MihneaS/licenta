#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/glm.h>

class GPUBuffers;
class Texture2D;

struct VertexFormat
{
	VertexFormat(glm::vec3 position, 
				glm::vec3 color = glm::vec3(1),
				glm::vec3 normal = glm::vec3(0, 1, 0),
				glm::vec2 text_coord = glm::vec2(0))
		: position(position), normal(normal), color(color), text_coord(text_coord) { };

	// position of the vertex
	glm::vec3 position;		

	// vertex normal
	glm::vec3 normal;

	// vertex texture coordinate
	glm::vec2 text_coord;

	// vertex color
	glm::vec3 color;
};

struct Material
{
	Material()
	{
		texture = nullptr;
	}

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emissive;
	float shininess;

	Texture2D* texture;
};

static const unsigned int INVALID_MATERIAL = 0xFFFFFFFF;

struct MeshEntry
{
	MeshEntry()
	{
		nrIndices = 0;
		baseVertex = 0;
		baseIndex = 0;
		materialIndex = INVALID_MATERIAL;
	}
	unsigned int nrIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
	unsigned int materialIndex;
};

namespace migine {
#define MIGINE_MESH_IDS \
MIGINE_X(plane, "plane"s)\
MIGINE_X(line, "line"s)\
MIGINE_X(box_wireframe, "boxWireframe"s) \
MIGINE_X(box, "box"s)\
MIGINE_X(sphere, "sphere"s)

#define MIGINE_X(id, name) id,
	enum class Mesh_id {
		MIGINE_MESH_IDS
	};
#undef MIGINE_X

	extern std::unordered_map<Mesh_id, const std::string> mesh_id_to_id_names;

}


class Mesh
{
	typedef unsigned int GLenum;

	public:
		Mesh(migine::Mesh_id id);
		virtual ~Mesh();

		void ClearData();

		// Initializes the mesh object using a VAO GPU buffer that contains the specified number of indices
		bool InitFromBuffer(unsigned int VAO, unsigned int nrIndices);

		// Initializes the mesh object and upload data to GPU using the provided data buffers
		bool InitFromData(std::vector<VertexFormat> vertices,
						std::vector<unsigned int>& indices);

		// Initializes the mesh object and upload data to GPU using the provided data buffers
		bool InitFromData(std::vector<glm::vec3>& positions,
						std::vector<glm::vec3>& normals,
						std::vector<unsigned int>& indices);

		// Initializes the mesh object and upload data to GPU using the provided data buffers
		bool InitFromData(std::vector<glm::vec3>& positions,
						std::vector<glm::vec3>& normals,
						std::vector<glm::vec2>& texCoords,
						std::vector<unsigned int>& indices);

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);

		void UseMaterials(bool value);

		// GL_POINTS, GL_TRIANGLES, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
		// GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY
		void SetDrawMode(GLenum primitive);
		GLenum GetDrawMode() const;

		void Render() const;

		const GPUBuffers* GetBuffers() const;
		migine::Mesh_id get_id() const;

	protected:
		void InitFromData();

		void InitMesh(const aiMesh* paiMesh);
		bool InitMaterials(const aiScene* pScene);
		bool InitFromScene(const aiScene* pScene);

	private:
		migine::Mesh_id id;
		glm::vec3 halfSize;
		glm::vec3 meshCenter;

	public:
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;

	protected:
		std::string fileLocation;

		bool useMaterial;
		GLenum glDrawMode;
		GPUBuffers *buffers;

		std::vector<MeshEntry> meshEntries;
		std::vector<Material*> materials;
};