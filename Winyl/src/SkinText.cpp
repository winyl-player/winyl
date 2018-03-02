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

// SkinText.cpp : implementation file
//

#include "stdafx.h"
#include "SkinText.h"

//#define RGB(r, g, b)(((DWORD)((BYTE)(r))) | ((DWORD)((BYTE)(g)) << 8) \
//                | ((DWORD)((BYTE)(b)) << 16))
//#define GetRValue(rgba)((BYTE)(rgba))
//#define GetGValue(rgba)((BYTE)(((DWORD)(rgba)) >> 8))
//#define GetBValue(rgba)((BYTE)((DWORD)(rgba) >> 16))
#define RGBA(r, g, b, a)(((DWORD)((BYTE)(r))) | ((DWORD)((BYTE)(g)) << 8) \
                | ((DWORD)((BYTE)(b)) << 16) | ((DWORD)((BYTE)(a)) << 24))
#define GetAValue(rgba)((BYTE)((DWORD)(rgba) >> 24))

// SkinText

SkinText::SkinText()
{

}

SkinText::~SkinText()
{
	if (font)
		::DeleteObject(font);
}

bool SkinText::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	if (isDoubleText)
		dash = L" - ";

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Text");

		if (xmlMain)
		{
			XmlNode xmlFont = xmlMain.FirstChild("Font");
			if (xmlFont)
			{
				const char* fname = nullptr; int fsize = 0; int fbold = 0; int fitalic = 0; int fclear = 0;

				fname = xmlFont.AttributeRaw("Name");
				xmlFont.Attribute("Size", &fsize);
				xmlFont.Attribute("Bold", &fbold);
				xmlFont.Attribute("Italic", &fitalic);
				xmlFont.Attribute("ClearType", &fclear);

				xmlFont.Attribute("Align", &align);

				xmlFont.Attribute("UpperCase", &isUpperCase);

				font = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);

				const char* strColor = xmlFont.AttributeRaw("Color");

				colorText = 0x00000000; // Default (black)
				colorTextAlpha = 0xFF; // Default (opaque)

				if (strColor)
				{
					DWORD c = strtoul(strColor, 0, 16);
					
					if (strlen(strColor) == 6) // Color in RGB format (convert to ABGR)
					{
						colorText = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
						colorTextAlpha = 0xFF;
					}
					else if (strlen(strColor) == 8) // Color in RGBA format (convert to ABGR)
					{
						colorText = RGB(GetAValue(c), GetBValue(c), GetGValue(c));
						colorTextAlpha = GetRValue(c);
					}
				}

				if (isDoubleText)
				{
					const char* strColor = xmlFont.AttributeRaw("Color2");

					colorText2 = 0x00000000; // Default (black)
					colorTextAlpha2 = 0xFF; // Default (opaque)

					if (strColor)
					{
						DWORD c = strtoul(strColor, 0, 16);

						if (strlen(strColor) == 6) // Color in RGB format (convert to ABGR)
						{
							colorText2 = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
							colorTextAlpha2 = 0xFF;
						}
						else if (strlen(strColor) == 8) // Color in RGBA format (convert to ABGR)
						{
							colorText2 = RGB(GetAValue(c), GetBValue(c), GetGValue(c));
							colorTextAlpha2 = GetRValue(c);
						}
					}
					else
					{
						colorText2 = colorText;
						colorTextAlpha2 = colorTextAlpha;
					}
				}
			}

			XmlNode xmlEffect = xmlMain.FirstChild("Effect");
			if (xmlEffect)
			{
				if (!xmlEffect.Attribute("FixOpacity", &effect.isFixOpacity))
					xmlEffect.Attribute("FixOpacityXP", &effect.isFixOpacity); // Old name

				xmlEffect.Attribute("GlowSize", &effect.glowSize);

				const char* strColor = xmlEffect.AttributeRaw("ShadowColor");
				if (strColor)
				{
					effect.isShadow = true;

					effect.colorShadow = 0x00000000; // Default (black)

					DWORD c = strtoul(strColor, 0, 16);

					if (strlen(strColor) == 6) // Color in RGB format (convert to ABGR)
					{
						effect.colorShadow = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
						effect.colorShadowAlpha = 0xFF;
					}
					else if (strlen(strColor) == 8) // Color in RGBA format (convert to ABGR)
					{
						effect.colorShadow = RGB(GetAValue(c), GetBValue(c), GetGValue(c));
						effect.colorShadowAlpha = GetRValue(c);
					}

					xmlEffect.Attribute("ShadowX", &effect.shadowX);
					xmlEffect.Attribute("ShadowY", &effect.shadowY);
				}
			}
		}
	}
	else
		return false;

	return true;
}

