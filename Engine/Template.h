#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <string>
#include <fstream>
#include "mesh.h"
#include "../framework/object.h"

class Template : public object
{
public:
	Template(void);
	~Template(void);

public:
	virtual base* clone();
	// get a parameter by name or ordinal
	virtual base* get(base* param);
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

public:
	std::vector<Mesh>& meshes() { return m_meshes; }
	float radius() const { return m_radius; }

public:
	void addMesh(Mesh& mesh);

public:
	void write(std::wostream& file);

protected:
	std::vector<Mesh> m_meshes;
	float m_radius;
};
