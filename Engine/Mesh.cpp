#include "../stdafx.h"
#include "constants.h"
#include ".\mesh.h"
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLuint Mesh::m_namecount = 1000000;

float VectorLength(const Vector3 &vec)
{
    return (float)sqrt(vec.x*vec.x + vec.y*vec.y+vec.z*vec.z);
}

Vector3 NormalizeVector(const Vector3 &vec)
{
    float a = VectorLength(vec);
    if (a == 0)
        return vec;
    float b = 1/a;
    Vector3 v;
    v.x = vec.x*b;
    v.y = vec.y*b;
    v.z = vec.z*b;
    return v;
}

Vector3 CrossProduct(const Vector3 &a, const Vector3 &b)
{
    Vector3 v;
    v.x = a.y*b.z - a.z*b.y;
    v.y = a.z*b.x - a.x*b.z;
    v.z = a.x*b.y - a.y*b.x;
    return v;
}

double VectorAngle(const Vector3& a, const Vector3& b)
{
	double dot = a.x * b.x + a.y * b.y + a.z * b.z;
	double c = dot / (VectorLength(a) * VectorLength(b));
	return acos(c);
}

Vector3 SubtractVectors(const Vector3 &a, const Vector3 &b)
{
    Vector3 t;
    t.x = a.x-b.x;
    t.y = a.y-b.y;
    t.z = a.z-b.z;
    return t;
}

void LoadIdentityMatrix(Matrix4 &m)
{
    m._11 = 1.0f;
    m._12 = 0.0f;
    m._13 = 0.0f;
    m._14 = 0.0f;

    m._21 = 0.0f;
    m._22 = 1.0f;
    m._23 = 0.0f;
    m._24 = 0.0f;

    m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = 1.0f;
    m._34 = 0.0f;

    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 1.0f;
    m._44 = 0.0f;
}

Mesh::Mesh(void)
	: m_ffMaterial(NULL)
	, m_bfMaterial(NULL)
	, m_glName(m_namecount++)
	, m_radius(0.0f)
{
}

Mesh::~Mesh(void)
{
	Clear();
}

void Mesh::Clear()
{
    m_vertices.clear();
    m_normals.clear();
    m_uv.clear();
    m_tangents.clear();
    m_binormals.clear();
    m_triangles.clear();
    m_tris.clear();
    m_materials.clear();
}

unsigned int Mesh::GetVertexCount()
{
    return m_vertices.size();
}

void Mesh::SetVertexArraySize(unsigned int value)
{
    m_vertices.resize(value);
    m_normals.resize(value);
    m_uv.resize(value);
    m_tangents.resize(value);
    m_binormals.resize(value);
}

unsigned int Mesh::GetTriangleCount()
{
    return m_triangles.size();
}

void Mesh::SetTriangleArraySize(unsigned int value)
{
    m_triangles.resize(value);
    m_tris.resize(value);
}

const Vector3& Mesh::GetVertex(unsigned int index)
{
    return m_vertices[index];    
}

const Vector3& Mesh::GetNormal(unsigned int index)
{
    return m_normals[index];
}

const Vector2& Mesh::GetUV(unsigned int index)
{
    return m_uv[index];
}

const Vector3& Mesh::GetTangent(unsigned int index)
{
    return m_tangents[index];
}

const Vector3& Mesh::GetBinormal(unsigned int index)
{
    return m_binormals[index];
}

void Mesh::SetVertex(const Vector3 &vec, unsigned int index)
{
    if (index >= m_vertices.size())
        return;
    m_vertices[index] = vec;
}

void Mesh::SetNormal(const Vector3 &vec, unsigned int index)
{
    if (index >= m_vertices.size())
        return;
    m_normals[index] = vec;
}

void Mesh::SetUV(const Vector2 &vec, unsigned int index)
{
    if (index >= m_vertices.size())
        return;
    m_uv[index] = vec;
}

void Mesh::SetTangent(const Vector3 &vec, unsigned int index)
{
    if (index >= m_vertices.size())
        return;
    m_tangents[index] = vec;    
}

void Mesh::SetBinormal(const Vector3 &vec, unsigned int index)
{
    if (index >= m_vertices.size())
        return;
    m_binormals[index] = vec;
}

const Triangle& Mesh::GetTriangle(unsigned int index)
{
    return m_triangles[index];
}

Triangle2 Mesh::GetTriangle2(unsigned int index)
{
    Triangle2 f;
    Triangle t = GetTriangle(index);
    f.vertices[0] = GetVertex(t.a);
    f.vertices[1] = GetVertex(t.b);
    f.vertices[2] = GetVertex(t.c);

    f.vertexNormals[0] = GetNormal(t.a);
    f.vertexNormals[1] = GetNormal(t.b);
    f.vertexNormals[2] = GetNormal(t.c);
    
    f.textureCoords[0] = GetUV(t.a);
    f.textureCoords[1] = GetUV(t.b);
    f.textureCoords[2] = GetUV(t.c);
    
    Vector3 a, b;

    a = SubtractVectors(f.vertices[1], f.vertices[0]);
    b = SubtractVectors(f.vertices[1], f.vertices[2]);

    f.faceNormal = CrossProduct(b, a);

    f.faceNormal = NormalizeVector(f.faceNormal);

    f.materialId = m_tris[index].materialId;
    
    return f;
}

