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
#include "XmlNode.h"
#include "ZipFile.h"

class XmlFile
{
public:
	XmlFile() {}
	~XmlFile() {}

	inline bool LoadFile(const std::wstring& file)
	{
		if (doc.load_file(file.c_str(), pugi::parse_minimal|pugi::parse_escapes, pugi::encoding_utf8).status == pugi::status_ok)
			return true;

		return false;
	}

	inline bool LoadBuffer(const void* buffer, size_t size)
	{
		if (doc.load_buffer(buffer, size, pugi::parse_minimal|pugi::parse_escapes, pugi::encoding_utf8).status == pugi::status_ok)
			return true;

		return false;
	}
	
	inline bool LoadEx(const std::wstring& file, ZipFile* zipFile)
	{
		if (zipFile == nullptr)
		{
			if (doc.load_file(file.c_str(), pugi::parse_minimal|pugi::parse_escapes, pugi::encoding_utf8).status == pugi::status_ok)
				return true;
		}
		else
		{
			if (zipFile->UnzipToBuffer(file))
			{
				if (doc.load_buffer(zipFile->GetBuffer(), zipFile->GetBufferSize(),
					pugi::parse_minimal|pugi::parse_escapes, pugi::encoding_utf8).status == pugi::status_ok)
				{
					zipFile->FreeBuffer();
					return true;
				}
			}
		}

		return false;
	}

	inline bool SaveFile(const std::wstring& file)
	{
		// add a custom declaration node
		pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "UTF-8";

		if (doc.save_file(file.c_str(), "    ",
			pugi::format_default|pugi::format_save_file_text|pugi::format_no_declaration, pugi::encoding_utf8))
			return true;

		return false;
	}

	inline XmlNode RootNode()
	{
		XmlNode xmlNode;
		xmlNode.node = doc;

		return xmlNode;
	}

private:
	pugi::xml_document doc;
};
