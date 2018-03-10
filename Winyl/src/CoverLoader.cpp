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

#include "stdafx.h"
#include "CoverLoader.h"
#include "TagLibCover.h"
#include "FileSystem.h"

CoverLoader::CoverLoader()
{

}

CoverLoader::~CoverLoader()
{

}

bool CoverLoader::LoadCoverImage(const std::wstring& file)
{
	coverImage.Free();

	std::wstring path = PathEx::PathFromFile(file);

	std::wstring fileCover;

	if (FindImageByName(path, L"*front*.*", fileCover) ||
		FindImageByName(path, L"*cover*.*", fileCover))
	{
		coverImage.LoadFile(fileCover);
	}
	else if (LoadCoverFromTrack(file))
	{
		return true;
	}
	else if (FindImageByName(path, L"folder.*", fileCover))
	{
		coverImage.LoadFile(fileCover);
	}
	else if (FindImageByExt(path, fileCover))
	{
		coverImage.LoadFile(fileCover);
	}

	if (coverImage.IsValid())
		return true;

	return false;
}

bool CoverLoader::LoadCoverImageTagEditor(const std::wstring& file)
{
	coverImage.Free();

	std::wstring path = PathEx::PathFromFile(file);

	std::wstring fileCover;

	if (LoadCoverFromTrack(file))
	{
		return true;
	}
	else if (FindImageByName(path, L"*front*.*", fileCover) ||
		FindImageByName(path, L"*cover*.*", fileCover))
	{
		coverImage.LoadFile(fileCover);
	}
	else if (FindImageByName(path, L"folder.*", fileCover))
	{
		coverImage.LoadFile(fileCover);
	}
	else if (FindImageByExt(path, fileCover))
	{
		coverImage.LoadFile(fileCover);
	}

	if (coverImage.IsValid())
		return true;

	return false;
}

bool CoverLoader::FindImageByName(const std::wstring& path, const std::wstring& match, std::wstring& file)
{
	FileSystem::Find find(path, match);

	while (find.Next())
	{
		std::wstring ext = PathEx::ExtFromFile(find.GetFileName());

		if (ext.empty())
			continue;

		if (ext == L"jpeg" || ext == L"jpg" || ext == L"png" || ext == L"gif")
		{
			std::wstring path2 = PathEx::PathFromFile(path);

			file = path2 + find.GetFileName();

			return true;
		}
	}

	return false;
}

bool CoverLoader::FindImageByExt(const std::wstring& path, std::wstring& file)
{
	FileSystem::Find find1(path, L"*.jp*g"); // *.jpeg & *.jpg
	if (find1.Next())
	{
		file = path + find1.GetFileName();
		return true;
	}

	FileSystem::Find find2(path, L"*.png");
	if (find2.Next())
	{
		file = path + find2.GetFileName();
		return true;
	}

//	FileSystem::Find find3(path, L"*.gif");
//	if (find3.Step())
//	{
//		file = path + find3.GetFileName();
//		return true;
//	}

	return false;
}

bool CoverLoader::LoadCoverFromTrack(const std::wstring& file)
{
	TagLibCover taglib;
	taglib.outImage = &coverImage;
	taglib.ReadCoverFromFile(file);

	if (coverImage.IsValid())
		return true;

	return false;
}
