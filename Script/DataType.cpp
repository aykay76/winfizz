#include ".\datatype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DataType::DataType(TypeCode tc)
	: m_tc(tc)
{
}

DataType::~DataType(void)
{
}
