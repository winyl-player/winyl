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

// TagLibReader.cpp : implementation file
//

#include "stdafx.h"
#include "TagLibReader.h"
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


// TagLibWriter

TagLibReader::TagLibReader()
{
	// Useful links (Winyl uses MusicBrainz tags mapping):
	// https://picard.musicbrainz.org/docs/mappings/
	// http://wiki.hydrogenaudio.org/index.php?title=Tag_Mapping
	//
	// http://qoobar.sourceforge.net/en/documentation.htm#subtitle
	// http://www.hydrogenaudio.org/forums/index.php?showtopic=92623&st=25
	// https://docs.google.com/spreadsheet/ccc?key=0Ai8FOnydTkfodF9PTjF2SFJhU0trM2NtS1RHamRZbHc#gid=0
	// http://help.mp3tag.de/main_tags.html
	// http://www.hamstrup.dk/tagdefinitions.htm
	// http://www.proppfrexx.radio42.com/forum/viewtopic.php?f=9&t=7
	// http://kid3.sourceforge.net/kid3_en.html
	// http://www.thewelltemperedcomputer.com/SW/Players/MusiCHI.htm
	// http://www.thewelltemperedcomputer.com/TG/2_Classical.html
	// http://msdn.microsoft.com/en-au/library/dd743065%28v=vs.85%29.aspx
}

TagLibReader::~TagLibReader()
{

}

bool TagLibReader::ReadFileTags(const std::wstring& file)
{
	//TagLib::FileRef f(file.c_str(), true, TagLib::AudioProperties::Fast);
	TagLibReader::File f(file, true, true);

	return ReadFileTagsFromTagLibFile(f);
}

bool TagLibReader::ReadFileTagsFromTagLibFile(const TagLibReader::File& f)
{
	if (!f.IsOpen())
		return false;

	if (f.file()->audioProperties())
		ReadAudioProperties(f.file()->audioProperties());

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		if (mpeg->ID3v2Tag() && mpeg->hasID3v2Tag())
			ReadID3v2Tags(mpeg->ID3v2Tag());
		else if (mpeg->APETag() && mpeg->hasAPETag())
			ReadAPETags(mpeg->APETag());
		else if (mpeg->ID3v1Tag() && mpeg->hasID3v1Tag())
			ReadID3v1Tags(mpeg->ID3v1Tag());
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		if (mp4->tag())
			ReadMP4Tags(mp4->tag());
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		if (asf->tag())
			ReadASFTags(asf->tag());
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			if (ogg_vorbis->tag())
				ReadOGGTags(ogg_vorbis->tag());
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			if (ogg_flac->tag())
				ReadOGGTags(ogg_flac->tag());
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			if (ogg_opus->tag())
				ReadOGGTags(ogg_opus->tag());
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			if (ogg_speex->tag())
				ReadOGGTags(ogg_speex->tag());
		}
	}
	else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		if (flac->xiphComment() && flac->hasXiphComment())
			ReadOGGTags(flac->xiphComment());
		else if (flac->ID3v2Tag() && flac->hasID3v2Tag())
			ReadID3v2Tags(flac->ID3v2Tag());
		else if (flac->ID3v1Tag() && flac->hasID3v1Tag())
			ReadID3v1Tags(flac->ID3v1Tag());
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		if (ape->APETag() && ape->hasAPETag())
			ReadAPETags(ape->APETag());
		else if (ape->ID3v1Tag() && ape->hasID3v1Tag())
			ReadID3v1Tags(ape->ID3v1Tag());
	}
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			if (riff_wav->ID3v2Tag() && riff_wav->hasID3v2Tag())
				ReadID3v2Tags(riff_wav->ID3v2Tag());
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			if (riff_aiff->tag() && riff_aiff->hasID3v2Tag())
				ReadID3v2Tags(riff_aiff->tag());
		}
	}
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		if (mpc->APETag() && mpc->hasAPETag())
			ReadAPETags(mpc->APETag());
		else if (mpc->ID3v1Tag() && mpc->hasID3v1Tag())
			ReadID3v1Tags(mpc->ID3v1Tag());
	}
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		if (wavpack->APETag() && wavpack->hasAPETag())
			ReadAPETags(wavpack->APETag());
		else if (wavpack->ID3v1Tag() && wavpack->hasID3v1Tag())
			ReadID3v1Tags(wavpack->ID3v1Tag());
	}
	else if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		if (tta->ID3v2Tag() && tta->hasID3v2Tag())
			ReadID3v2Tags(tta->ID3v2Tag());
		else if (tta->ID3v1Tag() && tta->hasID3v1Tag())
			ReadID3v1Tags(tta->ID3v1Tag());
	}

	return true;
}

