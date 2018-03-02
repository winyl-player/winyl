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

#include "ZipFile.h"
#include <wincodec.h>

// ExImage

class ExImage
{
public:
	ExImage();
	virtual ~ExImage();
	ExImage(const ExImage&) = delete;
	ExImage& operator=(const ExImage&) = delete;

	class Source;

	bool LoadFile(const std::wstring& file);
	bool LoadFromSource(const ExImage::Source& image);
	bool ThumbnailFromFile(const std::wstring& file, ZipFile* zipFile, int cx, int cy);
	bool ThumbnailFromSource(const ExImage::Source& image, int cx, int cy);
	bool LoadEx(const std::wstring& file, ZipFile* zipFile);
	void MoveTo(ExImage* image);
	void Clear();

	inline int Width() {return bitmapWidth;}
	inline int Height() {return bitmapHeight;}
	inline bool IsValid() {return bitmapHandle ? true : false;}
	void Draw(HDC dc, int x, int y, int cx, int cy);
	void Draw(HDC dc, int x, int y);
	void Draw(HDC dc, const CRect& rcDraw);
	void DrawFade(HDC dc, int x, int y, int cx, int cy, int alpha);
	void DrawFade(HDC dc, int x, int y, int alpha);
	void DrawFade(HDC dc, const CRect& rc, int alpha);
	void Crop(HDC dc, int x, int y, int cx, int cy);
	void Crop(HDC, const CRect& rcDraw);
	void Crop2(HDC dc, int x, int y, int x2, int y2, int cx, int cy);
	void Crop2(HDC dc, int x, int y, const CRect &rc);
	void CropFade(HDC dc, int x, int y, int cx, int cy, int alpha);
	void Tile(HDC dc, int x, int y, int cx, int cy);
	void Tile(HDC dc, const CRect& rcDraw);

	void CreateGradientFromDC(bool reverse, int width, int height, HDC dc, int x, int y);
	static HBITMAP Create32BppBitmap(int width, int height);
	static HFONT CreateElementFont(const char* name, int size, int bold, int italic, int clear = 0);
	static void DrawAlphaFill(HDC dc, int x, int y, int cx, int cy, COLORREF clr);
	static void DrawAlphaRect(HDC dc, int x, int y, int cx, int cy, COLORREF clr);
	static void DrawAlphaFill(HDC dc, const CRect& rc, COLORREF clr);
	static void DrawAlphaRect(HDC dc, const CRect& rc, COLORREF clr);
	static void FillDC(HDC dc, int x, int y, int cx, int cy)
	{
		CRect rc(x, y, x + cx, y + cy);
		::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	static void FillDC(HDC dc, RECT rc)
	{
		::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}

private:
	int bitmapWidth = 0;
	int bitmapHeight = 0;
	HBITMAP bitmapHandle = NULL;

public:
	class Source
	{
	public:
		Source();
		virtual ~Source();
		Source(const Source&) = delete;
		Source& operator=(const Source&) = delete;

		bool LoadFile(const std::wstring& file);
		bool LoadBuffer(const char* buffer, int size);
		bool LoadEx(const std::wstring& file, ZipFile* zipFile);
		void Free();
		bool IsValid() const;
		void GetSize(int* width, int* height) const;
		HBITMAP GetHBITMAP() const;
		bool MakeThumbnail(Source& thumb, int cx, int cy, bool border = false) const;
		void MirrorVertical();
		bool SaveToFile(const std::wstring& file, bool jpeg);
		bool SaveToBuffer(std::vector<char> buffer, bool jpeg);
	private:
		IWICBitmapSource* bitmap = nullptr;

		HBITMAP CreateHBITMAP(IWICBitmapSource* wicBitmapSource) const;
		IWICBitmapSource* LoadBitmapFromStream(IStream* imageStream);
		IWICBitmapSource* LoadBitmapFromFile(const std::wstring& file);
		IWICBitmapSource* LoadBitmapFromDecoder(IWICBitmapDecoder* wicDecoder);
		IWICBitmapSource* DrawThumbnail(IWICBitmapSource* source, int cx, int cy, int newx, int newy, int newcx, int newcy, bool border) const;
		bool SaveBitmapToStream(IWICBitmapSource* wicBitmapSource, IWICStream* wicStream, bool jpeg);


	public:
		class ImagingFactory // Singleton
		{
		private:
			ImagingFactory() {}
		public:
			virtual ~ImagingFactory()
			{
				assert(wicFactory == nullptr);
			}
			ImagingFactory(const ImagingFactory& root) = delete;
			ImagingFactory& operator=(const ImagingFactory&) = delete;

			void Init()
			{
				//::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
				::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
			}
			void Free()
			{
				if (wicFactory)
				{
					wicFactory->Release();
					wicFactory = nullptr;
				}
			}
			static ImagingFactory& Instance()
			{
				static ImagingFactory factory;
				return factory;
			}
			IWICImagingFactory* Get() const
			{
				return wicFactory;
			}
		private:
			IWICImagingFactory* wicFactory = nullptr;
		};
	};
};


