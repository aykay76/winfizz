#include ".\commandmove.h"
#include "engine.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CommandMove::CommandMove(std::vector<std::pair<std::wstring, std::wstring> >& attrs)
	: Command()
	, m_dx(0.0f)
	, m_dy(0.0f)
	, m_da(0.0f)
	, m_dr(0.0f)
	, m_first(true)
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
		else if (it->first.compare(L"dx") == 0)
		{
			m_dx = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"dy") == 0)
		{
			m_dy = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"da") == 0)
		{
			m_da = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"dr") == 0)
		{
			m_dr = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"cx") == 0)
		{
			m_cx = (float)wcstod(it->second.c_str(), &p);
		}
		else if (it->first.compare(L"cy") == 0)
		{
			m_cy = (float)wcstod(it->second.c_str(), &p);
		}
	}

	if (m_dx > 0.0f && m_dy > 0.0f)
	{
		m_type = Linear;
	}
	else if (m_da > 0.0f)
	{
		m_type = Angular;

		// da is specified in degrees for ease of use for the user
		// but stored internally in radians for speed of computation
		m_da = m_da * 3.141592653f / 180.0f;
	}
}

CommandMove::~CommandMove(void)
{
}

bool CommandMove::init()
{
/*	std::map<std::wstring, Entity *>::iterator obj_it = Engine::instance()->pipeline().find(m_name);
	if (obj_it == Engine::instance()->pipeline().end())
	{
		m_object = NULL;
		return false;
	}
	else
	{
		m_object = obj_it->second;
	}

	// add myself to the objects command stack
	m_object->commands().push_back(this);

	// make a note of the radius
	m_radius = sqrt(((m_object->x() - m_cx) * (m_object->x() - m_cx)) + ((m_object->y() - m_cy) * (m_object->y() - m_cy)));

	// make a record of the current angle as it stands
	if (m_object->y() < m_cy)
	{
		m_angle = (float)-atan((m_object->x() - m_cx) / (m_object->y() - m_cy));
		if (m_angle < 0.0) {
			m_angle = 2.0f * 3.141592f + m_angle;
		}
	}
	else if (m_object->y() > m_cy)
	{
		m_angle = 3.141592f - atan((m_object->x() - m_cx) / (m_object->y() - m_cy));
	}
	else
	{
		if (m_object->x() < 10)
		{
			m_angle = 4.712388980f;
		}
		else
		{
			m_angle = 1.570796326f;
		}
	}

	m_first = false;
8*/
	return true;
}

bool CommandMove::step(double fractionSecond)
{
	if (m_object)
	{
		float x = m_object->x();
		float y = m_object->y();

		if (m_type == Linear)
		{
			x += (float)(m_dx * fractionSecond / m_duration);
			y += (float)(m_dy * fractionSecond / m_duration);
		}
		else if (m_type == Angular)
		{
			m_radius += (float)(m_dr * fractionSecond / m_duration);
			m_angle += (float)(m_da * fractionSecond / m_duration);
			x = m_cx + m_radius * sin(m_angle);
			y = m_cy - m_radius * cos(m_angle);
		}

		m_object->x(x);
		m_object->y(y);
	}

	return Command::step(fractionSecond);
}