void TagLibReader::ReadAudioProperties(TagLib::AudioProperties* properties)
{
	tags.bitrate = properties->bitrate();
	tags.channels = properties->channels();
	tags.duration = properties->lengthInMilliseconds();
	tags.sampleRate = properties->sampleRate();
}

void TagLibReader::ReadID3v2Tags(TagLib::ID3v2::Tag* tag)
{
	ReadID3v2TagFrameText(tag, "TIT2", tags.title);
	ReadID3v2TagFrameText(tag, "TALB", tags.album);
	ReadID3v2TagFrameText(tag, "TPE1", tags.artist, &tags.artists);
	ReadID3v2TagFrameText(tag, "TPE2", tags.albumArtist, &tags.albumArtists);
	ReadID3v2TagFrameTextGenre(tag, "TCON", tags.genre, &tags.genres);
	ReadID3v2TagFrameText(tag, "TCOM", tags.composer, &tags.composers);
	ReadID3v2TagFrameText(tag, "TPUB", tags.publisher);
	ReadID3v2TagFrameText(tag, "TPE3", tags.conductor, &tags.conductors);
	ReadID3v2TagFrameText(tag, "TEXT", tags.lyricist, &tags.lyricists);
	ReadID3v2TagFrameText(tag, "TIT1", tags.grouping);
	ReadID3v2TagFrameText(tag, "TIT3", tags.subtitle);
	ReadID3v2TagFrameText(tag, "TCOP", tags.copyright);
	ReadID3v2TagFrameText(tag, "TENC", tags.encodedby);
	ReadID3v2TagFrameText(tag, "TPE4", tags.remixer);
	ReadID3v2TagFrameText(tag, "TCMP", tags.compilation);
	// Use different frame type for comment
	ReadID3v2TagFrameComment(tag, "COMM", tags.comment);

	//if (tag->header()->majorVersion() == 4)
	//	ReadID3v2TagFrameText(tag, "TMOO", tags.mood);
	//else
	//	ReadID3v2TagFrameTxxx(tag, "MOOD", tags.mood);

	ReadID3v2TagFrameText(tag, "TBPM", tags.bpm);
	ReadID3v2TagFrameText(tag, "TDRC", tags.year);
	ReadID3v2TagFrameText(tag, "TRCK", tags.track);
	ReadID3v2TagFrameText(tag, "TPOS", tags.disc);
	SplitNumberTotal(tags.track, tags.totalTracks);
	SplitNumberTotal(tags.disc, tags.totalDiscs);
}

bool TagLibReader::ReadID3v2TagFrameText(TagLib::ID3v2::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator itmap = mapFrames.find(id);
	if (itmap != mapFrames.end())
	{
		const TagLib::ID3v2::FrameList& listFrames = itmap->second;
		if (!listFrames.isEmpty())
		{
			TagLib::ID3v2::TextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(listFrames.front());
			if (frame)
			{
				TagLib::StringList listFields = frame->fieldList();
				if (!listFields.isEmpty())
				{
					if (frame->textEncoding() == TagLib::String::Latin1)
						outText = UTF::UTF8S(UTF::UTF16AS(listFields.front().to8Bit()));
					else
						outText = UTF::UTF8S(listFields.front().toWString());

					if (outArray)
					{
						for (auto it = ++listFields.begin(), end = listFields.end(); it != end; ++it)
						{
							if (frame->textEncoding() == TagLib::String::Latin1)
								outArray->push_back(UTF::UTF8S(UTF::UTF16AS(it->to8Bit())));
							else
								outArray->push_back(UTF::UTF8S(it->toWString()));
						}
					}

					return true;
				}
			}
		}
	}

	return false;
}

bool TagLibReader::ReadID3v2TagFrameComment(TagLib::ID3v2::Tag* tag, char* id, std::string& outText)
{
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator itmap = mapFrames.find(id);
	if (itmap != mapFrames.end())
	{
		const TagLib::ID3v2::FrameList& listFrames = itmap->second;
		if (!listFrames.isEmpty())
		{
			TagLib::ID3v2::CommentsFrame* frame = dynamic_cast<TagLib::ID3v2::CommentsFrame*>(listFrames.front());
			if (frame)
			{
				if (frame->textEncoding() == TagLib::String::Latin1)
					outText = UTF::UTF8S(UTF::UTF16AS(frame->text().to8Bit()));
				else
					outText = UTF::UTF8S(frame->text().toWString());

				return true;
			}
		}
	}

	return false;
}

