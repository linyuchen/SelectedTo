// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
using namespace std;

#define DLL_EXPORT __declspec(dllexport)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



extern "C" {
    DLL_EXPORT char* CopySelectedText() {
        // 获取当前活动窗口的句柄
        HWND hWindow = GetForegroundWindow();
        
        // 模拟按下和释放Ctrl键
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event(VkKeyScan('C'), 0, 0, 0);

        // 模拟释放Ctrl键
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VkKeyScan('C'), 0, KEYEVENTF_KEYUP, 0);

        // 将Ctrl + C消息发送给活动窗口
        SendMessage(hWindow, WM_COPY, 0, 0);
        Sleep(500);
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
        }
        return NULL;
    }
}
