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

// TagLibWriter.cpp : implementation file
//

#include "stdafx.h"
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


// TagLibWriter

TagLibWriter::TagLibWriter()
{
	// The global encoding doesn't work. Do not uncomment! Because TagLib doesn't work with broken Latin1 (we do it manually).
	//TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::Type::UTF8);
}

TagLibWriter::~TagLibWriter()
{

}

bool TagLibWriter::SaveFileTags(const std::wstring& file)
{
	// Do not read AudioProperties it can speed up file reading.
	//TagLib::FileRef f(file.c_str(), false, TagLib::AudioProperties::Fast);
	TagLibReader::File f(file, false, false);

	if (tags.track.second || tags.totalTracks.second ||
		tags.disc.second || tags.totalDiscs.second)
		loader.ReadFileTagsFromTagLibFile(f);

	return SaveFileTagsToTagLibFile(f);
}

bool TagLibWriter::SaveFileTagsToTagLibFile(const TagLibReader::File& f)
{
	if (!f.IsOpen())
		return false;

	// Use dynamic_cast to find out what kind of file and tags.

	if (TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file()))
	{
		int version = 3; // ID3v2 tags version (3 or 4). If tags are empty use 3.

		if (mpeg->ID3v2Tag() && mpeg->hasID3v2Tag())
		{
			// If tags are not empty then use the same tags version.
			version = mpeg->ID3v2Tag()->header()->majorVersion();
			// If tags version is unknown (2 for example) then use 3.
			if (version != 3 && version != 4)
				version = 3;
		}

		// ID3v2 save always
		SaveID3v2Tags(mpeg->ID3v2Tag(true), version);
		int type = TagLib::MPEG::File::ID3v2;

		// ID3v1 save only if it wasn't empty
		if (mpeg->ID3v1Tag() && mpeg->hasID3v1Tag())
		{
			SaveID3v1Tags(mpeg->ID3v1Tag(true));
			type |= TagLib::MPEG::File::ID3v1;
		}

		// APE also save only if it wasn't empty
		if (mpeg->APETag() && mpeg->hasAPETag())
		{
			SaveAPETags(mpeg->APETag(true));
			type |= TagLib::MPEG::File::APE;
		}

		lyrics.SaveLyricsToTagLibFile(f, version);
		cover.SaveCoverToTagLibFile(f);

		return mpeg->save(type, true, version, false);
	}
	else if (TagLib::APE::File* ape = dynamic_cast<TagLib::APE::File*>(f.file()))
	{
		SaveAPETags(ape->APETag(true));
		
		// Remove ID3v1 tags
		ape->strip(TagLib::APE::File::ID3v1);
		
		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);

		return ape->save();
    }
	else if (TagLib::MPC::File* mpc = dynamic_cast<TagLib::MPC::File*>(f.file()))
	{
		SaveAPETags(mpc->APETag(true));
		
		// Remove ID3v1 and ID3v2 tags
		mpc->strip(TagLib::MPC::File::ID3v1|TagLib::MPC::File::ID3v2);
		
		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);

		return mpc->save();
    }
	else if (TagLib::WavPack::File* wavpack = dynamic_cast<TagLib::WavPack::File*>(f.file()))
	{
		SaveAPETags(wavpack->APETag(true));
		
		// Remove ID3v1 tags
		wavpack->strip(TagLib::WavPack::File::ID3v1);

		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);
		
		return wavpack->save();
    }
	if (TagLib::TrueAudio::File* tta = dynamic_cast<TagLib::TrueAudio::File*>(f.file()))
	{
		int version = 3; // ID3v2 tags version (3 or 4). If tags are empty use 3.

		if (tta->ID3v2Tag() && tta->hasID3v2Tag())
		{
			// If tags are not empty then use the same tags version.
			version = tta->ID3v2Tag()->header()->majorVersion();
			// If tags version is unknown (2 for example) then use 3.
			if (version != 3 && version != 4)
				version = 3;
		}

		// ID3v2 always save
		SaveID3v2Tags(tta->ID3v2Tag(true), version);

		// ID3v1 save only if it wasn't empty
		if (tta->ID3v1Tag() && tta->hasID3v1Tag())
			SaveID3v1Tags(tta->ID3v1Tag(true));

		lyrics.SaveLyricsToTagLibFile(f, version);
		cover.SaveCoverToTagLibFile(f);

		return tta->save();
	}
	else if (TagLib::RIFF::File* riff = dynamic_cast<TagLib::RIFF::File*>(f.file()))
	{
		if (TagLib::RIFF::WAV::File* riff_wav = dynamic_cast<TagLib::RIFF::WAV::File*>(riff))
		{
			int version = 3; // ID3v2 tags version (3 or 4). If tags are empty use 3.

			if (riff_wav->ID3v2Tag() && riff_wav->hasID3v2Tag())
			{
				// If tags are not empty then use the same tags version.
				version = riff_wav->ID3v2Tag()->header()->majorVersion();
				// If tags version is unknown (2 for example) then use 3.
				if (version != 3 && version != 4)
					version = 3;
			}

			// ID3v2 always save
			SaveID3v2Tags(riff_wav->ID3v2Tag(), version);

			lyrics.SaveLyricsToTagLibFile(f, version);
			cover.SaveCoverToTagLibFile(f);

			// Do not strip info tag.
			return riff_wav->save(TagLib::RIFF::WAV::File::ID3v2, false, version);
		}
		else if (TagLib::RIFF::AIFF::File* riff_aiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(riff))
		{
			int version = 3; // ID3v2 tags version (3 or 4). If tags are empty use 3.

			if (riff_aiff->tag() && riff_aiff->hasID3v2Tag())
			{
				// If tags is not empty then use the same tags version.
				version = riff_aiff->tag()->header()->majorVersion();
				// If tags version is unknown (2 for example) then use 3.
				if (version != 3 && version != 4)
					version = 3;
			}

			// ID3v2 always save
			SaveID3v2Tags(riff_aiff->tag(), version);

			lyrics.SaveLyricsToTagLibFile(f, version);
			cover.SaveCoverToTagLibFile(f);

			// Do not strip info tag.
			return riff_aiff->save();
		}
	}
    else if(TagLib::FLAC::File* flac = dynamic_cast<TagLib::FLAC::File*>(f.file()))
	{
		SaveOGGTags(flac->xiphComment(true));
		
		// This function is not yet implemented in TagLib
		//flac->strip(TagLib::FLAC::File::ID3v1|TagLib::FLAC::File::ID3v2);

		// because the previous function is not yet implemented, we need to rewrite all ID3v2 and ID3v1 tags if present

		// ID3v2 save only if it wasn't empty
		if (flac->ID3v2Tag() && flac->hasID3v2Tag())
		{
			int version = 3; // ID3v2 tags version (3 or 4). If tags are empty use 3.

			// If tags is not empty then use the same tags version.
			version = flac->ID3v2Tag()->header()->majorVersion();
			// If tags version is unknown (2 for example) then use 3.
			if (version != 3 && version != 4)
				version = 3;

			SaveID3v2Tags(flac->ID3v2Tag(true), version);
		}

		// ID3v1 save only if it wasn't empty
		if (flac->ID3v1Tag() && flac->hasID3v1Tag())
			SaveID3v1Tags(flac->ID3v1Tag(true));

		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);

		return flac->save();
	}
	else if (TagLib::Ogg::File* ogg = dynamic_cast<TagLib::Ogg::File*>(f.file()))
	{
		if (TagLib::Ogg::Vorbis::File* ogg_vorbis = dynamic_cast<TagLib::Ogg::Vorbis::File*>(ogg))
		{
			if (ogg_vorbis->tag())
				SaveOGGTags(ogg_vorbis->tag());

			lyrics.SaveLyricsToTagLibFile(f);
			cover.SaveCoverToTagLibFile(f);

			return ogg_vorbis->save();
		}
		else if (TagLib::Ogg::FLAC::File* ogg_flac = dynamic_cast<TagLib::Ogg::FLAC::File*>(ogg))
		{
			if (ogg_flac->tag())
				SaveOGGTags(ogg_flac->tag());

			lyrics.SaveLyricsToTagLibFile(f);
			cover.SaveCoverToTagLibFile(f);

			return ogg_flac->save();
		}
		else if (TagLib::Ogg::Opus::File* ogg_opus = dynamic_cast<TagLib::Ogg::Opus::File*>(ogg))
		{
			if (ogg_opus->tag())
				SaveOGGTags(ogg_opus->tag());

			lyrics.SaveLyricsToTagLibFile(f);
			cover.SaveCoverToTagLibFile(f);

			return ogg_opus->save();
		}
		else if (TagLib::Ogg::Speex::File* ogg_speex = dynamic_cast<TagLib::Ogg::Speex::File*>(ogg))
		{
			if (ogg_speex->tag())
				SaveOGGTags(ogg_speex->tag());

			lyrics.SaveLyricsToTagLibFile(f);
			cover.SaveCoverToTagLibFile(f);

			return ogg_speex->save();
		}
	}
	else if (TagLib::MP4::File* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file()))
	{
		if (mp4->tag())
			SaveMP4Tags(mp4->tag());
		
		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);

		return mp4->save();
	}
	else if (TagLib::ASF::File* asf = dynamic_cast<TagLib::ASF::File*>(f.file()))
	{
		if (asf->tag())
			SaveASFTags(asf->tag());

		lyrics.SaveLyricsToTagLibFile(f);
		cover.SaveCoverToTagLibFile(f);

		return asf->save();
	}

	return false;
}