bool TagLibReader::ReadID3v2TagFrameTextGenre(TagLib::ID3v2::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator itmap = mapFrames.find(id);
	if (itmap != mapFrames.end())
	{
		const TagLib::ID3v2::FrameList& listFrames = itmap->second;
		if (!listFrames.isEmpty())
		{
			TagLib::ID3v2::TextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(listFrames.front());
			if (frame)
			{
				TagLib::StringList listFields = frame->fieldList();
				if (!listFields.isEmpty())
				{
					// ID3v2.4 lists genres as the fields in its frames field list. If the field
					// is simply a number it can be assumed that it is an ID3v1 genre number.
					// etc. for more info see taglib id3v2tag.cpp
					const TagLib::String& genre = listFields.front();

					bool ok = false;
					int number = genre.toInt(&ok);

					if (ok && number >= 0 && number <= 255)
						outText = UTF::UTF8S(TagLib::ID3v1::genre(number).toWString());
					else
					{
						if (frame->textEncoding() == TagLib::String::Latin1)
							outText = UTF::UTF8S(UTF::UTF16AS(genre.to8Bit()));
						else
							outText = UTF::UTF8S(genre.toWString());
					}

					if (outArray)
					{
						for (auto it = ++listFields.begin(), end = listFields.end(); it != end; ++it)
						{
							if (frame->textEncoding() == TagLib::String::Latin1)
								outArray->push_back(UTF::UTF8S(UTF::UTF16AS(it->to8Bit())));
							else
								outArray->push_back(UTF::UTF8S(it->toWString()));
						}
					}

					return true;
				}
			}
		}
	}

	return false;
}

bool TagLibReader::ReadID3v2TagFrameLyrics(TagLib::ID3v2::Tag* tag, char* id, std::string& outText)
{
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator itmap = mapFrames.find(id);
	if (itmap != mapFrames.end())
	{
		const TagLib::ID3v2::FrameList& listFrames = itmap->second;
		if (!listFrames.isEmpty())
		{
			TagLib::ID3v2::UnsynchronizedLyricsFrame* frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(listFrames.front());
			if (frame)
			{
				if (frame->textEncoding() == TagLib::String::Latin1)
					outText = UTF::UTF8S(UTF::UTF16AS(frame->text().to8Bit()));
				else
					outText = UTF::UTF8S(frame->text().toWString());

				return true;
			}
		}
	}

	return false;
}
/*
bool TagLibReader::ReadID3v2TagFrameTxxx(TagLib::ID3v2::Tag* tag, char* id, std::string& outText)
{
	const TagLib::ID3v2::FrameListMap& mapFrames = tag->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator it = mapFrames.find("TXXX");
	if (it != mapFrames.end())
	{
		TagLib::String description = id;

		const TagLib::ID3v2::FrameList& listFrames = it->second;
		for (int i = 0, size = (int)listFrames.size(); i < size; ++i)
		{
			TagLib::ID3v2::UserTextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(listFrames[i]);
			if (frame && frame->description() == description)
			{
				TagLib::StringList listFields = frame->fieldList();
				// 1 because first field is description
				if (listFields.size() > 1)
				{
					if (frame->textEncoding() == TagLib::String::Latin1)
						outText = UTF::UTF8S(UTF::UTF16AS(listFields[1].to8Bit()));
					else
						outText = UTF::UTF8S(listFields[1].toWString());

					return true;
				}
			}
		}
	}

	return false;
}
*/
void TagLibReader::ReadID3v1Tags(TagLib::ID3v1::Tag* tag)
{
	tags.title = UTF::UTF8S(UTF::UTF16AS(tag->title().to8Bit()));
	tags.album = UTF::UTF8S(UTF::UTF16AS(tag->album().to8Bit()));
	tags.artist = UTF::UTF8S(UTF::UTF16AS(tag->artist().to8Bit()));
	tags.comment = UTF::UTF8S(UTF::UTF16AS(tag->comment().to8Bit()));

	tags.genre = UTF::UTF8S(tag->genre().toWString());

	int track = tag->track();
	if (track > 0) tags.track = std::to_string(track);

	int year = tag->year();
	if (year > 0) tags.year = std::to_string(year);
}

