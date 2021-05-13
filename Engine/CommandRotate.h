#pragma once

#include "command.h"

#include <vector>
#include <memory>

class Engine;

class CommandRotate : public Command
{
public:
	CommandRotate(std::vector<std::pair<std::wstring, std::wstring> >& attrs);
	virtual ~CommandRotate(void);

public:
	virtual bool init();
	virtual bool step(double fractionSecond);

protected:
	// amount to rotate by in each of the axes - measured in degrees (just like GL)
	float m_dax;
	float m_day;
	float m_daz;
	float m_cx;
	float m_cy;
	float m_cz;
	std::wstring m_name;

	bool m_first;
};
