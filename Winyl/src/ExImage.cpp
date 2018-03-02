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

// ExImage.cpp : implementation file
//

#include "stdafx.h"
#include "ExImage.h"

// ExImage

ExImage::ExImage()
{

}

ExImage::~ExImage()
{
	if (bitmapHandle)
		::DeleteObject(bitmapHandle);
}

bool ExImage::LoadFile(const std::wstring& file)
{
	Clear();

	ExImage::Source image;
	if (image.LoadFile(file))
	{
		bitmapHandle = image.GetHBITMAP();

		if (bitmapHandle)
		{
			image.GetSize(&bitmapWidth, &bitmapHeight);

			return true;
		}
	}

	return false;
}

bool ExImage::LoadEx(const std::wstring& file, ZipFile* zipFile)
{
	Clear();

	ExImage::Source image;
	if (image.LoadEx(file, zipFile))
	{
		bitmapHandle = image.GetHBITMAP();

		if (bitmapHandle)
		{
			image.GetSize(&bitmapWidth, &bitmapHeight);

			return true;
		}
	}

	return false;
}

bool ExImage::LoadFromSource(const ExImage::Source& image)
{
	Clear();

	bitmapHandle = image.GetHBITMAP();

	if (bitmapHandle)
	{		
		image.GetSize(&bitmapWidth, &bitmapHeight);

		return true;
	}
	
	return false;
}

void ExImage::Clear()
{
	if (bitmapHandle)
		::DeleteObject(bitmapHandle);

	bitmapHandle = NULL;

	bitmapWidth = 0;
	bitmapHeight = 0;
}

void ExImage::MoveTo(ExImage* image)
{
	image->Clear();

	image->bitmapHandle = bitmapHandle;
	image->bitmapWidth = bitmapWidth;
	image->bitmapHeight = bitmapHeight;

	bitmapHandle = NULL;
	bitmapWidth = 0;
	bitmapHeight = 0;
}

bool ExImage::ThumbnailFromFile(const std::wstring& file, ZipFile* zipFile, int cx, int cy)
{
	Clear();

	ExImage::Source image;
	image.LoadEx(file, zipFile);

	if (image.IsValid())
	{
		return ThumbnailFromSource(image, cx, cy);
	}

	return false;
}

bool ExImage::ThumbnailFromSource(const ExImage::Source& image, int cx, int cy)
{
	Clear();

	ExImage::Source thumb;
	if (image.MakeThumbnail(thumb, cx, cy))
	{
		bitmapHandle = thumb.GetHBITMAP();
		if (bitmapHandle)
		{
			thumb.GetSize(&bitmapWidth, &bitmapHeight);

			return true;
		}
	}

	return false;
}

void ExImage::Tile(HDC dc, int x, int y, int cx, int cy)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		for (int iy = y; iy < y + cy; iy += bitmapHeight)
		{
			for (int ix = x; ix < x + cx; ix += bitmapWidth)
			{
				//::GdiAlphaBlend(dc, ix, iy, bitmapWidth, bitmapHeight, dcBitmap, 0, 0, bitmapWidth, bitmapHeight, bf);
				int width = std::min(bitmapWidth, x + cx - ix);
				int height = std::min(bitmapHeight, y + cy - iy);
				::GdiAlphaBlend(dc, ix, iy, width, height, dcBitmap, 0, 0, width, height, bf);
			}
		}


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::Tile(HDC dc, const CRect& rcDraw)
{
	Tile(dc, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
}

void ExImage::Draw(HDC dc, int x, int y, int cx, int cy)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, 0, 0, bitmapWidth, bitmapHeight, bf);


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::Draw(HDC dc, int x, int y)
{
	Draw(dc, x, y, bitmapWidth, bitmapHeight);
}

void ExImage::Draw(HDC dc, const CRect &rcDraw)
{
	Draw(dc, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
}

void ExImage::DrawFade(HDC dc, int x, int y, int cx, int cy, int alpha)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		alpha = (int)(alpha*1.5f);
		alpha = std::min(255, alpha);


		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)alpha, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, 0, 0, bitmapWidth, bitmapHeight, bf);


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::DrawFade(HDC dc, int x, int y, int alpha)
{
	DrawFade(dc, x, y, bitmapWidth, bitmapHeight, alpha);
}

