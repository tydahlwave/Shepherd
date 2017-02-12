//
//  AudioEngine.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 2/3/17.
//
//

#ifndef AudioEngine_h
#define AudioEngine_h

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "glm/glm.hpp"

using namespace std;

//Vector3 Placeholder struct
struct Vector3 {
    float x;
    float y;
    float z;
};


//Implementation Struct
struct Implementation {
    Implementation();
    ~Implementation();
    
    void Update();
    
    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;
    
    int mnNextChannelId;
    
    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance*> EventMap;
    typedef map<string, FMOD::Studio::Bank*> BankMap;
    
    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};


//AudioEngine header
class CAudioEngine {
    static CAudioEngine *a_instance;
public:
    static string resourceDir;
    
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);
    static CAudioEngine *instance();
    static void LoadSounds(std::string resDir);
    
    void LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const string& strEventName);
    void Loadsound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const string& strSoundName);
    void Set3dListenerAndOrientation(const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    int PlaySound(const string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    void StopSound(int nchannelId);
    void PlayEvent(const string& strEventName);
    void toggleSound(int nChannelId, bool state);
    void StopEvent(const string& strEventName, bool bImmediate = false);
    void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
    void SetEventParameter(const string& strEventName, const string& strParameterName, float fValue);
    void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
    void SetChannelvolume(int nChannelId, float fVolumedB);
    bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const string& strEventName) const;
    float dbToVolume(float db);
    float VolumeTodb(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
};

#endif /* AudioEngine_h */
