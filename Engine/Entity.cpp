#include "../stdafx.h"
#include <windows.h>
#include ".\entity.h"
#include "action.h"
#include "material.h"
#include "../script/symboltable.h"
#include "../script/symboltablenode.h"
#include "../framework/string.h"
#include "../framework/real.h"
#include "../framework/integer.h"
#include "../framework/boolean.h"
#include "../engine/constants.h"
#include "actionrotate.h"
#include "actiontranslate.h"
#include "texture.h"
#include "engine.h"

#include <math.h>
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLuint Entity::m_namecount = 0;
wchar_t* Entity::m_className = L"Entity";

Entity::Entity()
	: m_template(NULL)
	, m_parent(NULL)
	, m_parentChanged(true)
	, m_selectedMesh(NULL)
	, m_radius(0.0)
	, m_allowCollisions(false)
	, m_editable(false)
	, m_mass(1.0f)
{
	m_glName = m_namecount++;
}

base* Entity::clone()
{
	return new Entity(*this);
}

Entity::~Entity(void)
{
	// we don't delete sub-objects here because we don't own them

	// we do delete actions though
	std::vector<Action *>::iterator it = m_actions.begin();
	while (it != m_actions.end())
	{
		delete *it;
		++it;
	}
}

bool Entity::step(double fractionSecond)
{
	std::vector<Action *>::iterator cmd_it = m_actions.begin();
	std::vector<Action *>::iterator cmd_end = m_actions.end();
	while (cmd_it != m_actions.end())
	{
		if ((*cmd_it)->step(fractionSecond) == false)
		{
			delete (*cmd_it);
			cmd_it = m_actions.erase(cmd_it);
		}
		else
		{
			++cmd_it;
		}
	}

	// TODO: somewhere we need to calculate forces
	m_linearAcceleration = m_forces / m_mass;

	m_linearVelocity += (m_linearAcceleration * (float)fractionSecond);

	m_matrix.m_values[12] += m_linearVelocity.m_values[0] * (float)fractionSecond;
	m_matrix.m_values[13] += m_linearVelocity.m_values[1] * (float)fractionSecond;
	m_matrix.m_values[14] += m_linearVelocity.m_values[2] * (float)fractionSecond;

	// TODO: angular components

	return true;
}

void Entity::calcAABB()
{
	m_minx = FLT_MAX;
	m_maxx = -FLT_MAX;
	m_miny = FLT_MAX;
	m_maxy = -FLT_MAX;
	m_minz = FLT_MAX;
	m_maxz = -FLT_MAX;

	Matrix m = m_matrix;
	m.m_values[12] = m.m_values[13] = m.m_values[14] = 0.0f;
	if (m_template)
	{
		std::vector<Mesh>::iterator it = m_template->meshes().begin();
		std::vector<Mesh>::iterator end = m_template->meshes().end();
		while (it != end)
		{
			unsigned int count = it->GetVertexCount();
			for (unsigned int i = 0; i < count; i++)
			{
				const Vector3& v = it->GetVertex(i);
				Vector vv(v.x, v.y, v.z);
				vv = vv * m;
				
				if (vv.m_values[0] < m_minx) m_minx = vv.m_values[0];
				if (vv.m_values[0] > m_maxx) m_maxx = vv.m_values[0];
				if (vv.m_values[1] < m_miny) m_miny = vv.m_values[1];
				if (vv.m_values[1] > m_maxy) m_maxy = vv.m_values[1];
				if (vv.m_values[2] < m_minz) m_minz = vv.m_values[2];
				if (vv.m_values[2] > m_maxz) m_maxz = vv.m_values[2];
			}

			++it;
		}
	}
	else
	{
		Vector vv(-1.0f, -1.0f, -1.0f, 1.0f);
		vv = vv * m;

		if (vv.m_values[0] < m_minx) m_minx = vv.m_values[0];
		if (vv.m_values[0] > m_maxx) m_maxx = vv.m_values[0];
		if (vv.m_values[1] < m_miny) m_miny = vv.m_values[1];
		if (vv.m_values[1] > m_maxy) m_maxy = vv.m_values[1];
		if (vv.m_values[2] < m_minz) m_minz = vv.m_values[2];
		if (vv.m_values[2] > m_maxz) m_maxz = vv.m_values[2];

		vv = Vector(1.0f, 1.0f, 1.0f, 1.0f);
		vv = vv * m;

		if (vv.m_values[0] < m_minx) m_minx = vv.m_values[0];
		if (vv.m_values[0] > m_maxx) m_maxx = vv.m_values[0];
		if (vv.m_values[1] < m_miny) m_miny = vv.m_values[1];
		if (vv.m_values[1] > m_maxy) m_maxy = vv.m_values[1];
		if (vv.m_values[2] < m_minz) m_minz = vv.m_values[2];
		if (vv.m_values[2] > m_maxz) m_maxz = vv.m_values[2];
	}
}

