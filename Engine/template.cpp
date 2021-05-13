#include "../stdafx.h"
#include ".\template.h"
#include "material.h"
#include <gdiplus.h>
using namespace Gdiplus;
#include <gl/glu.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Template::Template(void)
	: m_radius(0.0f)
{
}

base* Template::clone() 
{
	return new Template(*this);
}

Template::~Template(void)
{
}

// get a parameter by name or ordinal
base* Template::get(base* param)
{
	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Template::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* Template::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}

void Template::write(std::wostream& file)
{
	size_t sz;
	sz = m_meshes.size();
	file.write((const wchar_t *)&sz, sizeof(size_t));
	std::vector<Mesh>::iterator it = m_meshes.begin();
	while (it != m_meshes.end())
	{
		// output core mesh information first
		unsigned int c = it->GetVertexCount();
		file.write((const wchar_t *)&c, sizeof(unsigned int));

		// output vertex, normal, binormal(colour) and texture coords
		for (unsigned int i = 0; i < c; i++)
		{
			const Vector3& v = it->GetVertex(i);
			file.write((const wchar_t *)&v.x, sizeof(float));
			file.write((const wchar_t *)&v.y, sizeof(float));
			file.write((const wchar_t *)&v.z, sizeof(float));

			const Vector3& n = it->GetNormal(i);
			file.write((const wchar_t *)&n.x, sizeof(float));
			file.write((const wchar_t *)&n.y, sizeof(float));
			file.write((const wchar_t *)&n.z, sizeof(float));

			const Vector3& b = it->GetBinormal(i);
			file.write((const wchar_t *)&b.x, sizeof(float));
			file.write((const wchar_t *)&b.y, sizeof(float));
			file.write((const wchar_t *)&b.z, sizeof(float));

			const Vector2& t = it->GetUV(i);
			file.write((const wchar_t *)&t.x, sizeof(float));
			file.write((const wchar_t *)&t.y, sizeof(float));
		}

		// output the triangle information
		c = it->GetTriangleCount();
		file.write((const wchar_t *)&c, sizeof(unsigned int));

		for (unsigned int i = 0; i < c; i++)
		{
			const Triangle& t = it->GetTriangle(i);
			file.write((const wchar_t *)&t.a, sizeof(unsigned short));
			file.write((const wchar_t *)&t.b, sizeof(unsigned short));
			file.write((const wchar_t *)&t.c, sizeof(unsigned short));
		}

		// output some material information
		std::wstring str = (std::wstring)it->ffMaterial()->name();
		sz = str.length();
		file.write((const wchar_t *)&sz, sizeof(size_t));
		file.write(str.c_str(), (std::streamsize)sz);

		str = (std::wstring)it->bfMaterial()->name();
		sz = str.length();
		file.write((const wchar_t *)&sz, sizeof(size_t));
		file.write(str.c_str(), (std::streamsize)sz);

		++it;
	}
}

void Template::addMesh(Mesh& mesh)
{
	m_meshes.push_back(mesh);
	if (mesh.radius() > m_radius) m_radius = mesh.radius();
}