void TagLibWriter::SaveID3v2Tags(TagLib::ID3v2::Tag* tag, int version)
{
	// Save all the data to frames
	if (tags.title.second)
		SaveID3v2TagFrameText(tag, version, "TIT2", tags.title.first);
	if (tags.album.second)
		SaveID3v2TagFrameText(tag, version, "TALB", tags.album.first);
	if (tags.artist.second)
		SaveID3v2TagFrameText(tag, version, "TPE1", tags.artist.first, &tags.artists);
	if (tags.albumArtist.second)
		SaveID3v2TagFrameText(tag, version, "TPE2", tags.albumArtist.first, &tags.albumArtists);
	if (tags.genre.second)
		SaveID3v2TagFrameText(tag, version, "TCON", tags.genre.first, &tags.genres);
	if (tags.composer.second)
		SaveID3v2TagFrameText(tag, version, "TCOM", tags.composer.first, &tags.composers);
	if (tags.publisher.second)
		SaveID3v2TagFrameText(tag, version, "TPUB", tags.publisher.first);
	if (tags.conductor.second)
		SaveID3v2TagFrameText(tag, version, "TPE3", tags.conductor.first, &tags.conductors);
	if (tags.lyricist.second)
		SaveID3v2TagFrameText(tag, version, "TEXT", tags.lyricist.first, &tags.lyricists);
	if (tags.grouping.second)
		SaveID3v2TagFrameText(tag, version, "TIT1", tags.grouping.first);
	if (tags.subtitle.second)
		SaveID3v2TagFrameText(tag, version, "TIT3", tags.subtitle.first);
	if (tags.copyright.second)
		SaveID3v2TagFrameText(tag, version, "TCOP", tags.copyright.first);
	if (tags.encodedby.second)
		SaveID3v2TagFrameText(tag, version, "TENC", tags.encodedby.first);
	if (tags.remixer.second)
		SaveID3v2TagFrameText(tag, version, "TPE4", tags.remixer.first);
	// Use different frame type for comment
	if (tags.comment.second)
		SaveID3v2TagFrameComment(tag, version, "COMM", tags.comment.first);

	//if (tags.mood.second)
	//{
	//	if (version == 4)
	//		SaveID3v2TagFrameText(tag, version, "TMOO", tags.composer.first);
	//	else
	//		SaveID3v2TagFrameTxxx(tag, version, "MOOD", tags.mood.first);
	//}

	if (tags.bpm.second)
		SaveID3v2TagFrameText(tag, version, "TBPM", tags.bpm.first);
	if (tags.year.second)
		SaveID3v2TagFrameText(tag, version, "TDRC", tags.year.first);

	if (tags.track.second || tags.totalTracks.second)
	{
		std::wstring track = MixNumberTotal(loader.tags.track, loader.tags.totalTracks, tags.track, tags.totalTracks);
		SaveID3v2TagFrameText(tag, version, "TRCK", track);
	}
	if (tags.disc.second || tags.totalDiscs.second)
	{
		std::wstring disc = MixNumberTotal(loader.tags.disc, loader.tags.totalDiscs, tags.disc, tags.totalDiscs);
		SaveID3v2TagFrameText(tag, version, "TPOS", disc);
	}

	// Fix the encoding of all frames if needed.
	FixID3v2Encoding(tag, version);
}

