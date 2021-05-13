#pragma once

#include <list>

class Observer;

class Event
{
public:
	Event(void);
	~Event(void);

public:
	void attach(Observer* observer);
	void detach(Observer* observer);
	void notify();

protected:
	std::wstring m_object;
	std::wstring m_name;
	std::list<Observer *> m_observers;
};

