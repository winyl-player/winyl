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
#include "DialogEx.h"
#include "Language.h"
#include "LibAudio.h"
#include "XmlFile.h"

// DlgEqualizer dialog

class DlgEqualizer : public DialogEx
{

public:
	DlgEqualizer();
	virtual ~DlgEqualizer();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetLibAudio(LibAudio* bass) {libAudio = bass;}
	inline void SetProgramPath(const std::wstring& path) {programPath = path;}
	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}

private:
	Language* lang = nullptr;
	LibAudio* libAudio = nullptr;

	std::wstring programPath;
	std::wstring profilePath;

	HFONT fontBar = NULL;
	HFONT fontSmall = NULL;
	HWND presetsBar = NULL;
	HMENU presetsMenu = NULL;

	bool isUserDefined = false;

	int eqElapse;
	int eqSteps;

	int eqValues[10];
	float eqValuesOld[10];
	float eqValuesStep[10];

	int eqPreamp;
	float eqPreampStep;
	float eqPreampOld;

	struct structPreset
	{
		std::wstring name;
		float eqPreamp;
		float eqValues[10];
	};

	std::vector<structPreset> presets;

	void CalculateOld();
	void CalculateStep();
	void LoadPreset(int preset);
	int FromFloat(float f);
	float ToFloat(int i);
	void ApplyEqualizer();
	bool LoadPresets();
	void LoadGain(XmlNode& nNode, char* name, float* f);

	HWND sliderPreamp = NULL;
	HWND sliderEQ0 = NULL;
	HWND sliderEQ1 = NULL;
	HWND sliderEQ2 = NULL;
	HWND sliderEQ3 = NULL;
	HWND sliderEQ4 = NULL;
	HWND sliderEQ5 = NULL;
	HWND sliderEQ6 = NULL;
	HWND sliderEQ7 = NULL;
	HWND sliderEQ8 = NULL;
	HWND sliderEQ9 = NULL;

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnTimer(UINT_PTR nIDEvent);
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnBnClickedCheckEQ();
	void OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
};
