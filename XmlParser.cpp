#include "StdAfx.h"
#include ".\XmlParser.h"
#include "engine.h"
#include "object.h"
#include "command.h"
#include "resourcebase.h"

#include <vector>
#include <memory>

#include <comdef.h>

#include "msxml2.h"
#pragma comment(lib, "msxml2.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

XmlParser::XmlParser(void)
{
}

XmlParser::~XmlParser(void)
{
}

bool XmlParser::parseFile(const wchar_t* filename)
{
	HRESULT hr;
	IXMLDOMDocument2* pXMLDoc = NULL;

	hr = CoInitialize(NULL); 
	if (FAILED(hr))
	{
		return false;
	}

	hr = CoCreateInstance(CLSID_DOMDocument40, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument2, (void**)&pXMLDoc);
	if (FAILED(hr))
	{
		return false;
	}

	CString str(filename);
	BSTR bstr = str.AllocSysString();
	VARIANT_BOOL vtb = TRUE;
	VARIANT xmlSource;
	xmlSource.vt = VT_BSTR;
	xmlSource.bstrVal = _bstr_t(filename);
	hr = pXMLDoc->load(xmlSource, &vtb);

	return parse(pXMLDoc);
}

bool XmlParser::parseBuffer(const wchar_t* buffer)
{
	HRESULT hr;
	IXMLDOMDocument2 * pXMLDoc;

	hr = CoInitialize(NULL); 
	if (FAILED(hr))
	{
		return false;
	}

	hr = CoCreateInstance(CLSID_DOMDocument40, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument2, (void**)&pXMLDoc);
	if (FAILED(hr))
	{
		return false;
	}

	VARIANT_BOOL isSuccessful;
	hr = pXMLDoc->loadXML((BSTR)buffer, &isSuccessful);

	return parse(pXMLDoc);
}

bool XmlParser::parse(IXMLDOMDocument2* pXMLDoc)
{
	HRESULT hr;
	IXMLDOMNode* pXDN;

	// get the node interface to the document
	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pXDN);
	if (FAILED(hr))
	{
		return false;
	}

	BSTR name;
	pXDN->get_nodeName(&name);

	IXMLDOMNode* child;
	hr = pXDN->get_firstChild(&child);
	if (FAILED(hr) || child == NULL)
	{
		return false;
	}

	// the first child of the document is the root node of the document "<xml>" or whatever
	child->get_nodeName(&name);

	// now get the first child of the child
	hr = child->get_firstChild(&child);
	do
	{
		child->get_nodeName(&name);
		std::wstring str(name);

		if (str.compare(L"configuration") == 0)
		{
			loadConfiguration(child);
		}
		else if (str.compare(L"resources") == 0)
		{
			loadResources(child);
		}
		else if (str.compare(L"objects") == 0)
		{
			loadObjects(child);
		}
		else if (str.compare(L"keyframe") == 0)
		{
			loadKeyframe(child);
		}

		child->get_nextSibling(&child);
	} while (hr == S_OK && child);

	pXDN->Release();
	pXMLDoc->Release();

	return true;
}

void XmlParser::loadConfiguration(IXMLDOMNode* node)
{
	IXMLDOMNode* child = NULL;
	
	HRESULT hr = node->get_firstChild(&child);
	while (hr == S_OK && child != NULL)
	{
		IXMLDOMElement* element;

		hr = child->QueryInterface(IID_IXMLDOMElement, (void **)&element);
		if (FAILED(hr))
		{
			// badly formed XML
		}

		// check the name
		BSTR nodeName;
		VARIANT nodeValue;

		// get the value node
		IXMLDOMNode* valueNode;
		child->get_firstChild(&valueNode);
		child->get_nodeName(&nodeName);
		valueNode->get_nodeValue(&nodeValue);
		valueNode->Release();

		std::wstring key(nodeName);
		std::wstring value(nodeValue.bstrVal);
		Engine::instance()->configuration()[key] = value;

		element->Release();

		child->get_nextSibling(&child);
	}
}

// load resources into the engine for use in drawing objects - resources include
// pens, brushes, images, sounds etc. things that other objects rely on.
// this allows us to cache resources instead of creating them multiple times
// 
void XmlParser::loadResources(IXMLDOMNode* node)
{
	IXMLDOMNode* child = NULL;
	std::vector<std::pair<std::wstring, std::wstring> > attrs;
	HRESULT hr = node->get_firstChild(&child);
	while (hr == S_OK && child != NULL)
	{
		// one node per object
		BSTR typeName;
		child->get_nodeName(&typeName);
		std::wstring type(typeName);
		std::wstring brushType = getAttributeList(child, attrs);

		ResourceBase* resource = ResourceBase::create(type, attrs);
		if (resource)
			Engine::instance()->resources()[resource->name()] = resource;

		child->get_nextSibling(&child);
	}
}