// if the fromParent flag is set the model matrix already contains the parent
// transformations - otherwise we need to get a derived matrix
void Entity::drawSetup(int mode, bool fromParent)
{
	if (fromParent == false && m_parent)
	{
		m_parent->drawSetup(mode, fromParent);
	}

	// quite simply multiply the modelview matrix by my matrix
	glMultMatrixf(m_matrix.m_values);
}

void Entity::draw(int mode, Mesh* onlyMesh)
{
	glLoadName(m_glName);

	if (m_template)
	{
		std::vector<Mesh>::iterator it = m_template->meshes().begin();
		std::vector<Mesh>::iterator end = m_template->meshes().end();
		while (it != end)
		{
			Mesh& mesh = *it;
			bool draw = false;

			if (onlyMesh)
			{
				if (&mesh == onlyMesh) draw = true;
			}
			else
			{
				draw = true;
			}

			if (draw)
			{
				// TODO: apply the material here?
				// TODO: associate texture with material or mesh (or both?)
				// the texture is part of the material, but the coordinates
				// are in the mesh
				Material* ffmat = mesh.ffMaterial();
				if (ffmat)
				{
					glEnable(GL_COLOR_MATERIAL);
					if (ffmat->m_texture)
					{
						glPolygonMode(GL_FRONT, GL_FILL);
						glPolygonMode(GL_BACK, GL_LINE);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, ffmat->m_texture->m_id);
					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, 0);
						glDisable(GL_TEXTURE_2D);
					}
					glMaterialfv(GL_FRONT, GL_AMBIENT, ffmat->m_ambient.m_values);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, ffmat->m_diffuse.m_values);
					glMaterialfv(GL_FRONT, GL_SPECULAR, ffmat->m_specular.m_values);
					glMaterialfv(GL_FRONT, GL_EMISSION, ffmat->m_emission.m_values);
					glMateriali(GL_FRONT, GL_SHININESS, ffmat->m_shininess);
				}
				else
				{
					glDisable(GL_COLOR_MATERIAL);
				}

				glCullFace(GL_BACK);
				glPushName(mesh.glName());
				glVertexPointer(3, GL_FLOAT, 0, &mesh.GetVertex(0));
				glNormalPointer(GL_FLOAT, 0, &mesh.GetNormal(0));
				glTexCoordPointer(2, GL_FLOAT, 0, &mesh.GetUV(0));
				glColorPointer(3, GL_FLOAT, 0, &mesh.GetBinormal(0));
				glDrawElements(GL_TRIANGLES, mesh.GetTriangleCount() * 3, GL_UNSIGNED_SHORT, &mesh.GetTriangle(0));

				Material* bfmat = mesh.bfMaterial();
				if (bfmat)
				{
					if (bfmat->m_texture)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, bfmat->m_texture->m_id);
					}
					glMaterialfv(GL_BACK, GL_AMBIENT, ffmat->m_ambient.m_values);
					glMaterialfv(GL_BACK, GL_DIFFUSE, ffmat->m_ambient.m_values);
					glMaterialfv(GL_BACK, GL_SPECULAR, ffmat->m_ambient.m_values);
					glMaterialfv(GL_BACK, GL_EMISSION, ffmat->m_ambient.m_values);
					glMateriali(GL_BACK, GL_SHININESS, ffmat->m_shininess);

					glCullFace(GL_FRONT);
					glVertexPointer(3, GL_FLOAT, 0, &mesh.GetVertex(0));
					glNormalPointer(GL_FLOAT, 0, &mesh.GetNormal(0));
					glColorPointer(3, GL_FLOAT, 0, &mesh.GetBinormal(0));
					glTexCoordPointer(2, GL_FLOAT, 0, &mesh.GetUV(0));
					glDrawElements(GL_TRIANGLES, mesh.GetTriangleCount() * 3, GL_UNSIGNED_SHORT, &mesh.GetTriangle(0));
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glDisable(GL_TEXTURE_2D);
				}
				glPopName();

				// draw the normals
				if (0)
				{
					glDisable(GL_LIGHTING);
					glColor3ub(200, 0, 0);
					glBegin(GL_LINES);
					unsigned int sz = mesh.GetVertexCount();
					for (unsigned int i = 0; i < sz; i++)
					{
						const Vector3& v = mesh.GetVertex(i);
						const Vector3& n = mesh.GetNormal(i);
						glVertex3f(v.x, v.y, v.z);
						glVertex3f(v.x + n.x, v.y + n.y, v.z + n.z);
					}
					glEnd();
				}
			}

			++it;
		}
	}

	// now draw the sub-objects
	if (onlyMesh == NULL)
	{
		std::vector<Entity *>::iterator it = m_subs.begin();
		std::vector<Entity *>::iterator end = m_subs.end();
		while (it != end)
		{
			glPushMatrix();
			(*it)->drawSetup(mode, true);
			(*it)->draw(mode);
			glPopMatrix();
			++it;
		}
	}
}