void Mesh::CalcNormals(bool useSmoothingGroups)
{
    unsigned int i;

    // first calculate the face normals
    for (i=0; i<m_triangles.size(); i++)
    {
        Vector3 a, b;
        a = SubtractVectors(m_vertices[m_tris[i].b], m_vertices[m_tris[i].a]);
        b = SubtractVectors(m_vertices[m_tris[i].b], m_vertices[m_tris[i].c]);
        m_tris[i].normal = NormalizeVector(CrossProduct(b, a));
    }

	// create an array of triangles that each vertex is a part of
    std::vector< std::vector<int> > array;
	size_t sz = m_vertices.size();
    array.resize(sz);
    for (i=0; i<m_triangles.size(); i++)
    {
        unsigned int k = m_tris[i].a;
        array[k].push_back(i);

        k = m_tris[i].b;
        array[k].push_back(i);

        k = m_tris[i].c;
        array[k].push_back(i);
    } 

    Vector3 temp;
    
    if (!useSmoothingGroups)
    {
        // smooth the normals by creating an average of all the faces that
		// each vertex is a part of
        for (i=0; i<m_vertices.size(); i++)
        {
            temp;
            int t = array[i].size();

            for (int k=0; k<t; k++)
            {
                temp.x += m_tris[array[i][k]].normal.x;
                temp.y += m_tris[array[i][k]].normal.y;
                temp.z += m_tris[array[i][k]].normal.z;
            }
            m_normals[i] = NormalizeVector(temp);
        }
    }
    else
    {
        // now calculate the normals _USING_ smoothing groups
        // I'm assuming a triangle can only belong to one smoothing group at a time!
        std::vector<unsigned long> smGroups;
        std::vector< std::vector <unsigned int> > smist;
        
        unsigned int loop_size = m_vertices.size();

        for (i=0; i<loop_size; i++)
        {
            int t = array[i].size();

            if (t == 0)
                continue;

            smGroups.clear();
            smist.clear();
            smGroups.push_back(m_tris[array[i][0]].smoothingGroups);
            smist.resize(smGroups.size());
            smist[smGroups.size()-1].push_back(array[i][0]);

            // first build a list of smoothing groups for the vertex
            for (int k=0; k<t; k++)
            {
                bool found = false;
                for (unsigned int j=0; j<smGroups.size(); j++)
                {
                    if (m_tris[array[i][k]].smoothingGroups == smGroups[j])
                    {
                        smist[j].push_back(array[i][k]);
                        found = true;
                    }
                }
                if (!found)
                {
                    smGroups.push_back(m_tris[array[i][k]].smoothingGroups);
                    smist.resize(smGroups.size());
                    smist[smGroups.size()-1].push_back(array[i][k]);
                }
            }
            // now we have the list of faces for the vertex sorted by smoothing groups


            // now duplicate the vertices so that there's only one smoothing group "per vertex"
            if (smGroups.size() > 1)
                for (unsigned int j=1; j< smGroups.size(); j++)
                {
                    m_vertices.push_back(m_vertices[i]);
                    m_normals.push_back(m_normals[i]);
                    m_uv.push_back(m_uv[i]);
                    m_tangents.push_back(m_tangents[i]);
                    m_binormals.push_back(m_binormals[i]);
                    
                    unsigned int t = m_vertices.size()-1;
                    for (unsigned int h=0; h<smist[j].size(); h++)
                    {
                        if (m_tris[smist[j][h]].a == i)
                            m_tris[smist[j][h]].a = t;
                        if (m_tris[smist[j][h]].b == i)
                            m_tris[smist[j][h]].b = t;
                        if (m_tris[smist[j][h]].c == i)
                            m_tris[smist[j][h]].c = t;
                    }
                }
        }
        
        // now rebuild a face list for each vertex, since the old one is invalidated
        for (i=0; i<array.size(); i++)
            array[i].clear();
        array.clear();
        array.resize(m_vertices.size());
        for (i=0; i<m_triangles.size(); i++)
        {
            unsigned int k = m_tris[i].a;
            array[k].push_back(i);

            k = m_tris[i].b;
            array[k].push_back(i);

            k = m_tris[i].c;
            array[k].push_back(i);
        } 

        // now compute the normals
        for (i=0; i<m_vertices.size(); i++)
        {
            temp;
            int t = array[i].size();

            for (int k=0; k<t; k++)
            {
                temp.x += m_tris[array[i][k]].normal.x;
                temp.y += m_tris[array[i][k]].normal.y;
                temp.z += m_tris[array[i][k]].normal.z;
            }
            m_normals[i] = NormalizeVector(temp);
        }

    }
   
    // copy m_tris to m_triangles
    for (i=0; i<m_triangles.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }
}

