#include ".\bufferreader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// the buffer reader take a pointer to a null terminated string
BufferReader::BufferReader(wchar_t* buffer)
	: m_buffer(buffer)
	, m_bp(buffer)
{
}

BufferReader::~BufferReader(void)
{
}

wchar_t BufferReader::getChar()
{
	return *m_bp++;
}

