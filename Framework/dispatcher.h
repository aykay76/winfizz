#pragma once

class Dispatcher
{
public:
	Dispatcher(void);
	virtual ~Dispatcher(void);

public:
	virtual void invoke(const wchar_t* name) = 0;
};