void TagLibWriter::SaveID3v2TagFrameText(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text, std::vector<std::wstring>* array)
{
	if (text.empty())
		tag->removeFrames(id);
	else
	{
		TagLib::String::Type encoding = TagLib::String::UTF8;
		if (version == 3)
			encoding = TagLib::String::UTF16;

		const TagLib::ID3v2::FrameList& listFrames = tag->frameListMap()[id];
		if (!listFrames.isEmpty()) // If the frame is not empty, just rewrite it
		{
			TagLib::ID3v2::TextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(listFrames.front());
			if (frame)
			{
				frame->setTextEncoding(encoding);

				TagLib::StringList listFields;
				listFields.append(text);
				if (array)
				{
					for (std::size_t i = 0, size = array->size(); i < size; ++i)
						listFields.append((*array)[i]);
				}

				frame->setText(listFields);
			}
		}
		else // Or create a new frame
		{
			TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame(id, encoding);
			if (frame)
			{
				tag->addFrame(frame);

				TagLib::StringList listFields;
				listFields.append(text);
				if (array)
				{
					for (std::size_t i = 0, size = array->size(); i < size; ++i)
						listFields.append((*array)[i]);
				}

				frame->setText(listFields);
			}
		}
	}
}

void TagLibWriter::SaveID3v2TagFrameComment(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text)
{
	if (text.empty())
		tag->removeFrames(id);
	else
	{
		TagLib::String::Type encoding = TagLib::String::UTF8;
		if (version == 3)
			encoding = TagLib::String::UTF16;

		const TagLib::ID3v2::FrameList& listFrames = tag->frameListMap()[id];
		if (!listFrames.isEmpty()) // If the frame is not empty, just rewrite it
		{
			TagLib::ID3v2::CommentsFrame* frame = dynamic_cast<TagLib::ID3v2::CommentsFrame*>(listFrames.front());
			if (frame)
			{
				frame->setTextEncoding(encoding);
				// If we don't set a language, iTunes won't see the comment frame.
				frame->setLanguage("eng"); // language always in Latin1
				frame->setText(text);
			}
		}
		else // Or create a new frame
		{
			TagLib::ID3v2::CommentsFrame* frame = new TagLib::ID3v2::CommentsFrame(encoding);
			if (frame)
			{
				tag->addFrame(frame);
				// If we don't set a language, iTunes won't see the comment frame.
				frame->setLanguage("eng"); // language always in Latin1
				frame->setText(text);
			}
		}
	}
}

