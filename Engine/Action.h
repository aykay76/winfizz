#pragma once

#include <vector>
#include <memory>

class Entity;

class Action
{
public:
	Action(Entity* object, double duration);
	virtual ~Action(void);

public:
	virtual bool init();

	// returns false when life is over
	virtual bool step(double fractionSecond);

public:
	double duration() { return m_duration; }

protected:
	double m_lifespan;
	double m_duration;
	Entity* m_object;
};
