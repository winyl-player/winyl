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

#include "XmlFile.h"
#include "UTF.h"
#include "Threading.h"
#include "bass/bass.h"
//#include "bass/tags.h"
#include "bass/bass_fx.h"
#include "bass/bassmix.h"
#include "bass/basswasapi.h"
#include "bass/bassasio.h"

// Forward declaration
class WinylWnd;

class LibAudio
{

public:
	LibAudio();
	virtual ~LibAudio();

	enum class Error
	{
		None = 0,
		Device = 1,
		File = 2,
		Inet = 3
	};

	struct Buffer
	{
		static int DirectSound; // Buffer size for DirectSound in ms
		static double WasapiAsio; // Reading buffer size for WASAPI/ASIO in sec
		static int BassRead; // Bass reading buffer in bytes, default is 65536
	};

	struct FadeTime
	{
		static int Pause;
		static int Stop;
		static int Pos;
		static int Mute;
	};

	inline void SetProgramPath(const std::wstring& path) {programPath = path;}
	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}

	bool Init(WinylWnd* wnd, int driver, int device, bool isBit32, bool isSoftMix, bool isLoadEq);
	void Free();
	bool GetDeviceName(int device, std::wstring& deviceName);
	bool GetDeviceNameASIO(int device, std::wstring& deviceName);
	bool GetDeviceNameWASAPI(int device, std::wstring& deviceName);
	bool GetChannelNameASIO(int channel, std::wstring& channelName);

	void SetUserAgent(const std::string& agent);

	void Stop(); // Stop playback
	void Pause(); // Pause playback
	void Play(); // Resume playback

	Error PlayFile(const std::wstring& file, long long cue);
	void PlayURL(const std::wstring& url, bool isReconnect = false);

	bool StartPlayWASAPI(bool isFile, bool needFade, bool gaplessResume = false);
	bool StartPlayASIO(bool isFile, bool needFade, bool gaplessResume = false);
	bool StartPlayDS(bool isFile, bool needFade, bool gaplessResume = false);
	void ResetCueSync(long long cue = 0, double lenght = 0.0, QWORD position = 0, QWORD offset = 0);
	void FreePreload();
	void PrepareOpen(bool needFade);
	HSTREAM OpenMediaFile(const std::wstring& file, QWORD* outByteLength, double* outTimeSecond, double* position = nullptr, QWORD* outBytePosition = nullptr, int* outError = nullptr);
	HSTREAM OpenMediaURL(const std::wstring& url, int* outError = nullptr);

	int GetTimePosition();
	inline int GetTimeLength() {return (int)std::round(timeSecond);}

	int GetPosition();
	int GetVolume();
	inline bool GetMute() {return isSoundMute;}
	void GetFFT(float* fft);
	void GetFFT2(float* fft, int size); // Not used

	void SetPosition(int position);
	void SetVolume(int volume);
	void SetMute(bool isMute);

	void SetPreamp(float preamp);
	float GetPreamp() {return eqPreamp;}
	void SetEq(int band, float gain);
	float GetEq(int band) {return eqValues[band];}
	void SetEqPreset(const std::wstring& preset) {eqPreset = preset;}
	const std::wstring& GetEqPreset() {return eqPreset;}
	
	void EnableEq(bool enable);
	bool IsEqEnable() {return isEqEnable;}

	bool GetRadioTags(std::wstring& title, std::wstring& artist, std::wstring& album);

	void SetProxy(int proxy, const std::wstring& host, const std::wstring& port, const std::wstring& login, const std::wstring& pass);

	void SyncProcEndImpl();
	void SyncProcPreloadImpl();
	void SyncWAStopPauseImpl(bool isStop, HSTREAM channel);
	void SyncFreeChannelImpl(HSTREAM channel);

	bool GetRadioInfo(int& type, int& kbps, int& chans, int& freq, std::wstring& meta);
	Error StartRadio(int error);

	void SetNoVolumeEffect(bool volume, bool effect, bool reset = false);
	void SetPropertiesWA(bool wasapi, int asio) {wasapiEvent = wasapi; asioChannel = asio;}

	bool IsRadioStream() {return isRadioStream;}

