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

// TagLibCover.cpp : implementation file
//

#include "stdafx.h"
#include "TagLibCover.h"
#define TAGLIB_STATIC
#include "taglib/tag.h"
#include "taglib/fileref.h"
#include "taglib/tfilestream.h"
#include "taglib/mpegfile.h"
#include "taglib/id3v1tag.h"
#include "taglib/id3v1genres.h"
#include "taglib/id3v2tag.h"
#include "taglib/attachedpictureframe.h"
#include "taglib/commentsframe.h"
#include "taglib/generalencapsulatedobjectframe.h"
#include "taglib/textidentificationframe.h"
#include "taglib/unsynchronizedlyricsframe.h"
#include "taglib/urllinkframe.h"
#include "taglib/apefile.h"
#include "taglib/apetag.h"
#include "taglib/asffile.h"
#include "taglib/mp4file.h"
#include "taglib/mpcfile.h"
#include "taglib/trueaudiofile.h"
#include "taglib/wavpackfile.h"
#include "taglib/flacfile.h"
#include "taglib/oggfile.h"
#include "taglib/xiphcomment.h"
#include "taglib/oggflacfile.h"
#include "taglib/opusfile.h"
#include "taglib/vorbisfile.h"
#include "taglib/speexfile.h"
#include "taglib/rifffile.h"
#include "taglib/wavfile.h"
#include "taglib/aifffile.h"


// TagLibCover

TagLibCover::TagLibCover()
{

}

TagLibCover::~TagLibCover()
{

}

bool TagLibCover::ReadCoverFromFile(const std::wstring& file)
{
	// Do not read AudioProperties it can speed up file reading.
	//TagLib::FileRef f(file.c_str(), false, TagLib::AudioProperties::Fast);
	TagLibReader::File f(file, true, false);

	if (!f.IsOpen())
		return false;

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		if (mpeg->ID3v2Tag() && mpeg->hasID3v2Tag())
			ReadCoverFromID3v2Tags(mpeg->ID3v2Tag());
		//else if (mpeg->APETag() && mpeg->hasAPETag())
		//	ReadCoverFromAPE(mpeg->APETag()); // Cover in mp3 from APE tags is disabled (I don't think it's needed).
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		if (mp4->tag())
			ReadCoverFromMP4Tags(mp4->tag());
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		if (asf->tag())
			ReadCoverFromASFTags(asf->tag());
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			if (ogg_vorbis->tag())
				ReadCoverFromOGGTags(ogg_vorbis->tag());
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			if (ogg_flac->tag())
				ReadCoverFromOGGTags(ogg_flac->tag());
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			if (ogg_opus->tag())
				ReadCoverFromOGGTags(ogg_opus->tag());
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			if (ogg_speex->tag())
				ReadCoverFromOGGTags(ogg_speex->tag());
		}
	}
    else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		ReadCoverFromFLACFile(flac);
		// To add a support for a cover from xiphComment in FLAC file,
		// the previous function must return bool value (it's not implemented)
		// and only after that we can uncomment the next line.
		//{
		//	if (flac->xiphComment())
		//		ReadCoverFromOGG(flac->xiphComment());
		////else if (flac->ID3v2Tag())
		////	ReadCoverFromMP3(flac->ID3v2Tag()); // Cover in FLAC from id3v2 is disabled (it's not standard anyway).
		//}
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		if (ape->APETag() && ape->hasAPETag())
			ReadCoverFromAPETags(ape->APETag());
    }
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			if (riff_wav->ID3v2Tag() && riff_wav->hasID3v2Tag())
				ReadCoverFromID3v2Tags(riff_wav->ID3v2Tag());
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			if (riff_aiff->tag() && riff_aiff->hasID3v2Tag())
				ReadCoverFromID3v2Tags(riff_aiff->tag());
		}
	}
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		if (mpc->APETag() && mpc->hasAPETag())
			ReadCoverFromAPETags(mpc->APETag());
    }
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		if (wavpack->APETag() && wavpack->hasAPETag())
			ReadCoverFromAPETags(wavpack->APETag());
    }
	else if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		if (tta->ID3v2Tag() && tta->hasID3v2Tag())
			ReadCoverFromID3v2Tags(tta->ID3v2Tag());
    }

	return true;
}

