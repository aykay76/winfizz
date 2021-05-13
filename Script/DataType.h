#pragma once

#include <string>

enum TypeCode
{
	tcUndefined,
	tcBoolean,
	tcInteger,
	tcReal,
	tcString,
	tcTexture,
	tcMaterial
};

// TODO: Get rid of this - we are no longer having these horrid unions
// we are instead using our new base-derived objects
class DataType
{
public:
	DataType(TypeCode tc);
	~DataType(void);

public:
	TypeCode type() { return m_tc; }

	union
	{
		double real;
		int integer;
		bool boolean;
	} value;

	std::wstring string;

protected:
	TypeCode m_tc;
};
