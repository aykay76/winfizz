#include "../stdafx.h"
#include "boolean.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace framework
{

boolean::boolean()
{
}

base* boolean::clone()
{
	return new boolean(*this);
}

boolean::~boolean()
{
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void boolean::describe()
{
}

};