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
#include "SkinCover.h"

SkinCover::SkinCover()
{

}

SkinCover::~SkinCover()
{

}

bool SkinCover::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Cover");

		if (xmlMain)
		{
			XmlNode xmlMirror = xmlMain.FirstChild("Mirror");
			if (xmlMirror)
			{
				std::wstring attr = xmlMirror.Attribute16("File");
				if (!attr.empty())
				{
					if (imMirror.LoadEx(path + attr, zipFile))
						isMirror = true;
				}
			}

			XmlNode xmlCover = xmlMain.FirstChild("Cover");
			if (xmlCover)
			{
				std::wstring attr = xmlCover.Attribute16("File");
				if (!attr.empty())
				{
					ExImage::Source temp;
					temp.LoadEx(path + attr, zipFile);
					
					imCover.LoadFromSource(temp);

					if (isMirror)
					{
						temp.MirrorVertical();
						imCoverMirror.LoadFromSource(temp);
					}
				}
			}

			XmlNode xmlOverlay = xmlMain.FirstChild("Overlay");
			if (xmlOverlay)
			{
				std::wstring attr = xmlOverlay.Attribute16("File");
				if (!attr.empty())
					imOverlay.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlShadow = xmlMain.FirstChild("Shadow");
			if (xmlShadow)
			{
				std::wstring attr = xmlShadow.Attribute16("File");
				if (!attr.empty())
					imShadow.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlEffect = xmlMain.FirstChild("Effect");
			if (xmlEffect)
				xmlEffect.Attribute("Fade", &isFadeEffect);
		}
	}
	else
		return false;

	return true;
}

void SkinCover::Draw(HDC dc, bool isAlpha)
{
	int margin = 0; // Indent due to shadow

	if (imShadow.Width() > imCover.Width())
		margin = (imShadow.Width() - imCover.Width())/2;

	if (isMirror)
	{
		if (imMirrorImage.IsValid())
			imMirrorImage.Crop(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height());
		else
			imCoverMirror.Crop(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height());

		imMirror.Draw(dc, rcRect.left + margin, rcRect.top + imCover.Width());
	}

	imShadow.Draw(dc, rcRect.left, rcRect.top + imCover.Height() - imShadow.Height()/2);

	if (imCoverImage.IsValid())
	{
		imCoverImage.Draw(dc, rcRect.left + margin, rcRect.top);
		imOverlay.Draw(dc, rcRect.left + margin, rcRect.top);
	}
	else
		imCover.Draw(dc, rcRect.left + margin, rcRect.top);
}

bool SkinCover::DrawFade(HDC dc, bool isAlpha)
{
	fadeAlpha += FadeValue::Step;
	fadeAlpha = std::min(255, fadeAlpha);

	int margin = 0; // Indent due to shadow

	if (imShadow.Width() > imCover.Width())
		margin = (imShadow.Width() - imCover.Width())/2;

	if (fadeAlpha == 255)
	{
		if (isMirror)
		{
			if (imMirrorImage.IsValid())
				imMirrorImage.Crop(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height());
			else
				imCoverMirror.Crop(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height());

			imMirror.Draw(dc, rcRect.left + margin, rcRect.top + imCover.Width());
		}

		imShadow.Draw(dc, rcRect.left, rcRect.top + imCover.Height() - imShadow.Height()/2);

		if (imCoverImage.IsValid())
		{
			imCoverImage.Draw(dc, rcRect.left + margin, rcRect.top);
			imOverlay.Draw(dc, rcRect.left + margin, rcRect.top);
		}
		else
			imCover.Draw(dc, rcRect.left + margin, rcRect.top);
	}
	else
	{
		if (isMirror)
		{
			if (imBackupMirror.IsValid())
				imBackupMirror.CropFade(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height(), 255 - fadeAlpha);
			else
				imCoverMirror.CropFade(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height(), 255 - fadeAlpha);

			if (imMirrorImage.IsValid())
				imMirrorImage.CropFade(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height(), fadeAlpha);
			else
				imCoverMirror.CropFade(dc, rcRect.left + margin, rcRect.top + imCover.Width(), imMirror.Width(), imMirror.Height(), fadeAlpha);

			imMirror.Draw(dc, rcRect.left + margin, rcRect.top + imCover.Width());
		}

		imShadow.Draw(dc, rcRect.left, rcRect.top + imCover.Height() - imShadow.Height()/2);

		if (imBackupImage.IsValid())
		{
			imBackupImage.DrawFade(dc, rcRect.left + margin, rcRect.top, 255 - fadeAlpha);
			imOverlay.DrawFade(dc, rcRect.left + margin, rcRect.top, 255 - fadeAlpha);
		}
		else
			imCover.DrawFade(dc, rcRect.left + margin, rcRect.top, 255 - fadeAlpha);

		if (imCoverImage.IsValid())
		{
			imCoverImage.DrawFade(dc, rcRect.left + margin, rcRect.top, fadeAlpha);
			imOverlay.DrawFade(dc, rcRect.left + margin, rcRect.top, fadeAlpha);
		}
		else
			imCover.DrawFade(dc, rcRect.left + margin, rcRect.top, fadeAlpha);
	}

	if (fadeAlpha == 255)
		return true;
	else
		return false;
}

int SkinCover::GetWidth()
{
	if (imShadow.Width() > imCover.Width())
		return imShadow.Width();
	else
		return imCover.Width();
}

int SkinCover::GetHeight()
{
	if (isMirror)
		return imCover.Height() + imMirror.Height();
	else
		return imCover.Height() + imShadow.Height()/2;
}

void SkinCover::SetImage(ExImage::Source* image)
{
	fadeAlpha = 0;

	if (isFadeEnabled)
	{
		imCoverImage.MoveTo(&imBackupImage);
		imMirrorImage.MoveTo(&imBackupMirror);
	}
	else
	{
		if (imBackupImage.IsValid())
			imBackupImage.Clear();
		if (isMirror && imBackupMirror.IsValid())
			imBackupMirror.Clear();
	}

	if (image == nullptr)
	{
		imCoverImage.Clear();

		if (isMirror)
			imMirrorImage.Clear();

		return;
	}


	ExImage::Source thumb;
	image->MakeThumbnail(thumb, imCover.Width(), imCover.Height());

	if (thumb.IsValid())
	{
		imCoverImage.LoadFromSource(thumb);
		
		if (isMirror)
		{
			thumb.MirrorVertical();

			imMirrorImage.LoadFromSource(thumb);
		}
	}
}

bool SkinCover::IsImage()
{
	return imCoverImage.IsValid();
}

void SkinCover::EnableFade(bool isEnable)
{
	isFadeEnabled = isEnable;
}

