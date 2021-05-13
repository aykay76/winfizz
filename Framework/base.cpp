#include "base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

base::base()
	: m_temporary(false)
	, m_local(false)
{
}

base::~base()
{
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void base::describe(SymbolTable* table)
{
}