void TagLibReader::ReadAPETags(TagLib::APE::Tag* tag)
{
	ReadAPETagItem(tag, "TITLE", tags.title); // Title
	ReadAPETagItem(tag, "ALBUM", tags.album); // Album
	ReadAPETagItem(tag, "ARTIST", tags.artist, &tags.artists); // Artist
	if (!ReadAPETagItem(tag, "ALBUM ARTIST", tags.albumArtist, &tags.albumArtists)) // Album Artist
	{
		if (!ReadAPETagItem(tag, "ALBUMARTIST", tags.albumArtist, &tags.albumArtists))
			ReadAPETagItem(tag, "ENSEMBLE", tags.albumArtist, &tags.albumArtists);
	}
	ReadAPETagItem(tag, "GENRE", tags.genre, &tags.genres); // Genre
	ReadAPETagItem(tag, "COMPOSER", tags.composer, &tags.composers); // Composer
	if (!ReadAPETagItem(tag, "LABEL", tags.publisher)) // Label
		ReadAPETagItem(tag, "PUBLISHER", tags.publisher); // Publisher
	ReadAPETagItem(tag, "CONDUCTOR", tags.conductor, &tags.conductors); // Conductor
	ReadAPETagItem(tag, "LYRICIST", tags.lyricist, &tags.lyricists); // Lyricist
	if (!ReadAPETagItem(tag, "GROUPING", tags.grouping)) // Grouping
		ReadAPETagItem(tag, "CONTENTGROUP", tags.grouping); // ContentGroup
	ReadAPETagItem(tag, "SUBTITLE", tags.subtitle); // Subtitle
	ReadAPETagItem(tag, "COPYRIGHT", tags.copyright); // Copyright
	ReadAPETagItem(tag, "ENCODEDBY", tags.encodedby); // EncodedBy
	ReadAPETagItem(tag, "MIXARTIST", tags.remixer); // MixArtist
	ReadAPETagItem(tag, "COMPILATION", tags.compilation); // Compilation
	ReadAPETagItem(tag, "COMMENT", tags.comment); // Comment
	//ReadAPETagItem(tag, "MOOD", tags.mood); // Mood

	ReadAPETagItem(tag, "BPM", tags.bpm);
	ReadAPETagItem(tag, "YEAR", tags.year); // Year
	ReadAPETagItem(tag, "TRACK", tags.track); // Track
	if (!ReadAPETagItem(tag, "DISC", tags.disc)) // Disc
		ReadAPETagItem(tag, "DISCNUMBER", tags.disc);
	SplitNumberTotal(tags.track, tags.totalTracks);
	SplitNumberTotal(tags.disc, tags.totalDiscs);
}

bool TagLibReader::ReadAPETagItem(TagLib::APE::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::APE::ItemListMap& mapItems = tag->itemListMap();
	TagLib::APE::ItemListMap::ConstIterator itmap = mapItems.find(id);
	if (itmap != mapItems.end())
	{
		const TagLib::APE::Item& item = itmap->second;
		if (!item.isEmpty())
		{
			TagLib::StringList listValues = item.values();
			if (!listValues.isEmpty())
			{
				outText = UTF::UTF8S(listValues.front().toWString());

				if (outArray)
				{
					for (auto it = ++listValues.begin(), end = listValues.end(); it != end; ++it)
						outArray->push_back(UTF::UTF8S(it->toWString()));
				}

				return true;
			}
		}
	}

	return false;
}