void Mesh::CalcTextureSpace()
{
    // a understandable description of how to do that can be found here:
    // http://members.rogers.com/deseric/tangentspace.htm
    // first calculate the tangent for each triangle
    Vector3 x_vec, y_vec, z_vec;
    Vector3 v1, v2;
    for (unsigned int i = 0; i < m_triangles.size(); i++)
    {
        v1.x = m_vertices[m_tris[i].b].x - m_vertices[m_tris[i].a].x;
        v1.y = m_uv[m_tris[i].b].x - m_uv[m_tris[i].a].x;
        v1.z = m_uv[m_tris[i].b].y - m_uv[m_tris[i].a].y;
        
        v2.x = m_vertices[m_tris[i].c].x - m_vertices[m_tris[i].a].x;
        v2.y = m_uv[m_tris[i].c].x - m_uv[m_tris[i].a].x;
        v2.z = m_uv[m_tris[i].c].y - m_uv[m_tris[i].a].y;

        x_vec = CrossProduct(v1, v2);

        v1.x = m_vertices[m_tris[i].b].y - m_vertices[m_tris[i].a].y;
        v1.y = m_uv[m_tris[i].b].x - m_uv[m_tris[i].a].x;
        v1.z = m_uv[m_tris[i].b].y - m_uv[m_tris[i].a].y;

        v2.x = m_vertices[m_tris[i].c].y - m_vertices[m_tris[i].a].y;
        v2.y = m_uv[m_tris[i].c].x - m_uv[m_tris[i].a].x;
        v2.z = m_uv[m_tris[i].c].y - m_uv[m_tris[i].a].y;

        y_vec = CrossProduct(v1, v2);

        v1.x = m_vertices[m_tris[i].b].z - m_vertices[m_tris[i].a].z;
        v1.y = m_uv[m_tris[i].b].x - m_uv[m_tris[i].a].x;
        v1.z = m_uv[m_tris[i].b].y - m_uv[m_tris[i].a].y;

        v2.x = m_vertices[m_tris[i].c].z - m_vertices[m_tris[i].a].z;
        v2.y = m_uv[m_tris[i].c].x - m_uv[m_tris[i].a].x;
        v2.z = m_uv[m_tris[i].c].y - m_uv[m_tris[i].a].y;

        z_vec = CrossProduct(v1, v2);

        m_tris[i].tangent.x = -(x_vec.y/x_vec.x);
        m_tris[i].tangent.y = -(y_vec.y/y_vec.x);
        m_tris[i].tangent.z = -(z_vec.y/z_vec.x);

        m_tris[i].binormal.x = -(x_vec.z/x_vec.x);
        m_tris[i].binormal.y = -(y_vec.z/y_vec.x);
        m_tris[i].binormal.z = -(z_vec.z/z_vec.x);
    }

    // now for each vertex build a list of face that share this vertex
    std::vector< std::vector<int> > array;
    array.resize(m_vertices.size());
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        unsigned int k = m_tris[i].a;
        array[k].push_back(i);

        k = m_tris[i].b;
        array[k].push_back(i);

        k = m_tris[i].c;
        array[k].push_back(i);
    } 

    // now average the tangents and compute the binormals as (tangent X normal)
    for (size_t i = 0; i < m_vertices.size(); i++)
    {
        v1;
        v2;
        int t = array[i].size();

        for (int k=0; k<t; k++)
        {
            v1.x += m_tris[array[i][k]].tangent.x;
            v1.y += m_tris[array[i][k]].tangent.y;
            v1.z += m_tris[array[i][k]].tangent.z;

            v2.x += m_tris[array[i][k]].binormal.x;
            v2.y += m_tris[array[i][k]].binormal.y;
            v2.z += m_tris[array[i][k]].binormal.z;
        }
        m_tangents[i] = NormalizeVector(v1);
        
        m_binormals[i] = NormalizeVector(CrossProduct(m_tangents[i], m_normals[i]));
    }
}

void Mesh::Optimise(OptimisationLevel value)
{
    switch (value)
    {
    case None:
        //TransformVertices();
        break;
    case Simple:
        CalcNormals(false);
        break;
    case Full:
        CalcNormals(true);
        CalcTextureSpace();
        break;
    }
}

void Mesh::SetTri(const Tri &tri, unsigned int index)
{
    if (index >= m_triangles.size())
        return;
    m_tris[index] = tri;
}

Tri& Mesh::GetTri(unsigned int index)
{
    return m_tris[index];
}

unsigned int Mesh::GetMaterial(unsigned int index)
{
    return m_materials[index];
}

unsigned int Mesh::AddMaterial(unsigned int id)
{
    m_materials.push_back(id);
    return m_materials.size()-1;
}

unsigned int Mesh::GetMaterialCount()
{
    return m_materials.size();
}

