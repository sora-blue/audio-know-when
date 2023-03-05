//
// Created by HP on 2023/3/4.
//

#ifndef AUDIODETECTOR_AD_WINAPI_H
#define AUDIODETECTOR_AD_WINAPI_H

#include <audiopolicy.h>
namespace ad{
    HRESULT CreateSessionManager(IAudioSessionManager2** ppSessionManager);
    HRESULT EnumSessions(IAudioSessionManager2* pSessionManager, int*);
}
#endif //AUDIODETECTOR_AD_WINAPI_H