void SkinText::Draw(HDC dc, bool isAlpha)
{
	if (thisText.empty())
		return;

	if (isDoubleText)
	{
		DrawDoubleText(dc, isAlpha);
		return;
	}

	// DT_NOCLIP Draws without clipping. DrawTextEx is somewhat faster when DT_NOCLIP is used. 

	DWORD format = DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
	if (effect.glowSize)
		format |= DT_VCENTER;
	else if (!effect.isShadow)
		format |= DT_BOTTOM;

	if (align == 0)
		DrawTextSimple(dc, isAlpha, thisText, rcRect, colorText, colorTextAlpha, DT_LEFT|format);
	else if (align == 1)
		DrawTextSimple(dc, isAlpha, thisText, rcRect, colorText, colorTextAlpha, DT_RIGHT|format);
	else // if (align == 2)
		DrawTextSimple(dc, isAlpha, thisText, rcRect, colorText, colorTextAlpha, DT_CENTER|format);
}

void SkinText::DrawDoubleText(HDC dc, bool isAlpha)
{
	HFONT oldFond = (HFONT)::SelectObject(dc, font);

	CSize szDash, szText1, szText2;
	::GetTextExtentPoint32(dc, dash.c_str(), (int)dash.size(), &szDash);
	::GetTextExtentPoint32(dc, thisText.c_str(), (int)thisText.size(), &szText1);
	::GetTextExtentPoint32(dc, thisText2.c_str(), (int)thisText2.size(), &szText2);

	::SelectObject(dc, oldFond);

	CRect rc = rcRect;

	if (align > 0)
	{
		int textWidth = szText1.cx + szDash.cx + szText2.cx;
		if (textWidth < rcRect.Width())
		{
			if (align == 1)
				rc.left += rcRect.Width() - textWidth;
			else
			{
				int cut = (rcRect.Width() - textWidth) / 2;
				rc.DeflateRect(cut, 0, cut, 0);
			}
		}
	}

	CRect rcText1 = CRect(rc.left, rc.top, rc.left + std::min((int)szText1.cx, rc.Width()), rc.bottom);
	CRect rcDash = CRect(rcText1.right, rc.top, rcText1.right + szDash.cx, rc.bottom);
	CRect rcText2 = CRect(rcDash.right, rc.top, rc.right, rc.bottom);

	DWORD format = DT_SINGLELINE|DT_NOPREFIX;
	if (effect.glowSize)
		format |= DT_VCENTER;
	else if (!effect.isShadow)
		format |= DT_BOTTOM;

	DrawTextSimple(dc, isAlpha, thisText, rcText1, colorText, colorTextAlpha, DT_LEFT|DT_END_ELLIPSIS|format);

	if (rcDash.right + szDash.cx < rc.right) // If there is a place to draw the second text
	{
		DrawTextSimple(dc, isAlpha, dash, rcDash, colorText, colorTextAlpha, DT_LEFT|format);
		DrawTextSimple(dc, isAlpha, thisText2, rcText2, colorText2, colorTextAlpha2, DT_LEFT|DT_END_ELLIPSIS|format);
	}
}

int SkinText::GetWidth()
{
	return 100; // Default width
}

int SkinText::GetHeight()
{
	return 20; // Default height
}

void SkinText::SetText(const std::wstring& text)
{
	if (isUpperCase)
		thisText = StringEx::ToUpper(text);
	else
		thisText = text;
}

void SkinText::SetText2(const std::wstring& text)
{
	if (isUpperCase)
		thisText2 = StringEx::ToUpper(text);
	else
		thisText2 = text;
}