void ExImage::DrawFade(HDC dc, const CRect &rc, int alpha)
{
	DrawFade(dc, rc.left, rc.top, rc.Width(), rc.Height(), alpha);
}

void ExImage::Crop(HDC dc, int x, int y, int cx, int cy)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		if (cx > bitmapWidth) cx = bitmapWidth;
		if (cy > bitmapHeight) cy = bitmapHeight;

		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, 0, 0, cx, cy, bf);


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::Crop(HDC dc, const CRect &rcDraw)
{
	Crop(dc, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
}

void ExImage::Crop2(HDC dc, int x, int y, int x2, int y2, int cx, int cy)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, x2, y2, cx, cy, bf);


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::Crop2(HDC dc, int x, int y, const CRect &rc)
{
	Crop2(dc, x, y, rc.left, rc.top, rc.Width(), rc.Height());
}

void ExImage::CropFade(HDC dc, int x, int y, int cx, int cy, int alpha)
{
	if (cx == 0 || cy == 0)
		return;

	if (bitmapHandle)
	{
		alpha = (int)(alpha*1.5f);
		alpha = std::min(255, alpha);

		if (cx > bitmapWidth) cx = bitmapWidth;
		if (cy > bitmapHeight) cy = bitmapHeight;

		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);


		BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)alpha, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, 0, 0, cx, cy, bf);


		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

void ExImage::CreateGradientFromDC(bool reverse, int width, int height, HDC dc, int x, int y)
{
	if (bitmapHandle) ::DeleteObject(bitmapHandle);
	bitmapHandle = ExImage::Create32BppBitmap(width, height);
	if (bitmapHandle)
	{
		bitmapWidth = width;
		bitmapHeight = height;

		HDC dcBitmap = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmapHandle);

		int step = 255 / height;
		int stepmax = step * (height + 1); // +1 because i+1 below
		for (int i = 0; i < height; i++)
		{
			int alpha = 0;
			if (!reverse)
				alpha = step * (i + 1);
			else
				alpha = stepmax - step * (i + 1);
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)alpha, AC_SRC_ALPHA};
			::GdiAlphaBlend(dcBitmap, 0, i, width, 1, dc, x, y + i, width, 1, bf);
		}

		::SelectObject(dcBitmap, oldBitmap);
		::DeleteDC(dcBitmap);
	}
}

HBITMAP ExImage::Create32BppBitmap(int width, int height)
{
	// Use this function instead of CreateCompatibleBitmap.
	// The function always use 32 bit color, CreateCompatibleBitmap does not!
	// Also we draw skin with AlphaBlend function, and it works good only with 32 bit DC
	// (otherwise there was a bug on my ATI videocard, sometimes the last pixel did not draw when stretching)

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi32.bmiHeader.biWidth = width;
	bi32.bmiHeader.biHeight = height;
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32; 
	bi32.bmiHeader.biCompression = BI_RGB;

	void* bits = NULL;
	return CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
}

ExImage::Source::Source()
{

}

ExImage::Source::~Source()
{
	Free();
}

void ExImage::Source::Free()
{
	if (bitmap)
	{
		bitmap->Release();
		bitmap = nullptr;
	}
}

bool ExImage::Source::LoadFile(const std::wstring& file)
{
	assert(bitmap == nullptr);

	bitmap = LoadBitmapFromFile(file);

	return bitmap ? true : false;
}

bool ExImage::Source::LoadBuffer(const char* buffer, int size)
{
	assert(bitmap == nullptr);

	IStream* stream = nullptr;
	if (::CreateStreamOnHGlobal(NULL, TRUE, &stream) == S_OK)
	{
		ULARGE_INTEGER lu;
		lu.LowPart = size;
		lu.HighPart = 0;

		if (stream->SetSize(lu) == S_OK &&
			stream->Write(buffer, size, 0) == S_OK)
		{
			LARGE_INTEGER li = {0, 0};
			stream->Seek(li, STREAM_SEEK_SET, NULL);

			bitmap = LoadBitmapFromStream(stream);
		}

		stream->Release();
	}

	return bitmap ? true : false;
}