void XmlParser::loadObjects(IXMLDOMNode* node)
{
	IXMLDOMNode* child = NULL;
	HRESULT hr = node->get_firstChild(&child);
	while (hr == S_OK && child != NULL)
	{
		IXMLDOMNamedNodeMap* attributes = NULL;
		hr = child->get_attributes(&attributes);

		std::wstring type;
		long count;
		hr = attributes->get_length(&count);
		std::vector<std::pair<std::wstring, std::wstring> > attrs;

		for (long l = 0; l < count; l++)
		{
			IXMLDOMNode* attributeNode = NULL;
			hr = attributes->get_item(l, &attributeNode);

			BSTR name;
			VARIANT value;
			attributeNode->get_nodeName(&name);
			attributeNode->get_nodeValue(&value);

			// get the type separately because we need to know which type
			// of command to create
			if (std::wstring(name).compare(L"type") == 0)
			{
				type = value.bstrVal;
			}

			attrs.push_back(std::pair<std::wstring, std::wstring>(std::wstring(name), std::wstring(value.bstrVal)));
		}

		// got all the attribute, create the command - keyframe takes ownership
//		Object* object = Object::create(type, attrs);
//		if (object) 
//			Engine::instance()->objects()[object->name()] = object;

		child->get_nextSibling(&child);
	}
}

void XmlParser::loadKeyframe(IXMLDOMNode* node)
{
	double position = 0.0;

	IXMLDOMNamedNodeMap* attributes = NULL;
	HRESULT hr = node->get_attributes(&attributes);
	long count;
	hr = attributes->get_length(&count);
	for (long l = 0; l < count; l++)
	{
		IXMLDOMNode* attribute = NULL;
		hr = attributes->get_item(l, &attribute);

		BSTR name;
		VARIANT value;
		attribute->get_nodeName(&name);
		attribute->get_nodeValue(&value);
		std::wstring attrName(name);

		if (attrName.compare(L"position") == 0)
		{
			wchar_t* p;
			position = wcstod(value.bstrVal, &p);
		}
	}

	Keyframe* keyframe = new Keyframe(position);
	double duration = 0.0;

	IXMLDOMNode* child = NULL;
	hr = node->get_firstChild(&child);
	while (hr == S_OK && child != NULL)
	{
		BSTR name;
		child->get_nodeName(&name);

		// name should always be "command" for now
		IXMLDOMNamedNodeMap* attributes;
		IXMLDOMNode* attribute;
		child->get_attributes(&attributes);
		long count;
		attributes->get_length(&count);

		VARIANT value;

		std::vector<std::pair<std::wstring, std::wstring> > attrs;
		std::wstring type;

		for (long l = 0; l < count; l++)
		{
			attributes->get_item(l, &attribute);
			
			attribute->get_nodeName(&name);
			attribute->get_nodeValue(&value);

			// get the type separately because we need to know which type
			// of command to create
			if (std::wstring(name).compare(L"type") == 0)
			{
				type = value.bstrVal;
			}

			attrs.push_back(std::pair<std::wstring, std::wstring>(std::wstring(name), std::wstring(value.bstrVal)));
		}

		// got all the attribute, create the command - keyframe takes ownership
		Command* command = Command::create(type, attrs);
		if (command) keyframe->commands().push_back(command);

		if (command->duration() > duration) duration = command->duration();

		child->get_nextSibling(&child);
	}

	keyframe->duration(duration);
	Engine::instance()->keyframes().push_back(keyframe);
}

std::wstring XmlParser::getAttributeList(IXMLDOMNode* node, std::vector<std::pair<std::wstring, std::wstring> >& attrs)
{
	IXMLDOMNamedNodeMap* attributes = NULL;
	HRESULT hr = node->get_attributes(&attributes);
	std::wstring type;

	long count;
	hr = attributes->get_length(&count);

	for (long l = 0; l < count; l++)
	{
		IXMLDOMNode* attributeNode = NULL;
		hr = attributes->get_item(l, &attributeNode);

		BSTR name;
		VARIANT value;
		attributeNode->get_nodeName(&name);
		attributeNode->get_nodeValue(&value);

		if (std::wstring(name).compare(L"type") == 0)
		{
			type = value.bstrVal;
		}

		attrs.push_back(std::pair<std::wstring, std::wstring>(std::wstring(name), std::wstring(value.bstrVal)));
	}

	attributes->Release();

	return type;
}