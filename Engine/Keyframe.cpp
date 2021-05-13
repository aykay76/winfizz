#include ".\keyframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Keyframe::Keyframe(void)
	: m_position(0.0)
{
}

Keyframe::Keyframe(double position)
	: m_position(position)
{
}

Keyframe::~Keyframe(void)
{
	std::list<Command *>::iterator it = m_commands.begin();
	for (; it != m_commands.end(); ++it)
	{
		delete *it;
	}
	m_commands.clear();
}
