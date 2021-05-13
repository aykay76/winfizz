#include "../stdafx.h"
#include ".\texture.h"
#include <gdiplus.h>
using namespace Gdiplus;
#include <io.h>
#include <gl/glu.h>

long Texture::m_count = 1;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Texture::Texture(void)
{
	glGenTextures(1, &m_id);
}

base* Texture::clone()
{
	return new Texture(*this);
}

Texture::~Texture(void)
{
}

// get a parameter by name or ordinal
base* Texture::get(base* param)
{
	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Texture::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* Texture::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}

void Texture::fromFile(std::wstring& filename)
{
	if (_waccess(filename.c_str(), 0) != 0) return;

	Gdiplus::Bitmap bmp(filename.c_str());
	BitmapData bd;
	Rect rc(0, 0, bmp.GetWidth(), bmp.GetHeight());
	bmp.RotateFlip(RotateNoneFlipY);
	bmp.LockBits(&rc, ImageLockModeRead, bmp.GetPixelFormat(), &bd);

	GLenum err = glGetError();
	const char* error = (const char *)gluErrorString(err);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);				// Bind Our Texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // Mipmap Linear Filtering

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texturing Contour Anchored To The Object
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Generate Mipmapped Texture
	if (GetPixelFormatSize(bd.PixelFormat) == 32)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bd.Width, bd.Height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bd.Scan0);
//		glTexImage2D(GL_TEXTURE_2D, 0, 4, bd.Width, bd.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bd.Scan0);
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bd.Width, bd.Height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bd.Scan0);
//		glTexImage2D(GL_TEXTURE_2D, 0, 3, bd.Width, bd.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, bd.Scan0);
	}

	GLboolean b = glIsTexture(m_id);
	bmp.UnlockBits(&bd);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::fromBits(int width, int height, int bpp, void *bits)
{
	GLenum err = glGetError();
	const char* error = (const char *)gluErrorString(err);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, m_id);				// Bind Our Texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // Mipmap Linear Filtering

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texturing Contour Anchored To The Object
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Generate Mipmapped Texture
	if (bpp == 32)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits);
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bits);
	}
}