void TagLibReader::ReadOGGTags(TagLib::Ogg::XiphComment* tag)
{
	ReadOGGTagComment(tag, "TITLE", tags.title);
	ReadOGGTagComment(tag, "ALBUM", tags.album);
	ReadOGGTagComment(tag, "ARTIST", tags.artist, &tags.artists);
	if (!ReadOGGTagComment(tag, "ALBUMARTIST", tags.albumArtist, &tags.albumArtists))
	{
		if (!ReadOGGTagComment(tag, "ALBUM ARTIST", tags.albumArtist, &tags.albumArtists))
			ReadOGGTagComment(tag, "ENSEMBLE", tags.albumArtist, &tags.albumArtists);
	}
	ReadOGGTagComment(tag, "GENRE", tags.genre, &tags.genres);
	ReadOGGTagComment(tag, "COMPOSER", tags.composer, &tags.composers);
	if (!ReadOGGTagComment(tag, "LABEL", tags.publisher))
		ReadOGGTagComment(tag, "ORGANIZATION", tags.publisher);
	ReadOGGTagComment(tag, "CONDUCTOR", tags.conductor, &tags.conductors);
	ReadOGGTagComment(tag, "LYRICIST", tags.lyricist, &tags.lyricists);
	if (!ReadOGGTagComment(tag, "GROUPING", tags.grouping))
		ReadOGGTagComment(tag, "CONTENTGROUP", tags.grouping);
	ReadOGGTagComment(tag, "SUBTITLE", tags.subtitle);
	ReadOGGTagComment(tag, "COPYRIGHT", tags.copyright);
	ReadOGGTagComment(tag, "ENCODEDBY", tags.encodedby);
	ReadOGGTagComment(tag, "REMIXER", tags.remixer);
	ReadOGGTagComment(tag, "COMPILATION", tags.compilation);
	ReadOGGTagComment(tag, "COMMENT", tags.comment);
	//ReadOGGTagComment(tag, "MOOD", tags.mood);

	ReadOGGTagComment(tag, "BPM", tags.bpm);
	ReadOGGTagComment(tag, "DATE", tags.year);
	ReadOGGTagComment(tag, "TRACKNUMBER", tags.track);
	ReadOGGTagComment(tag, "DISCNUMBER", tags.disc);
	if (!ReadOGGTagComment(tag, "TRACKTOTAL", tags.totalTracks))
		ReadOGGTagComment(tag, "TOTALTRACKS", tags.totalTracks);
	if (!ReadOGGTagComment(tag, "DISCTOTAL", tags.totalDiscs))
		ReadOGGTagComment(tag, "TOTALDISCS", tags.totalTracks);
	SplitNumberTotal(tags.track, tags.totalTracks);
	SplitNumberTotal(tags.disc, tags.totalDiscs);
}

bool TagLibReader::ReadOGGTagComment(TagLib::Ogg::XiphComment* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::Ogg::FieldListMap& mapFields = tag->fieldListMap();
	TagLib::Ogg::FieldListMap::ConstIterator itmap = mapFields.find(id);
	if (itmap != mapFields.end())
	{
		const TagLib::StringList& listFields = itmap->second;
		if (!listFields.isEmpty())
		{
			outText = UTF::UTF8S(listFields.front().toWString());

			if (outArray)
			{
				for (auto it = ++listFields.begin(), end = listFields.end(); it != end; ++it)
					outArray->push_back(UTF::UTF8S(it->toWString()));
			}

			return true;
		}
	}

	return false;
}

void TagLibReader::ReadASFTags(TagLib::ASF::Tag* tag)
{
	// Some tags in ASF are not saved in attributes (title, artist, copyright, comment, rating)
	// so first we read these tags separately, and then attributes.
	tags.title = UTF::UTF8S(tag->title().toWString());
	tags.artist = UTF::UTF8S(tag->artist().toWString());
	tags.comment = UTF::UTF8S(tag->comment().toWString());
	tags.copyright = UTF::UTF8S(tag->copyright().toWString());

	//if (tags.title.empty())
	//	ReadASFTagAttributeText(tag, "Title", tags.album);
	//if (tags.artist.empty())
	//	ReadASFTagAttributeText(tag, "Author", tags.artist);
	//if (tags.comment.empty())
	//	ReadASFTagAttributeText(tag, "Description", tags.comment);

	if (!tags.artist.empty())
		ReadASFTagAttributeTextArray(tag, "Author", tags.artists);
	ReadASFTagAttributeText(tag, "WM/AlbumTitle", tags.album);
	ReadASFTagAttributeText(tag, "WM/AlbumArtist", tags.albumArtist, &tags.albumArtists);
	ReadASFTagAttributeText(tag, "WM/Genre", tags.genre, &tags.genres);
	ReadASFTagAttributeText(tag, "WM/Composer", tags.composer, &tags.composers);
	ReadASFTagAttributeText(tag, "WM/Publisher", tags.publisher);
	ReadASFTagAttributeText(tag, "WM/Conductor", tags.conductor, &tags.conductors);
	ReadASFTagAttributeText(tag, "WM/Writer", tags.lyricist, &tags.lyricists);
	ReadASFTagAttributeText(tag, "WM/ContentGroupDescription", tags.grouping);
	ReadASFTagAttributeText(tag, "WM/SubTitle", tags.subtitle);
	ReadASFTagAttributeText(tag, "WM/EncodedBy", tags.encodedby);
	ReadASFTagAttributeText(tag, "WM/ModifiedBy", tags.remixer);
	ReadASFTagAttributeBool(tag, "WM/IsCompilation", tags.compilation);
	//ReadASFTagAttributeText(tag, "WM/Mood", tags.mood);

	ReadASFTagAttributeText(tag, "WM/BeatsPerMinute", tags.bpm);
	ReadASFTagAttributeText(tag, "WM/Year", tags.year);
	// I'm not sure we can save track count across slash (1/10) but many tags editors do this.
	// And PartOfSet we certainly can, it's in the standard:
	// http://msdn.microsoft.com/en-us/library/ms867702#wm_metadata_usage_partofset
	if (!ReadASFTagAttributeTextOrInt(tag, "WM/TrackNumber", tags.track))
		ReadASFTagAttributeTextOrInt(tag, "WM/Track", tags.track);
	ReadASFTagAttributeText(tag, "WM/PartOfSet", tags.disc);
	SplitNumberTotal(tags.track, tags.totalTracks);
	SplitNumberTotal(tags.disc, tags.totalDiscs);
}

