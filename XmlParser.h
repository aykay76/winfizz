#pragma once

#include "msxml2.h"

/******************************************************************************

this should be a base class for XmlParsers of different file formats but we're 
only going to have xml so no point really

******************************************************************************/

#include <vector>

struct IXMLDOMNode;
class Engine;

class XmlParser
{
public:
	XmlParser(void);
	~XmlParser(void);

public:
	// parse the specified file into the engine provided
	bool parseFile(const wchar_t* filename);
	bool parseBuffer(const wchar_t* buffer);
protected:
	bool parse(IXMLDOMDocument2* pXMLDoc);

protected:
	void loadConfiguration(IXMLDOMNode* node);
	void loadResources(IXMLDOMNode* node);
	void loadObjects(IXMLDOMNode* node);
	void loadKeyframe(IXMLDOMNode* node);

	std::wstring getAttributeList(IXMLDOMNode* node, std::vector<std::pair<std::wstring, std::wstring> >& attrs);
};
