#include "../stdafx.h"
#include <windows.h>
#include "entity.h"
#include "Engine.h"

#include "colour.h"
#include "vector.h"
#include "collision.h"

#include <gl/gl.h>
#include "glext.h"

#include <math.h>

#include "../script/filereader.h"
#include "../script/symboltable.h"
#include "../engine/constants.h"

#include "scriptobserver.h"

#include "structs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Engine* Engine::m_instance = NULL;

Engine::Engine(void)
	: m_fps(0.0)
	, m_running(false)
	, m_finishTime(0.0)
	, m_activeCamera(0)
	, m_selectedEntity(0)
	, m_capturingEntity(0)
	, m_parser(new FileReader("collision.es"))
	, m_editMode(true)
	, m_winZ(0.0)
	, m_feedbackBufferSize(10000)
	, m_overplot(false)
	, m_wireframe(false)
	, m_leftButtonDown(false)
	, m_rightButtonDown(false)
{
	m_feedbackBuffer = new float[m_feedbackBufferSize];

	*m_nearPlane.m_value = 10.0f;
	*m_farPlane.m_value = 1000.0f;
}

Engine::~Engine(void)
{
	m_keyIterator = m_keyframes.begin();
	for (; m_keyIterator != m_keyframes.end(); ++m_keyIterator)
	{
		delete *m_keyIterator;
	}
	m_keyframes.clear();

	m_cameras.clear();
	m_materials.clear();
	m_entities.clear();
	m_lights.clear();
	m_templates.clear();
	m_textures.clear();
	m_timers.clear();

	if (m_feedbackBuffer) delete [] m_feedbackBuffer;
}

Engine* Engine::instance()
{
	if (m_instance) return m_instance;

	m_instance = new Engine();
	return m_instance;
}

void Engine::destroy()
{
	if (m_instance) 
	{
		SetEvent(m_instance->m_threadEvent);
		WaitForSingleObject(m_instance->m_threadHandle, INFINITE);

		delete m_instance;
	}
}

// invoke a method on this object by name 
base* Engine::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}

void Engine::init()
{
	// now we flag the init event
	m_init.notify();

	// if we don't have an active camera we must create one and set it to active
	if (m_activeCamera == NULL) 
	{
		if (m_cameras.members().size() == 0)
		{
			Camera* camera = new Camera;
			m_cameras.members()[framework::string(L"default_camera")] = camera;
		}
		m_activeCamera = static_cast<Camera *>(m_cameras.members().begin()->second);
	}

	// check for some extensions we might use
//	m_ambientShadowSupported = m_gl->isExtensionSupported("GL_ARB_shadow_ambient");
	if (m_gl->isExtensionSupported("WGL_EXT_swap_control"))
	{
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC) (int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;

		// this accepts one param, 0 = off and 1 = on
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(false);
	}
}

void Engine::start(bool doFirstStep)
{
	// get the frequency of the high res timer, saves doing it all over the place
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_startTime);
	QueryPerformanceCounter(&m_lastFrame);

	m_frame = 0;
	m_duration = 0.0;

	// point to the first keyframe ready to slot it in at the righ time
	m_keyIterator = m_keyframes.begin();

	m_running = true;

	// I honestly don't think that multithreading this makes any improvement
	// time would be better spent making the processor intensive stuff 
	// My reasoning behind this is that there is a certain amount of latency
	// when swapping the buffers- we can't do anything in GL until that has
	// happened. At the minute everything else we do is non-GL apart from
	// the drawing (ie. collision detection etc.)
	if (0)
	{
		m_threadEvent = CreateEvent(NULL, TRUE, FALSE, L"Thread event");
		m_threadHandle = CreateThread(NULL, 1048576, Engine::workerThread, this, CREATE_SUSPENDED, &m_threadID);
		ResumeThread(m_threadHandle);
	}

	// and we're going to do the first step right here
//	if (doFirstStep)
//	{
//		step();
//	}
}

void Engine::reset()
{
}

void Engine::pause()
{
}

float rtri = 0.0f;

