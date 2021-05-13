#pragma once

/******************************************************************************
the Engine is responsible for making sure that everything runs in correct
order. keyframes and tweens are triggered at particular times and the Engine
is responsible for making sure that they start and stop at the right times

the Engine is the core of the whole engine. it will use classes like the
parser to read the script information but the data will be stored in this class
it is possible that the sequence will loop so we need to keep the keyframes and
tweens around, but having a moving pointer and be able to know when we are at 
the end of the list.

******************************************************************************/

#include <string>
#include <list>
#include <map>
#include <queue>
#include <stack>

#include "../framework/real.h"
#include "vector.h"
#include "keyframe.h"
#include "event.h"

#include "timers.h"
#include "textures.h"
#include "materials.h"
#include "templates.h"
#include "entities.h"
#include "lights.h"
#include "cameras.h"

#include "../script/parser.h"

class Entity;
class Command;
class Object;
class SymbolTable;
class Collision;

class Engine : public Entity
{
private:
	Engine(void);
public:
	~Engine(void);

public:
	static Engine* instance();
	static void destroy();
private:
	static Engine* m_instance;

// object interface
public:
	// get a parameter by name or ordinal
	virtual base* get(base* param);
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);
	// describe myself to the script subsystem
	virtual void describe(SymbolTable* table);

// the entity interface - everything to display and interact within the scene
public:
	virtual void drawSetup(int mode, bool fromParent = false);
	virtual void draw(int mode, Mesh* onlyMesh = NULL);
	virtual bool step(double fractionSecond);
	virtual void leftButtonDown(int x, int y);
	virtual void leftButtonUp(int x, int y);
	virtual void mouseMove(int x, int y, bool leftButtonDown = false, bool rightButtonDown = false);
	virtual void rightButtonDown(int x, int y);
	virtual void rightButtonUp(int x, int y);
	virtual void keyPress(int key);

	virtual void generateShadowMaps();

public:
	// initialise any objects that need to be initialised after GL has been setup (like textures for example)
	void init();
	// kick off the sequence
	void start(bool doFirstStep);
	// progress the sequence by a frame
	bool step();
	// i guess we should have a pause function
	void pause();
	// and probably a stop/reset
	void reset();

	static unsigned long __stdcall workerThread(void* param);

public:
	bool feedbackBoundingBox(float*& buffer, int& i);
	bool feedbackSelectedMesh(float*& buffer, int& i);
//	int getVertexOnSelectedMesh(int x, int y);

public:
	double fps() { return m_fps; }
	double duration() { return m_duration; }
	std::list<Keyframe *>& keyframes() { return m_keyframes; }
	std::map<std::wstring, std::wstring>& configuration() { return m_configuration; }
	void setGL(GL* gl) { m_gl = gl; }
	GL* gl() { return m_gl; }
	std::stack<base *>& runtimeStack() { return m_runtimeStack; }
	SymbolTable& globals() { return m_parser.symbols(); }
	std::queue<Event *>& eventQueue() { return m_eventQueue; }
	bool editMode() { return m_editMode; }
	Camera* camera() { return m_activeCamera; }
	Textures& textures() { return m_textures; }
	Materials& materials() { return m_materials; }
	Templates& templates() { return m_templates; }
	Entities& entities() { return m_entities; }
	Timers& timers() { return m_timers; }
	Lights& lights() { return m_lights; }
	Cameras& cameras() { return m_cameras; }

	Entity* selectedEntity() { return m_selectedEntity; }
	void setSelectedEntity(Entity* e) { m_selectedEntity = e; }

	Entity* capturingEntity() { return m_capturingEntity; }
	void setCapturingEntity(Entity* e) { m_capturingEntity = e; }

	void setActiveCamera(Camera* camera) { m_activeCamera = camera; }

public:
	void addEvent(Event *event);
	void processEventQueue();
	void processInput();
	Entity* select(int x, int y, int w, int h);
	void project(double ox, double oy, double oz, double& wx, double& wy, double &wz);
	void unProject(int x, int y, double* ox, double* oy, double* oz);
	void unProject(int x, int y, double z, double* ox, double* oy, double* oz);

	Event& initEvent() { return m_init; }
	Event& frameStartEvent() { return m_frameStart; }
	Event& frameDoneEvent() { return m_frameDone; }
	Event& mouseMoveEvent() { return m_mouseMove; }

public:
	// check for collisions between entities
	void checkForCollisions();
	void checkVertexFaceCollisions(Entity* e1, Entity* e2);
	// resolve any collisions that occur
	void resolveCollisions();

public:
	bool m_ambientShadowSupported;

public:
	void initialiseScriptSystem();
	void wireUpFixedEvents(SymbolTable& global);

private:
	// the frequency of the high resolution timer
	LARGE_INTEGER m_frequency;

	// the time at which this sequence started
	LARGE_INTEGER m_startTime;

	// the time of the last frame
	LARGE_INTEGER m_lastFrame;

	// total running time so far
	double m_duration;

	// estimated finish time based on all active elements and keyframes
	double m_finishTime;

	// current frame number
	int m_frame;

	// average frames per second
	double m_fps;

	// list of keyframes
	std::list<Keyframe *> m_keyframes;
	std::list<Keyframe *>::iterator m_keyIterator;

	// map of name -> value configuration options
	std::map<std::wstring, std::wstring> m_configuration;

	Textures m_textures;
	Materials m_materials;
	Templates m_templates;
	Entities m_entities;
	Timers m_timers;
	Lights m_lights;
	Cameras m_cameras;

	Camera* m_activeCamera;
	Entity* m_selectedEntity;
	Entity* m_capturingEntity;
	double m_winZ;

	// switches for mouse buttons
	bool m_leftButtonDown;
	bool m_rightButtonDown;
	bool m_middleButtonDown;

	// run state
	bool m_running;

	// edit or view
	bool m_wireframe;
	bool m_editMode;
	bool m_overplot;

	// an event queue
	std::queue<Event *> m_eventQueue;
	// fixed events
	Event m_init;
	Event m_mouseMove;
	Event m_frameStart;
	Event m_frameDone;
	Event m_lbDown;
	Event m_lbUp;
	Event m_rbDown;
	Event m_rbUp;

	// not owned pointer to OpenGL implementation
	GL* m_gl;

	real m_nearPlane;
	real m_farPlane;

	std::stack<base *> m_runtimeStack;
	Parser m_parser;

	float* m_feedbackBuffer;
	int m_feedbackBufferSize;
	int m_feedbackBufferLength;

	std::vector<Collision> m_collisions;

	HANDLE m_threadHandle;
	DWORD m_threadID;
	HANDLE m_threadEvent;
};