void Mesh::fromSphere(float radius, int lod)
{
	// we start with an octahedron
	SetVertexArraySize(13);

	Vector3 p;
	Vector2 uv;
	int vidx = 0;

	// 0
	p.x = 0.0; p.y = radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.25; uv.y = 1.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 1
	p.x = 0.0; p.y = radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.5; uv.y = 1.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 2
	p.x = 0.0; p.y = radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.75; uv.y = 1.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 3
	p.x = 0.0; p.y = radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 1.0; uv.y = 1.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);

	// 4
	p.x = 0.0; p.y = 0.0; p.z = -radius;
	SetVertex(p, vidx);
	uv.x = 0.0; uv.y = 0.5;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 5
	p.x = -radius; p.y = 0.0; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.25; uv.y = 0.5;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 6
	p.x = 0.0; p.y = 0.0; p.z = radius;
	SetVertex(p, vidx);
	uv.x = 0.5; uv.y = 0.5;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 7
	p.x = radius; p.y = 0.0; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.75; uv.y = 0.5;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 8
	p.x = 0.0; p.y = 0.0; p.z = -radius;
	SetVertex(p, vidx);
	uv.x = 1.0; uv.y = 0.5;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);

	// 9
	p.x = 0.0; p.y = -radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.25; uv.y = 0.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 10
	p.x = 0.0; p.y = -radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.5; uv.y = 0.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 11
	p.x = 0.0; p.y = -radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 0.75; uv.y = 0.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);
	// 12
	p.x = 0.0; p.y = -radius; p.z = 0.0;
	SetVertex(p, vidx);
	uv.x = 1.0; uv.y = 0.0;
	SetUV(uv, vidx);
	SetNormal(NormalizeVector(p), vidx++);

	m_radius = radius;

	Tri t;
	int tidx = 0;
	t.smoothingGroups = 1;
	t.materialId = 0;

	SetTriangleArraySize(8);

	t.a = 0; t.b = 4; t.c = 5; SetTri(t, tidx++);
	t.a = 1; t.b = 5; t.c = 6; SetTri(t, tidx++);
	t.a = 2; t.b = 6; t.c = 7; SetTri(t, tidx++);
	t.a = 3; t.b = 7; t.c = 8; SetTri(t, tidx++);
	t.a = 4; t.b = 9; t.c = 5; SetTri(t, tidx++);
	t.a = 5; t.b = 10; t.c = 6; SetTri(t, tidx++);
	t.a = 6; t.b = 11; t.c = 7; SetTri(t, tidx++);
	t.a = 7; t.b = 12; t.c = 8; SetTri(t, tidx++);

	// now do the subdivision to the level required
	for (int i = 0; i < lod; i++)
	{
		// we need twice as many vertices
		std::vector<Vector3> vertices;
		vertices.resize(m_vertices.size());
		for (int k = 0; k < m_vertices.size(); k++) vertices[k] = m_vertices[k];

		std::vector<Vector3> normals;
		normals.resize(m_normals.size());
		for (int k = 0; k < m_normals.size(); k++) normals[k] = m_normals[k];

		std::vector<Vector2> uvs;
		uvs.resize(m_uv.size());
		for (int k = 0; k < m_uv.size(); k++) uvs[k] = m_uv[k];

		// we need four times as many triangles
		std::vector<Tri> tris;
		tris.resize(m_tris.size() * 4);

		// set some indices for the new points and triangles to be inserted at
		tidx = 0;
		vidx = m_vertices.size();

		// now we go through all the triangles, subdividing each of the sides
		std::vector<Tri>::iterator it = m_tris.begin();
		while (it != m_tris.end())
		{
			int ao = it->a;
			int bo = it->b;
			int co = it->c;

			// let's split each edge first
			Vector3 a;
			int ai = vertices.size();
			a.x = (m_vertices[it->a].x + m_vertices[it->b].x) / 2.0;
			a.y = (m_vertices[it->a].y + m_vertices[it->b].y) / 2.0;
			a.z = (m_vertices[it->a].z + m_vertices[it->b].z) / 2.0;
			a = NormalizeVector(a);
			a.x *= radius; a.y *= radius; a.z *= radius;
			vertices.push_back(a);
			uv.x = (m_uv[it->a].x + m_uv[it->b].x) / 2.0;
			uv.y = (m_uv[it->a].y + m_uv[it->b].y) / 2.0;
			uvs.push_back(uv);
			normals.push_back(NormalizeVector(a));

			// let's split each edge first
			Vector3 b;
			int bi = vertices.size();
			b.x = (m_vertices[it->b].x + m_vertices[it->c].x) / 2.0;
			b.y = (m_vertices[it->b].y + m_vertices[it->c].y) / 2.0;
			b.z = (m_vertices[it->b].z + m_vertices[it->c].z) / 2.0;
			b = NormalizeVector(b);
			b.x *= radius; b.y *= radius; b.z *= radius;
			vertices.push_back(b);
			uv.x = (m_uv[it->b].x + m_uv[it->c].x) / 2.0;
			uv.y = (m_uv[it->b].y + m_uv[it->c].y) / 2.0;
			uvs.push_back(uv);
			normals.push_back(NormalizeVector(b));

			// let's split each edge first
			Vector3 c;
			int ci = vertices.size();
			c.x = (m_vertices[it->c].x + m_vertices[it->a].x) / 2.0;
			c.y = (m_vertices[it->c].y + m_vertices[it->a].y) / 2.0;
			c.z = (m_vertices[it->c].z + m_vertices[it->a].z) / 2.0;
			c = NormalizeVector(c);
			c.x *= radius; c.y *= radius; c.z *= radius;
			vertices.push_back(c);
			uv.x = (m_uv[it->c].x + m_uv[it->a].x) / 2.0;
			uv.y = (m_uv[it->c].y + m_uv[it->a].y) / 2.0;
			uvs.push_back(uv);
			normals.push_back(NormalizeVector(c));

			// now we reduce the main triangle to it's smaller part of itself
			Tri t;
			t.smoothingGroups = 1;
			t.materialId = 0;

			t.a = ao; t.b = ai; t.c = ci;
			tris[tidx++] = t;

			t.a = ai; t.b = bo; t.c = bi;
			tris[tidx++] = t;

			t.a = ai; t.b = bi; t.c = ci;
			tris[tidx++] = t;

			t.a = ci; t.b = bi; t.c = co;
			tris[tidx++] = t;

			++it;
		}

		m_tris.swap(tris);
		m_vertices.swap(vertices);
		m_normals.swap(normals);
		m_uv.swap(uvs);
	}

	// optimisation!!!! we need to remove identical vertices
