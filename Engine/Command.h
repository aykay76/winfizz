#pragma once

#include <vector>
#include <memory>

class Entity;

class Command
{
public:
	Command();
	virtual ~Command(void);

public:
	// a factory style interface
	static Command* create(std::wstring& strType, std::vector<std::pair<std::wstring, std::wstring> >& attrs);

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
