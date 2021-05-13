#pragma once

#include "vector.h"
#include "structs.h"
#include <vector>

class Material;

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

public:
    // clears the mesh, deleteing all data
    void Clear();
    // returns the number of vertices in the mesh
    unsigned int GetVertexCount();
    // sets the the size fo the vertex array - for internal use
    void SetVertexArraySize(unsigned int value);
    // returns the number of triangles in the mesh
    unsigned int GetTriangleCount();
    // sets the size of the triangle array - for internal use
    void SetTriangleArraySize(unsigned int value);
    // returns given vertex    
    const Vector3& GetVertex(unsigned int index);
    // returns the given normal
    const Vector3& GetNormal(unsigned int index);
    // returns the given texture coordinates vector
    const Vector2& GetUV(unsigned int index);
    // returns the pointer to the array of tangents
    const Vector3& GetTangent(unsigned int index);
    // returns the pointer to the array of binormals
    const Vector3& GetBinormal(unsigned int index);
    // sets the vertex at a given index to "vec" - for internal use    
    void SetVertex(const Vector3 &vec, unsigned int index);
    // sets the normal at a given index to "vec" - for internal use    
    void SetNormal(const Vector3 &vec, unsigned int index);
    // sets the texture coordinates vector at a given index to "vec" - for internal use    
    void SetUV(const Vector2 &vec, unsigned int index);
    // sets the tangent at a given index to "vec" - for internal use    
    void SetTangent(const Vector3 &vec, unsigned int index);
    // sets the binormal at a given index to "vec" - for internal use    
    void SetBinormal(const Vector3 &vec, unsigned int index);
    // returns the triangle with a given index
    const Triangle& GetTriangle(unsigned int index);
    // returns the triangle with a given index, see Triangle2 structure description
    Triangle2 GetTriangle2(unsigned int index);
    // sets an internal triangle structure with index "index" - for internal use only
    void SetTri(const Tri &tri, unsigned int index);
    // returns the pointer to the internal triangle structure - for internal use only
    Tri& GetTri(unsigned int index);
    // returns the material id with a given index for the mesh
    unsigned int GetMaterial(unsigned int index);
    // adds a material to the mesh and returns its index - for internal use
    unsigned int AddMaterial(unsigned int id);
    // returns the number of materials used in the mesh
    unsigned int GetMaterialCount();
	void Mesh::Optimise(OptimisationLevel value);
	GLuint glName() { return m_glName; }

	// front-face material
	Material* ffMaterial() { return m_ffMaterial; }
	void ffMaterial(Material *mat) { m_ffMaterial = mat; }
	// back-face material
	Material* bfMaterial() { return m_bfMaterial; }
	void bfMaterial(Material* mat) { m_bfMaterial = mat; }

	float radius() { return m_radius; }

	void fromSphere(float radius, int lod);
	void fromSphere(float radius, int stacks, int slices);
	void fromTorus(float innerRadius, float outerRadius, int stacks, int slices);
	void fromHeightmap(int x, int y, short *heights);
	void fromCylinder(float radius, float height, int stacks, int slices);
	void fromCone(float radiusBottom, float radiusTop, float length, int stacks, int slices);
	void fromSpline(Vector start, Vector knot1, Vector knot2, Vector end, int segments, float width);
	void fromSquare(float width, float height);
	float SplineMatrix(float a, float b, float c, float d, float alpha, float m[4][4]);

protected:
    // calculates the normals, either using the smoothing groups information or not
    void CalcNormals(bool useSmoothingGroups);
    // calculates the texture(tangent) space for each vertex
    void CalcTextureSpace();

// a mesh is essentially a collection of vertices and faces
protected:
    // the vertices, normals, etc.
    std::vector<Vector3> m_vertices;
    std::vector<Vector3> m_normals;
    std::vector<Vector3> m_binormals;
    std::vector<Vector3> m_tangents;
    std::vector<Vector2> m_uv;
	float m_radius;
    
    // triangles
    std::vector<Triangle> m_triangles;

    //used internally
    std::vector<Tri> m_tris;

    // the material ID array
//	std::vector<Material *> m_material;
	Material* m_ffMaterial;
	Material* m_bfMaterial;
    std::vector<unsigned int> m_materials;

	// opengl name for picking
	GLuint m_glName;
	// incremental counter
	static GLuint m_namecount;
};