/*	for (int v = 0; v < m_vertices.size(); v++)
	{
		Vector3& v1 = m_vertices[v];

		for (int u = v + 1; u < m_vertices.size(); u++)
		{
			Vector3& v2 = m_vertices[u];

			// if the two vertices are a singularity
			if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
			{
				// change the reference to the second in any triangles to the first
				for (int t = 0; t < m_tris.size(); t++)
				{
					Tri& tri = m_tris[t];

					if (tri.a == u) tri.a = v;
					if (tri.b == u) tri.b = v;
					if (tri.c == u) tri.c = v;

					if (tri.a > u) tri.a--;
					if (tri.b > u) tri.b--;
					if (tri.c > u) tri.c--;
				}

				m_vertices.erase(m_vertices.begin() + u);
				m_normals.erase(m_normals.begin() + u);
				m_uv.erase(m_uv.begin() + u);
			}
		}
	}
*/
	m_uv.resize(m_vertices.size());
	for (int v = 0; v < m_vertices.size(); v++)
	{
		double x = m_vertices[v].x;
		double y = m_vertices[v].y;
		double z = m_vertices[v].z;
		double r = radius;

		//
		//r = sqrt(x*x+y*y)
		//rho = radius
		//theta = atan(y/x)
		//phi = asin(rho*sin(theta)/rho)
		//phi = acos(rho*cos(theta)/rho)
		//
		double r1 = sqrt(r * r - y * y);
		if (r1 == 0.0) r1 = 1e-9;
		double latitude = asin(y / radius);
		double t = x / r1; if (t > 1.0) t = 1.0; if (t < -1.0) t = -1.0;
		double longitude = asin(t);

		if (x > 0.0 && z < 0.0) longitude = constants::pi - longitude;
		if (x < 0.0 && z < 0.0) longitude = -constants::pi - longitude;
		if (x == 0 && z < 0.0) longitude = constants::pi;

		uv.x = (longitude + constants::pi) / (2.0 * constants::pi);
		uv.y = (latitude + 0.5 * constants::pi) / constants::pi;

		SetUV(uv, v);
	}

    // copy m_tris to m_triangles
	m_triangles.resize(m_tris.size());
    for (int i = 0; i < m_tris.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }

	// check triangles texture coordinates
	int ts = m_tris.size();
	for (int t = 0; t < ts; t++)
	{
		int a = m_tris[t].a;
		int b = m_tris[t].b;
		int c = m_tris[t].c;

		wchar_t sz[128];
		swprintf(sz, 128, L"%d: %f, %f, %f\n", t, m_uv[a].x, m_uv[b].x, m_uv[c].x);
		OutputDebugString(sz);

		if (m_uv[a].x == 1.0 && m_uv[b].x < 0.1 && m_uv[c].x < 0.1)
		{
			m_uv[a].x = 0.0;
		}
		else if (m_uv[a].x == 1.0 && m_uv[b].y == 1.0 && m_uv[c].x < 0.2)
		{
			m_uv[a].x = 0.0;
			m_uv[b].x = 0.0;
		}
	}
}

void Mesh::fromSphere(float radius, int stacks, int slices)
{
	double latstep = (180.0 / (double)stacks) * constants::pi / 180.0;
	double lonstep = (360.0 / (double)slices) * constants::pi / 180.0;

	m_radius = radius;

	// first of all we need to know how many points we will have
	SetVertexArraySize((stacks + 1) * (slices + 1));

	// put one point at the bottom of the sphere
	double lon = 0.0;
	double lat = -90.0 * constants::pi / 180.0;
	Vector3 p;
	Vector2 uv;
	int vidx = 0;
	int uvidx = 0;

	double du = 1.0 / slices;
	double dv = 1.0 / stacks;
	uv.x = 0.0;
	uv.y = 0.0;

	Vector3 colour; colour.x = 1.0; colour.y = 0.0; colour.z = 0.0;

	for (int slice = 0; slice <= slices; slice++)
	{
		// we start from a step up from the bottom
		lat = -90.0 * constants::pi / 180.0;
		lon += lonstep;
		uv.y = 0.0;

		for (int stack = 0; stack <= stacks; stack++)
		{
			p.x = cos(lat) * sin(lon) * radius;
			p.y = sin(lat) * radius;
			p.z = cos(lat) * cos(lon) * radius;
			SetNormal(NormalizeVector(p), vidx);
			SetBinormal(colour, vidx);
			SetVertex(p, vidx++);

			// range from zero to one
			SetUV(uv, uvidx++);

			uv.y += dv;
			lat += latstep;
		}
		uv.x += du;
	}

	// now add some triangles
	Tri t;
	int tidx = 0;
	t.smoothingGroups = 1;
	t.materialId = 0;

	SetTriangleArraySize(stacks * slices * 2);

	int i = 0;
	for (i = 0; i < slices; i++)
	{
		int l = i * (stacks + 1);
		int r = (i + 1) * (stacks + 1);

		for (int stack = 0; stack < stacks; stack++)
		{
			t.a = l; t.b = r; t.c = l + 1;
			SetTri(t, tidx++);

			t.a = r; t.b = r + 1; t.c = l + 1;
			SetTri(t, tidx++);

			l++; r++;
		}
	}

    // copy m_tris to m_triangles
    for (i = 0; i < m_triangles.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }
}

