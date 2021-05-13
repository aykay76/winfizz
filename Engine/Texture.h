#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <string>
#include "../framework/object.h"

class Texture : public object
{
public:
	Texture(void);
	~Texture(void);

public:
	virtual base* clone();
	// get a parameter by name or ordinal
	virtual base* get(base* param);
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

public:
	void fromFile(std::wstring& filename);
	void fromBits(int width, int height, int bpp, void* bits);

public:
	GLuint m_id;
	static long m_count;
};