private:
	HSTREAM streamPlay = NULL; // Playing stream
	HSTREAM streamMixer = NULL; // Mixer stream
	HSTREAM streamPreload = NULL; // Preloaded stream
	std::wstring filePreload; // Preloaded file
	bool isPreloadRate = false;
	HSTREAM streamMixerCopyWASAPI = NULL;
	HSYNC syncEnd = NULL;
	HSYNC syncEndMix = NULL;

	bool isRadioStream = false;
	bool isRadioReconnect = false;

	int asioChannel = 0;
	bool wasapiEvent = false;

	long long cueThis = 0;
	long long cuePreload = 0;

	QWORD cueOffset = 0;
	QWORD cueOffsetPreload = 0;

	QWORD byteLengthCue = 0;
	double timeSecondCue = 0.0;

	HSYNC syncEndCue = NULL;
	HSYNC syncEndMixCue = NULL;

	int radioBitrate = 0;

	bool bassNoVolume = false; // Disable volume control
	bool bassNoEffect = false; // Disable all sound effects

	BASS_CHANNELINFO oldci;

	//std::vector<int> channelsASIO;

	int bassDriver = 0; // 0 - DirecSound, 1 - WASAPI, 2 - ASIO
	int bassDevice = 0; // BASS device number

	std::wstring programPath;
	std::wstring profilePath;

	std::wstring fileThis; // Playing file
	std::wstring urlThis; // Playing URL address

	QWORD byteLength = 0; // Playing file size in bytes
	double timeSecond = 0.0; // Playing file length in seconds
	QWORD byteLengthPreload = 0; // Preloaded file size in bytes
	double timeSecondPreload = 0.0; // Preloaded file length in bytes


	Threading::Thread threadRadio; // Thread for radio

	bool isMediaPlay = false; // Play state
	bool isMediaPause = false; // Pause state
	bool isMediaRadio = false; // Radio state

	bool isEqEnable = false; // Equalizer state (on/off)

	HFX fxPreamp = NULL; // Pream Bass_FX
	HFX fxEqualizer = NULL; // Equalizer Bass_FX

	float eqPreamp = 0.0f; // Preamp setting in db
	float eqValues[10] = {0.0f}; // Equalizer settings in db
	std::wstring eqPreset; // Name of the equalizer preset

	WinylWnd* wndWinyl = nullptr; // Main window (to send messages)

	float soundVolume = 1.0f; // Sound volume
	float realVolume = 1.0f; // Real sound volume (depending on Mute state)
	bool isSoundMute = false; // Mute state (on/off)

	DWORD dwSampleEx = 0;

	std::string proxyString; // Proxy text line (ANSI)

	void ApplyEqualizer();
	bool SaveEqualizer();
	void SaveEqBand(XmlNode& xmlNode, char* name, float f);
	bool LoadEqualizer();
	void LoadEqBand(XmlNode& xmlNode, char* name, float* f);

	static void CALLBACK SyncProcEnd(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncRadioMeta(HSYNC handle, DWORD channel, DWORD data, void* user);

	void RunThreadRadio();

	// WASAPI/ASIO proc functions
	static DWORD CALLBACK AsioProc(BOOL input, DWORD channel, void* buffer, DWORD length, void* user);
	static DWORD CALLBACK WasapiProc(void* buffer, DWORD length, void* user);
	
	// Sync functions
	static void CALLBACK SyncFreePlay(HSYNC handle, DWORD channel, DWORD data, void* user);
	//static void CALLBACK SyncFreeMixer(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncWASAPIPause(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncASIOPause(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncWASAPIStop(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncASIOStop(HSYNC handle, DWORD channel, DWORD data, void* user);

	// Gapless Playback functions
	static void CALLBACK SyncProcGaplessWA(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncProcGaplessDS(HSYNC handle, DWORD channel, DWORD data, void* user);
	static void CALLBACK SyncProcGaplessCueWA(HSYNC handle, DWORD channel, DWORD data, void* user);

	std::atomic<bool> nextCueTrack = false;

	QWORD posPlus = 0;
	std::unique_ptr<Threading::Thread> threadBuffer;
	std::unique_ptr<Threading::Event> eventBuffer;
	std::unique_ptr<Threading::Event> eventPause;
	std::unique_ptr<Threading::Mutex> mutexBuffer;
	std::atomic<bool> stopThreadBuffer = false;
	void RunThreadBuffer();

	struct STRUCTBUFFER
	{
		HSTREAM streamFile;
		HSTREAM streamBuffer;
		char* buffer;
		int bytes;
	};
	std::vector<STRUCTBUFFER*> bufferStreams;
};