void SkinText::SetTextEmpty()
{
	thisText.clear();
	if (isDoubleText)
		thisText.clear();
}

void SkinText::SetTime(int time, bool isRemains, int length, bool isLength)
{
	thisText = TimeString(time, isRemains, length, isLength);
}

std::wstring SkinText::TimeString(int time, bool isRemains, int length, bool isLength)
{
	// Test
	//time += 36000;
	//length += 36000;

	if (time < 0) time = 0;

	if (!isLength)
	{
		if (!isRemains)
		{
			if (time < 3600)
				return StringEx::Format(L"%d:%.2d", time / 60, time % 60);
			else
				return StringEx::Format(L"%d:%.2d:%.2d", time / 3600, time / 60 % 60, time % 60);
		}
		else
		{
			if (time < 3600)
				return StringEx::Format(L"-%d:%.2d", time / 60, time % 60);
			else
				return StringEx::Format(L"-%d:%.2d:%.2d", time / 3600, time / 60 % 60, time % 60);
		}
	}
	else
	{
		if (length < 3600)
		{
			return StringEx::Format(L"%d:%.2d / %d:%.2d",
				time / 60, time % 60, length / 60, length % 60);
		}
		else
		{
			if (time < 3600)
			{
				return StringEx::Format(L"%d:%.2d / %d:%.2d:%.2d",
					time / 60, time % 60,
					length / 3600, length / 60 % 60, length % 60);
			}
			else
			{
				return StringEx::Format(L"%d:%.2d:%.2d / %d:%.2d:%.2d",
					time / 3600, time / 60 % 60, time % 60,
					length / 3600, length / 60 % 60, length % 60);
			}
		}
	}
}

void SkinText::SetStatusLine(int count, int total, int time, long long size, bool isShort, Language* lang)
{
	thisText.clear();

	if (count > 1)
	{
		//thisText += L"Selected";
		//thisText.push_back(' ');

		thisText += std::to_wstring(count);
		thisText.push_back(' ');
		if (isUpperCase)
			thisText += StringEx::ToUpper(lang->GetLineS(Lang::StatusLine, 3));
		else
			thisText += lang->GetLineS(Lang::StatusLine, 3);
		thisText.push_back(' ');
	}

	thisText += std::to_wstring(total);
	thisText.push_back(' ');
	if (isUpperCase)
		thisText += StringEx::ToUpper(lang->GetLineS(Lang::StatusLine, 5));
	else
		thisText += lang->GetLineS(Lang::StatusLine, 5);
	thisText.push_back(',');
	thisText.push_back(' ');

	if (time < 3600)
		thisText += StringEx::Format(L"%d:%.2d", time / 60, time % 60);
	else if (time < 3600 * 24)
		thisText += StringEx::Format(L"%d:%.2d:%.2d", time / 3600, time / 60 % 60, time % 60);
	else
		thisText += StringEx::Format(L"%d:%.2d:%.2d:%.2d", time / (3600 * 24), time / 3600 % 24, time / 60 % 60, time % 60);

	if (isShort)
		return;

	thisText.push_back(' ');
	if (isUpperCase)
		thisText += StringEx::ToUpper(lang->GetLineS(Lang::StatusLine, 6));
	else
		thisText += lang->GetLineS(Lang::StatusLine, 6);

	thisText.push_back(',');
	thisText.push_back(' ');

	if (size < 1024 * 1024 * 1024)
	{
		thisText += StringEx::FormatFloat(L"%.1f", (float)size / (1024 * 1024));
		thisText.push_back(' ');
		if (isUpperCase)
			thisText += StringEx::ToUpper(lang->GetLineS(Lang::StatusLine, 1));
		else
			thisText += lang->GetLineS(Lang::StatusLine, 1);
	}
	else
	{
		thisText += StringEx::FormatFloat(L"%.2f", (float)size / (1024 * 1024 * 1024));
		thisText.push_back(' ');
		if (isUpperCase)
			thisText += StringEx::ToUpper(lang->GetLineS(Lang::StatusLine, 2));
		else
			thisText += lang->GetLineS(Lang::StatusLine, 2);
	}
}

