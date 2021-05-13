#include "../stdafx.h"
#include ".\command.h"
#include "commandmove.h"
#include "commandrotate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Command::Command()
	: m_duration(0.0)
	, m_lifespan(0.0)
	, m_object(0)
{
}

Command::~Command(void)
{
}

bool Command::step(double fractionSecond)
{
	m_lifespan -= fractionSecond;
	if (m_lifespan <= 0.0) 
		return false;

	return true;
}

Command* Command::create(std::wstring& strType, std::vector<std::pair<std::wstring, std::wstring> >& attrs)
{
	if (strType.compare(L"add") == 0)
	{
//		return new CommandAdd(attrs);
	}
	else if (strType.compare(L"move") == 0)
	{
		return new CommandMove(attrs);
	}
	else if (strType.compare(L"rotate") == 0)
	{
		return new CommandRotate(attrs);
	}

	// TODO: haha, not really - need to implement this
	return 0;
}

bool Command::init()
{
	return true;
}