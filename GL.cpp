#include "StdAfx.h"
#include ".\gl.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GL::GL(void)
{
}

GL::~GL(void)
{
}

bool GL::create(int width, int height, int bpp, bool fullscreen)
{
	m_fullscreen = fullscreen;

	return false;
}

bool GL::init()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);											// The Type Of Depth Test To Do

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// setup the lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// set texture parameter

	// fix ambient light so that nothing is ever in total darkness
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
//	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glLineWidth(0.5f);

	return true;
}

bool GL::resize(int width, int height)
{
	if (height==0)								// Prevent A Divide By Zero By
	{
		height=1;							// Making Height Equal One
	}

	m_viewport[0] = 0; m_viewport[1] = 0; m_viewport[2] = width; m_viewport[3] = height;
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

bool GL::destroy()
{
	if (m_fullscreen)								// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);						// Show Mouse Pointer
	}

	if (m_hrc)								// Do We Have A Rendering Context?
	{
		HGLRC hrc = wglGetCurrentContext();
		HDC hdc = wglGetCurrentDC();

		if (hrc != NULL && hrc != m_hrc)
		{
			if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				GLenum err = glGetError();
				const GLubyte* errs = gluErrorString(err);
				DWORD dw = GetLastError();
	
				MessageBox(NULL, L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
		}

		if (!wglDeleteContext(m_hrc))					// Are We Able To Delete The RC?
		{
			MessageBox(NULL,L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		m_hrc = NULL;							// Set RC To NULL
	}

	return true;
}

void GL::swap()
{
}

bool GL::prep()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool GL::isExtensionSupported(const char* extension)
{
	GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	extensions = (GLubyte *)glGetString(GL_EXTENSIONS);

	start = extensions;
	for (;;) 
	{
		where = (GLubyte *) strstr((const char *) start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ') 
		{
			if (*terminator == ' ' || *terminator == '\0') 
				return true;
		}
		start = terminator;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// CUSTOM DRAW FUNCTIONS

void GL::glDrawEllipse(float xradius, float yradius)
{
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < 360; i++)
	{
		//convert degrees into radians
		float degInRad = i * 3.141592653f / 180.0f;
		glVertex2f(cos(degInRad) * xradius, sin(degInRad) * yradius);
	}

	glEnd();
}

void GL::glFillEllipse(float xradius, float yradius)
{
	glBegin(GL_POLYGON);

	for (int i = 0; i < 360; i++)
	{
		//convert degrees into radians
		float degInRad = i * 3.141592653f / 180.0f;
		glVertex2f(cos(degInRad) * xradius, sin(degInRad) * yradius);
	}

	glEnd();
}