void Entity::drawAdornments()
{
	glPushMatrix();

	// to draw the bounding box we need to translate but not rotate
	glTranslatef(m_matrix.m_values[12], m_matrix.m_values[13], m_matrix.m_values[14]);

	// at this point should be identity matrix.
	glDisable(GL_LIGHTING);
	glColor3ub(64, 64, 255);

	glBegin(GL_LINE_LOOP);
		glVertex3f(m_minx, m_miny, m_minz);
		glVertex3f(m_minx, m_maxy, m_minz);
		glVertex3f(m_minx, m_maxy, m_maxz);
		glVertex3f(m_minx, m_miny, m_maxz);
		glVertex3f(m_minx, m_miny, m_minz);
		glVertex3f(m_maxx, m_miny, m_minz);
		glVertex3f(m_maxx, m_maxy, m_minz);
		glVertex3f(m_minx, m_maxy, m_minz);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(m_maxx, m_miny, m_maxz);
		glVertex3f(m_maxx, m_miny, m_minz);
		glVertex3f(m_maxx, m_maxy, m_minz);
		glVertex3f(m_maxx, m_maxy, m_maxz);
		glVertex3f(m_maxx, m_miny, m_maxz);
		glVertex3f(m_minx, m_miny, m_maxz);
		glVertex3f(m_minx, m_maxy, m_maxz);
		glVertex3f(m_maxx, m_maxy, m_maxz);
	glEnd();

	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void Entity::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Entity"), dtTypeDeclaration);

	// we need to add these parameters to the tree to aid syntax checking
	// these are just holding places for the names - the actual values are not
	// stored in the sript object model. if type checking is not needed this
	// doesn't need to happen
	SymbolTable* objectTable = new SymbolTable(table);
	node->symbols(objectTable);

	SymbolTableNode* newNode = objectTable->enter(std::wstring(L"x"), dtParameter);
	newNode = objectTable->enter(std::wstring(L"y"), dtParameter);
	newNode = objectTable->enter(std::wstring(L"z"), dtParameter);
	newNode = objectTable->enter(std::wstring(L"forces"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Vector")));

	newNode = objectTable->enter(std::wstring(L"rotate"), dtMethod);
	newNode = objectTable->enter(std::wstring(L"rotatei"), dtMethod);
	newNode = objectTable->enter(std::wstring(L"translatei"), dtMethod);
	newNode = objectTable->enter(std::wstring(L"position"), dtParameter);
	newNode = objectTable->enter(std::wstring(L"material"), dtReference);
}

// get a parameter by name or ordinal
base* Entity::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[12], true);
	else if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[13], true);
	else if (wcscmp(L"z", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[14], true);
	else if (wcscmp(L"forces", (const wchar_t *)(*name)) == 0) return &m_forces;
	else if (wcscmp(L"position", (const wchar_t *)(*name)) == 0)
	{
		Vector* v = new Vector(m_matrix.m_values[12], m_matrix.m_values[13], m_matrix.m_values[14]);
		v->m_temporary = true;
		return v;
	}
	else if (wcscmp(L"material", (const wchar_t *)(*name)) == 0)
	{
		if (m_template->meshes().begin()->ffMaterial())
		{
			return m_template->meshes().begin()->ffMaterial()->name().clone();
		}
	}
	
	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Entity::set(base* param, base* value)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	if (wcscmp(L"material", (const wchar_t *)(*name)) == 0)
	{
		// find the material
		framework::string* matName = static_cast<framework::string *>(value);
		std::map<framework::string, base* >::iterator mat_it = Engine::instance()->materials().members().find(*matName);

		std::vector<Mesh>::iterator it = m_template->meshes().begin();
		while (it != m_template->meshes().end())
		{
			it->ffMaterial((Material *)mat_it->second);
//			it->bfMaterial((Material *)mat_it->second);
			++it;
		}
	}
}