void TagLibWriter::SaveID3v2TagFrameLyrics(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text)
{
	if (text.empty())
		tag->removeFrames(id);
	else
	{
		TagLib::String::Type encoding = TagLib::String::UTF8;
		if (version == 3)
			encoding = TagLib::String::UTF16;

		const TagLib::ID3v2::FrameList& listFrames = tag->frameListMap()[id];
		if (!listFrames.isEmpty()) // If the frame is not empty, just rewrite it
		{
			TagLib::ID3v2::UnsynchronizedLyricsFrame* frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(listFrames.front());
			if (frame)
			{
				frame->setTextEncoding(encoding);
				frame->setLanguage("eng"); // language always in Latin1
				frame->setText(text);
			}
		}
		else // Or create a new frame
		{
			TagLib::ID3v2::UnsynchronizedLyricsFrame* frame = new TagLib::ID3v2::UnsynchronizedLyricsFrame(encoding);
			if (frame)
			{
				tag->addFrame(frame);
				frame->setLanguage("eng"); // language always in Latin1
				frame->setText(text);
			}
		}
	}
}
/*
void TagLibWriter::SaveID3v2TagFrameTxxx(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text)
{
	TagLib::String description = id;

	if (text.empty())
	{
		// Same as taglib sources id3v2tag.cpp -> ID3v2::Tag::removeFrames(const ByteVector &id)
		// but with checking TXXX description
		const TagLib::ID3v2::FrameList& listFrames = tag->frameListMap()["TXXX"];
		for (TagLib::ID3v2::FrameList::Iterator it = listFrames.begin(); it != listFrames.end(); ++it)
		{
			TagLib::ID3v2::UserTextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(*it);
			if (frame && frame->description() == description)
				tag->removeFrame(*it, true);
		}
	}
	else
	{
		TagLib::String::Type encoding = TagLib::String::UTF8;
		if (version == 3)
			encoding = TagLib::String::UTF16;

		bool found = false;

		const TagLib::ID3v2::FrameList& listFrames = tag->frameListMap()["TXXX"];
		for (int i = 0, size = (int)listFrames.size(); i < size; ++i)
		{
			TagLib::ID3v2::UserTextIdentificationFrame* frame = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(listFrames[i]);
			if (frame && frame->description() == description)
			{
				frame->setTextEncoding(encoding);
				frame->setText(text);

				found = true;
				break;
			}
		}

		if (!found)
		{
			TagLib::ID3v2::UserTextIdentificationFrame* frame = new TagLib::ID3v2::UserTextIdentificationFrame(description, TagLib::String(text), encoding);
			if (frame)
			{
				tag->addFrame(frame);
			}
		}
	}
}
*/
void TagLibWriter::FixID3v2Encoding(TagLib::ID3v2::Tag* tag, int version)
{
	// Function to fix frames encoding
	// Fix broken Latin1 (ANSI saved in Latin1 tags)
	// Also changes encoding of all known frames to UTF8 if ID3v2.4 or UTF16 if ID3v2.3

	// Also we need to patch TagLib to always save UTF16 if ID3v2.3
	// In file id3v2tag.cpp in function downgradeFrames need to replace all String::Latin1 to String::UTF16 (5 replacements)

	TagLib::String::Type encoding = TagLib::String::UTF8;
	if (version == 3)
		encoding = TagLib::String::UTF16;

	const TagLib::ID3v2::FrameList& listFrames = tag->frameList();

	for (auto itframe = listFrames.begin(), endframe = listFrames.end(); itframe != endframe; ++itframe)
	{
		// UserTextIdentificationFrame must be first because it inherited from TextIdentificationFrame that is below
		// User text identification frame (TXXX)
		if (TagLib::ID3v2::UserTextIdentificationFrame* frameTxxx = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(*itframe))
		{
			// Do not change encoding of this frame, some programs use it to save own data
			continue;
			//if (frameTxxx->textEncoding() == TagLib::String::Latin1)
			//{
			//	frameTxxx->setTextEncoding(encoding);
			//	// Do not do this, description is the first field in the field list
			//	//frameTxxx->setDescription(UTF::UTF16AS(frameTxxx->description().to8Bit()));
			//	TagLib::StringList listFields = frameTxxx->fieldList();
			//	for (auto it = listFields.begin(), end = listFields.end(); it != end; ++it)
			//		*it = UTF::UTF16AS(it->to8Bit());
			//	frameText->setText(listFields);
			//}
		}
		// All other text frames (all started with T: TALB, TPE1 etc.)
		else if (TagLib::ID3v2::TextIdentificationFrame* frameText = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(*itframe))
		{
			if (frameText->textEncoding() == TagLib::String::Latin1)
			{
				frameText->setTextEncoding(encoding);
				TagLib::StringList listFields = frameText->fieldList();
				for (auto it = listFields.begin(), end = listFields.end(); it != end; ++it)
					*it = UTF::UTF16AS(it->to8Bit());
				frameText->setText(listFields);
			}
		}
		// Comments frame (COMM)
		else if (TagLib::ID3v2::CommentsFrame* frameComm = dynamic_cast<TagLib::ID3v2::CommentsFrame*>(*itframe))
		{
			if (frameComm->textEncoding() == TagLib::String::Latin1)
			{
				frameComm->setTextEncoding(encoding);
				frameComm->setDescription(UTF::UTF16AS(frameComm->description().to8Bit()));
				frameComm->setText(UTF::UTF16AS(frameComm->text().to8Bit()));
				// Do not setLanguage! Because Language must be always in Latin1.
			}
		}
		// Cover art (APIC) (fix description encoding)
		else if (TagLib::ID3v2::AttachedPictureFrame* frameApic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*itframe))
		{
			// Do not change encoding for APIC description or iTunes does not display cover art
			continue;
			//if (frameApic->textEncoding() == TagLib::String::Latin1)
			//{
			//	frameApic->setTextEncoding(encoding);
			//	frameApic->setDescription(UTF::UTF16AS(frameApic->description().to8Bit()));
			//}
		}
		// Lyrics frame (USLT)
		else if (TagLib::ID3v2::UnsynchronizedLyricsFrame* frameLyrc = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(*itframe))
		{
			if (frameLyrc->textEncoding() == TagLib::String::Latin1)
			{
				frameLyrc->setTextEncoding(encoding);
				frameLyrc->setDescription(UTF::UTF16AS(frameLyrc->description().to8Bit()));
				frameLyrc->setText(UTF::UTF16AS(frameLyrc->text().to8Bit()));
			}
		}
		// Do not change encoding of these frames, first again contains the user data, and I don't know nothing adout second
		//if (TagLib::ID3v2::UserUrlLinkFrame* frameLink = dynamic_cast<TagLib::ID3v2::UserUrlLinkFrame*>(*itframe))
		//{
		//	if (frameLink->textEncoding() == TagLib::String::Latin1)
		//	{
		//		frameLink->setTextEncoding(encoding);
		//		frameLink->setDescription(UTF::UTF16AS(frameLink->description().to8Bit()));
		//	}	
		//}
		//if (TagLib::ID3v2::GeneralEncapsulatedObjectFrame* frameGeob = dynamic_cast<TagLib::ID3v2::GeneralEncapsulatedObjectFrame*>(*itframe))
		//{
		//	if (frameGeob->textEncoding() == TagLib::String::Latin1)
		//	{
		//		frameGeob->setTextEncoding(encoding);
		//		frameGeob->setDescription(UTF::UTF16AS(frameGeob->description().to8Bit()));
		//		frameGeob->setFileName(UTF::UTF16AS(frameGeob->fileName().to8Bit()));
		//	}
		//}
	}
}