bool ExImage::Source::LoadEx(const std::wstring& file, ZipFile* zipFile)
{
	if (zipFile == nullptr)
	{
		return LoadFile(file);
	}
	else
	{
		if (zipFile->UnzipToBuffer(file))
		{
			bool result = LoadBuffer(zipFile->GetBuffer(), zipFile->GetBufferSize());
			zipFile->FreeBuffer();
			return result;
		}
	}

	return false;
}

IWICBitmapSource* ExImage::Source::LoadBitmapFromStream(IStream* imageStream)
{
	if (!ImagingFactory::Instance().Get())
		return nullptr;

	IWICBitmapSource* wicBitmapSource = nullptr;
	IWICBitmapDecoder* wicDecoder = nullptr;

	// WICDecodeMetadataCacheOnLoad = load at the current moment
	// WICDecodeMetadataCacheOnDemand = load at the first access
	//HRESULT hr = ::CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicDecoder));
	HRESULT hr = ImagingFactory::Instance().Get()->CreateDecoderFromStream(imageStream, NULL, WICDecodeMetadataCacheOnDemand, &wicDecoder);
	if (SUCCEEDED(hr))
	{
		//wicDecoder->Initialize(wicImageStream, WICDecodeMetadataCacheOnLoad);
		wicBitmapSource = LoadBitmapFromDecoder(wicDecoder);
		wicDecoder->Release();
	}

	return wicBitmapSource;
}

IWICBitmapSource* ExImage::Source::LoadBitmapFromFile(const std::wstring& file)
{
	if (!ImagingFactory::Instance().Get())
		return nullptr;

	IWICBitmapSource* wicBitmapSource = nullptr;
	IWICBitmapDecoder* wicDecoder = nullptr;
	
	// WICDecodeMetadataCacheOnLoad = load at the current moment
	// WICDecodeMetadataCacheOnDemand = load at the first access
	HRESULT hr = ImagingFactory::Instance().Get()->CreateDecoderFromFilename(file.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &wicDecoder);
	if (SUCCEEDED(hr))
	{
		wicBitmapSource = LoadBitmapFromDecoder(wicDecoder);
		wicDecoder->Release();
	}

	return wicBitmapSource;
}

IWICBitmapSource* ExImage::Source::LoadBitmapFromDecoder(IWICBitmapDecoder* wicDecoder)
{
	IWICBitmapSource* wicBitmapSource = nullptr;

	UINT nFrameCount = 0;
	HRESULT hr = wicDecoder->GetFrameCount(&nFrameCount);

	if (SUCCEEDED(hr) && nFrameCount == 1)
	{
		IWICBitmapFrameDecode* wicFrame = nullptr;
		HRESULT hr = wicDecoder->GetFrame(0, &wicFrame);

		if (SUCCEEDED(hr))
		{
			//if (outBitsPerPixel)
			//	*outBitsPerPixel = GetBitsPerPixel(wicFrame);

			//WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, wicFrame, &wicBitmapSource);
			IWICFormatConverter* wicConverter = nullptr;
			hr = ImagingFactory::Instance().Get()->CreateFormatConverter(&wicConverter);
			if (SUCCEEDED(hr))
			{
				// Use BGRA with premiltiplied alpha format here (PBGRA).
				hr = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);
				if (SUCCEEDED(hr))
					wicConverter->QueryInterface(IID_PPV_ARGS(&wicBitmapSource));

				wicConverter->Release();
			}

			wicFrame->Release();
		}

		/*// Without converting
		if (SUCCEEDED(hr))
		{
			wicFrame->QueryInterface(IID_PPV_ARGS(&wicBitmapSource));
			wicFrame->Release();
		}*/
	}

	return wicBitmapSource;
}
/*
int ExImage::Source::GetBitsPerPixel(IWICBitmapFrameDecode* wicFrame) const
{
	UINT bpp = 0;

	WICPixelFormatGUID wicPixelFormatGUID;
	HRESULT hr = wicFrame->GetPixelFormat(&wicPixelFormatGUID);
	if (SUCCEEDED(hr))
	{
		IWICComponentInfo* wicComponentInfo = nullptr;
		hr = ImagingFactory::Instance().Get()->CreateComponentInfo(wicPixelFormatGUID, &wicComponentInfo);
		if (SUCCEEDED(hr))
		{
			IWICPixelFormatInfo* wicPixelFormatInfo = nullptr;
			hr = wicComponentInfo->QueryInterface(IID_PPV_ARGS(&wicPixelFormatInfo));
			if (SUCCEEDED(hr))
			{
				wicPixelFormatInfo->GetBitsPerPixel(&bpp);
				wicPixelFormatInfo->Release();
			}
			wicComponentInfo->Release();
		}
	}

	return (int)bpp;
}
*/
void ExImage::Source::GetSize(int* width, int* height) const
{
	if (!bitmap)
		return;

	UINT w = 0;
	UINT h = 0;
	bitmap->GetSize(&w, &h);
	*width = (int)w;
	*height = (int)h;
}

