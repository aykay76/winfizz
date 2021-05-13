#pragma once

#include "../framework/collection.h"
#include "mesh.h"
#include <map>
#include <string>

class Templates : public collection
{
public:
	Templates(void);
	virtual ~Templates(void);
	
	void describe(SymbolTable* table);

// Dispatcher interface
public:

// Collection interface
public:
	virtual base* clone();
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

	base* add(std::vector<base *>& params);
	base* sphere(std::vector<base *>& params);
	base* spline(std::vector<base *>& params);
	base* torus(std::vector<base *>& params);
	base* cylinder(std::vector<base *>& params);
	base* cone(std::vector<base *>& params);

protected:
    // used internally for reading
    char m_objName[100];
    // true if end of file is reached
    bool m_eof;
    // buffer for loading, used for speedup
    unsigned char *m_buffer;
    // the size of the buffer
    unsigned int m_bufferSize;
    // the current cursor position in the buffer
    unsigned int m_pos;

};