void TagLibWriter::SaveID3v1Tags(TagLib::ID3v1::Tag* tag)
{
	// Just use ANSI encoding for ID3v1 (all other programs do this).
	if (tags.title.second)
		tag->setTitle(UTF::ANSI16S(tags.title.first));
	if (tags.album.second)
		tag->setAlbum(UTF::ANSI16S(tags.album.first));
	if (tags.artist.second)
		tag->setArtist(UTF::ANSI16S(tags.artist.first));
	if (tags.comment.second)
		tag->setComment(UTF::ANSI16S(tags.comment.first));
	if (tags.genre.second)
		tag->setGenreNumber(TagLib::ID3v1::genreIndex(tags.genre.first));
	if (tags.year.second)
		tag->setYear(_wtoi(tags.year.first.c_str()));
	if (tags.track.second)
		tag->setTrack(_wtoi(tags.track.first.c_str()));
}

void TagLibWriter::SaveAPETags(TagLib::APE::Tag* tag)
{
	if (tags.title.second)
		SaveAPETagItem(tag, "Title", tags.title.first);
	if (tags.album.second)
		SaveAPETagItem(tag, "Album", tags.album.first);
	if (tags.artist.second)
		SaveAPETagItem(tag, "Artist", tags.artist.first, &tags.artists);
	if (tags.albumArtist.second)
		SaveAPETagItem(tag, "Album Artist", tags.albumArtist.first, &tags.albumArtists);
	if (tags.genre.second)
		SaveAPETagItem(tag, "Genre", tags.genre.first, &tags.genres);
	if (tags.composer.second)
		SaveAPETagItem(tag, "Composer", tags.composer.first, &tags.composers);
	if (tags.publisher.second)
		SaveAPETagItem(tag, "Label", tags.publisher.first);
	if (tags.conductor.second)
		SaveAPETagItem(tag, "Conductor", tags.conductor.first, &tags.conductors);
	if (tags.lyricist.second)
		SaveAPETagItem(tag, "Lyricist", tags.lyricist.first, &tags.lyricists);
	if (tags.grouping.second)
		SaveAPETagItem(tag, "Grouping", tags.grouping.first);
	if (tags.subtitle.second)
		SaveAPETagItem(tag, "Subtitle", tags.subtitle.first);
	if (tags.copyright.second)
		SaveAPETagItem(tag, "Copyright", tags.copyright.first);
	if (tags.encodedby.second)
		SaveAPETagItem(tag, "EncodedBy", tags.encodedby.first);
	if (tags.remixer.second)
		SaveAPETagItem(tag, "MixArtist", tags.remixer.first);
	if (tags.comment.second)
		SaveAPETagItem(tag, "Comment", tags.comment.first);
	//if (tags.mood.second)
	//	SaveAPETagItem(tag, "Mood", tags.mood.first);

	if (tags.bpm.second)
		SaveAPETagItem(tag, "BPM", tags.bpm.first);
	if (tags.year.second)
		SaveAPETagItem(tag, "Year", tags.year.first);

	if (tags.track.second || tags.totalTracks.second)
	{
		std::wstring track = MixNumberTotal(loader.tags.track, loader.tags.totalTracks, tags.track, tags.totalTracks);
		SaveAPETagItem(tag, "Track", track);
	}
	if (tags.disc.second || tags.totalDiscs.second)
	{
		std::wstring disc = MixNumberTotal(loader.tags.disc, loader.tags.totalDiscs, tags.disc, tags.totalDiscs);
		SaveAPETagItem(tag, "Disc", disc);
	}
}

