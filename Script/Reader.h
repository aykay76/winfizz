#pragma once

class Reader
{
public:
	Reader(void);
	~Reader(void);

public:
	virtual wchar_t getChar() = 0;
};
