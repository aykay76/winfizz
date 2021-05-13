#include ".\commandrotate.h"
#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CommandRotate::CommandRotate(std::vector<std::pair<std::wstring, std::wstring> >& attrs)
	: Command()
	, m_dax(0)
	, m_day(0)
	, m_daz(0)
	, m_cx(0)
	, m_cy(0)
	, m_cz(0)
{
	wchar_t* p;
	std::vector<std::pair<std::wstring, std::wstring> >::iterator it = attrs.begin();

	for (; it != attrs.end(); ++it) 
	{
		if (it->first.compare(L"duration") == 0)
		{
			m_duration = wcstod(it->second.c_str(), &p);
			m_lifespan = m_duration;
		}
		else if (it->first.compare(L"object") == 0)
		{
			m_name = it->second;
		}
		else if (it->first.compare(L"dax") == 0)
		{
			m_dax = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"day") == 0)
		{
			m_day = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"daz") == 0)
		{
			m_daz = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"cx") == 0)
		{
			m_cx = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"cy") == 0)
		{
			m_cy = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"cz") == 0)
		{
			m_cz = (float)wcstod(it->second.c_str(), &p);
		}
	}
}

CommandRotate::~CommandRotate(void)
{
}

bool CommandRotate::init()
{
/*	std::map<std::wstring, Object *>::iterator obj_it = Engine::instance()->pipeline().find(m_name);
	if (obj_it == Engine::instance()->pipeline().end())
	{
		m_object = NULL;
		return false;
	}
	else
	{
		m_object = obj_it->second;
	}

	m_object->commands().push_back(this);
*/
	return true;
}

bool CommandRotate::step(double fractionSecond)
{
	bool result = Command::step(fractionSecond);

	float proportion = 1.0f - (float)(m_lifespan / m_duration);
	if (proportion > 1.0) proportion = 1.0f;

	glTranslatef(m_cx, m_cy, m_cz);

	glRotatef(m_dax * proportion, 1.0f, 0.0f, 0.0f);
	glRotatef(m_day * proportion, 0.0f, 1.0f, 0.0f);
	glRotatef(m_daz * proportion, 0.0f, 0.0f, 1.0f);

	glTranslatef(-m_cx, -m_cy, -m_cz);

	return result;
}