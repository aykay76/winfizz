#include ".\FileReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// the buffer reader take a pointer to a null terminated string
FileReader::FileReader(const char* filename)
{
	m_strm.open(filename, std::ios::in);
}

FileReader::~FileReader(void)
{
	m_strm.close();
}

wchar_t FileReader::getChar()
{
	return (wchar_t)m_strm.get();
}

