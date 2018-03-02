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
#include "TagLibLyrics.h"
#include "TagLibWriter.h"
#include "UTF.h"
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


TagLibLyrics::TagLibLyrics()
{

}

TagLibLyrics::~TagLibLyrics()
{

}

bool TagLibLyrics::ReadLyricsFromFile(const std::wstring& file)
{
	// Do not read AudioProperties it can speed up file reading.
	//TagLib::FileRef f(file.c_str(), false, TagLib::AudioProperties::Fast);
	TagLibReader::File f(file, true, false);

	return ReadFileTagsFromTagLibFile(f);
}

bool TagLibLyrics::ReadFileTagsFromTagLibFile(const TagLibReader::File& f)
{
	if (!f.IsOpen())
		return false;

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		if (mpeg->ID3v2Tag() && mpeg->hasID3v2Tag())
			ReadLyricsFromID3v2Tags(mpeg->ID3v2Tag());
		else if (mpeg->APETag() && mpeg->hasAPETag())
			ReadLyricsFromAPETags(mpeg->APETag());
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		if (mp4->tag())
			ReadLyricsFromMP4Tags(mp4->tag());
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		if (asf->tag())
			ReadLyricsFromASFTags(asf->tag());
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			if (ogg_vorbis->tag())
				ReadLyricsFromOGGTags(ogg_vorbis->tag());
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			if (ogg_flac->tag())
				ReadLyricsFromOGGTags(ogg_flac->tag());
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			if (ogg_opus->tag())
				ReadLyricsFromOGGTags(ogg_opus->tag());
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			if (ogg_speex->tag())
				ReadLyricsFromOGGTags(ogg_speex->tag());
		}
	}
	else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		if (flac->xiphComment() && flac->hasXiphComment())
			ReadLyricsFromOGGTags(flac->xiphComment());
		else if (flac->ID3v2Tag() && flac->hasID3v2Tag())
			ReadLyricsFromID3v2Tags(flac->ID3v2Tag());
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		if (ape->APETag() && ape->hasAPETag())
			ReadLyricsFromAPETags(ape->APETag());
	}
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			if (riff_wav->ID3v2Tag() && riff_wav->hasID3v2Tag())
				ReadLyricsFromID3v2Tags(riff_wav->ID3v2Tag());
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			if (riff_aiff->tag() && riff_aiff->hasID3v2Tag())
				ReadLyricsFromID3v2Tags(riff_aiff->tag());
		}
	}
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		if (mpc->APETag() && mpc->hasAPETag())
			ReadLyricsFromAPETags(mpc->APETag());
	}
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		if (wavpack->APETag() && wavpack->hasAPETag())
			ReadLyricsFromAPETags(wavpack->APETag());
	}
	else if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		if (tta->ID3v2Tag() && tta->hasID3v2Tag())
			ReadLyricsFromID3v2Tags(tta->ID3v2Tag());
	}

	return true;
}

void TagLibLyrics::SaveLyricsToTagLibFile(const TagLibReader::File& f, int id3v2version)
{
	if (!newLyrics.second)
		return;

	if (!f.IsOpen())
		return;

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		// Save lyrics to one tag only
		if (mpeg->hasAPETag() && !mpeg->hasID3v2Tag())
			SaveLyricsToAPETags(mpeg->APETag(true));
		else
			SaveLyricsToID3v2Tags(mpeg->ID3v2Tag(true), id3v2version);
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		SaveLyricsToAPETags(ape->APETag(true));
	}
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		SaveLyricsToAPETags(mpc->APETag(true));
	}
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		SaveLyricsToAPETags(wavpack->APETag(true));
	}
	if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		SaveLyricsToID3v2Tags(tta->ID3v2Tag(true), id3v2version);
	}
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			SaveLyricsToID3v2Tags(riff_wav->ID3v2Tag(), id3v2version);
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			SaveLyricsToID3v2Tags(riff_aiff->tag(), id3v2version);
		}
	}
	else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		SaveLyricsToOGGTags(flac->xiphComment(true));
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			SaveLyricsToOGGTags(ogg_vorbis->tag());
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			SaveLyricsToOGGTags(ogg_flac->tag());
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			SaveLyricsToOGGTags(ogg_opus->tag());
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			SaveLyricsToOGGTags(ogg_speex->tag());
		}
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		SaveLyricsToMP4Tags(mp4->tag());
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		SaveLyricsToASFTags(asf->tag());
	}
}

void TagLibLyrics::ReadLyricsFromID3v2Tags(TagLib::ID3v2::Tag* tag)
{
	TagLibReader::ReadID3v2TagFrameLyrics(tag, "USLT", lyrics);
}

void TagLibLyrics::ReadLyricsFromAPETags(TagLib::APE::Tag* tag)
{
	TagLibReader::ReadAPETagItem(tag, "LYRICS", lyrics); // Lyrics
}

void TagLibLyrics::ReadLyricsFromOGGTags(TagLib::Ogg::XiphComment* tag)
{
	TagLibReader::ReadOGGTagComment(tag, "LYRICS", lyrics);
}

void TagLibLyrics::ReadLyricsFromASFTags(TagLib::ASF::Tag* tag)
{
	TagLibReader::ReadASFTagAttributeText(tag, "WM/Lyrics", lyrics);
}

void TagLibLyrics::ReadLyricsFromMP4Tags(TagLib::MP4::Tag* tag)
{
	TagLibReader::ReadMP4TagItemText(tag, "\251lyr", lyrics);
}

void TagLibLyrics::SaveLyricsToID3v2Tags(TagLib::ID3v2::Tag* tag, int version)
{
	TagLibWriter::SaveID3v2TagFrameLyrics(tag, version, "USLT", newLyrics.first);
}

void TagLibLyrics::SaveLyricsToAPETags(TagLib::APE::Tag* tag)
{
	TagLibWriter::SaveAPETagItem(tag, "Lyrics", newLyrics.first);
}

void TagLibLyrics::SaveLyricsToOGGTags(TagLib::Ogg::XiphComment* tag)
{
	TagLibWriter::SaveOGGTagComment(tag, "LYRICS", newLyrics.first);
}

void TagLibLyrics::SaveLyricsToASFTags(TagLib::ASF::Tag* tag)
{
	TagLibWriter::SaveASFTagAttribute(tag, "WM/Lyrics", newLyrics.first);
}

void TagLibLyrics::SaveLyricsToMP4Tags(TagLib::MP4::Tag* tag)
{
	TagLibWriter::SaveMP4TagItem(tag, "\251lyr", newLyrics.first);
}