void TagLibWriter::SaveAPETagItem(TagLib::APE::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array)
{
	tag->removeItem(id);

	if (text.empty())
		return;

	TagLib::APE::Item item;
	item.setKey(id);
	item.setType(TagLib::APE::Item::Text);

	TagLib::StringList listValues;
	listValues.append(text);
	if (array)
	{
		for (std::size_t i = 0, size = array->size(); i < size; ++i)
			listValues.append((*array)[i]);
	}
	item.setValues(listValues);

	tag->setItem(item.key(), item);
}

void TagLibWriter::SaveOGGTags(TagLib::Ogg::XiphComment* tag)
{
	// TOTALDISCS
	// TOTALTRACKS

	if (tags.title.second)
		SaveOGGTagComment(tag, "TITLE", tags.title.first);
	if (tags.album.second)
		SaveOGGTagComment(tag, "ALBUM", tags.album.first);
	if (tags.artist.second)
		SaveOGGTagComment(tag, "ARTIST", tags.artist.first, &tags.artists);
	if (tags.albumArtist.second)
		SaveOGGTagComment(tag, "ALBUMARTIST", tags.albumArtist.first, &tags.albumArtists);
	if (tags.genre.second)
		SaveOGGTagComment(tag, "GENRE", tags.genre.first, &tags.genres);
	if (tags.composer.second)
		SaveOGGTagComment(tag, "COMPOSER", tags.composer.first, &tags.composers);
	if (tags.publisher.second)
		SaveOGGTagComment(tag, "LABEL", tags.publisher.first);
	if (tags.conductor.second)
		SaveOGGTagComment(tag, "CONDUCTOR", tags.conductor.first, &tags.conductors);
	if (tags.lyricist.second)
		SaveOGGTagComment(tag, "LYRICIST", tags.lyricist.first, &tags.lyricists);
	if (tags.grouping.second)
		SaveOGGTagComment(tag, "GROUPING", tags.grouping.first);
	if (tags.subtitle.second)
		SaveOGGTagComment(tag, "SUBTITLE", tags.subtitle.first);
	if (tags.copyright.second)
		SaveOGGTagComment(tag, "COPYRIGHT", tags.copyright.first);
	if (tags.encodedby.second)
		SaveOGGTagComment(tag, "ENCODEDBY", tags.encodedby.first);
	if (tags.remixer.second)
		SaveOGGTagComment(tag, "REMIXER", tags.remixer.first);
	if (tags.comment.second)
		SaveOGGTagComment(tag, "COMMENT", tags.comment.first);
	//if (tags.mood.second)
	//	SaveOGGTagComment(tag, "MOOD", tags.mood.first);

	if (tags.bpm.second)
		SaveOGGTagComment(tag, "BPM", tags.bpm.first);
	if (tags.year.second)
		SaveOGGTagComment(tag, "DATE", tags.year.first);

	if (tags.track.second || tags.totalTracks.second)
	{
		std::pair<std::wstring, std::wstring> track = MixNumberTotal2(loader.tags.track, loader.tags.totalTracks, tags.track, tags.totalTracks);
		SaveOGGTagComment(tag, "TRACKNUMBER", track.first);
		SaveOGGTagComment(tag, "TRACKTOTAL", track.second);
	}
	if (tags.disc.second || tags.totalDiscs.second)
	{
		std::pair<std::wstring, std::wstring> disc = MixNumberTotal2(loader.tags.disc, loader.tags.totalDiscs, tags.disc, tags.totalDiscs);
		SaveOGGTagComment(tag, "DISCNUMBER", disc.first);
		SaveOGGTagComment(tag, "DISCTOTAL", disc.second);
	}
}

void TagLibWriter::SaveOGGTagComment(TagLib::Ogg::XiphComment* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array)
{
	tag->removeFields(id);

	if (text.empty())
		return;

	tag->addField(id, text, false);
	if (array)
	{
		for (std::size_t i = 0, size = array->size(); i < size; ++i)
			tag->addField(id, (*array)[i], false);
	}
}

