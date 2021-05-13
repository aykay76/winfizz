#pragma once

#include "gl.h"

// Windows specific implementation of the GL class
class GLWindows :
	public GL
{
public:
	GLWindows(void);
	virtual ~GLWindows(void);

public:
	virtual bool create(HWND hwnd, int width, int height, int bpp, bool fullscreen);
	virtual bool init();
	virtual bool resize(int width, int height);
	virtual bool destroy();
	virtual bool prep();
	virtual void swap();
	virtual bool isExtensionSupported(const char* ext);

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	HDC hdc() { return m_hdc; }

protected:
	HWND m_hwnd;
	HDC m_hdc;
	bool m_localWindow;
	bool m_active;
};
