#pragma once
#include "reader.h"

class BufferReader :
	public Reader
{
public:
	BufferReader(wchar_t* buffer);
	virtual ~BufferReader(void);

public:
	virtual wchar_t getChar();

protected:
	wchar_t* m_buffer;
	wchar_t* m_bp;
};