void TagLibCover::SaveCoverToTagLibFile(const TagLibReader::File& f)
{
	if (!newCover)
		return;

	if (!f.IsOpen())
		return;

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		// Save cover to one tag only
		if (mpeg->hasAPETag() && !mpeg->hasID3v2Tag())
			SaveCoverToAPETags(mpeg->APETag(true));
		else
			SaveCoverToID3v2Tags(mpeg->ID3v2Tag(true));
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		SaveCoverToAPETags(ape->APETag(true));
	}
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		SaveCoverToAPETags(mpc->APETag(true));
	}
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		SaveCoverToAPETags(wavpack->APETag(true));
	}
	if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		SaveCoverToID3v2Tags(tta->ID3v2Tag(true));
	}
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			SaveCoverToID3v2Tags(riff_wav->ID3v2Tag());
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			SaveCoverToID3v2Tags(riff_aiff->tag());
		}
	}
	else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		SaveCoverToFLAC(flac);
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			SaveCoverToOGGTags(ogg_vorbis->tag());
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			SaveCoverToOGGTags(ogg_flac->tag());
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			SaveCoverToOGGTags(ogg_opus->tag());
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			SaveCoverToOGGTags(ogg_speex->tag());
		}
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		SaveCoverToMP4Tags(mp4->tag());
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		SaveCoverToASFTags(asf->tag());
	}
}

void TagLibCover::ReadCoverFromID3v2Tags(TagLib::ID3v2::Tag* tag)
{
	// Search for a frame with a cover and read it
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator itmap = mapFrames.find("APIC");
	if (itmap != mapFrames.end())
	{
		const TagLib::ID3v2::FrameList& listFrames = itmap->second;
		if (!listFrames.isEmpty())
		{
			auto itcover = listFrames.begin();
			// Try to find front cover
			//for (int i = 0, size = (int)listFrames.size(); i < size; ++i)
			for (auto it = listFrames.begin(), end = listFrames.end(); it != end; ++it)
			{
				TagLib::ID3v2::AttachedPictureFrame* frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*it);
				if (frame)
				{
					if (frame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
					{
						itcover = it;
						break;
					}
				}
			}

			// Use front cover or if not found use first picture
			TagLib::ID3v2::AttachedPictureFrame* frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*itcover);
			if (frame)
			{
				TagLib::ByteVector pic = frame->picture();
				const char* data = pic.data();
				int size = pic.size();

				if (data && size > 0)
					CreateCoverImage(data, size);
			}
		}
	}
}

void TagLibCover::ReadCoverFromAPETags(TagLib::APE::Tag* tag)
{
	// Search for an item with a cover and read it
	// In APE it's not standard to keep the cover in tags (but many programs do this)
	const TagLib::APE::ItemListMap& mapItems = tag->itemListMap();
	TagLib::APE::ItemListMap::ConstIterator itmap = mapItems.find("COVER ART (FRONT)"); // "COVER ART (BACK)"
	if (itmap != mapItems.end())
	{
		const TagLib::APE::Item& item = itmap->second;
		if (!item.isEmpty())
		{
			TagLib::ByteVector pic = item.binaryData();

			// Skip the description of the cover (max 1024 bytes)
			const int maxoffset = 1024;
			int offset = 0;
			for (offset = 0; pic[offset] != 0 && offset < (int)pic.size() && offset <= maxoffset; offset++);

			if (offset <= maxoffset)
			{
				const char* data = pic.data() + offset + 1;
				int size = pic.size() - offset - 1;

				if (data && size > 0)
					CreateCoverImage(data, size);
			}
		}
	}
}

void TagLibCover::ReadCoverFromFLACFile(TagLib::FLAC::File* file)
{
	// Search for a cover (in FLAC file it's located in the file instead of tags)
	// therefore this function takes a pointer to a file instead of tags like other functions
	TagLib::List<TagLib::FLAC::Picture*> listPictures = file->pictureList();
	if (!listPictures.isEmpty())
	{
		auto itcover = listPictures.begin();
		// Try to find front cover
		for (auto it = listPictures.begin(), end = listPictures.end(); it != end; ++it)
		{
			if ((*it)->type() == TagLib::FLAC::Picture::FrontCover)
			{
				itcover = it;
				break;
			}
		}

		TagLib::ByteVector pic = (*itcover)->data();
		int size = pic.size();
		const char* data = pic.data();

		if (data && size > 0)
			CreateCoverImage(data, size);
	}
}

