#pragma once
#include "reader.h"

#include <fstream>

class FileReader :
	public Reader
{
public:
	FileReader(const char* filename);
	virtual ~FileReader(void);

public:
	virtual wchar_t getChar();

protected:
	std::wifstream m_strm;
};