bool ExImage::Source::IsValid() const
{
	return (bitmap ? true : false);
}

HBITMAP ExImage::Source::GetHBITMAP() const
{
	return CreateHBITMAP(bitmap);
}

HBITMAP ExImage::Source::CreateHBITMAP(IWICBitmapSource* wicBitmapSource) const
{
	if (!wicBitmapSource)
		return NULL;

	HBITMAP bitmapHandle = NULL;

	UINT width = 0;
	UINT height = 0;

	HRESULT hr = wicBitmapSource->GetSize(&width, &height);
	if (SUCCEEDED(hr) && width > 0 && height > 0)
	{
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -((LONG)height);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* bits = nullptr;
		bitmapHandle = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);

		if (bitmapHandle)
		{
			const UINT cbStride = width * 4;
			const UINT cbImage = cbStride * height;

			hr = wicBitmapSource->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(bits));
			if (FAILED(hr))
			{
				::DeleteObject(bitmapHandle);
				bitmapHandle = NULL;
			}
		}
	}

	return bitmapHandle;
}

bool ExImage::Source::MakeThumbnail(Source& thumb, int cx, int cy, bool border) const
{
	if (!ImagingFactory::Instance().Get())
		return false;
	if (!bitmap || thumb.bitmap)
		return false;

	int imagecx = 0;
	int imagecy = 0;
	GetSize(&imagecx, &imagecy);

	if (cx == cy && imagecx == imagecy)
	{
		thumb.bitmap = DrawThumbnail(bitmap, cx, cy, 0, 0, cx, cy, border);
	}
	else
	{
		float fAspect = (float)cx / (float)cy, fScale = 0.0f;
		if (fAspect * imagecy > imagecx)
			fScale = (float)cy / imagecy;
		else
			fScale = (float)cx / imagecx;

		int newcx = (int)(fScale * imagecx + 0.5f);
		int newcy = (int)(fScale * imagecy + 0.5f);

		// If diff less than 4 pixels, then draw fast
		if (abs(cx - cy) < 4 && abs(newcx - newcy) < 4)
		{
			thumb.bitmap = DrawThumbnail(bitmap, cx, cy, 0, 0, cx, cy, border);
		}
		else
		{
			int newx = (cx - newcx) / 2;
			int newy = (cy - newcy) / 2;

			thumb.bitmap = DrawThumbnail(bitmap, cx, cy, newx, newy, newcx, newcy, border);
		}
	}

	return true;
}