void TagLibCover::ReadCoverFromOGGTags(TagLib::Ogg::XiphComment* tag)
{
	TagLib::List<TagLib::FLAC::Picture*> listPictures = tag->pictureList();
	if (!listPictures.isEmpty())
	{
		auto itcover = listPictures.begin();
		// Try to find front cover
		for (auto it = listPictures.begin(), end = listPictures.end(); it != end; ++it)
		{
			if ((*it)->type() == TagLib::FLAC::Picture::FrontCover)
			{
				itcover = it;
				break;
			}
		}

		TagLib::ByteVector pic = (*itcover)->data();
		int size = pic.size();
		const char* data = pic.data();

		if (data && size > 0)
			CreateCoverImage(data, size);
	}
}

void TagLibCover::ReadCoverFromASFTags(TagLib::ASF::Tag* tag)
{
	// Search for an attribute with a cover and read it
	const TagLib::ASF::AttributeListMap& mapAttributes = tag->attributeListMap();
	TagLib::ASF::AttributeListMap::ConstIterator itmap = mapAttributes.find("WM/Picture");
	if (itmap != mapAttributes.end())
	{
		const TagLib::ASF::AttributeList& listAttributes = itmap->second;
		if (!listAttributes.isEmpty())
		{
			auto itcover = listAttributes.begin();
			// Try to find front cover
			for (auto it = listAttributes.begin(), end = listAttributes.end(); it != end; ++it)
			{
				const TagLib::ASF::Attribute& attribute = *it;
				if (attribute.type() == TagLib::ASF::Attribute::BytesType &&
					attribute.toPicture().type() == TagLib::ASF::Picture::FrontCover)
				{
					itcover = it;
					break;
				}
			}

			// Use front cover or if not found use first picture
			const TagLib::ASF::Attribute& attribute = *itcover;
			if (attribute.type() == TagLib::ASF::Attribute::BytesType)
			{
				TagLib::ByteVector pic = attribute.toPicture().picture();
				int size = pic.size();
				const char* data = pic.data();

				if (data && size > 0)
					CreateCoverImage(data, size);
			}
		}
	}
}

void TagLibCover::ReadCoverFromMP4Tags(TagLib::MP4::Tag* tag)
{
	// Search for an item with a cover and read it
	const TagLib::MP4::ItemListMap& mapItems = tag->itemMap();
	TagLib::MP4::ItemListMap::ConstIterator itmap = mapItems.find("covr");
	if (itmap != mapItems.end())
	{
		const TagLib::MP4::Item& item = itmap->second;
		if (item.isValid())
		{
			TagLib::MP4::CoverArtList listCoverArts = item.toCoverArtList();
			if (!listCoverArts.isEmpty())
			{
				TagLib::ByteVector pic = listCoverArts.front().data();
				int size = pic.size();
				const char* data = pic.data();

				if (data && size > 0)
					CreateCoverImage(data, size);
			}
		}
	}
}

void TagLibCover::CreateCoverImage(const char* data, int size)
{
	outImage->LoadBuffer(data, size);
}

bool TagLibCover::IsCoverJPG(const std::vector<char>& data)
{
	if (data.size() > 4 &&
		data[0] == (char)0xFF && data[1] == (char)0xD8 &&
		data[2] == (char)0xFF && data[3] == (char)0xE0)
		return true;

	return false;
}

bool TagLibCover::IsCoverPNG(const std::vector<char>& data)
{
	if (data.size() > 4 &&
		data[0] == (char)0x89 && data[1] == (char)0x50 &&
		data[2] == (char)0x4E && data[3] == (char)0x47)
		return true;

	return false;
}

void TagLibCover::SaveCoverToID3v2Tags(TagLib::ID3v2::Tag* tag)
{
	tag->removeFrames("APIC");

	if (newCover->empty())
		return;

	char* mimeType = nullptr;
	if (IsCoverJPG(*newCover))
		mimeType = "image/jpeg";
	else if (IsCoverPNG(*newCover))
		mimeType = "image/png";
	else
		return;

	TagLib::ByteVector picture(newCover->data(), newCover->size());

	// Add new Front Cover frame
	TagLib::ID3v2::AttachedPictureFrame* frame = new TagLib::ID3v2::AttachedPictureFrame();
	if (frame)
	{
		frame->setPicture(picture);
		frame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
		frame->setMimeType(mimeType);
		tag->addFrame(frame);
	}
}