void Mesh::fromHeightmap(int x, int y, short* heights)
{
	SetVertexArraySize(x * y); 

	int vidx = 0; 
	for (int yi = 0; yi < y; yi++) 
	{
		for (int xi = 0; xi < x; xi++) 
		{
			Vector3 v;
			v.x = xi;
			v.z = yi;
			v.y = (float)heights[vidx] / 100.0f;
			SetVertex(v, vidx++);
			float l = VectorLength(v);
			if (l > m_radius) m_radius = l;
		}
	}
	SetTriangleArraySize((x - 1) * (y - 1) * 2); 
	int tidx = 0; 
	Tri t; 
	t.smoothingGroups = 1; 
	t.materialId = 0; 
	for (int yi = 0; yi < y - 1; yi++) 
	{
		for (int xi = 0; xi < x - 1; xi++) 
		{
			// create two triangles 
			t.a = yi * x + xi; t.b = t.a + x; t.c = t.a + 1; 
			SetTri(t, tidx++); 

			t.a = yi * x + xi + 1; t.b = t.a + x - 1; t.c = t.a + x; 
			SetTri(t, tidx++); 
		}
	}
	
	CalcNormals(false);
}

void Mesh::fromTorus(float innerRadius, float outerRadius, int stacks, int slices)
{
	Vector2 uv;
	Vector3 p;
	Vector3 n;
	// for each slice, calculate the center as the middle of the radii
	float spoke = (innerRadius + outerRadius) / 2.0;
	float thickness = (outerRadius - innerRadius) / 2.0;
	float azimuth = 0.0;
	float azimuthStep = constants::pi * 2.0f / (float)slices;
	double du = 1.0 / slices;
	double dv = 1.0 / stacks;
	uv.x = 0.0;
	uv.y = 0.0;
	Vector3 colour; colour.x = 0.0; colour.y = 0.0; colour.z = 0.0;

	m_radius = outerRadius;

	// setup the vertex array
	SetVertexArraySize((stacks + 1) * (slices + 1));

	int vidx = 0;
	for (int i = 0; i <= slices; i++)
	{
		float sx = spoke * cos(azimuth);
		float sy = 0.0f;
		float sz = spoke * sin(azimuth);

		double elevation = -constants::pi;
		double elevationStep = constants::pi * 2.0 / (double)stacks;
		uv.y = 0.0;
		// now we create a vertex per stack
		for (int j = 0; j <= stacks; j++)
		{
			p.x = sx + cos(elevation) * sin(constants::pi / 2.0 - azimuth) * thickness;
			p.y = sy + sin(elevation) * thickness;
			p.z = sz + cos(elevation) * cos(constants::pi / 2.0 - azimuth) * thickness;

			n.x = p.x - sx;
			n.y = p.y - sy;
			n.z = p.z - sz;

			SetUV(uv, vidx);
			SetNormal(n, vidx);
			SetBinormal(colour, vidx);
			SetVertex(p, vidx++);
			float l = VectorLength(p);
			if (l > m_radius) m_radius = l;

			elevation += elevationStep;
			uv.y += dv;
		}

		azimuth += azimuthStep;
		uv.x += du;
	}

	// create the mesh
	SetTriangleArraySize(stacks * slices * 2);

	Tri t;
	t.smoothingGroups = 1;
	t.materialId = 0;
	int tidx = 0;
	for (int i = 0; i < slices; i++)
	{
		int l = i * (stacks + 1);
		int r = (i + 1) * (stacks + 1);

		for (int j = 0; j < stacks; j++)
		{
			t.a = l; t.b = l + 1; t.c = r; 
			SetTri(t, tidx++);

			t.a = r; t.b = l + 1; t.c = r + 1;
			SetTri(t, tidx++);

			l++; r++;
		}
	}

    // copy m_tris to m_triangles
	m_triangles.resize(m_tris.size());
    for (int i = 0; i < m_tris.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }
//	CalcNormals(false);
}