IWICBitmapSource* ExImage::Source::DrawThumbnail(IWICBitmapSource* source, int cx, int cy, int newx, int newy, int newcx, int newcy, bool border) const
{
	IWICBitmapScaler* wicBitmapScaler = nullptr;
	IWICBitmapSource* wicBitmapSource = nullptr;

	HRESULT hr = ImagingFactory::Instance().Get()->CreateBitmapScaler(&wicBitmapScaler);
	if (SUCCEEDED(hr))
	{
		hr = wicBitmapScaler->Initialize(source, newcx, newcy, WICBitmapInterpolationModeFant);
		if (SUCCEEDED(hr))
		{
			if (cx == newcx && cy == newcy && !border)
			{
				wicBitmapScaler->QueryInterface(IID_PPV_ARGS(&wicBitmapSource));
			}
			else
			{
				HBITMAP hThumb = Create32BppBitmap(cx, cy);
				HBITMAP hBitmap = CreateHBITMAP(wicBitmapScaler);

				HDC dcThumb = ::CreateCompatibleDC(NULL);
				HDC dcBitmap = ::CreateCompatibleDC(NULL);
				HGDIOBJ oldThumb = ::SelectObject(dcThumb, hThumb);
				HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, hBitmap);

				ExImage::DrawAlphaFill(dcThumb, 0, 0, cx, cy, 0xFF000000);

				BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
				::GdiAlphaBlend(dcThumb, newx, newy, newcx, newcy, dcBitmap, 0, 0, newcx, newcy, bf);

				if (border)
					ExImage::DrawAlphaRect(dcThumb, 0, 0, cx, cy, 0xFF000000);

				::SelectObject(dcThumb, oldThumb);
				::SelectObject(dcBitmap, oldBitmap);
				::DeleteDC(dcThumb);
				::DeleteDC(dcBitmap);
				::DeleteObject(hBitmap);

				IWICBitmap* wicBitmap = nullptr;
				hr = ImagingFactory::Instance().Get()->CreateBitmapFromHBITMAP(hThumb, NULL, WICBitmapUseAlpha, &wicBitmap);

				::DeleteObject(hThumb);

				if (SUCCEEDED(hr))
				{
					wicBitmap->QueryInterface(IID_PPV_ARGS(&wicBitmapSource));
					wicBitmap->Release();
				}
			}
		}

		wicBitmapScaler->Release();
	}

	return wicBitmapSource;
}


void ExImage::Source::MirrorVertical()
{
	if (!ImagingFactory::Instance().Get())
		return;
	if (!bitmap)
		return;

	IWICBitmapFlipRotator* wicFlipRotator = nullptr;
	HRESULT hr = ImagingFactory::Instance().Get()->CreateBitmapFlipRotator(&wicFlipRotator);
	if (SUCCEEDED(hr))
	{
		hr = wicFlipRotator->Initialize(bitmap, WICBitmapTransformFlipVertical);
		if (SUCCEEDED(hr))
		{
			bitmap->Release();
			bitmap = nullptr;

			wicFlipRotator->QueryInterface(IID_PPV_ARGS(&bitmap));
		}

		wicFlipRotator->Release();
	}
}

bool ExImage::Source::SaveToFile(const std::wstring& file, bool jpeg)
{
	if (!ImagingFactory::Instance().Get())
		return false;
	if (!bitmap)
		return false;

	bool result = false;

	IWICStream* wicStream = nullptr;
	HRESULT hr = ImagingFactory::Instance().Get()->CreateStream(&wicStream);
	if (SUCCEEDED(hr))
	{
		hr = wicStream->InitializeFromFilename(file.c_str(), GENERIC_WRITE);
		if (SUCCEEDED(hr))
		{
			result = SaveBitmapToStream(bitmap, wicStream, jpeg);
		}
		wicStream->Release();
	}

	return result;
}

bool ExImage::Source::SaveToBuffer(std::vector<char> buffer, bool jpeg)
{
	if (!ImagingFactory::Instance().Get())
		return false;
	if (!bitmap)
		return false;

	bool result = false;

	IStream* stream = nullptr;
	if (::CreateStreamOnHGlobal(NULL, TRUE, &stream) == S_OK)
	{
		IWICStream* wicStream = nullptr;
		HRESULT hr = ImagingFactory::Instance().Get()->CreateStream(&wicStream);
		if (SUCCEEDED(hr))
		{
			hr = wicStream->InitializeFromIStream(stream);
			if (SUCCEEDED(hr))
			{
				result = SaveBitmapToStream(bitmap, wicStream, jpeg);
			}
			wicStream->Release();
		}

		LARGE_INTEGER li = {0, 0};
		ULARGE_INTEGER uli = {0, 0};

		stream->Seek(li, STREAM_SEEK_END, &uli);
		if (uli.HighPart == 0)
		{
			buffer.resize((std::size_t)uli.LowPart);
			stream->Seek(li, STREAM_SEEK_SET, NULL);
			stream->Read(&buffer[0], uli.LowPart, NULL);
		}

		stream->Release();
	}

	return result && !buffer.empty();
}

