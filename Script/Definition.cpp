#include ".\definition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Definition::Definition(DefinitionType dt)
	: m_dt(dt)
	, m_symbolTable(NULL)
	, m_icode(NULL)
	, m_dispatcher(NULL)
{
}

Definition::~Definition(void)
{
}