void TagLibCover::SaveCoverToAPETags(TagLib::APE::Tag* tag)
{
	tag->removeItem("COVER ART (FRONT)");
	tag->removeItem("COVER ART (BACK)");

	if (newCover->empty())
		return;

	char* description = nullptr;
	if (IsCoverJPG(*newCover))
		description = "Cover Art (Front).jpg";
	else if (IsCoverPNG(*newCover))
		description = "Cover Art (Front).png";
	else
		return;

	TagLib::ByteVector picture;
	picture.append(description);
	picture.append('\0');
	picture.append(TagLib::ByteVector(newCover->data(), newCover->size()));

	// Add new "COVER ART (FRONT)" item
	TagLib::APE::Item item;
	item.setKey("Cover Art (Front)");
	item.setType(TagLib::APE::Item::Binary);
	item.setBinaryData(picture);
	tag->setItem(item.key(), item);
}

void TagLibCover::SaveCoverToFLAC(TagLib::FLAC::File* file)
{
	file->removePictures();

	if (newCover->empty())
		return;

	char* mimeType = nullptr;
	if (IsCoverJPG(*newCover))
		mimeType = "image/jpeg";
	else if (IsCoverPNG(*newCover))
		mimeType = "image/png";
	else
		return;

	TagLib::ByteVector picture(newCover->data(), newCover->size());

	// Add new Front Cover picture
	TagLib::FLAC::Picture* pic = new TagLib::FLAC::Picture();
	pic->setWidth(0);
	pic->setHeight(0);
	pic->setColorDepth(0);
	pic->setNumColors(0);
	pic->setType(TagLib::FLAC::Picture::FrontCover);
	pic->setMimeType(mimeType);
	pic->setData(picture);

	file->addPicture(pic);
}

void TagLibCover::SaveCoverToOGGTags(TagLib::Ogg::XiphComment* tag)
{
	tag->removeAllPictures();

	if (newCover->empty())
		return;

	char* mimeType = nullptr;
	if (IsCoverJPG(*newCover))
		mimeType = "image/jpeg";
	else if (IsCoverPNG(*newCover))
		mimeType = "image/png";
	else
		return;

	TagLib::ByteVector picture(newCover->data(), newCover->size());

	// Add new Front Cover picture
	TagLib::FLAC::Picture* pic = new TagLib::FLAC::Picture();
	pic->setWidth(0);
	pic->setHeight(0);
	pic->setColorDepth(0);
	pic->setNumColors(0);
	pic->setType(TagLib::FLAC::Picture::FrontCover);
	pic->setMimeType(mimeType);
	pic->setData(picture);

	tag->addPicture(pic);
}

void TagLibCover::SaveCoverToASFTags(TagLib::ASF::Tag* tag)
{
	tag->removeItem("WM/Picture");

	if (newCover->empty())
		return;

	char* mimeType = nullptr;
	if (IsCoverJPG(*newCover))
		mimeType = "image/jpeg";
	else if (IsCoverPNG(*newCover))
		mimeType = "image/png";
	else
		return;

	TagLib::ByteVector picture(newCover->data(), newCover->size());

	// Add new Front Cover attribute
	TagLib::ASF::Picture pic;
	pic.setType(TagLib::ASF::Picture::FrontCover);
	pic.setMimeType(mimeType);
	pic.setPicture(picture);

	tag->addAttribute("WM/Picture", TagLib::ASF::Attribute(pic));
}

void TagLibCover::SaveCoverToMP4Tags(TagLib::MP4::Tag* tag)
{
	tag->removeItem("covr");

	if (newCover->empty())
		return;

	TagLib::MP4::CoverArt::Format format = TagLib::MP4::CoverArt::Unknown;
	if (IsCoverJPG(*newCover))
		format = TagLib::MP4::CoverArt::JPEG;
	else if (IsCoverPNG(*newCover))
		format = TagLib::MP4::CoverArt::PNG;
	else
		return;

	TagLib::ByteVector picture(newCover->data(), newCover->size());

	// Add new "covr" item
	TagLib::MP4::CoverArtList coverArtList;
	coverArtList.append(TagLib::MP4::CoverArt(format, picture));

	tag->setItem("covr", TagLib::MP4::Item(coverArtList));
}