bool Engine::step()
{
	if (!m_running) return true;
	// calculate the time from the last frame completing to now, this will give us a proportion 
	// of a second that has passed. we use this to interpolate tweens.
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	double fractionSecond = (double)(now.QuadPart - m_lastFrame.QuadPart) / (double)m_frequency.QuadPart;
	if (now.QuadPart - m_lastFrame.QuadPart == 0) return true;
	// update the last frame time
	m_lastFrame = now;

	return step(fractionSecond);
}

bool Engine::step(double fractionSecond)
{
	double finishTime = 0.0;
	// and the total duration
	m_duration += fractionSecond;

	// do any pre-frame stuff here
	m_frameStart.notify();

	m_gl->prep();
	drawSetup(GL_RENDER);
	draw(GL_RENDER);

	// ok, we've done our bit - flag this event immediately
	m_frameDone.notify();

	// process any input
	processInput();

	// if there are still frames left, run through those that are past their
	// due start time - hopefully they are no more than a frame late  :S
	if (m_keyIterator != m_keyframes.end())
	{
		while ((*m_keyIterator)->position() <= m_duration)
		{
			// iterate through the commands, processing each accordingly
			std::list<Command *>::iterator it = (*m_keyIterator)->commands().begin();
			for (; it != (*m_keyIterator)->commands().end(); ++it)
			{
				// we initialise the command and let it do what it needs to do
				(*it)->init();
			}

			// set the finish time to check for
			finishTime = (*m_keyIterator)->position() + (*m_keyIterator)->duration();
			if (finishTime > m_finishTime) m_finishTime = finishTime;

			// increment the frame iterator ready to see when the next one will be inserted
			++m_keyIterator;
			if (m_keyIterator == m_keyframes.end()) break;
		}
	}

	// step all the timers
	m_timers.step(fractionSecond);

	std::map<framework::string, base*>::iterator it = m_entities.members().begin();
	std::map<framework::string, base*>::iterator end = m_entities.members().end();
	while (it != end)
	{
		Entity* entity = reinterpret_cast<Entity *>(it->second);
		entity->step(fractionSecond);
		++it;
	}
	// GET READY TO DRAW THIS STEP

	checkForCollisions();
	if (m_collisions.size() > 0)
	{
		resolveCollisions();
	}

	// process the event queue - this will entail updating all the observers of all the 
	// events that are currently in the queue
	processEventQueue();

	// now swap the buffers - reason being we might want to do some post-frame
	// processing before it's displayed
	m_gl->swap();

	m_frame++;
	m_fps = (double)m_frame / m_duration;

	// if all the commands have finished and there are no more keyframes we can call it a wrap
	// unless we are looping in which case we reset and increment the loop count
	if (m_keyIterator == m_keyframes.end() && m_duration >= m_finishTime)
	{
//		m_running = false;
//		return false;
	}
	return true;
}

void Engine::addEvent(Event* event)
{
	m_eventQueue.push(event);
}

// notifies all events in the queue then gets rid of them
void Engine::processEventQueue()
{
	while (m_eventQueue.size() > 0)
	{
		m_eventQueue.front()->notify();
		m_eventQueue.pop();
	}
}

void Engine::initialiseScriptSystem()
{
	SymbolTable& global = m_parser.symbols();

	describe(&global);

	// start with our "root namespace" System - after all we need a starting
	// pointing from which to reference our engine
	SymbolTableNode* systemNode = global.enter(std::wstring(L"K3D"), dtObject);
	SymbolTable* systemTable = new SymbolTable(&global);
	systemNode->symbols(systemTable);

	// register any new data types in the global scope
	Vector::describe(&global);
	Colour::describe(&global);
	Texture::describe(&global);
	Material::describe(&global);
	Template::describe(&global);
	Entity::describe(&global);
	Light::describe(&global);
	Camera::describe(&global);

	m_textures.describe(&global);
	m_materials.describe(&global);
	m_templates.describe(&global);
	m_entities.describe(&global);
	m_timers.describe(&global);
	m_lights.describe(&global);
	m_cameras.describe(&global);

	// now we can parse the script - all known types are in the symbol tables
	m_parser.parse();

	// wire up the system events
	Engine::instance()->wireUpFixedEvents(m_parser.symbols());
}