bool TagLibReader::ReadASFTagAttributeText(TagLib::ASF::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::ASF::AttributeListMap& mapAttributes = tag->attributeListMap();
	TagLib::ASF::AttributeListMap::ConstIterator itmap = mapAttributes.find(id);
	if (itmap != mapAttributes.end())
	{
		const TagLib::ASF::AttributeList& listAttributes = itmap->second;
		if (!listAttributes.isEmpty())
		{
			// TagLib has a bug when multiple attributes with same id (incorrect order)
			// so if multiple attributes then read last one to fix, I don't know why.
			if (listAttributes.size() > 1)
				outText = UTF::UTF8S((--listAttributes.end())->toString().toWString());
			else
				outText = UTF::UTF8S(listAttributes.front().toString().toWString());

			if (outArray)
			{
				// Also fix the previous bug here
				for (auto it = listAttributes.begin(), end = --listAttributes.end(); it != end; ++it)
					outArray->push_back(UTF::UTF8S(it->toString().toWString()));
			}

			return true;
		}
	}

	return false;
}

bool TagLibReader::ReadASFTagAttributeTextArray(TagLib::ASF::Tag* tag, char* id, std::vector<std::string>& outArray)
{
	const TagLib::ASF::AttributeListMap& mapAttributes = tag->attributeListMap();
	TagLib::ASF::AttributeListMap::ConstIterator itmap = mapAttributes.find(id);
	if (itmap != mapAttributes.end())
	{
		const TagLib::ASF::AttributeList& listAttributes = itmap->second;
		if (!listAttributes.isEmpty())
		{
			for (auto it = listAttributes.begin(), end = listAttributes.end(); it != end; ++it)
				outArray.push_back(UTF::UTF8S(it->toString().toWString()));

			return true;
		}
	}

	return false;
}

bool TagLibReader::ReadASFTagAttributeTextOrInt(TagLib::ASF::Tag* tag, char* id, std::string& outText)
{
	const TagLib::ASF::AttributeListMap& mapAttributes = tag->attributeListMap();
	TagLib::ASF::AttributeListMap::ConstIterator itmap = mapAttributes.find(id);
	if (itmap != mapAttributes.end())
	{
		const TagLib::ASF::AttributeList& listAttributes = itmap->second;
		if (!listAttributes.isEmpty())
		{
			const TagLib::ASF::Attribute& attribute = listAttributes.front();
			if (attribute.type() == TagLib::ASF::Attribute::DWordType)
				outText = std::to_string(attribute.toUInt());
			else
				outText = UTF::UTF8S(attribute.toString().toWString());

			return true;
		}
	}

	return false;
}

bool TagLibReader::ReadASFTagAttributeBool(TagLib::ASF::Tag* tag, char* id, std::string& outText)
{
	const TagLib::ASF::AttributeListMap& mapAttributes = tag->attributeListMap();
	TagLib::ASF::AttributeListMap::ConstIterator itmap = mapAttributes.find(id);
	if (itmap != mapAttributes.end())
	{
		const TagLib::ASF::AttributeList& listAttributes = itmap->second;
		if (!listAttributes.isEmpty())
		{
			const TagLib::ASF::Attribute& attribute = listAttributes.front();
			if (attribute.type() == TagLib::ASF::Attribute::BoolType && attribute.toBool())
				outText = '1';
			else // Do not write false bool (or do?)
				outText = '0';

			return true;
		}
	}

	return false;
}