// invoke a method on this object by name 
base* Entity::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"rotate") == 0)
	{
		return rotate(params);
	}
	else if (name.compare(L"rotatei") == 0)
	{
		return rotatei(params);
	}
	else if(name.compare(L"translatei") == 0)
	{
		return translatei(params);
	}

	return NULL;
}

Mesh* Entity::findMeshForglName(GLuint name)
{

	return NULL;
}

base* Entity::rotate(std::vector<base *>& params)
{
	real* a = (real *)params[0];
	Vector* axis = (Vector *)params[1];

	int rel = 0;
	if (params.size() > 2)
	{
		integer* r = (integer *)params[2];
		rel = (int)(*r);
	}

	rotate((float)(*a) * constants::pi / 180.0f, axis->m_values[0], axis->m_values[1], axis->m_values[2], rel);

	return NULL;
}

// perform interpolate rotation over time
base* Entity::rotatei(std::vector<base *>& params)
{
	Vector* centre = (Vector *)params[0];
	Vector* axis = (Vector *)params[1];
	real* angle = (real *)params[2];
	real* duration = (real *)params[3];

	// TODO: validate the parameters
	
	// TODO: then add an action or command to be processed on each subsequent frame until it expires
	ActionRotate* a = new ActionRotate(this, *centre, *axis, *angle, *duration);
	m_actions.push_back(a);

	return NULL;
}

base* Entity::translatei(std::vector<base *>& params)
{
	Vector* direction = (Vector *)params[0];
	real* duration = (real *)params[1];

	ActionTranslate* a = new ActionTranslate(this, *direction, *duration);
	m_actions.push_back(a);

	return NULL;
}

// the aim here is to transform the axis of rotation backwards through the entities
// transformation chain.
void Entity::rotate(float angle, float x, float y, float z, int relativeTo)
{
	Matrix m; m.fromRotation(angle, x, y, z);
	
	if (relativeTo == 0) // local coordinates
	{
		// simplest case, just transform the existing matrix
		m_matrix = m_matrix * m;
	}
	else if (relativeTo == 1) // parent coordinates
	{
		// rotating about the parent is pretty straight forward too
		m_matrix = m * m_matrix;
	}
	else if (relativeTo == 2) // world coordinates
	{
		// we need to step back up the matrix ladder to get to world space
		m_matrix = m_matrix * derivedMatrix().inverse() * m * derivedMatrix();
	}
    else if (relativeTo == 3) // view coordinates
	{
		// rotate around the camera, this could be wrong
		Matrix rot = m_matrix; rot.m_values[12] = 0.0; rot.m_values[13] = 0.0; rot.m_values[14] = 0.0;
		Matrix inv = rot.inverse();
		m = inv * m;
		m_matrix = m_matrix * m * rot;
	}

	std::vector<Entity *>::iterator it = m_subs.begin();
	while (it != m_subs.end())
	{
		(*it)->parentChanged(true);
		++it;
	}

	// recalculate the bounding box to be axis-aligned
	calcAABB();

	notify();
}

