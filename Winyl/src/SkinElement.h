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

// SkinElement

#include "UTF.h"
#include "XmlFile.h"
#include "ExImage.h"
#include "SkinTrigger.h"

//Forward declarations
class ToolTips;

class SkinElement
{

public:
	SkinElement();
	virtual ~SkinElement();

	enum class Type
	{
		None           = 0,
		Play           = 1,   // Play button
		PlayPause      = 2,   // Play/pause button
		Pause          = 3,   // Pause button
		Stop           = 4,   // Stop button
		Next           = 5,   // Next track button
		Prev           = 6,   // Previous track button
		Volume         = 7,   // Volume slider
		Track          = 8,   // Track slider
		Mute           = 9,   // Mute button
		Close          = 10,  // Close button
		Minimize       = 11,  // Minimize button
		Library        = 12,  // Library window
		Playlist       = 13,  // Playlist window
		Button         = 14,  // Button with trigger
		Cover          = 15,  // Cover art
		Spectrum       = 16,  // Visualizer window
		Rating         = 17,  // Rating
		Title          = 18,  // Title text line
		Artist         = 19,  // Artist text line
		Repeat         = 20,  // Repeat switch
		Shuffle        = 21,  // Shuffle switch
		Album          = 22,  // Album text line
		Equalizer      = 23,  // Equalizer button
		Search         = 24,  // Search window
		SearchMenu     = 25,  // Search menu button
		SearchClear    = 26,  // Search clear button
		Maximize       = 27,  // Maximize button
		MainMenu       = 28,  // Main menu button
		Switch         = 29,  // Switch with trigger (deprecated)
		MiniPlayer     = 30,  // Mini player button
		TimeLength     = 31,  // Time lenght text line
		TimeElapsed    = 32,  // Time elapsed text line
		TimeRemains    = 33,  // Time remains text line
		TimeElapsed2   = 34,  // Time elapsed 2 text line
		TimeRemains2   = 35,  // Time remains 2 text line
		Splitter       = 36,  // Splitter
		Year           = 37,  // Year text line
		ArtistTitle    = 38,  // Artist - Title text line
		StatusLine     = 39,  // Status text line
		StatusLine2    = 40,  // Status short text line
		Genre          = 41,  // Genre text line
		Lyrics         = 42   // Lyrics window
	};

	bool isPress = false; // Element is pressed?
	bool isHover = false; // Mouse over element?

	bool isHidden = false; // Visibility state of element, used for drawing
	bool isHiddenReal = false; // Real visibility state of element (regardless of visibility of layout and element)
	bool isHiddenLayout = false; // Visibility state of layout where element is located

	bool isFadeEffect = false; // Animation is enabled?

	SkinTrigger skinTrigger; // Trigger for Button

	Type type = Type::None; // Element type
	HWND window = NULL; // Element window
	ToolTips* toolTip = nullptr; // Element tooltip

	ZipFile* zipFile = nullptr;

	int fadeAlpha = 0;
	struct FadeValue
	{
		static const int Step = /*35*/25; // Animation step for cover art
		static const int StepIn = 51; // Animation step for button (fade in)
		static const int StepOut = 17; // Animation step for button (fade out)
	};

	CRect rcRect = {0, 0, 0, 0};
	std::wstring skinName;

	virtual bool LoadSkin(const std::wstring& file, ZipFile* zipFile); // Load element skin
	virtual void Draw(HDC dc, bool isAlpha); // Draw element
	virtual bool DrawFade(HDC dc, bool isAlpha); // Draw one frame of animation

	virtual void SetRect(CRect& rcDraw); // Set new position for element
	inline CRect& GetRect() {return rcRect;} // Get current position of element

	virtual int GetWidth(); // Element width
	virtual int GetHeight(); // Element height

	// Return element when something changed else return nullptr
	// For example OnMouseMove of Button element return element when
	// mouse enters element area or leaves it (isHover flag changed)
	virtual SkinElement* OnMouseMove(unsigned flags, CPoint& point);
	virtual SkinElement* OnButtonDown(unsigned flags, CPoint& point);
	virtual SkinElement* OnButtonUp(unsigned flags, CPoint& point);
	virtual SkinElement* OnMouseLeave();

	virtual bool IsRedrawHover(); // Need to redraw when mouse over element?
	virtual bool IsRedrawPress(); // Need to redraw when element is pressed?

	// Check element states
	inline bool IsPress() {return isPress;} // Element is pressed
	inline bool IsHover() {return isHover;} // Mouse over element
	inline bool IsHidden() {return isHidden;} // Element is hidden

	inline bool IsFadeEffect() {return isFadeEffect;}
//	inline void SetHide(bool hide) {isHide = hide;}
	inline bool IsHiddenLayout() {return isHiddenLayout;}
	inline void HideReal(bool isHide)
	{
		isHiddenReal = isHide;
		if (!isHiddenLayout)
			isHidden = isHiddenReal;
	}
	inline void HideLayout(bool isHide)
	{
		isHiddenLayout = isHide;
		if (!isHiddenLayout)
			isHidden = isHiddenReal;
		else
			isHidden = true;
	}


	inline Type GetType() {return type;} // Get element type

	virtual int GetParam();
	virtual void SetParam(int param);

	inline void SetWindow(HWND wnd) {window = wnd;}
	
	// Next line with commented part should be added to class where tooltip is needed
	virtual bool SetToolTip(ToolTips* tips) {toolTip = nullptr; return false;}// override {toolTip = tips; return true;}
};