void Mesh::fromCylinder(float radius, float length, int stacks, int slices)
{
	Vector2 uv; uv.x = 0.0; uv.y = 0.0;
	int vidx = 0;
	Vector3 p;
	Vector3 colour; colour.x = 0.0; colour.y = 0.0; colour.z = 0.0;

	// setup the vertex array
	SetVertexArraySize((stacks + 1) * (slices + 1));

	double azimuthStep = constants::pi * 2.0 / (double)slices;
	double azimuth = 0.0;
	double ydiff = length / (double)stacks;
	double du = 1.0 / slices;
	double dv = 1.0 / stacks;

	for (int i = 0; i <= slices; i++)
	{
		double y = -length / 2.0;
		for (int j = 0; j <= stacks; j++)
		{
			p.z = sin(azimuth) * radius;
			p.y = y;
			p.x = cos(azimuth) * radius;

			SetUV(uv, vidx);
			SetNormal(p, vidx);
			SetBinormal(colour, vidx);
			SetVertex(p, vidx++);

			float l = VectorLength(p);
			if (l > m_radius) m_radius = l;

			uv.y += dv;
			y += ydiff;
		}

		azimuth += azimuthStep;
		uv.x += du;
		uv.y = 0.0;
	}

	// setup the triangle array
	SetTriangleArraySize(stacks * slices * 2);

	Tri t;
	t.smoothingGroups = 1;
	t.materialId = 0;
	int tidx = 0;
	for (int i = 0; i < slices; i++)
	{
		int l = i * (stacks + 1);
		int r = (i + 1) * (stacks + 1);

		for (int j = 0; j < stacks; j++)
		{
			t.a = l; t.b = r; t.c = l + 1;
			SetTri(t, tidx++);

			t.a = r; t.b = r + 1; t.c = l + 1;
			SetTri(t, tidx++);

			l++; r++;
		}
	}

    // copy m_tris to m_triangles
	m_triangles.resize(m_tris.size());
    for (int i = 0; i < m_tris.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }
//	CalcNormals(false);
}

void Mesh::fromCone(float radiusBottom, float radiusTop, float length, int stacks, int slices)
{
	Vector2 uv; uv.x = 0.0; uv.y = 0.0;
	int vidx = 0;
	Vector3 p;
	Vector3 colour; colour.x = 0.0; colour.y = 0.0; colour.z = 0.0;

	// setup the vertex array
	SetVertexArraySize((stacks + 1) * (slices + 1));

	double azimuthStep = constants::pi * 2.0 / (double)slices;
	double azimuth = 0.0;
	double ydiff = length / (double)stacks;
	double dr = (radiusTop - radiusBottom) / (double)stacks;
	double du = 1.0 / slices;
	double dv = 1.0 / stacks;

	for (int i = 0; i <= slices; i++)
	{
		double radius = radiusBottom;
		double y = -length / 2.0;

		for (int j = 0; j <= stacks; j++)
		{
			p.x = sin(azimuth) * radius;
			p.y = y;
			p.z = cos(azimuth) * radius;

			SetUV(uv, vidx);
			SetNormal(p, vidx);
			SetBinormal(colour, vidx);
			SetVertex(p, vidx++);

			float l = VectorLength(p);
			if (l > m_radius) m_radius = l;

			radius += dr;
			uv.y += dv;
			y += ydiff;
		}

		uv.x += du;
		uv.y = 0.0;
		azimuth += azimuthStep;
	}

	// setup the triangle array
	SetTriangleArraySize(stacks * slices * 2);

	Tri t;
	t.smoothingGroups = 1;
	t.materialId = 0;
	int tidx = 0;
	for (int i = 0; i < slices; i++)
	{
		int l = i * (stacks + 1);
		int r = (i + 1) * (stacks + 1);

		for (int j = 0; j < stacks; j++)
		{
			t.a = l; t.b = r; t.c = l + 1;
			SetTri(t, tidx++);

			t.a = r; t.b = r + 1; t.c = l + 1;
			SetTri(t, tidx++);

			l++; r++;
		}
	}

    // copy m_tris to m_triangles
	m_triangles.resize(m_tris.size());
    for (int i = 0; i < m_tris.size(); i++)
    {
        m_triangles[i].a = m_tris[i].a;
        m_triangles[i].b = m_tris[i].b;
        m_triangles[i].c = m_tris[i].c;
    }

//	CalcNormals(false);
}

float Mesh::SplineMatrix(float a, float b, float c, float d, float alpha, float m[4][4])
{
	register float p0, p1, p2, p3;
	p0 = m[0][0] * a + m[0][1] * b + m[0][2] * c + m[0][3] * d;
	p1 = m[1][0] * a + m[1][1] * b + m[1][2] * c + m[1][3] * d;
	p2 = m[2][0] * a + m[2][1] * b + m[2][2] * c + m[2][3] * d;
	p3 = m[3][0] * a + m[3][1] * b + m[3][2] * c + m[3][3] * d;
	return (p3 + alpha * (p2 + alpha * (p1 + alpha * p0)));
}

