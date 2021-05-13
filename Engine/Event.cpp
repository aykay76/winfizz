#include ".\event.h"

#include "observer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Event::Event(void)
{
}

Event::~Event(void)
{
	std::list<Observer *>::iterator it = m_observers.begin();
	std::list<Observer*>::iterator end = m_observers.end();
	while (it != end)
	{
		delete *it;
		++it;
	}
}

void Event::attach(Observer* observer)
{
	m_observers.push_back(observer);
}

void Event::detach(Observer* observer)
{
	std::list<Observer*>::iterator it = m_observers.begin();
	std::list<Observer*>::iterator end = m_observers.end();
	while (it != end)
	{
		if (*it == observer) 
		{
			it = m_observers.erase(it);
			break;
		}

		++it;
	}
}

void Event::notify()
{
	// update all observers
	std::list<Observer*>::iterator it = m_observers.begin();
	std::list<Observer*>::iterator end = m_observers.end();
	while (it != end)
	{
		(*it)->update();

		++it;
	}
}