void TagLibReader::ReadMP4Tags(TagLib::MP4::Tag* tag)
{
	// "\251" it's copyright symbol, in MP4 some keys started with it
	// http://atomicparsley.sourceforge.net/mpeg-4files.html

	ReadMP4TagItemText(tag, "\251nam", tags.title);
	ReadMP4TagItemText(tag, "\251alb", tags.album);
	ReadMP4TagItemText(tag, "\251ART", tags.artist, &tags.artists);
	ReadMP4TagItemText(tag, "aART", tags.albumArtist, &tags.albumArtists);
	if (!ReadMP4TagItemText(tag, "\251gen", tags.genre, &tags.genres))
	{
		if (!ReadMP4TagItemText(tag, "----:com.apple.iTunes:GENRE", tags.genre, &tags.genres)) // foobar2000 multiple genres
			ReadMP4TagItemText(tag, "----:com.apple.iTunes:Genre", tags.genre, &tags.genres); // mp3tag multiple genres
	}
	ReadMP4TagItemText(tag, "\251wrt", tags.composer, &tags.composers);
	ReadMP4TagItemText(tag, "----:com.apple.iTunes:LABEL", tags.publisher); // labl
	ReadMP4TagItemText(tag, "----:com.apple.iTunes:CONDUCTOR", tags.conductor, &tags.conductors); // cond
	ReadMP4TagItemText(tag, "----:com.apple.iTunes:LYRICIST", tags.lyricist, &tags.lyricists); // lyrt
	ReadMP4TagItemText(tag, "\251grp", tags.grouping);
	ReadMP4TagItemText(tag, "----:com.apple.iTunes:SUBTITLE", tags.subtitle); // desc (MP3Tag), subt
	ReadMP4TagItemText(tag, "cprt", tags.copyright);
	ReadMP4TagItemText(tag, "\251too", tags.encodedby);
	ReadMP4TagItemText(tag, "----:com.apple.iTunes:REMIXER", tags.remixer); // rmix
	ReadMP4TagItemText(tag, "\251cmt", tags.comment);
	//ReadMP4TagItemText(tag, "----:com.apple.iTunes:MOOD", tags.mood); // mood

	ReadMP4TagItemText(tag, "\251day", tags.year);
	// Use different function for digits (track/disk number)
	ReadMP4TagItemBool(tag, "cpil", tags.compilation);
	ReadMP4TagItemInt(tag, "tmpo", tags.bpm);
	ReadMP4TagItemIntPair(tag, "trkn", tags.track, tags.totalTracks);
	ReadMP4TagItemIntPair(tag, "disk", tags.disc, tags.totalDiscs);
}

bool TagLibReader::ReadMP4TagItemText(TagLib::MP4::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray)
{
	const TagLib::MP4::ItemListMap& mapItems = tag->itemMap();
	TagLib::MP4::ItemListMap::ConstIterator itmap = mapItems.find(id);
	if (itmap != mapItems.end())
	{
		const TagLib::MP4::Item& item = itmap->second;
		if (item.isValid())
		{
			TagLib::StringList listValues = item.toStringList();
			if (!listValues.isEmpty())
			{
				outText = UTF::UTF8S(listValues.front().toWString());
				// TagLib does not support multiple atoms with the same id but many apps use it,
				// the following code supports multiple string in one atom, but it useless for us.
//				if (outArray)
//				{
//					for (auto it = ++listValues.begin(), end = listValues.end(); it != end; ++it)
//						outArray->push_back(UTF::UTF8S(it->toWString()));
//				}

				return true;
			}
		}
	}

	return false;
}

bool TagLibReader::ReadMP4TagItemIntPair(TagLib::MP4::Tag* tag, char* id, std::string& outText1, std::string& outText2)
{
	const TagLib::MP4::ItemListMap& mapItems = tag->itemMap();
	TagLib::MP4::ItemListMap::ConstIterator itmap = mapItems.find(id);
	if (itmap != mapItems.end())
	{
		const TagLib::MP4::Item& item = itmap->second;
		if (item.isValid())
		{
			TagLib::MP4::Item::IntPair pair = item.toIntPair();

			outText1 = std::to_string(pair.first);
			outText2 = std::to_string(pair.second);

			return true;
		}
	}

	return false;
}