// wire up any known events by looking up the System object and it's event objects
// to see if the symbol table contains icode for them
// Then add a ScriptObserver to each of the events.
void Engine::wireUpFixedEvents(SymbolTable& global)
{
	// first of all - find system object in global table
	SymbolTableNode* systemNode = global.find(L"K3D");
	SymbolTable* systemTable = systemNode->symbols();

	// now find the events that we know about
	SymbolTableNode* scriptlet = systemTable->find(L"init");
	m_init.attach(new ScriptObserver(scriptlet, m_runtimeStack));

	// frame start
	scriptlet = systemTable->find(L"frameStart");
	if (scriptlet) m_frameStart.attach(new ScriptObserver(scriptlet, m_runtimeStack));

	// frame done
	scriptlet = systemTable->find(L"frameDone");
	if (scriptlet) m_frameDone.attach(new ScriptObserver(scriptlet, m_runtimeStack));

	// left button down
	scriptlet = systemTable->find(L"leftButtonDown");
	if (scriptlet) m_lbDown.attach(new ScriptObserver(scriptlet, m_runtimeStack));

	// left button up
	scriptlet = systemTable->find(L"leftButtonUp");
	if (scriptlet) m_lbUp.attach(new ScriptObserver(scriptlet, m_runtimeStack));
	
	// right button down
	scriptlet = systemTable->find(L"rightButtonDown");
	if (scriptlet) m_rbDown.attach(new ScriptObserver(scriptlet, m_runtimeStack));
	
	// left button down
	scriptlet = systemTable->find(L"rightButtonUp");
	if (scriptlet) m_rbUp.attach(new ScriptObserver(scriptlet, m_runtimeStack));
}

void Engine::drawSetup(int mode, bool fromParent)
{
	// get ready to start naming stuff
	glInitNames();
	glPushName(999);

	// if we are selecting we have already setup the projection matrix to suit our neds
	if (mode == GL_RENDER)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(m_activeCamera->m_fov, (GLfloat)m_gl->viewport()[2] / (float)m_gl->viewport()[3], m_nearPlane, (double)m_farPlane);
		glGetDoublev(GL_PROJECTION_MATRIX, m_gl->projectionMatrix());
	}

	// load the identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// apply the camera transform
	if (m_activeCamera)
	{
		m_activeCamera->init();
	}

	// initialise the camera and lights ready for our frame
	m_lights.init();
}

// This function is responsible for drawing the scene in normal mode but also
// passing the geometry through the pipeline for feedback and selection modes
void Engine::draw(int mode, Mesh* onlyMesh)
{
	// if we are not in wireframe mode we draw the entire scene, filled and textures etc.
	if (m_wireframe == false)
	{
		// now draw all the objects
		std::map<framework::string, base*>::iterator it = m_entities.members().begin();
		std::map<framework::string, base*>::iterator end = m_entities.members().end();
		while (it != end)
		{
			Entity* entity = reinterpret_cast<Entity *>(it->second);

			// we only instigate the drawing of objects here that have no parent
			// all the others should get encompassed as they will have parents which
			// will be one of these entities. if not, we have zombies... wwaaaaaaargh!
			if (entity->parent() == NULL)
			{
				glPushMatrix();
				entity->drawSetup(mode);
				entity->draw(mode);
				glPopMatrix();
			}
			++it;
		}
	}

	// if we are in wireframe mode or we are overplotting antialiased lines for better rendering
	// we draw the entire scene with smooth lines
/*	if (m_overplot || m_wireframe && mode == GL_RENDER)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(0.5f);

		// now draw all the objects
		std::map<framework::string, base*>::iterator it = m_entities.members().begin();
		std::map<framework::string, base*>::iterator end = m_entities.members().end();
		while (it != end)
		{
			Entity* entity = reinterpret_cast<Entity *>(it->second);

			// we only instigate the drawing of objects here that have no parent
			// all the others should get encompassed as they will have parents which
			// will be one of these entities. if not, we have zombies... wwaaaaaaargh!
			if (entity->parent() == NULL)
			{
				glPushMatrix();
				entity->drawSetup(mode);
				entity->draw(mode);
				glPopMatrix();
			}
			++it;
		}
	}*/

	glDisable(GL_LIGHTING);

	glColor3ub(211, 208, 192);
	glBegin(GL_LINES);
	for (int x = -100; x <= 100; x += 10)
	{
		glVertex3i(x, 0, -100);
		glVertex3i(x, 0, 100);
	}
	for (int z = -100; z <= 100; z += 10)
	{
		glVertex3i(-100, 0, z);
		glVertex3i(100, 0, z);
	}
	glEnd();

	// draw some axis reference arrows
/*	glColor3ub(255, 0, 0);
	glPushMatrix();
	glTranslatef(-100, 0, 100);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	auxSolidCylinder(0.5, 10.0);
	glPopMatrix();

	glColor3ub(0, 255, 0);
	glPushMatrix();
	glTranslatef(-100, 10, 100);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	auxSolidCylinder(0.5, 10.0);
	glPopMatrix();

	glColor3ub(0, 0, 255);
	glPushMatrix();
	glTranslatef(-100, 0, 100);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	auxSolidCylinder(0.5, 10.0);
	glPopMatrix();
*/
	glEnable(GL_LIGHTING);

	// special case - allow the selected item to draw any additional stuff
	if (m_selectedEntity)
	{
		m_selectedEntity->drawAdornments();
	}
}