void TagLibWriter::SaveASFTags(TagLib::ASF::Tag* tag)
{
	// Some tags in ASF are not saved in attributes (title, artist, copyright, comment, rating)
	// so first we read these tags separately, and then attributes.
	if (tags.title.second)
		tag->setTitle(tags.title.first);
	if (tags.artist.second)
	{
		tag->setArtist(tags.artist.first);
		SaveASFTagAttributeArray(tag, "Author", tags.artists);
	}
	if (tags.comment.second)
		tag->setComment(tags.comment.first);
	if (tags.copyright.second)
		tag->setCopyright(tags.copyright.first);

	if (tags.album.second)
		SaveASFTagAttribute(tag, "WM/AlbumTitle", tags.album.first);
	if (tags.albumArtist.second)
		SaveASFTagAttribute(tag, "WM/AlbumArtist", tags.albumArtist.first, &tags.albumArtists);
	if (tags.genre.second)
		SaveASFTagAttribute(tag, "WM/Genre", tags.genre.first, &tags.genres);
	if (tags.composer.second)
		SaveASFTagAttribute(tag, "WM/Composer", tags.composer.first, &tags.composers);
	if (tags.publisher.second)
		SaveASFTagAttribute(tag, "WM/Publisher", tags.publisher.first);
	if (tags.conductor.second)
		SaveASFTagAttribute(tag, "WM/Conductor", tags.conductor.first, &tags.conductors);
	if (tags.lyricist.second)
		SaveASFTagAttribute(tag, "WM/Writer", tags.lyricist.first, &tags.lyricists);
	if (tags.grouping.second)
		SaveASFTagAttribute(tag, "WM/ContentGroupDescription", tags.grouping.first);
	if (tags.subtitle.second)
		SaveASFTagAttribute(tag, "WM/SubTitle", tags.subtitle.first);
	if (tags.encodedby.second)
		SaveASFTagAttribute(tag, "WM/EncodedBy", tags.encodedby.first);
	if (tags.remixer.second)
		SaveASFTagAttribute(tag, "WM/ModifiedBy", tags.remixer.first);
	//if (tags.mood.second)
	//	SaveASFTagAttribute(tag, "WM/Mood", tags.mood.first);

	if (tags.bpm.second)
		SaveASFTagAttribute(tag, "WM/BeatsPerMinute", tags.bpm.first);
	if (tags.year.second)
		SaveASFTagAttribute(tag, "WM/Year", tags.year.first);

	// I'm not sure we can save track count across slash (1/10) but many tags editors do this.
	// And PartOfSet we certainly can, it's in the standard:
	// http://msdn.microsoft.com/en-us/library/ms867702#wm_metadata_usage_partofset
	if (tags.track.second || tags.totalTracks.second)
	{
		std::wstring track = MixNumberTotal(loader.tags.track, loader.tags.totalTracks, tags.track, tags.totalTracks);
		SaveASFTagAttribute(tag, "WM/TrackNumber", track);
	}
	if (tags.disc.second || tags.totalDiscs.second)
	{
		std::wstring disc = MixNumberTotal(loader.tags.disc, loader.tags.totalDiscs, tags.disc, tags.totalDiscs);
		SaveASFTagAttribute(tag, "WM/PartOfSet", disc);
	}
}

void TagLibWriter::SaveASFTagAttribute(TagLib::ASF::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array)
{
	tag->removeItem(id);

	if (text.empty())
		return;

	tag->addAttribute(id, TagLib::ASF::Attribute(text));
	if (array)
	{
		// TagLib has a bug when multiple attributes with same id (incorrect order)
		for (std::size_t i = 0, size = array->size(); i < size; ++i)
			tag->addAttribute(id, TagLib::ASF::Attribute((*array)[i]));
	}
}

void TagLibWriter::SaveASFTagAttributeArray(TagLib::ASF::Tag* tag, char* id, std::vector<std::wstring>& array)
{
	tag->removeItem(id);

	if (array.empty())
		return;

	// TagLib has a bug when multiple attributes with same id (incorrect order)
	for (std::size_t i = 0, size = array.size(); i < size; ++i)
		tag->addAttribute(id, TagLib::ASF::Attribute(array[i]));
}