void SkinText::DrawTextSimple(HDC dc, bool isAlpha, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format)
{
	// Test
	//::DrawFocusRect(dc, rc);

	if (isAlpha) // If draw on layered window
	{
		if (effect.isFixOpacity)
		{
			if (futureWin->IsVistaOrLater() && futureWin->IsTheme())
				DrawTextAlpha7(dc, text, rc, clr, alpha, format);
			else
				DrawTextAlphaFixXP(dc, text, rc, clr, alpha, format);
		}
		else if (effect.glowSize && (futureWin->IsVistaOrLater() && futureWin->IsTheme()))
			DrawTextAlpha7(dc, text, rc, clr, alpha, format);
		else
			DrawTextAlphaXP(dc, text, rc, clr, alpha, format);

		return;
	}
	else if (effect.glowSize && futureWin->IsVistaOrLater() && futureWin->IsTheme()) // If glow effect
	{
		DrawTextAlpha7(dc, text, rc, clr, alpha, format);

		return;
	}


	HGDIOBJ oldFont = ::SelectObject(dc, font);
	::SetBkMode(dc, TRANSPARENT);

	if (effect.isShadow)
	{
		::SetTextColor(dc, effect.colorShadow);
		::DrawText(dc, text.c_str(), (int)text.size(),
			CRect(rc.left + effect.shadowX, rc.top + effect.shadowY, rc.right + effect.shadowX, rc.bottom + effect.shadowY), format);
	}

	::SetTextColor(dc, clr);
	::DrawText(dc, text.c_str(), (int)text.size(), rc, format);

	::SelectObject(dc, oldFont);
}

void SkinText::DrawTextAlphaXP(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format)
{
//	if (effect.isFixOpacity) // Fix opacity in WinXP is enabled
//	{
//		if (!futureWin->IsVistaOrBetter() || !futureWin->IsTheme())
//			DrawTextAlpha7(dc, text, rc, clr, alpha, format);
//
//		DrawTextAlphaFixXP(dc, text, rc, clr, alpha, format);
//		return;
//	}

	HDC dcBitmap = ::CreateCompatibleDC(dc);

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bi32.bmiHeader.biWidth = rc.Width();
	bi32.bmiHeader.biHeight = rc.Height();
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32; 
	bi32.bmiHeader.biCompression = BI_RGB;

	BYTE* bits = nullptr;
	HBITMAP bitmap = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	int bytes = bi32.bmiHeader.biWidth * bi32.bmiHeader.biHeight * 4;

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, 0};

	// Transfer background
	//::GdiAlphaBlend(dcBitmap, 0, 0, rc.Width(), rc.Height(), dc, rc.left, rc.top, rc.Width(), rc.Height(), bf);
	::BitBlt(dcBitmap, 0, 0, rc.Width(), rc.Height(), dc, rc.left, rc.top, SRCCOPY);
	
	HGDIOBJ oldFont = ::SelectObject(dcBitmap, font);
	::SetBkMode(dcBitmap, TRANSPARENT);

	if (effect.isShadow)
	{
		::SetTextColor(dcBitmap, effect.colorShadow);
		::DrawText(dcBitmap, text.c_str(), (int)text.size(),
			CRect(effect.shadowX, effect.shadowY, rc.Width()+effect.shadowX, rc.Height()+effect.shadowY), format);
	}

	// Draw text
	::SetTextColor(dcBitmap, clr);
	::DrawText(dcBitmap, text.c_str(), (int)text.size(), CRect(0, 0, rc.Width(), rc.Height()), format);

//	 ::DrawFocusRect(dcBitmap, CRect(0, 0, rc.Width(), rc.Height()));

	// Make opaque all pixels with the text
	for (int i = 0; i < bytes; i += 4)
	{
		if (bits[i + 3] == 0) // All pixels with the text are transparent so change only them
			bits[i + 3] = 255;
	}

	// Transfer background with text into window
	//::GdiAlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, rc.Width(), rc.Height(), bf);
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, SRCCOPY);

	// Release resources
	::SelectObject(dcBitmap, oldFont);
	::SelectObject(dcBitmap, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteDC(dcBitmap);
}

