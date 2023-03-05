#include <memory>
#include <tchar.h>

#include "include/ad_winapi.h"
class AudioStateDetector{
    // test shared_ptr
    struct ASMDeleter2{
        void operator()(IAudioSessionManager2 *m)const{
            m->Release();
        }
    };
    std::shared_ptr<IAudioSessionManager2> mgr;
    // run command with current directory
    void bat_business(const char *command){
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        WINBOOL err;
        LPTSTR szCmdline;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        si.cb = sizeof(si);
        si.wShowWindow = SW_HIDE;

        szCmdline = _tcsdup(TEXT(command));

        err = CreateProcessA(0, szCmdline, 0, 0, false, 0, 0, 0, &si, &pi);
        if(!err){
            log("failed to execute \"%s\" (error code: %d).\n", command, GetLastError());
            return;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    // resume bgm
    void start_play(){
        log("play starts\n");
        bat_business(R"(cmd /c ..\\script\\resume.bat)");
    }
    // pause bgm
    void stop_play(){
        log("play stops\n");
        bat_business(R"(cmd /c ..\\script\\pause.bat)");
    }
    // pretend to have a logger
    template<typename... T>
    void log(const char *format, T... args){
        printf(format, args...);
    }
public:
    // init
    AudioStateDetector(){
        IAudioSessionManager2 *tmp;
        HRESULT hr;
        // init session manager
        hr = ad::CreateSessionManager(&tmp);
        if(hr != S_OK){
            log("Err: %x\n", hr);
            return;
        }
        mgr.reset(tmp, ASMDeleter2{});
        // init listen process
        stop_play();
    }
    // uninit
    ~AudioStateDetector(){
    }
    // work once
    void exec(){
        static int exec_cnt = 0;

        HRESULT hr;
        int active_cnt;

        log("--- %d ---\n", ++exec_cnt);
        // check active sessions
        hr = ad::EnumSessions(mgr.get(), &active_cnt);
        if(hr != S_OK){
            log("Error code: 0x%x\n", hr);
        }
        log("active sessions: %d\n", active_cnt);
        // if other programs are playing, pause
        // else, resume
        if(active_cnt >= 2){
            stop_play();
        }else if(active_cnt == 1){
            start_play();
        }
    }
};
int main(int argc, char **argv) {
    int interval = 500;
    // args
    const char * optStr = "ht:";
    int opt;
    while((opt = getopt(argc, argv, optStr)) != -1){
        switch(opt){
            case 't':
                interval = atoi(optarg);
                break;
            case 'h':
            default:
                printf("Usage: audiokw [-t interval] [-h]\n"
                       "\t-h\tPrint help message\n"
                       "\t-t\tCheck session state every $interval ms");
                exit(0);
        }
    }
    //
    CoInitialize(nullptr);
    //
    auto detector = new AudioStateDetector;
    for( ;  ; ){
        detector->exec();
        Sleep(interval);
    }
    delete detector;
    //
    CoUninitialize();
    return 0;
}
