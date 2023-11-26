// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <map>
#include <queue>
#include <mutex>

typedef void(*FunctionPtr)();
using namespace std;

#define DLL_EXPORT __declspec(dllexport)

int globalHotKeyMsgId = 998;
map<int, FunctionPtr> hotkeyIds;
std::mutex mtx;



class HotKeyInfo
{
public:
    int hotkeyMsgId;
    int modId;
    char key;

    HotKeyInfo(int hotkeyMsgId, int modId, char key)
        : hotkeyMsgId(hotkeyMsgId), modId(modId), key(key) {
        
    }
};

queue<HotKeyInfo> hotkeyQueue;


void HotKeyThread()
{
    unique_lock<std::mutex> qlock(mtx, defer_lock);
    
    while (true) 
    {
        MSG msg = { 0 };
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
        {
            if (msg.message == WM_HOTKEY)
            {
                int hotkeyMsgId = msg.wParam;
                if (hotkeyIds.find(hotkeyMsgId) != hotkeyIds.end()) {
                    FunctionPtr callback = hotkeyIds[hotkeyMsgId];
                    callback();
                }
            }
        }
        
        qlock.lock();
        while (!hotkeyQueue.empty())
        {
            HotKeyInfo hotkeyInfo = hotkeyQueue.front();
            hotkeyQueue.pop();
            if (!RegisterHotKey(NULL, hotkeyInfo.hotkeyMsgId, hotkeyInfo.modId, hotkeyInfo.key))
            {
                cout << "Failed to register hot key " << hotkeyInfo.modId << hotkeyInfo.key << ". Error: " << GetLastError() << std::endl;
            }
        }
        qlock.unlock();
        Sleep(100);
    }
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{   
    


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        thread t(HotKeyThread);
        t.detach();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



extern "C" {
    DLL_EXPORT void AddHotKey(int modId, char key, FunctionPtr callback)
    {
        globalHotKeyMsgId++;
        std::lock_guard<std::mutex> lock(mtx);
        hotkeyIds[globalHotKeyMsgId] = callback;
        hotkeyQueue.push(HotKeyInfo(globalHotKeyMsgId, modId, key));
    }
    DLL_EXPORT char* CopySelectedText() {
        // 获取当前活动窗口的句柄
        HWND hWindow = GetForegroundWindow();
        
        for (int i = 0; i < 3; i++) {
            // 模拟按下和释放Ctrl键
            keybd_event(VK_CONTROL, 0, 0, 0);
            keybd_event(VkKeyScan('C'), 0, 0, 0);
            Sleep(100);
            // 模拟释放Ctrl键
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VkKeyScan('C'), 0, KEYEVENTF_KEYUP, 0);

            // 将Ctrl + C消息发送给活动窗口
            SendMessage(hWindow, WM_COPY, 0, 0);
            Sleep(100);
        }
        for (int i = 0; i < 3; i++) {
            if (OpenClipboard(NULL)) {
                HANDLE hData = GetClipboardData(CF_TEXT);
                if (hData != NULL) {
                    CloseClipboard();
                    UINT format = EnumClipboardFormats(0);
                    bool isTextFormatAvailable = IsClipboardFormatAvailable(CF_TEXT);

                    if (isTextFormatAvailable) {
                        char* pszData = static_cast<char*>(GlobalLock(hData));
                        if (pszData != NULL) {
                            GlobalUnlock(hData);
                            return pszData;
                        }
                    }
                    else {
                        //cout << "剪切板中没有文本" << endl;
                    }
                }
                CloseClipboard();
            }
            Sleep(100);
        }

        return NULL;
    }
}