void SkinText::DrawTextAlphaFixXP(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format)
{
	// Convert to black and white examples
	////#define RGB2GRAY(r,g,b) (((b)*114 + (g)*587 + (r)*299)/1000)
	//#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bi32.bmiHeader.biWidth = rc.Width();
	bi32.bmiHeader.biHeight = rc.Height();
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32; 
	bi32.bmiHeader.biCompression = BI_RGB;

	int bytes = bi32.bmiHeader.biWidth * bi32.bmiHeader.biHeight * 4;

	// Create mask

	HDC dcBitmapMask = ::CreateCompatibleDC(dc);

	BYTE* bitsMask = nullptr;
	HBITMAP bitmapMask = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bitsMask, NULL, NULL);
	HGDIOBJ oldBitmapMask = ::SelectObject(dcBitmapMask, bitmapMask);

	//::ZeroMemory(bitsMask, bytes); // Make black (should be black by default)

	::SetBkMode(dcBitmapMask, TRANSPARENT);
	::SetTextColor(dcBitmapMask, RGB(255, 255, 255)); // Draw white text on black

	HGDIOBJ oldFontMask = ::SelectObject(dcBitmapMask, font);
	::DrawText(dcBitmapMask, text.c_str(), (int)text.size(), CRect(0, 0, rc.Width(), rc.Height()), format);

	// End create mask

	HDC dcBitmap = ::CreateCompatibleDC(dc);

	BYTE* bits = nullptr;
	HBITMAP bitmap = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	// Transfer background (isn't needed we just use 32 bit color text later)
	//::GdiAlphaBlend(dcBitmap, 0, 0, rc.Width(), rc.Height(), hDC, rc.left, rc.top, rc.Width(), rc.Height(), bf);

	// Shadow (optional)
	if (effect.isShadow)
	{
		// Shadow color
		BYTE red   = GetRValue(effect.colorShadow);
		BYTE green = GetGValue(effect.colorShadow);
		BYTE blue  = GetBValue(effect.colorShadow);

		// Use mask to make 32 bit color (with alpha) text (alpha will be a bit wrong and black and white but it's fine)
		// Draw the text with offset (use i2 and w variables for this) this text will be our shadow
		int w = bi32.bmiHeader.biWidth * -effect.shadowY + effect.shadowX;
		int i2 = w * 4;

		for (int i = 0; i < bytes && i2 < bytes; i += 4, i2 += 4, w++)
		{
			if (w >= bi32.bmiHeader.biWidth)
				w = 0;

			//BYTE alpha = bitsMask[i];
			BYTE alpha = (BYTE)(((int)bitsMask[i]*117 + (int)bitsMask[i+1]*601 + (int)bitsMask[i+2]*306) >> 10);
			if (alpha && i2 >= 0)
			{
				if (w <= effect.shadowX)
					continue;

				BYTE alphaNeg = 255 - alpha;

				bits[  i2  ] = (bits[  i2  ] * alphaNeg + alpha * blue ) / 255;
				bits[i2 + 1] = (bits[i2 + 1] * alphaNeg + alpha * green) / 255;
				bits[i2 + 2] = (bits[i2 + 2] * alphaNeg + alpha * red  ) / 255;
				bits[i2 + 3] = (bits[i2 + 3] * alphaNeg + alpha * 255  ) / 255;
			}
		}

		BLENDFUNCTION bf = {AC_SRC_OVER, 0, effect.colorShadowAlpha, AC_SRC_ALPHA};

		// Transfer shadow text into window
		::GdiAlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, rc.Width(), rc.Height(), bf);

		// Clear the buffer we need it below
		::ZeroMemory(bits, bytes);
	} // End shadow

	// Text color
	BYTE red   = GetRValue(clr);
	BYTE green = GetGValue(clr);
	BYTE blue  = GetBValue(clr);

	// Use mask to make 32 bit color (with alpha) text (alpha will be a bit wrong and black and white but it's fine)
	for (int i = 0; i < bytes; i += 4)
	{
		//BYTE alpha = bitsMask[i];
		BYTE alpha = (BYTE)(((int)bitsMask[i]*117 + (int)bitsMask[i+1]*601 + (int)bitsMask[i+2]*306) >> 10);
		if (alpha)
		{
			BYTE alphaNeg = 255 - alpha;

			bits[  i  ] = (bits[  i  ] * alphaNeg + alpha * blue ) / 255;
			bits[i + 1] = (bits[i + 1] * alphaNeg + alpha * green) / 255;
			bits[i + 2] = (bits[i + 2] * alphaNeg + alpha * red  ) / 255;
			bits[i + 3] = (bits[i + 3] * alphaNeg + alpha * 255  ) / 255;
		}
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};

	// Transfer text into window
	::GdiAlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, rc.Width(), rc.Height(), bf);

	// Release resources
	::SelectObject(dcBitmap, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteDC(dcBitmap);

	// Release mask resources
	::SelectObject(dcBitmapMask, oldFontMask);
	::SelectObject(dcBitmapMask, oldBitmapMask);
	::DeleteObject(bitmapMask);
	::DeleteDC(dcBitmapMask);
}

