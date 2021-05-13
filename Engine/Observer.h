#pragma once

class Observer
{
public:
	Observer(void);
	virtual ~Observer(void);

public:
	virtual void update();
};
