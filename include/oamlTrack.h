//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Marcelo Fernandez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef __OAMLTRACK_H__
#define __OAMLTRACK_H__

class ByteBuffer;
class oamlAudio;

class oamlTrack {
protected:
	bool verbose;

	std::string name;
	std::vector<std::string> groups;
	std::vector<std::string> subgroups;

	int lock;
	int fadeIn;
	int fadeOut;
	int xfadeIn;
	int xfadeOut;
	float volume;

	int Random(int min, int max);

	void ApplyVolPanTo(float *samlpes, int channels, float vol, float pan);
	float SafeAdd(float a, float b, bool debug);
	void MixAudio(oamlAudio *audio, float *samples, int channels, bool debug);
	unsigned int MixAudio(oamlAudio *audio, float *samples, int channels, bool debug, unsigned int pos);

	oamlAudio* FindAudio(std::vector<oamlAudio*> *audios, std::string filename);
	oamlRC FindAudioAndRemove(std::vector<oamlAudio*> *audios, std::string filename);
	void ClearAudios(std::vector<oamlAudio*> *audios);
	void ReadAudiosInfo(std::vector<oamlAudio*> *audios, oamlTrackInfo *info);
	void FreeAudiosMemory(std::vector<oamlAudio*> *audios);
	void FillAudiosList(std::vector<oamlAudio*> *audios, std::vector<std::string>& list);

	int GetFilesSamplesFor(std::vector<oamlAudio*> *audios);
	int LoadProgressFor(std::vector<oamlAudio*> *audios);

public:
	oamlTrack();
	virtual ~oamlTrack();

	void SetName(std::string trackName) { name = trackName; }
	void AddGroup(std::string trackGroup) { groups.push_back(trackGroup); }
	void AddSubgroup(std::string trackSubgroup) { subgroups.push_back(trackSubgroup); }
	void SetFadeIn(int trackFadeIn) { fadeIn = trackFadeIn; }
	void SetFadeOut(int trackFadeOut) { fadeOut = trackFadeOut; }
	void SetXFadeIn(int trackXFadeIn) { xfadeIn = trackXFadeIn; }
	void SetXFadeOut(int trackXFadeOut) { xfadeOut = trackXFadeOut; }
	void SetVolume(float trackVolume) { volume = trackVolume; }

	virtual void ReadInfo(oamlTrackInfo *info);

	const char *GetNameStr() const { return name.c_str(); }
	std::string GetName() const { return name; }
	std::vector<std::string> GetGroups() const { return groups; }
	std::vector<std::string> GetSubgroups() const { return subgroups; }
	bool HasGroup(std::string trackGroup) const { return std::find(groups.begin(), groups.end(), trackGroup) != groups.end(); }
	bool HasSubgroup(std::string trackSubgroup) const { return std::find(subgroups.begin(), subgroups.end(), trackSubgroup) != subgroups.end(); }
	int GetFadeIn() const { return fadeIn; }
	int GetFadeOut() const { return fadeOut; }
	int GetXFadeIn() const { return xfadeIn; }
	int GetXFadeOut() const { return xfadeOut; }
	float GetVolume() const { return volume; }

	virtual void GetAudioList(std::vector<std::string>&) { }
	virtual void AddAudio(oamlAudio *) { }
	virtual oamlAudio* GetAudio(std::string) { return NULL; }
	virtual oamlRC RemoveAudio(std::string) { return OAML_NOT_FOUND; }
	virtual oamlRC Load() { return OAML_NOT_FOUND; }
	virtual float LoadProgress() { return -1.f; }
	virtual void Stop() { }

	virtual bool IsPlaying() { return false; }
	void ShowPlaying();
	virtual std::string GetPlayingInfo() { return ""; }

	virtual void Mix(float *, int, bool) { }

	virtual void SetCondition(int, int) { }

	virtual bool IsMusicTrack() const { return false; }
	virtual bool IsSfxTrack() const { return false; }

	virtual void SetLayerGain(std::string, float) { }

	virtual void FreeMemory() { }
};

#endif
