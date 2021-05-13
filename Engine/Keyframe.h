#pragma once

/******************************************************************************
// a keyframe is essentially a collection of actions and tweens.
// the supported actions are ADD, REMOVE, HIDE
// supported tweens are MOVE, ROTATE, COLOUR
******************************************************************************/

#include <list>
#include "command.h"

class Keyframe
{
public:
	Keyframe(void);
	Keyframe(double position);
	virtual ~Keyframe(void);

public:
	double position() { return m_position; }
	double duration() { return m_duration; }
	std::list<Command *>& commands() { return m_commands; }

	void duration(double d) { m_duration = d; }

private:
	std::list<Command *> m_commands;
	double m_position;
	double m_duration;
};