bool TagLibReader::ReadMP4TagItemInt(TagLib::MP4::Tag* tag, char* id, std::string& outText)
{
	const TagLib::MP4::ItemListMap& mapItems = tag->itemMap();
	TagLib::MP4::ItemListMap::ConstIterator itmap = mapItems.find(id);
	if (itmap != mapItems.end())
	{
		const TagLib::MP4::Item& item = itmap->second;
		if (item.isValid())
		{
			int value = (unsigned short)item.toInt();

			outText = std::to_string(value);

			return true;
		}
	}

	return false;
}

bool TagLibReader::ReadMP4TagItemBool(TagLib::MP4::Tag* tag, char* id, std::string& outText)
{
	const TagLib::MP4::ItemListMap& mapItems = tag->itemMap();
	TagLib::MP4::ItemListMap::ConstIterator itmap = mapItems.find(id);
	if (itmap != mapItems.end())
	{
		const TagLib::MP4::Item& item = itmap->second;
		if (item.isValid())
		{
			if (item.toBool())
				outText = '1';
			else // Do not write false bool (or do?)
				outText = '0';

			return true;
		}
	}

	return false;
}

void TagLibReader::SplitNumberTotal(std::string& inoutNumber, std::string& outTotal)
{
	std::size_t find = inoutNumber.find('/');
	if (find != std::string::npos)
	{
		outTotal = inoutNumber.substr(find + 1);
		inoutNumber = inoutNumber.substr(0, find);
	}
}

void TagLibReader::SplitMultiplies(const std::string& text, std::string& outText, std::vector<std::string>& outArray)
{
	std::size_t start = 0;
	std::size_t find = 0;
	while ((find = text.find(';', find)) != std::string::npos)
	{
		if (start == 0)
			outText = text.substr(start, find - start);
		else
			outArray.push_back(text.substr(start, find - start));
		++find;
		start = find;
	}

	if (start == 0)
		outText = text;
	else
		outArray.push_back(text.substr(start));
}

TagLibReader::File::File(const std::wstring& file, bool openReadOnly, bool readAudioProperties)
{
	fileStreamPtr.reset(new TagLib::FileStream(file.c_str(), openReadOnly));

	if (!fileStreamPtr->isOpen())
		return;

	TagLib::AudioProperties::ReadStyle audioPropertiesStyle = TagLib::AudioProperties::Fast;
	TagLib::IOStream* ioStream = static_cast<TagLib::IOStream*>(fileStreamPtr.get());

	std::wstring ext = PathEx::ExtFromFile(file);

	if (!ext.empty())
	{
		// Similar to createInternal function in taglib fileref.cpp

		if (ext == L"mp3")
			filePtr.reset(new TagLib::MPEG::File(ioStream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle));
		else if (ext == L"ogg")
			filePtr.reset(new TagLib::Ogg::Vorbis::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"oga")
		{
			// .oga can be any audio in the Ogg container. First try FLAC, then Vorbis.
			filePtr.reset(new TagLib::Ogg::FLAC::File(ioStream, readAudioProperties, audioPropertiesStyle));
			if (!filePtr->isValid())
			{
				filePtr.reset(new TagLib::Ogg::Vorbis::File(ioStream, readAudioProperties, audioPropertiesStyle));
			}
		}
		else if (ext == L"flac" || ext == L"fla")
			filePtr.reset(new TagLib::FLAC::File(ioStream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle));
		else if (ext == L"m4a" || ext == L"m4b" || ext == L"m4r" || ext == L"mp4" || ext == L"aac")
			filePtr.reset(new TagLib::MP4::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"wma" || ext == L"asf")
			filePtr.reset(new TagLib::ASF::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"aif" || ext == L"aiff")
			filePtr.reset(new TagLib::RIFF::AIFF::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"wav")
			filePtr.reset(new TagLib::RIFF::WAV::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"ape")
			filePtr.reset(new TagLib::APE::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"mpc")
			filePtr.reset(new TagLib::MPC::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"wv")
			filePtr.reset(new TagLib::WavPack::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"tta")
			filePtr.reset(new TagLib::TrueAudio::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"spx")
			filePtr.reset(new TagLib::Ogg::Speex::File(ioStream, readAudioProperties, audioPropertiesStyle));
		else if (ext == L"opus")
			filePtr.reset(new TagLib::Ogg::Opus::File(ioStream, readAudioProperties, audioPropertiesStyle));
	}

	isFileOpen = filePtr && filePtr->isOpen();

	// If the file is open only to read tags we can close it here
	if (openReadOnly)
		fileStreamPtr.reset();
}

TagLibReader::File::~File()
{

}