bool ExImage::Source::SaveBitmapToStream(IWICBitmapSource* wicBitmapSource, IWICStream* wicStream, bool jpeg)
{
	// Make sure you do not use it with GUID_WICPixelFormat32bppPBGRA pixel format

	bool result = false;

	IWICBitmapEncoder* wicBitmapEncoder = nullptr;
	HRESULT hr = 0;
	if (jpeg)
		hr = ImagingFactory::Instance().Get()->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &wicBitmapEncoder);
	else
		hr = ImagingFactory::Instance().Get()->CreateEncoder(GUID_ContainerFormatPng, NULL, &wicBitmapEncoder);
	if (SUCCEEDED(hr))
	{
		hr = wicBitmapEncoder->Initialize(wicStream, WICBitmapEncoderNoCache);
		if (SUCCEEDED(hr))
		{
			IPropertyBag2* wicProperyBag2 = nullptr;
			IWICBitmapFrameEncode* wicBitmapFrameEncode = nullptr;
			if (jpeg)
				hr = wicBitmapEncoder->CreateNewFrame(&wicBitmapFrameEncode, &wicProperyBag2);
			else
				hr = wicBitmapEncoder->CreateNewFrame(&wicBitmapFrameEncode, NULL);
			if (SUCCEEDED(hr))
			{
				bool succeeded = true;
				if (jpeg)
				{
					PROPBAG2 name = { 0 };
					name.dwType = PROPBAG2_TYPE_DATA;
					name.vt = VT_R4;
					name.pstrName = L"ImageQuality";

					VARIANT value = { 0 };
					value.vt = VT_R4;
					value.fltVal = 0.95F;

					hr = wicProperyBag2->Write(1, &name, &value);
					if (FAILED(hr))
						succeeded = false;
				}

				if (succeeded)
				{
					if (jpeg)
						hr = wicBitmapFrameEncode->Initialize(wicProperyBag2);
					else
						hr = wicBitmapFrameEncode->Initialize(NULL);
					if (SUCCEEDED(hr))
					{
						WICPixelFormatGUID wicPixelFormatGUID = GUID_WICPixelFormat24bppBGR;
						hr = wicBitmapFrameEncode->SetPixelFormat(&wicPixelFormatGUID);
						if (SUCCEEDED(hr))
						{
							hr = wicBitmapFrameEncode->WriteSource(wicBitmapSource, NULL);
							if (SUCCEEDED(hr))
							{
								hr = wicBitmapFrameEncode->Commit();
								if (SUCCEEDED(hr))
								{
									hr = wicBitmapEncoder->Commit();
									if (SUCCEEDED(hr))
										result = true;
								}
							}
						}
					}
				}
				wicBitmapFrameEncode->Release();
			}
		}
		wicBitmapEncoder->Release();
	}

	return result;
}


HFONT ExImage::CreateElementFont(const char* name, int size, int bold, int italic, int clear)
{
	// Get system font
	static bool isFontLoaded = false;
	static NONCLIENTMETRICS ncm = {};
	if (!isFontLoaded)
	{
		isFontLoaded = true;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		// http://stackoverflow.com/questions/35288433/query-windows-color-and-appearance
	}

	if (name)
	{
		WCHAR utf16_text[LF_FACESIZE] = L"";
		MultiByteToWideChar(CP_UTF8, NULL, name, -1, utf16_text, LF_FACESIZE);

		wcscpy_s(ncm.lfMessageFont.lfFaceName, utf16_text);
	}
	else
	{
		if (futureWin->IsVistaOrLater())
			wcscpy_s(ncm.lfMessageFont.lfFaceName, L"Segoe UI");
		//else if (size == 9)
		//	wcscpy_s(ncm.lfMessageFont.lfFaceName, L"Arial");
		else
			wcscpy_s(ncm.lfMessageFont.lfFaceName, L"Tahoma");
	}

	ncm.lfMessageFont.lfOrientation = 0;

	// ::GetDeviceCaps(::GetDC(0), LOGPIXELSY) = 96 (By Default)
	if (size > 0)
		ncm.lfMessageFont.lfHeight = -MulDiv(size, 96, 72);
	else
		ncm.lfMessageFont.lfHeight = -MulDiv(8, 96, 72);


	if (bold)
		ncm.lfMessageFont.lfWeight = FW_BOLD;
	else
		ncm.lfMessageFont.lfWeight = FW_NORMAL;

	if (italic)
		ncm.lfMessageFont.lfItalic = TRUE;
	else
		ncm.lfMessageFont.lfItalic = FALSE;

	if (clear)
	{
		if (clear == 1)
			ncm.lfMessageFont.lfQuality = CLEARTYPE_QUALITY;
		else if (clear == 2)
			ncm.lfMessageFont.lfQuality = NONANTIALIASED_QUALITY;
	}

	return ::CreateFontIndirect(&ncm.lfMessageFont);
}

