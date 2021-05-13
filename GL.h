#pragma once

#include <gl/gl.h>
#include <gl/glaux.h>

class GL
{
public:
	GL(void);
	virtual ~GL(void);

public:
	virtual bool create(int width, int height, int bpp, bool fullscreen);
	virtual bool init();
	virtual bool resize(int width, int height);
	virtual bool destroy();
	virtual bool prep();
	virtual void swap();

	virtual bool isExtensionSupported(const char* ext);

public:
	void glDrawEllipse(float xradius, float yradius);
	void glFillEllipse(float xradius, float yradius);

public:
	float z() { return m_winz; }
	float scalex() { return m_scalex; }
	float scaley() { return m_scaley; }
	int* viewport() { return m_viewport; }
	double* modelMatrix() { return m_mm; }
	double* projectionMatrix() { return m_pm; }

protected:
	HGLRC m_hrc;
	bool m_fullscreen;
	double m_mm[16];
	double m_pm[16];
	int m_viewport[4];
	float m_winz;
	float m_scalex;
	float m_scaley;
};