void Engine::processInput()
{
	BYTE keys[512];
	GetKeyboardState(keys);

	if (keys[VK_DOWN] & 0x80)
	{
		m_activeCamera->translate(0.0, 0.0, 5.0, 0);
	}
	if (keys[VK_UP] & 0x80)
	{
		m_activeCamera->translate(0.0, 0.0, -5.0, 0);
	}
	if (keys[VK_LEFT] & 0x80)
	{
		m_activeCamera->rotate(2.0f * constants::pi / 180.0f, 0.0, 1.0, 0.0, 0);
	}
	if (keys[VK_RIGHT] & 0x80)
	{
		m_activeCamera->rotate(-2.0f * constants::pi / 180.0f, 0.0, 1.0, 0.0, 0);
	}
	if (keys[VK_NEXT] & 0x80)
	{
		m_activeCamera->rotate(2.0f * constants::pi / 180.0f, 1.0, 0.0, 0.0, 0);
	}
	if (keys[VK_PRIOR] & 0x80)
	{
		m_activeCamera->rotate(-2.0f * constants::pi / 180.0f, 1.0, 0.0, 0.0, 0);
	}
}

Entity* Engine::select(int x, int y, int width, int height)
{
	int hits = 0;
	static GLuint selectBuffer[64];

	// associate our buffer with the selection mechanism
	glSelectBuffer(64, selectBuffer);

	// we are about to change the projection matrix
	glMatrixMode(GL_PROJECTION);

	// enter selection mode and setup the viewport
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix((double)x, (double)m_gl->viewport()[3] - (double)y, 2, 2, m_gl->viewport());
	float fAspect = (float)m_gl->viewport()[2] / (float)m_gl->viewport()[3];
	gluPerspective(m_activeCamera->m_fov, fAspect, m_nearPlane, m_farPlane);
	glMatrixMode(GL_MODELVIEW);

	// draw the scene in the usual way
	drawSetup(GL_SELECT);
	draw(GL_SELECT);

	// now switch rendering mode back so that we can see if we hit anything
	hits = glRenderMode(GL_RENDER);

	// reset the projection matrix
	glMatrixMode(GL_MODELVIEW);

	if (hits == 0)
	{
		if (m_selectedEntity) 
		{
			Entity* e = m_selectedEntity;
			m_selectedEntity = NULL;
			e->notify();
		}
	}
	else if (hits == 1)
	{
		// the number of names on the stack - used for sub-objects etc.
		int count = selectBuffer[0];
		double mini = (double)selectBuffer[1] / (double)UINT_MAX;
		double maxi = (double)selectBuffer[2] / (double)UINT_MAX;
		int selectedID = selectBuffer[3];

		Entity* e = m_entities.findForglName(selectedID);

		// while we have selected an entity we will prepare everything for unproject
		// we project the centre of the selected entity to get a plane that is
		// perpendicular to the screen and passing through the centre of the entity
		double wx = 0.0, wy = 0.0, wz = 0.0;
		glLoadIdentity();
		drawSetup(GL_RENDER);
		e->drawSetup(GL_RENDER);
		glGetDoublev(GL_MODELVIEW_MATRIX, m_gl->modelMatrix());
		gluProject(e->x(), e->y(), e->z(), m_gl->modelMatrix(), m_gl->projectionMatrix(), m_gl->viewport(), &wx, &wy, &m_winZ);

		if (count > 1)
		{
			// the object has a selected sub-object, pass the buck
			e->select(selectBuffer[4]);
		}

		if (e != m_selectedEntity)
		{
			e->notify();
			m_selectedEntity = e;
		}

		return e;
	}
	else
	{
		int idx = 0;
		Entity* e = NULL;

		while (hits-- > 0)
		{
			int count = selectBuffer[idx++];
			double mini = (double)selectBuffer[idx++] / (double)UINT_MAX;
			double maxi = (double)selectBuffer[idx++] / (double)UINT_MAX;
			int selectedID = selectBuffer[idx++];

			e = m_entities.findForglName(selectedID);
		}
	}

	return NULL;
}