void TagLibWriter::SaveMP4Tags(TagLib::MP4::Tag* tag)
{
	// "\251" it's copyright symbol, in MP4 some keys started with it
	// http://atomicparsley.sourceforge.net/mpeg-4files.html

	if (tags.title.second)
		SaveMP4TagItem(tag, "\251nam", tags.title.first);
	if (tags.album.second)
		SaveMP4TagItem(tag, "\251alb", tags.album.first);
	if (tags.artist.second)
		SaveMP4TagItem(tag, "\251ART", tags.artist.first, &tags.artists);
	if (tags.albumArtist.second)
		SaveMP4TagItem(tag, "aART", tags.albumArtist.first, &tags.albumArtists);
	if (tags.genre.second)
		SaveMP4TagItem(tag, "\251gen", tags.genre.first, &tags.genres);
	if (tags.composer.second)
		SaveMP4TagItem(tag, "\251wrt", tags.composer.first, &tags.composers);
	if (tags.publisher.second)
		SaveMP4TagItem(tag, "----:com.apple.iTunes:LABEL", tags.publisher.first); // labl
	if (tags.conductor.second)
		SaveMP4TagItem(tag, "----:com.apple.iTunes:CONDUCTOR", tags.conductor.first, &tags.conductors); // cond
	if (tags.lyricist.second)
		SaveMP4TagItem(tag, "----:com.apple.iTunes:LYRICIST", tags.lyricist.first, &tags.lyricists); // lyrt
	if (tags.grouping.second)
		SaveMP4TagItem(tag, "\251grp", tags.grouping.first);
	if (tags.subtitle.second)
		SaveMP4TagItem(tag, "----:com.apple.iTunes:SUBTITLE", tags.subtitle.first); // desc (MP3Tag), subt
	if (tags.copyright.second)
		SaveMP4TagItem(tag, "cprt", tags.copyright.first);
	if (tags.encodedby.second)
		SaveMP4TagItem(tag, "\251too", tags.encodedby.first);
	if (tags.remixer.second)
		SaveMP4TagItem(tag, "----:com.apple.iTunes:REMIXER", tags.remixer.first); // rmix
	if (tags.comment.second)
		SaveMP4TagItem(tag, "\251cmt", tags.comment.first);
	//if (tags.mood.second)
	//	SaveMP4TagItem(tag, "----:com.apple.iTunes:MOOD", tags.mood.first); // mood

	if (tags.bpm.second)
		SaveMP4TagItemInt(tag, "tmpo", tags.bpm.first);
	if (tags.year.second)
		SaveMP4TagItem(tag, "\251day", tags.year.first);

	// Use different function for digits (track/disk number)
	if (tags.track.second || tags.totalTracks.second)
	{
		std::pair<std::wstring, std::wstring> track = MixNumberTotal2(loader.tags.track, loader.tags.totalTracks, tags.track, tags.totalTracks);
		SaveMP4TagItemIntPair(tag, "trkn", track.first, track.second);
	}
	if (tags.disc.second || tags.totalDiscs.second)
	{
		std::pair<std::wstring, std::wstring> disc = MixNumberTotal2(loader.tags.disc, loader.tags.totalDiscs, tags.disc, tags.totalDiscs);
		SaveMP4TagItemIntPair(tag, "disk", disc.first, disc.second);
	}
}

void TagLibWriter::SaveMP4TagItem(TagLib::MP4::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array)
{
	tag->removeItem(id);

	if (text.empty())
		return;

	tag->setItem(id, TagLib::MP4::Item(TagLib::StringList(text)));

	// TagLib does not support multiple atoms with the same id but many apps use it,
	// the following code supports multiple string in one atom, but it useless for us.

	// Test
	//TagLib::StringList listValues;
	//listValues.append(text);
	//if (array)
	//{
	//	for (std::size_t i = 0, size = array->size(); i < size; ++i)
	//		listValues.append((*array)[i]);
	//}
	//tag->setItem("covr", TagLib::MP4::Item(listValues));
}

void TagLibWriter::SaveMP4TagItemInt(TagLib::MP4::Tag* tag, char* id, const std::wstring& text)
{
	tag->removeItem(id);

	if (text.empty())
		return;

	tag->setItem(id, TagLib::MP4::Item(_wtoi(text.c_str())));
}

void TagLibWriter::SaveMP4TagItemIntPair(TagLib::MP4::Tag* tag, char* id, const std::wstring& text1, const std::wstring& text2)
{
	tag->removeItem(id);

	if (text1.empty())
		return;

	tag->setItem(id, TagLib::MP4::Item(_wtoi(text1.c_str()), _wtoi(text2.c_str())));
}

std::wstring TagLibWriter::MergeNumberTotal(const std::wstring& inNumber, const std::wstring& inTotal)
{
	std::wstring result = inNumber;

	if (!result.empty() &&!inTotal.empty())
	{
		result.push_back('/');
		result += inTotal;
	}

	return result;
}

std::wstring TagLibWriter::MixNumberTotal(const std::string& oldNumber, const std::string& oldTotal,
	const std::pair<std::wstring, bool>& number, std::pair<std::wstring, bool>& total)
{
	std::wstring newNumber = UTF::UTF16S(oldNumber);
	std::wstring newTotal = UTF::UTF16S(oldTotal);

	if (number.second)
		newNumber = number.first;

	if (total.second)
		newTotal = total.first;

	return MergeNumberTotal(newNumber, newTotal);
}

std::pair<std::wstring, std::wstring> TagLibWriter::MixNumberTotal2(const std::string& oldNumber, const std::string& oldTotal,
	const std::pair<std::wstring, bool>& number, std::pair<std::wstring, bool>& total)
{
	std::wstring newNumber = UTF::UTF16S(oldNumber);
	std::wstring newTotal = UTF::UTF16S(oldTotal);

	if (number.second)
		newNumber = number.first;

	if (total.second)
		newTotal = total.first;

	return std::make_pair(newNumber, newTotal);
}
