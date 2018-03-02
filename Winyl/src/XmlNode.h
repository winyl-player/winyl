/*  This file is part of Winyl Player source code.
    Copyright (C) 2008-2018, Alex Kras. <winylplayer@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "pugixml/pugixml.hpp"
#include <string>
#include "UTF.h"

class XmlNode
{
public:
	XmlNode() {}
	~XmlNode() {}
	friend class XmlFile;

public:
	inline explicit operator bool() {return (node ? true : false);}
//	inline bool IsEmpty()
//	{
//		return node.empty();
//	}

	inline XmlNode FirstChild()
	{
		XmlNode xmlNode;
		xmlNode.node = node.first_child();

		return xmlNode;
	}

	inline XmlNode NextChild()
	{
		XmlNode xmlNode;
		xmlNode.node = node.next_sibling();

		return xmlNode;
	}

	inline XmlNode FirstChild(const char* child)
	{
		XmlNode xmlNode;
		xmlNode.node = node.child(child);

		return xmlNode;
	}

	inline XmlNode NextChild(const char* child)
	{
		XmlNode xmlNode;
		xmlNode.node = node.next_sibling(child);

		return xmlNode;
	}

	inline XmlNode AddChild(const char* child)
	{
		XmlNode xmlNode;
		xmlNode.node = node.append_child(child);

		return xmlNode;
	}

	inline void AddAttributeRaw(const char* attr, const char* value)
	{
		node.append_attribute(attr).set_value(value);
	}

	inline void AddAttribute(const char* attr, int value)
	{
		node.append_attribute(attr).set_value(value);
	}

	inline void AddAttributeLong(const char* attr, long long value)
	{
		node.append_attribute(attr).set_value(value);
	}

	inline const char* AttributeRaw(const char* attr)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
			return (char*)xmlAttr.value();
		return nullptr;
	}

	inline bool Attribute(const char* attr, int* value)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
		{
			*value = xmlAttr.as_int();
			return true;
		}
		return false;
	}

	inline bool Attribute(const char* attr, bool* value)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
		{
			if (xmlAttr.as_int())
				*value = true;
			else
				*value = false;
			return true;
		}
		return false;
	}

	inline bool AttributeLong(const char* attr, long long* value)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
		{
			*value = xmlAttr.as_llong();
			return true;
		}
		return false;
	}

	inline const char* Name()
	{
		return node.name();
	}

	inline const char* Value()
	{
		return node.child_value();
	}

	inline bool SetName(const char* name)
	{
		return node.set_name(name);
	}

	inline bool SetValue(const char* value)
	{
		return node.set_value(value);
	}

	inline bool SetAttributeRaw(const char* attr, const char* value)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
			return xmlAttr.set_value(value);
		return false;
	}

	inline bool SetAttribute(const char* attr, int value)
	{
		pugi::xml_attribute xmlAttr = node.attribute(attr);
		if (xmlAttr)
			return xmlAttr.set_value(value);
		return false;
	}

	inline void AddAttribute8(const char* attr, const std::string& value)
	{
		AddAttributeRaw(attr, value.c_str());
	}

	inline void AddAttribute16(const char* attr, const std::wstring& value)
	{
		AddAttributeRaw(attr, UTF::UTF8S(value).c_str());
	}

	inline std::string Attribute8(const char* attr)
	{
		const char* value = AttributeRaw(attr);

		return value ? value : std::string();
	}

	inline std::wstring Attribute16(const char* attr)
	{
		const char* value = AttributeRaw(attr);

		return value ? UTF::UTF16(value) : std::wstring();
	}

	inline bool Attribute8(const char* attr, std::string* value)
	{
		const char* valueraw = AttributeRaw(attr);
		if (valueraw)
		{
			*value = valueraw;
			return true;
		}

		return false;
	}

	inline bool Attribute16(const char* attr, std::wstring* value)
	{
		const char* valueraw = AttributeRaw(attr);
		if (valueraw)
		{
			*value = UTF::UTF16(valueraw);
			return true;
		}

		return false;
	}

	inline std::string Value8()
	{
		const char* value = node.child_value();

		return value ? value : std::string();
	}

	inline bool SetAttribute8(const char* attr, const std::string& value)
	{
		return SetAttributeRaw(attr, value.c_str());
	}

	inline bool SetAttribute16(const char* attr, const std::wstring& value)
	{
		return SetAttributeRaw(attr, UTF::UTF8S(value).c_str());
	}

	////////////////

	inline XmlNode FirstChildNoCase(const char* child)
	{
		XmlNode xmlNode;
		for (pugi::xml_node n = node.first_child(); n; n = n.next_sibling())
		{
			if (StringEx::IsEqualAsciiRaw(n.name(), child))
			{
				xmlNode.node = n;
				break;
			}
		}

		return xmlNode;
	}

	inline XmlNode NextChildNoCase(const char* child)
	{
		XmlNode xmlNode;
		for (pugi::xml_node n = node.next_sibling(); n; n = n.next_sibling())
		{
			if (StringEx::IsEqualAsciiRaw(n.name(), child))
			{
				xmlNode.node = n;
				break;
			}
		}

		return xmlNode;
	}

	inline const char* AttributeNoCase(const char* attr)
	{
		for (pugi::xml_attribute a = node.first_attribute(); a; a = a.next_attribute())
		{
			if (StringEx::IsEqualAsciiRaw(a.name(), attr))
				return a.value();
		}

		return nullptr;
	}

private:
	pugi::xml_node node;
};