void Engine::project(double ox, double oy, double oz, double& wx, double& wy, double& wz)
{
	gluProject(ox, oy, oz, m_gl->modelMatrix(), m_gl->projectionMatrix(), m_gl->viewport(), &wx, &wy, &wz);
	wy = m_gl->viewport()[3] - wy;
}

void Engine::unProject(int x, int y, double* ox, double* oy, double* oz)
{
	gluUnProject(x, m_gl->viewport()[3] - y, m_winZ, m_gl->modelMatrix(), m_gl->projectionMatrix(), m_gl->viewport(), ox, oy, oz);
}

void Engine::unProject(int x, int y, double z, double* ox, double* oy, double* oz)
{
	gluUnProject(x, m_gl->viewport()[3] - y, z, m_gl->modelMatrix(), m_gl->projectionMatrix(), m_gl->viewport(), ox, oy, oz);
}

bool Engine::feedbackBoundingBox(float *&buffer, int &i)
{
	m_feedbackBufferLength = 0;

	do
	{
		// initialise the feedback buffer
		glFeedbackBuffer(m_feedbackBufferSize, GL_2D, m_feedbackBuffer);
		glRenderMode(GL_FEEDBACK);

		// draw the selected mesh only
		drawSetup(GL_FEEDBACK);
		glPushMatrix();
		m_selectedEntity->drawAdornments();
		glPopMatrix();

		m_feedbackBufferLength = glRenderMode(GL_RENDER);
		if (m_feedbackBufferLength == -1)
		{
			// the buffer wasn't big enough
			m_feedbackBufferSize = (int)(m_feedbackBufferSize * 1.5);
			delete [] m_feedbackBuffer;
			m_feedbackBuffer = new float[m_feedbackBufferSize];
		}
	}
	while (m_feedbackBufferLength == -1);

	// if no feedback was gained, return null
	if (i == 0)
	{
		buffer = NULL;
		i = 0;
		return false;
	}

	buffer = m_feedbackBuffer;
	i = m_feedbackBufferLength;

	return true;
}

bool Engine::feedbackSelectedMesh(float*& buf, int& i)
{
	m_feedbackBufferLength = 0;

	if (m_selectedEntity->m_selectedMesh)
	{
		do
		{
			// initialise the feedback buffer
			glFeedbackBuffer(m_feedbackBufferSize, GL_2D, m_feedbackBuffer);
			glRenderMode(GL_FEEDBACK);

			// draw the selected mesh only
			drawSetup(GL_FEEDBACK);
			glPushMatrix();
			m_selectedEntity->drawSetup(GL_FEEDBACK);
			m_selectedEntity->draw(GL_FEEDBACK, m_selectedEntity->m_selectedMesh);
			glPopMatrix();

			m_feedbackBufferLength = glRenderMode(GL_RENDER);
			if (m_feedbackBufferLength == -1)
			{
				// the buffer wasn't big enough
				m_feedbackBufferSize = (int)(m_feedbackBufferSize * 1.5);
				delete [] m_feedbackBuffer;
				m_feedbackBuffer = new float[m_feedbackBufferSize];
			}
		}
		while (m_feedbackBufferLength == -1);

		// if no feedback was gained, return null
		if (i == 0)
		{
			buf = NULL;
			i = 0;
			return false;
		}

		buf = m_feedbackBuffer;
		i = m_feedbackBufferLength;
	}

	return true;
}