void Entity:: rotate(float angle, Vector& centre, Vector& axis)
{
	Matrix m; m.fromRotation(angle, axis.m_values[0], axis.m_values[1], axis.m_values[2]);
	Matrix t; 
	t.m_values[12] = centre.m_values[0];
	t.m_values[13] = centre.m_values[1];
	t.m_values[14] = centre.m_values[2];

	m_matrix = m_matrix * derivedMatrix().inverse() * t * m * t.inverse() * derivedMatrix();

	std::vector<Entity *>::iterator it = m_subs.begin();
	while (it != m_subs.end())
	{
		(*it)->parentChanged(true);
		++it;
	}
}

void Entity::translate(float dx, float dy, float dz, int relativeTo)
{
	Matrix t; t.fromTranslation(dx, dy, dz);

	if (relativeTo == 0) // local
	{
		m_matrix = m_matrix * t;
	}
	else if (relativeTo == 1) // parent
	{
		m_matrix[12] += dx;
		m_matrix[13] += dy;
		m_matrix[14] += dz;
	}
	else if (relativeTo == 2) // world
	{
		m_matrix = m_matrix * derivedMatrix().inverse() * t * derivedMatrix();
	}
	else if (relativeTo == 3) // camera
	{
		// need to work this out...
	}

	std::vector<Entity *>::iterator it = m_subs.begin();
	while (it != m_subs.end())
	{
		(*it)->m_parentChanged = true;
		++it;
	}

	notify();
}

void Entity::addSub(Entity* sub)
{
	// make sure it isn't already there
	std::vector<Entity *>::iterator it = std::find(m_subs.begin(), m_subs.end(), sub);
	if (it == m_subs.end()) 
	{
		sub->setParent(this);
		m_subs.push_back(sub);
	}
}

void Entity::removeSub(Entity* sub)
{
	std::vector<Entity *>::iterator it = std::find(m_subs.begin(), m_subs.end(), sub);
	if (it != m_subs.end()) 
	{
		(*it)->setParent(NULL);
		m_subs.erase(it);
	}
}

void Entity::setParent(Entity* parent)
{
	if (m_parent)
	{
		m_parent->removeSub(this);
	}

	// now set the new parent
	m_parent = parent;
}

void Entity::updateSubs()
{
	// we are going to apply our transformation to each of our sub-objects
	// then get it to do the same - this will update the entire tree of objects
	std::vector<Entity *>::iterator it = m_subs.begin();
	while (it != m_subs.end())
	{
		(*it)->m_matrix = m_matrix * (*it)->m_matrix;
		(*it)->updateSubs();

		++it;
	}
}

Matrix Entity::derivedMatrix()
{
	if (m_parent)
	{
		if (m_parentChanged)
		{
			m_derived = m_parent->derivedMatrix();
			m_parentChanged = false;
		}
	}

	return m_derived * m_matrix;
}

void Entity::leftButtonDown(int x, int y)
{
	// we know this was clicked but this now needs information about where it was clicked
	// in order to find out which vertex is closest etc.
	double wx, wy, wz;  // <= not needed
	Engine::instance()->project(this->x(), this->y(), this->z(), wx, wy, m_depth);
	Engine::instance()->unProject(x, y, &m_ox, &m_oy, &m_oz);

	m_vertex = -1;
	// project every vertex and see if any of them come within a tolerance of the
	// query position, if so, record the depth of it and return a reference to it
	if (m_selectedMesh != NULL)
	{
		for (unsigned int i = 0; i < m_selectedMesh->GetVertexCount(); i++)
		{
			Vector3 v = m_selectedMesh->GetVertex(i);
			Engine::instance()->project(v.x, v.y, v.z, wx, wy, wz);

			if (fabs(wx - x) < 3.0 && fabs(wy - y) < 3.0)
			{
				m_depth = wz;
				m_vertex = i;
			}
		}
	}

	if (m_vertex == -1)
	{
		m_sx = this->x(); m_sy = this->y(); m_sz = this->z();
	}
	else
	{
		m_sx = m_ox; m_sy = m_oy; m_sz = m_oz;
	}

	// either way, i'm a selectable object so i'm going to make myself selected
	Engine::instance()->setSelectedEntity(this);

	notify();
}

