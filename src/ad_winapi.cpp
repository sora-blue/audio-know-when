//
// Created by HP on 2023/3/4.
//
#include <mmdeviceapi.h>
#include "../include/ad_winapi.h"
#include <cstdio>

namespace ad{

#define CHECK_HR(exp) \
do{ \
if((exp) != S_OK)     \
    goto done; \
} while(0);

#define SAFE_RELEASE(exp) \
do{              \
auto tmp = (exp); \
if (tmp){              \
    tmp->Release(); \
    tmp = NULL; \
}\
} while(0);

    HRESULT CreateSessionManager(IAudioSessionManager2** ppSessionManager)
    {

        HRESULT hr = S_OK;

        IMMDevice* pDevice = NULL;
        IMMDeviceEnumerator* pEnumerator = NULL;
        IAudioSessionManager2* pSessionManager = NULL;


        // Create the device enumerator.
        CHECK_HR( hr = CoCreateInstance(
                __uuidof(MMDeviceEnumerator),
                NULL, CLSCTX_ALL,
                __uuidof(IMMDeviceEnumerator),
                (void**)&pEnumerator));

        // Get the default audio device.
        CHECK_HR( hr = pEnumerator->GetDefaultAudioEndpoint(
                eRender, eConsole, &pDevice));

        // Get the session manager.
        CHECK_HR( hr = pDevice->Activate(
                __uuidof(IAudioSessionManager2), CLSCTX_ALL,
                NULL, (void**)&pSessionManager));

        // Return the pointer to the caller.
        *(ppSessionManager) = pSessionManager;
        (*ppSessionManager)->AddRef();

        done:

        // Clean up.
        SAFE_RELEASE(pSessionManager);
        SAFE_RELEASE(pEnumerator);
        SAFE_RELEASE(pDevice);

        return hr;
    }

    HRESULT EnumSessions(IAudioSessionManager2* pSessionManager, int *counter)
    {
        if (!pSessionManager || !counter)
        {
            return E_INVALIDARG;
        }

        HRESULT hr = S_OK;
        static const wchar_t *state2str[] = {L"inactive", L"active", L"expired"};

        int cbSessionCount = 0;
        LPWSTR pswSession = NULL;

        IAudioSessionEnumerator* pSessionList = NULL;
        IAudioSessionControl* pSessionControl = NULL;
        AudioSessionState state;

        // Get the current list of sessions.
        CHECK_HR( hr = pSessionManager->GetSessionEnumerator(&pSessionList));

        // Get the session count.
        CHECK_HR( hr = pSessionList->GetCount(&cbSessionCount));

        *counter = 0;
        for (int index = 0 ; index < cbSessionCount ; index++)
        {
            CoTaskMemFree(pswSession);
            SAFE_RELEASE(pSessionControl);

            // Get the <n>th session.
            CHECK_HR(hr = pSessionList->GetSession(index, &pSessionControl));
            CHECK_HR(hr = pSessionControl->GetDisplayName(&pswSession));
            wprintf_s(L"Session Name: %s\n", pswSession);

            CHECK_HR(hr = pSessionControl->GetState(&state));
            // Check active sessions.
            if (state < 0 || state > 2) continue;
            if (state == 1) {
                (*counter)++;
            }
            wprintf_s(L"Session State: %s\n", state2str[state]);
        }

        done:
        CoTaskMemFree(pswSession);
        CHECK_HR(hr);
        SAFE_RELEASE(pSessionControl);
        SAFE_RELEASE(pSessionList);

        return hr;

    }

}