void Engine::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(std::wstring(L"farPlane"), dtParameter);
	newNode->object(&m_farPlane);

	// add a reference to the camera which will be set by name
	newNode = table->enter(L"camera", dtReference);
	newNode->object(this);
}

// get a parameter by name or ordinal
base* Engine::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	if (wcscmp(L"camera", (const wchar_t *)(*name)) == 0) return new framework::string(m_activeCamera->m_name);
	else if (wcscmp(L"farPlane", (const wchar_t *)(*name)) == 0) return new real(m_farPlane);

	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Engine::set(base* param, base* value)
{
	framework::string* name = static_cast<framework::string *>(param);

	if (wcscmp(L"camera", static_cast<const wchar_t *>(*name)) == 0)
	{
		framework::string* cam = static_cast<framework::string *>(value);
		std::map<framework::string, base* >::iterator it = m_cameras.members().find(*cam);
		if (it != m_cameras.members().end())
		{
			m_activeCamera = static_cast<Camera* >(it->second);
		}
	}
}

// need to work out when an object is selected or just active in the interface
// because we want to interact with interface objects in order to manipulate
// world objects. how do we do this??
void Engine::leftButtonDown(int x, int y)
{
	if (m_editMode)
	{
		m_leftButtonDown = true;
		Entity* e = select(x, y, 2, 2);
		if (e)
		{
			e->leftButtonDown(x, y);
		}
	}
	else
	{
		addEvent(&m_lbDown);
	}
}

void Engine::leftButtonUp(int x, int y)
{
	if (m_editMode)
	{
		if (m_capturingEntity)
		{
			m_capturingEntity->leftButtonUp(x, y);
		}
		else
		{
			if (m_selectedEntity)
			{
				m_selectedEntity->leftButtonUp(x, y);
			}
			else
			{
			}
		}

		m_leftButtonDown = false;
	}
	else
	{
		addEvent(&m_lbUp);
	}
}

void Engine::mouseMove(int x, int y, bool leftButtonDown, bool rightButtonDown)
{
	// if there is an entity capturing input we must pass this message straight to it
	if (m_capturingEntity)
	{
		m_capturingEntity->mouseMove(x, y, m_leftButtonDown, m_rightButtonDown);
	}
	else
	{
		// otherwise we pass it to the selected entity
		if (m_selectedEntity)
		{
			m_selectedEntity->mouseMove(x, y, m_leftButtonDown, m_rightButtonDown);
		}
		else
		{
			// if we want to do something here is the place
		}
	}
}

void Engine::rightButtonDown(int x, int y)
{
	if (m_editMode)
	{
		m_rightButtonDown = true;
		Entity* e = select(x, y, 2, 2);
		if (e)
		{
			e->rightButtonDown(x, y);
		}
	}
	else
	{
		addEvent(&m_rbDown);
	}
}

void Engine::rightButtonUp(int x, int y)
{
	if (m_editMode)
	{
		m_selectedEntity = NULL;
		m_rightButtonDown = false;
	}
	else
	{
		addEvent(&m_rbUp);
	}
}

void Engine::keyPress(int key)
{
}

