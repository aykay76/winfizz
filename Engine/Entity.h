#pragma once

/******************************************************************************

this is the base class for all drawing objects (and maybe internal objects?)

******************************************************************************/

#include <vector>
#include <memory>

#include "../framework/object.h"
#include "../framework/real.h"
#include "matrix.h"
#include "vector.h"
#include "../gl.h"
#include "template.h"

#ifdef WIN32
#include <gdiplus.h>
#endif

class Engine;
class Action;
class ResourceTexture;

class Entity : public object, public Event
{
public:
	Entity();
	virtual ~Entity(void);

// the object interface, allows this class to be manipulated by eventScript
public:
	// describe myself to the script subsystem, entering nodes in the passed table
	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	base* get(base* param);
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);
	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

public:
//	static Object* create(std::wstring& type, std::vector<std::pair<std::wstring, std::wstring> >& attrs);

// the entity interface - everything to display and interact within the scene
public:
	virtual void calcAABB();
	virtual void drawSetup(int mode, bool fromParent = false);
	virtual bool step(double fractionSecond);
	virtual void draw(int mode, Mesh* onlyMesh = NULL);
	virtual void leftButtonDown(int x, int y);
	virtual void leftButtonUp(int x, int y);
	virtual void mouseMove(int x, int y, bool leftButtonDown = false, bool rightButtonDown = false);
	virtual void rightButtonDown(int x, int y);
	virtual void rightButtonUp(int x, int y);
	virtual void keyPress(int key);
	virtual void drawAdornments();

#ifdef WIN32
	virtual void drawOverlays(Gdiplus::Graphics& graphics);
#endif

	void select(GLuint name);

	base* rotate(std::vector<base *>& params);
	base* rotatei(std::vector<base *>& params);
	base* translatei(std::vector<base *>& params);

	void translate(float dx, float dy, float dz, int relativeTo);
	void rotate(float angle, float x, float y, float z, int relativeTo);
	void rotate(float angle, Vector& centre, Vector& axis);
	void parentChanged(bool changed = true);

	Mesh* findMeshForglName(GLuint name);

public:
	void x(float f) { m_matrix.m_values[12] = f; }
	float x() { return m_matrix[12]; }

	void y(float f) { m_matrix.m_values[13] = f; }
	float y() { return m_matrix[13]; }

	void z(float f) { m_matrix.m_values[14] = f; }
	float z() { return m_matrix[14]; }

	Vector position() { return Vector(m_matrix.m_values[12], m_matrix.m_values[13], m_matrix.m_values[14]); }
	void setPosition(Vector v) { m_matrix.m_values[12] = v.m_values[0]; m_matrix.m_values[13] = v.m_values[1]; m_matrix.m_values[14] = v.m_values[2]; }

	std::wstring& name() { return m_name; }
	void name(std::wstring& name) { m_name = name; }

	Template* prototype() { return m_template; }
	void prototype(Template* prototype) { m_template = prototype; m_radius = m_template->radius(); }

	std::vector<Action *>& actions() { return m_actions; }

	GLuint glName() { return m_glName; }

	Matrix& matrix() { return m_matrix; }
	Matrix rotationMatrix();
	Matrix translationMatrix();
	void setMatrix(const Matrix& m) { m_matrix = m; }
	Matrix derivedMatrix();

	Entity* parent() { return m_parent; }
	void addSub(Entity* sub);
	void removeSub(Entity* sub);

	virtual const wchar_t* className() { return m_className; }
protected:
	void setParent(Entity* parent);
	void updateSubs();

protected:
	std::vector<Action *> m_actions;
	
public:
	// this is our own local frame of reference matrix
	Matrix m_matrix;
	// and this is the cached derived frame of reference
	Matrix m_derived;
	// has the parent changed? do we need to update our matrix?
	bool m_parentChanged;

	// some fizziks
	float m_mass;
	Matrix m_inertia;
	Matrix m_inertiaInverse;
	Matrix m_inertiaInverseGlobal;
	Vector m_linearVelocity;
	Vector m_angularVelocity;
	Vector m_linearAcceleration;
	Vector m_angularAcceleration;
	Vector m_forces;
	Vector m_moments;
	double m_radius; // use spherical bounding
	bool m_allowCollisions;

	// axis-aligned bounding box
	float m_minx, m_maxx, m_miny, m_maxy, m_minz, m_maxz;

	// depth value of the centre point of this entity
	double m_depth;
	// point where mouse was originally clicked
	double m_ox, m_oy, m_oz;
	// starting point of anything that's moving
	double m_sx, m_sy, m_sz;
	
	// selected vertex
	bool m_editable;
	int m_vertex;
	Mesh* m_selectedMesh;

	// a unique name to identify this object
	std::wstring m_name;
	// and some subobjects which comprise a compound object
	std::vector<Entity *> m_subs;
	// a pointer back to parent for derived positions and rotations
	Entity* m_parent;

	// a list of meshes that make up this 'object'
	Template* m_template;

	GLuint m_glName;
	static GLuint m_namecount;

	static wchar_t* m_className;
};
