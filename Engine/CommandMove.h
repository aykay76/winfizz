#pragma once
#include "command.h"

class CommandMove :
	public Command
{
public:
	CommandMove(std::vector<std::pair<std::wstring, std::wstring> >& attrs);
	virtual ~CommandMove(void);

enum MoveType
{
	Linear, Angular
};

public:
	bool init();
	virtual bool step(double fractionSecond);

protected:
	MoveType m_type;

	bool m_first;

	std::wstring m_name; // name of object to act on

	// for linear moves
	float m_dx;			// delta x
	float m_dy;			// delta y

	// for angular moves
	float m_cx;			// centre x
	float m_cy;			// centre y
	float m_da;			// delta angle 
	float m_angle;		// current angle
	float m_radius;		// radius from centre to point
	float m_dr;			// delta radius 
};