void Mesh::fromSpline(Vector start, Vector knot1, Vector knot2, Vector end, int segments, float width)
{
	int vidx = 0;
	double alpha[256];
	double alphastep = 1.0 / (segments + 1);
	double alphav = 0.0;
	float halfWidth = width * 0.5f;
	for (int i = 0; i <= segments; i++)
	{
		alpha[i] = alphav;
		alphav += alphastep;
	}

	// find a vector perpendicular to the general direction, and up
	Vector v1 = end - start;
	Vector v2(0.0, 1.0, 0.0);
	Vector v3 = v1.crossProduct(v2); v3.normalise();

	// initialise the cardinal matrix based on the tension (a)
	float cm[4][4];
	double a = 0.5;
	cm[0][1] = 2. - a;
	cm[0][2] = a - 2.;
	cm[1][0] = 2. * a;
	cm[1][1] = a - 3.;
	cm[1][2] = 3. - 2 * a;
	cm[3][1] = 1.;
	cm[0][3] = cm[2][2] = a;
	cm[0][0] = cm[1][3] = cm[2][0] = -a;
	cm[2][1] = cm[2][3] = cm[3][0] = cm[3][2] = cm[3][3] = 0.;

	// reserve some space for some points
	int p = 4;
	SetVertexArraySize((segments + 1) * 2 * (p - 1));
	Vector km1 = start;
	Vector k0 = start;
	Vector k1 = knot1;
	Vector k2 = knot2;
	for (int i = 0; i < p - 1; i++)
	{
		for (int j = 0; j <= segments; j++)
		{
			Vector3 v;
			v.x = SplineMatrix(km1[0], k0[0], k1[0], k2[0], alpha[j], cm) - v3[0] * halfWidth;
			v.y = SplineMatrix(km1[1], k0[1], k1[1], k2[1], alpha[j], cm) - v3[1] * halfWidth;
			v.z = SplineMatrix(km1[2], k0[2], k1[2], k2[2], alpha[j], cm) - v3[2] * halfWidth;
			SetVertex(v, vidx++);
			float l = VectorLength(v);
			if (l > m_radius) m_radius = l;

			v.x = SplineMatrix(km1[0], k0[0], k1[0], k2[0], alpha[j], cm) + v3[0] * halfWidth;
			v.y = SplineMatrix(km1[1], k0[1], k1[1], k2[1], alpha[j], cm) + v3[1] * halfWidth;
			v.z = SplineMatrix(km1[2], k0[2], k1[2], k2[2], alpha[j], cm) + v3[2] * halfWidth;
			SetVertex(v, vidx++);
			l = VectorLength(v);
			if (l > m_radius) m_radius = l;
		}

		if (i == 0)
		{
			k0 = knot1;
			k1 = knot2;
			k2 = end;
		}
		else
		{
			km1 = knot1;
			k0 = knot2;
			k1 = end;
			k2 = end;
		}
//		if (km1 != k0) km1 = knot1;
//		k0 = knot2; k1 = end;
	}
//	(*s++) = m_points[m_count-1];

	Tri t;
	t.smoothingGroups = 1;
	t.materialId = 0;
	int tidx = 0;

	SetTriangleArraySize(segments * 2 * 3);
	int l = 0; int r = 1;
	for (int i = 0; i < segments * 3; i++)
	{
		t.a = l; t.b = l + 2; t.c = r;
		SetTri(t, tidx++);

		t.a = r; t.b = l + 2; t.c = r + 2;
		SetTri(t, tidx++);

		l += 2; r += 2;
	}

	CalcNormals(false);
}

void Mesh::fromSquare(float width, float height)
{
	Vector2 uv; uv.x = 0.0; uv.y = 0.0;
	int vidx = 0;
	Vector3 p;

	// setup the vertex array
	SetVertexArraySize(4);

	p.x = -width / 2.0;
	p.y = -height / 2.0;
	p.z = 0.0;

	Vector3 n;
	n.x = 0.0;
	n.y = 0.0;
	n.z = -1.0;

	uv.x = 0.0;
	uv.y = 0.0;

	SetUV(uv, vidx);
	SetNormal(n, vidx);
	SetVertex(p, vidx++);
			float l = VectorLength(p);
			if (l > m_radius) m_radius = l;

	//
	p.x = -width / 2.0;
	p.y = height / 2.0;
	p.z = 0.0;

	uv.x = 0.0;
	uv.y = 1.0;

	SetUV(uv, vidx);
	SetNormal(n, vidx);
	SetVertex(p, vidx++);
			l = VectorLength(p);
			if (l > m_radius) m_radius = l;

	p.x = width / 2.0;
	p.y = -height / 2.0;
	p.z = 0.0;

	uv.x = 1.0;
	uv.y = 0.0;

	SetUV(uv, vidx);
	SetNormal(n, vidx);
	SetVertex(p, vidx++);
			l = VectorLength(p);
			if (l > m_radius) m_radius = l;

	p.x = width / 2.0;
	p.y = height / 2.0;
	p.z = 0.0;

	uv.x = 1.0;
	uv.y = 1.0;

	SetUV(uv, vidx);
	SetNormal(n, vidx);
	SetVertex(p, vidx++);
			l = VectorLength(p);
			if (l > m_radius) m_radius = l;

	SetTriangleArraySize(2);

	Tri t;
	t.smoothingGroups = 1;
	t.materialId = 0;
	int tidx = 0;

	t.a = 0; t.b = 2; t.c = 1;
	SetTri(t, tidx++);

	t.a = 2; t.b = 3; t.c = 1;
	SetTri(t, tidx++);

	CalcNormals(false);
}