void Engine::generateShadowMaps()
{
	GLint shadowSize = 512;                 // set based on window size

	// generate a shadow map for each light
	std::map<framework::string, base *>::iterator it = m_lights.members().begin();
	while (it != m_lights.members().end())
	{
		Light* light = (Light *)it->second;
		Vector& v = light->position();
		GLfloat lightToSceneDistance, nearPlane, fieldOfView;
		GLfloat lightModelview[16], lightProjection[16];

		glGenTextures(1, &light->m_shadowMap);
		glBindTexture(GL_TEXTURE_2D, light->m_shadowMap);
	    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	    if (m_ambientShadowSupported)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.5f);

		// Save the depth precision for where it's useful
		lightToSceneDistance = light->position().length();

		nearPlane = lightToSceneDistance - 150.0f;
		if (nearPlane < 50.0f)
			nearPlane = 50.0f;
		// Keep the scene filling the depth texture
		fieldOfView = 17000.0f / lightToSceneDistance;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fieldOfView, 1.0f, nearPlane, nearPlane + 300.0f);
		glGetFloatv(GL_PROJECTION_MATRIX, lightProjection);
		// Switch to light's point of view
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(v.m_values[0], v.m_values[1], v.m_values[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, lightModelview);
		glViewport(0, 0, shadowSize, shadowSize);

		// Clear the window with current clearing color
		glClear(GL_DEPTH_BUFFER_BIT);

		// All we care about here is resulting depth values
		glShadeModel(GL_FLAT);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_NORMALIZE);
		glColorMask(0, 0, 0, 0);

		// Overcome imprecision
//		glEnable(GL_POLYGON_OFFSET_FILL);

		// Draw objects in the scene
		draw(GL_RENDER);

		// Copy depth values into depth texture
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
						 0, 0, shadowSize, shadowSize, 0);

		// Restore normal drawing state
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_NORMALIZE);
		glColorMask(1, 1, 1, 1);
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Set up texture matrix for shadow map projection
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5f, 0.5f, 0.5f);
		glScalef(0.5f, 0.5f, 0.5f);
		glMultMatrixf(lightProjection);
		glMultMatrixf(lightModelview);
		++it;
	}
}

void Engine::checkForCollisions()
{
	m_collisions.clear();

	if (m_entities.members().size() < 2) return;

	std::map<framework::string, base*>::iterator it = m_entities.members().begin();
	std::map<framework::string, base*>::iterator end = m_entities.members().end();
	while (it != end)
	{
		Entity* entity = reinterpret_cast<Entity *>(it->second);
		if (entity->m_allowCollisions == false)
		{
			++it;
			continue;
		}

		const wchar_t* className = entity->className();
		if (wcscmp(className, L"Light") == 0 || wcscmp(className, L"Camera") == 0)
		{
			++it;
			continue;
		}

		std::map<framework::string, base*>::iterator it2 = m_entities.members().begin();
		while (it2 != end)
		{
			Entity* entity2 = reinterpret_cast<Entity *>(it2->second);
			if (entity2 == entity)
			{
				++it2;
				continue;
			}

			if (entity2->m_allowCollisions == false)
			{
				++it2;
				continue;
			}

			Vector v = entity->position() - entity2->position();
			if (v.length() < entity->m_radius + entity2->m_radius)
			{
				// TODO: check for collisions
				checkVertexFaceCollisions(entity, entity2);
			}

			++it2;
		}

		++it;
	}

	if (m_collisions.size() > 0)
	{
		wchar_t sz[10];
		swprintf(sz, 10, L"%d\n", m_collisions.size());
		OutputDebugString(sz);
	}
}