void ExImage::DrawAlphaFill(HDC dc, int x, int y, int cx, int cy, COLORREF clr)
{
	HDC dcBitmap = ::CreateCompatibleDC(dc);

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi32.bmiHeader.biWidth = 1;
	bi32.bmiHeader.biHeight = 1;
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32;
	bi32.bmiHeader.biCompression = BI_RGB;

	BYTE* bits = NULL;
	HBITMAP bitmap = ::CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	if (bits)
	{
		BYTE alpha = HIBYTE(HIWORD(clr)); // GetAValue(clr)

		// Premultiple alpha: output = color * alpha / 255. To round to nearest integer use + 255 / 2. (PBGRA WIC format).
		bits[0] = (GetBValue(clr) * alpha + 255 / 2) / 255;
		bits[1] = (GetGValue(clr) * alpha + 255 / 2) / 255;
		bits[2] = (GetRValue(clr) * alpha + 255 / 2) / 255;
		bits[3] = alpha;

		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, cy, dcBitmap, 0, 0, 1, 1, bf);
	}

	::SelectObject(dcBitmap, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteDC(dcBitmap);
}

void ExImage::DrawAlphaRect(HDC dc, int x, int y, int cx, int cy, COLORREF clr)
{
	HDC dcBitmap = ::CreateCompatibleDC(dc);

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi32.bmiHeader.biWidth = 1;
	bi32.bmiHeader.biHeight = 1;
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32;
	bi32.bmiHeader.biCompression = BI_RGB;

	BYTE* bits = NULL;
	HBITMAP bitmap = ::CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	if (bits)
	{
		BYTE alpha = HIBYTE(HIWORD(clr)); // GetAValue(clr)

		// Premultiple alpha: output = color * alpha / 255. To round to nearest integer use + 255 / 2. (PBGRA WIC format).
		bits[0] = (GetBValue(clr) * alpha + 255 / 2) / 255;
		bits[1] = (GetGValue(clr) * alpha + 255 / 2) / 255;
		bits[2] = (GetRValue(clr) * alpha + 255 / 2) / 255;
		bits[3] = alpha;

		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		::GdiAlphaBlend(dc, x, y, cx, 1, dcBitmap, 0, 0, 1, 1, bf);
		::GdiAlphaBlend(dc, x, y + cy - 1, cx, 1, dcBitmap, 0, 0, 1, 1, bf);
		::GdiAlphaBlend(dc, x, y + 1, 1, cy - 2, dcBitmap, 0, 0, 1, 1, bf);
		::GdiAlphaBlend(dc, x + cx - 1, y + 1, 1, cy - 2, dcBitmap, 0, 0, 1, 1, bf);
	}

	::SelectObject(dcBitmap, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteDC(dcBitmap);
}

void ExImage::DrawAlphaFill(HDC dc, const CRect &rc, COLORREF clr)
{
	DrawAlphaFill(dc, rc.left, rc.top, rc.Width(), rc.Height(), clr);
}

void ExImage::DrawAlphaRect(HDC dc, const CRect& rc, COLORREF clr)
{
	DrawAlphaRect(dc, rc.left, rc.top, rc.Width(), rc.Height(), clr);
}
