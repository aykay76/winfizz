#include "../stdafx.h"
#include ".\icode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Icode::Icode(void)
{
	// start off with a 4k code segment
	m_code = new unsigned char[4096];
	m_maxcode = 4096;
	m_cursor = m_code;
	m_size = 0;
}

Icode::~Icode(void)
{
	delete [] m_code;
}

void Icode::put(Parser::TokenType tokenType)
{
	checkBounds(sizeof(tokenType));

	memcpy(m_cursor, (unsigned char *)&tokenType, sizeof(tokenType));
	m_cursor += sizeof(tokenType);

	m_size += sizeof(tokenType);
}

void Icode::put(const SymbolTableNode* node)
{
	// check bounds and re-allocate if necessary
	checkBounds(4);

	// we write out the node's table index and node index
	// this is enough information for us to find it again later
	short s = node->table();
	memcpy(m_cursor, (unsigned char *)&s, sizeof(short));
	m_cursor += sizeof(short);

	s = node->node();
	memcpy(m_cursor, (unsigned char *)&s, sizeof(short));
	m_cursor += sizeof(short);

	m_size += sizeof(short) + sizeof(short);
}

int Icode::putLocationMarker()
{
	checkBounds(4);

	int temp = m_cursor - m_code;
	int pos = m_size;
	memcpy(m_cursor, &temp, sizeof(int));

	m_cursor += sizeof(int);
	m_size += sizeof(int);

	return pos;
}

void Icode::fixupLocationMarker(int pos)
{
	int m = m_cursor - m_code;

	memcpy(m_code + pos, &m, sizeof(int));
}

void Icode::checkBounds(int size)
{
	if (m_cursor + size > m_code + m_maxcode)
	{
		// get the actual amount of code in the current buffer
		int cursize = (int)(m_cursor - m_code);

		// increase the max buffer size, reallocate and copy
		m_maxcode *= 2;
		unsigned char* newcode = new unsigned char[m_maxcode];
		memcpy(newcode, m_code, cursize);

		// delete the old code and adjust the pointers
		delete [] m_code;
		m_code = newcode;
		m_cursor = m_code + cursize;
	}
}

Parser::TokenType Icode::getTokenType()
{
	Parser::TokenType tt;

	if (m_cursor < m_code + m_size)
	{
		memcpy((unsigned char *)&tt, m_cursor, sizeof(tt));
		m_cursor += sizeof(tt);
	}
	else
	{
		tt = Parser::End;
	}

	return tt;
}

SymbolTableNode* Icode::getNode()
{
	short tableId = -1;
	memcpy(&tableId, m_cursor, sizeof(short));
	m_cursor += sizeof(short);
	SymbolTable* table = SymbolTable::m_tables[tableId];

	short nodeId = -1;
	memcpy(&nodeId, m_cursor, sizeof(short));
	m_cursor += sizeof(short);
	return table->m_nodelist[nodeId];
}

int Icode::getLocationMarker()
{
	int pos = -1;
	memcpy(&pos, m_cursor, sizeof(int));
	m_cursor += sizeof(int);

	return pos;
}

void Icode::seek(int pos)
{
	m_cursor = m_code + pos;
}