void Entity::leftButtonUp(int x, int y)
{
}

void Entity::mouseMove(int x, int y, bool leftButtonDown, bool rightButtonDown)
{
	if (leftButtonDown)
	{
		double tx, ty, tz;

		if (m_vertex == -1)
		{
			// prepare to move
			Engine::instance()->unProject(x, y, &tx, &ty, &tz);
			translate((float)(tx - m_ox), (float)(ty - m_oy), (float)(tz - m_oz), 0);
			m_ox = tx; m_oy = ty; m_oz = tz;
		}
		else
		{
			Engine::instance()->unProject(x, y, &tx, &ty, &tz);
			Vector3 v = m_selectedMesh->GetVertex(m_vertex);

			v.x = (float)(m_sx + tx - m_ox);
			v.y = (float)(m_sy + ty - m_oy);
			v.z = (float)(m_sz + tz - m_oz);

			m_selectedMesh->SetVertex(v, m_vertex);
		}

		// we need to flag a redraw of the view
		notify();
	}

	if (rightButtonDown)
	{
		double tx, ty, tz;

		Engine::instance()->unProject(x, y, &tx, &ty, &tz);

		// create a vector from the mouse down point to the current point
		Vector v1((float)(tx - m_ox), (float)(ty - m_oy), (float)(tz - m_oz));
		if (v1.length() > 1.0e-5)
		{
			// unproject two points in a straight line through the centre of the display
			double p1x, p1y, p1z, p2x, p2y, p2z;

			Engine::instance()->unProject(x, y, 0.1, &p1x, &p1y, &p1z);
			Engine::instance()->unProject(x, y, 0.2, &p2x, &p2y, &p2z);

			// this gives us a vector pointing right into the screen, but transformed into world coords
			Vector los((float)(p2x - p1x), (float)(p2y - p1y), (float)(p2z - p1z));

			// get the perp. vector to the mouse line and the camera direction
			Vector axis = v1.crossProduct(los);
			rotate(axis.length() / 10.0f, axis.m_values[0], axis.m_values[1], axis.m_values[2], 0);
		}

		m_ox = tx; m_oy = ty; m_oz = tz;

		notify();
	}
}

void Entity::rightButtonDown(int x, int y)
{
	// i'm a selectable object so i'm going to make myself selected
	Engine::instance()->setSelectedEntity(this);
	Engine::instance()->unProject(x, y, &m_ox, &m_oy, &m_oz);

	m_forces = Vector(0, 0, -10.0);
}

void Entity::rightButtonUp(int x, int y)
{
}

void Entity::keyPress(int key)
{
}

void Entity::parentChanged(bool changed)
{
	m_parentChanged = changed;
	std::vector<Entity *>::iterator it = m_subs.begin();
	while (it != m_subs.end())
	{
		(*it)->parentChanged(changed);
		++it;
	}
}

void Entity::select(GLuint name)
{
	if (m_template && m_editable)
	{
		std::vector<Mesh>::iterator it = m_template->meshes().begin();
		std::vector<Mesh>::iterator end = m_template->meshes().end();
		while (it != end)
		{
			Mesh& mesh = *it;
			if (mesh.glName() == name)
			{
				m_selectedMesh = &mesh;
				return;
			}

			++it;
		}
	}
}

Matrix Entity::rotationMatrix()
{
	Matrix m = matrix();
	m[12] = m[13] = m[14] = 0.0;

	return m;
}

Matrix Entity::translationMatrix()
{
	Matrix m;
	m[12] = m_matrix[12];
	m[13] = m_matrix[13];
	m[14] = m_matrix[14];

	return m;
}

#ifdef WIN32
void Entity::drawOverlays(Gdiplus::Graphics& graphics)
{
}
#endif