#include "StdAfx.h"
#include ".\glwindows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLWindows::GLWindows(void)
{
}

GLWindows::~GLWindows(void)
{
}

bool GLWindows::create(HWND hwnd, int width, int height, int bpp, bool fullscreen)
{
	GLuint		PixelFormat;						// Holds The Results After Searching For A Match
	WNDCLASS	wc;							// Windows Class Structure
	DWORD		dwExStyle;						// Window Extended Style
	DWORD		dwStyle;						// Window Style

	GL::create(width, height, bpp, fullscreen);

	if (hwnd == NULL)
	{
		m_localWindow = true;
	}
	else
	{
		m_localWindow = false;
		m_hwnd = hwnd;
	}

	if (m_localWindow)
	{
		RECT WindowRect;							// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left=(long)0;						// Set Left Value To 0
		WindowRect.right=(long)width;						// Set Right Value To Requested Width
		WindowRect.top=(long)0;							// Set Top Value To 0
		WindowRect.bottom=(long)height;						// Set Bottom Value To Requested Height

		HINSTANCE hInstance		= GetModuleHandle(NULL);			// Grab An Instance For Our Window
		wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Move, And Own DC For Window
		wc.lpfnWndProc		= (WNDPROC)GLWindows::WndProc;				// WndProc Handles Messages
		wc.cbClsExtra		= 0;						// No Extra Window Data
		wc.cbWndExtra		= 0;						// No Extra Window Data
		wc.hInstance		= hInstance;					// Set The Instance
		wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL;						// No Background Required For GL
		wc.lpszMenuName		= NULL;						// We Don't Want A Menu
		wc.lpszClassName	= L"OpenGL";					// Set The Class Name

		if (!RegisterClass(&wc))						// Attempt To Register The Window Class
		{
			MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;							// Exit And Return FALSE
		}

		if (m_fullscreen)								// Attempt Fullscreen Mode?
		{

			DEVMODE dmScreenSettings;					// Device Mode
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth	= width;			// Selected Screen Width
			dmScreenSettings.dmPelsHeight	= height;			// Selected Screen Height
			dmScreenSettings.dmBitsPerPel	= bpp;				// Selected Bits Per Pixel
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{

				// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
				if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				{

					m_fullscreen = false;				// Select Windowed Mode (Fullscreen=FALSE)
				}
				else
				{

					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL,L"Program Will Now Close.",L"ERROR",MB_OK|MB_ICONSTOP);
					return FALSE;					// Exit And Return FALSE
				}
			}
		}

		if (m_fullscreen)								// Are We Still In Fullscreen Mode?
		{
			dwExStyle=WS_EX_APPWINDOW;					// Window Extended Style
			dwStyle=WS_POPUP;						// Windows Style
			ShowCursor(FALSE);						// Hide Mouse Pointer
		}
		else
		{
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle=WS_OVERLAPPEDWINDOW;					// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

		if (!(m_hwnd = CreateWindowEx(dwExStyle,				// Extended Style For The Window
						L"OpenGL",				// Class Name
						L"",					// Window Title
						WS_CLIPSIBLINGS |			// Required Window Style
						WS_CLIPCHILDREN |			// Required Window Style
						dwStyle,				// Selected Window Style
						0, 0,					// Window Position
						WindowRect.right-WindowRect.left,	// Calculate Adjusted Window Width
						WindowRect.bottom-WindowRect.top,	// Calculate Adjusted Window Height
						NULL,					// No Parent Window
						NULL,					// No Menu
						hInstance,				// Instance
						NULL)))					// Don't Pass Anything To WM_CREATE

		{
			destroy();							// Reset The Display
			MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;							// Return FALSE
		}

		SetWindowLongPtrW(m_hwnd, GWL_USERDATA, (LONG_PTR)this);
	}

	static	PIXELFORMATDESCRIPTOR pfd=					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,							// Request An RGBA Format
		bpp,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,							// Main Drawing Layer
		0,								// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	if (!(m_hdc = GetDC(m_hwnd)))							// Did We Get A Device Context?
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(m_hdc, &pfd)))				// Did Windows Find A Matching Pixel Format?
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if(!SetPixelFormat(m_hdc, PixelFormat, &pfd))				// Are We Able To Set The Pixel Format?
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(m_hrc=wglCreateContext(m_hdc)))					// Are We Able To Get A Rendering Context?
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!wglMakeCurrent(m_hdc, m_hrc))						// Try To Activate The Rendering Context
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (m_localWindow)
	{
		ShowWindow(m_hwnd,SW_SHOW);						// Show The Window
		SetForegroundWindow(m_hwnd);						// Slightly Higher Priority
		SetFocus(m_hwnd);								// Sets Keyboard Focus To The Window
	}

	resize(width, height);						// Set Up Our Perspective GL Screen

	if (!init())								// Initialize Our Newly Created GL Window
	{
		destroy();							// Reset The Display
		MessageBox(NULL,L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	return true;
}

bool GLWindows::init()
{
	GL::init();

	return true;
}

bool GLWindows::resize(int width, int height)
{
	GL::resize(width, height);

	return true;
}

bool GLWindows::destroy()
{
	GL::destroy();

	if (m_hdc && !ReleaseDC(m_hwnd, m_hdc))					// Are We Able To Release The DC
	{
		MessageBox(NULL,L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hdc = NULL;							// Set DC To NULL
	}

	if (m_localWindow)
	{
		if (m_hwnd && !DestroyWindow(m_hwnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			m_hwnd = NULL;							// Set hWnd To NULL
		}

		if (!UnregisterClass(L"OpenGL", GetModuleHandle(NULL)))				// Are We Able To Unregister Class
		{
			MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
	}

	return true;
}

void GLWindows::swap()
{
	SwapBuffers(m_hdc);
}

bool GLWindows::prep()
{
	GL::prep();

	HGLRC hrc = wglGetCurrentContext();
	HDC hdc = wglGetCurrentDC();
	BOOL ret = FALSE;

	if (hdc != m_hdc)
	{
		ret = wglMakeCurrent(m_hdc, m_hrc);
	}

	return ret==TRUE?true:false;
}

LRESULT CALLBACK GLWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GLWindows* instance = (GLWindows *)GetWindowLongPtrA(hWnd, GWL_USERDATA);

	switch (uMsg)								// Check For Windows Messages
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			instance->destroy();
		}
		break;
		case WM_ACTIVATE:						// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				instance->m_active=true;					// Program Is Active
			}
			else
			{
				instance->m_active=false;					// Program Is No Longer Active
			}

			return 0;						// Return To The Message Loop
		}

		case WM_SYSCOMMAND:						// Intercept System Commands
		{
			switch (wParam)						// Check System Calls
			{
				case SC_SCREENSAVE:				// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;					// Prevent From Happening
			}
			break;							// Exit
		}

		case WM_CLOSE:							// Did We Receive A Close Message?
		{
			PostQuitMessage(0);					// Send A Quit Message
			return 0;						// Jump Back
		}

/*		case WM_KEYDOWN:						// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;						// Jump Back
		}

		case WM_KEYUP:							// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;						// Jump Back
		}
*/
		case WM_SIZE:							// Resize The OpenGL Window
		{
			if (instance) instance->resize(LOWORD(lParam), HIWORD(lParam));
			return 0;						// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

typedef const char* (* WINAPI extfunc)(HDC hdc);

bool GLWindows::isExtensionSupported(const char* extension)
{
	if (GL::isExtensionSupported(extension) == true) return true;

	extfunc wglGetEntensionsStringARB;
	wglGetEntensionsStringARB = (extfunc)wglGetProcAddress("wglGetExtensionsStringARB");
	GLubyte* wgl_ext_string = (GLubyte *)(char *)wglGetEntensionsStringARB(m_hdc);
	GLubyte *where, *terminator;
	const GLubyte *start = wgl_ext_string;
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