void Engine::checkVertexFaceCollisions(Entity* e1, Entity* e2)
{
	std::vector<Mesh>::iterator e1_mit = e1->m_template->meshes().begin();
	while (e1_mit != e1->m_template->meshes().end())
	{
		unsigned int verts = e1_mit->GetVertexCount();
		for (unsigned int vidx = 0; vidx < verts; vidx++)
		{
			Vector3 temp = e1_mit->GetVertex(vidx);
			Vector e1v(temp.x, temp.y, temp.z);

			// need this is world coordinates
			e1v = e1v * e1->m_matrix;

			// vector from point to centre of first object is within radius of second object?
			// (rules out any points on the dark side
			if ((e1v - e2->position()).length() > e2->m_radius + 0.01)
			{
				continue;
			}

			// need to check each vertex in e1 against each face in e2
			std::vector<Mesh>::iterator mit = e2->m_template->meshes().begin();
			while (mit != e2->m_template->meshes().end())
			{
				int tris = mit->GetTriangleCount();
				for (int t = 0; t < tris; t++)
				{
					const Triangle& tri = mit->GetTriangle(t);
					Vector3 temp = mit->GetVertex(tri.a);
					Vector v0(temp.x, temp.y, temp.z);
					v0 = v0 * e2->m_matrix;
					temp = mit->GetVertex(tri.b);
					Vector v1(temp.x, temp.y, temp.z);
					v1 = v1 * e2->m_matrix;
					temp = mit->GetVertex(tri.c);
					Vector v2(temp.x, temp.y, temp.z);
					v2 = v2 * e2->m_matrix;

					Vector u = v1 - v0; //v0 - v1;
					Vector v = v2 - v0; //v0 - v2;

					double d = e1v.distanceFromPlane(u, v, v0);

					if (d < 0.01)
					{
						Vector pt1 = e1v - e1->position();
						Vector pt2 = e1v - e2->position();

						Vector vel1 = e1->m_linearVelocity + e1->m_angularVelocity.crossProduct(pt1);
						Vector vel2 = e2->m_linearVelocity + e1->m_angularVelocity.crossProduct(pt2);

						Vector norm = u.crossProduct(v);
						norm.normalise();

						Vector Vr = (vel1 - vel2);
						double Vrn = Vr.dot(norm);
						if (Vrn <= 0.0)
						{
							Collision collision;
							collision.m_e1 = e1;
							collision.m_e2 = e2;
							collision.m_collisionNormal = norm;
							collision.m_collisionPoint = e1v;
							collision.m_collisionTangent = norm.crossProduct(Vr).crossProduct(norm);
							collision.m_collisionTangent.normalise();
							collision.m_relativeVelocity = Vr;
							m_collisions.push_back(collision);
							break;
						}
					}
				}

				++mit;
			}
		}

		++e1_mit;
	}
}

void Engine::resolveCollisions()
{
	float j = 0.0f;
	float vrt = 0.0f;
	float fCR = 0.6; // coefficient of restitution
	float mu = 0.3; // friction coefficient

	std::vector<Collision>::iterator it = m_collisions.begin();
	while (it != m_collisions.end())
	{
		Entity* e1 = it->m_e1;
		Entity* e2 = it->m_e2;

		Vector v1 = it->m_collisionPoint - e1->position();
		Vector v2 = it->m_collisionPoint - e2->position();

		j = ((it->m_relativeVelocity.dot(it->m_collisionNormal)) * -(1 + fCR)) / 
			(
				(1.0f / e1->m_mass + 1.0f / e2->m_mass) + 
				(it->m_collisionNormal.dot(((v1.crossProduct(it->m_collisionNormal) * e1->m_inertiaInverse).crossProduct(v1)))) + 
				(it->m_collisionNormal.dot(((v2.crossProduct(it->m_collisionNormal) * e2->m_inertiaInverse).crossProduct(v2))))
			);

		vrt = it->m_relativeVelocity.dot(it->m_collisionTangent);

		Vector jNormal = it->m_collisionNormal * j;
		Vector mujt = it->m_collisionTangent * (mu * j);

		if (fabs(vrt) > 0.0f)
		{
			e1->m_linearVelocity += (jNormal + mujt) / e1->m_mass;
			e1->m_angularVelocity += v1.crossProduct(jNormal + mujt) * e1->m_inertiaInverse;
			e2->m_linearVelocity -= (jNormal + mujt) / e2->m_mass;
			e2->m_angularVelocity -= v2.crossProduct(jNormal + mujt) * e2->m_inertiaInverse;
		}
		else
		{
			e1->m_linearVelocity += jNormal / e1->m_mass;
			e1->m_angularVelocity += v1.crossProduct(jNormal) * e1->m_inertiaInverse;
			e1->m_linearVelocity += jNormal / e2->m_mass;
			e1->m_angularVelocity += v2.crossProduct(jNormal) * e2->m_inertiaInverse;
		}

		++it;
	}
}

unsigned long Engine::workerThread(void* param)
{
	Engine* engine = (Engine *)param;

	while (WaitForSingleObject(engine->m_threadEvent, 0) != WAIT_OBJECT_0)
	{
		engine->step();
		Sleep(25);
	}

	return 0;
}