void SkinText::DrawTextAlpha7(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format)
{
	// Offsets to fix text position when glow effect
	int glowOffsetL = 0;
	int glowOffsetR = 0;

	DTTOPTS opt = {};
	opt.dwSize = sizeof(opt);
	opt.dwFlags = DTT_TEXTCOLOR|DTT_COMPOSITED;
	// Windows 8 does not support glow effect for text (do not do effect.glowSize = 0 for this!)
	if (effect.glowSize > 0 && !futureWin->IsEightOrLater())
	{
		opt.dwFlags |= DTT_GLOWSIZE;
		opt.iGlowSize = effect.glowSize;

		if (format & DT_RIGHT)
			glowOffsetR = effect.glowSize;
		else if (!(format & DT_CENTER))
			glowOffsetL = effect.glowSize;
	}

	HDC dcBitmap = ::CreateCompatibleDC(dc);

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bi32.bmiHeader.biWidth = rc.Width();
	bi32.bmiHeader.biHeight = -rc.Height();
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32; 
	bi32.bmiHeader.biCompression = BI_RGB;

	int bytes = bi32.bmiHeader.biWidth * -bi32.bmiHeader.biHeight * 4;

	BYTE* bits = nullptr;
	HBITMAP bitmap = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	HGDIOBJ oldFont = ::SelectObject(dcBitmap, font);

	// Windows 8 does not support glow effect for text so draw shadow in this case too
	if (effect.isShadow && (effect.glowSize == 0 || futureWin->IsEightOrLater())) // Shadow (optional)
	{
		opt.crText = effect.colorShadow;
		futureWin->DrawThemeTextEx(futureWin->handleTheme, dcBitmap, 0, 0, text.c_str(), -1, format,
			CRect(effect.shadowX, effect.shadowY, rc.Width()+effect.shadowX, rc.Height()+effect.shadowY), &opt);

		BLENDFUNCTION bf = {AC_SRC_OVER, 0, effect.colorShadowAlpha, AC_SRC_ALPHA};

		// Transfer shadow text into window
		::GdiAlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, rc.Width(), rc.Height(), bf);

		// We don't need to clear the buffer here and we can't because it's locked by DrawThemeTextEx
		// Try to clear will cause Write Memory error.
		// What is written above is not true.
		::ZeroMemory(bits, bytes);
	} // End shadow

	// Draw text
	opt.crText = clr;
	futureWin->DrawThemeTextEx(futureWin->handleTheme, dcBitmap, 0, 0, text.c_str(), -1, format,
		CRect(glowOffsetL, 0, rc.Width() - glowOffsetR, rc.Height()), &opt);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};

	// Transfer text into window
	::GdiAlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcBitmap, 0, 0, rc.Width(), rc.Height(), bf);

	// Release resources
	::SelectObject(dcBitmap, oldFont);
	::SelectObject(dcBitmap, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteDC(dcBitmap);
}

int SkinText::GetTextWidth(HDC dc)
{
	assert(dc);

	HFONT oldFond = (HFONT)::SelectObject(dc, font);
	SIZE szText = {};
	::GetTextExtentPoint32W(dc, thisText.c_str(), (int)thisText.size(), &szText);
	::SelectObject(dc, oldFond);

	return szText.cx;
}
