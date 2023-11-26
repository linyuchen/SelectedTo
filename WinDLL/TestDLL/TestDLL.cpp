#include <windows.h>
#include <iostream>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
using namespace std;
typedef void(*FunctionPtr)();
typedef char* (*CopySelectedTextFunc)();  // 根据函数的参数和返回值类型定义函数指针类型
typedef BOOL(*AddHotKeyFunc)(int modkey, char key, FunctionPtr func);
CopySelectedTextFunc CopySelectedText;
AddHotKeyFunc AddHotKey;


void getSelectedText() {
	char* selectedText = CopySelectedText();
	if (selectedText != NULL) {
		cout << "获取复制的内容:" << selectedText << endl;
	}
	else {
		cout << "没有复制文本" << endl;
	}
}

int main() {

    
	HINSTANCE hDLL = LoadLibrary(TEXT("GetSelected.dll"));
	if (hDLL == NULL) {
		// DLL加载失败的处理代码
		cout << "加载dll失败";
	}
	else {

		CopySelectedText = (CopySelectedTextFunc)GetProcAddress(hDLL, "CopySelectedText");
		AddHotKey = (AddHotKeyFunc)GetProcAddress(hDLL, "AddHotKey");
		if (AddHotKey == NULL) {
			// 获取函数地址失败的处理代码
			cout << "获取函数AddHotKey失败";
		}
		else {
			if (CopySelectedText == NULL) {
				// 获取函数地址失败的处理代码
				cout << "获取函数CopySelectedText失败";
			}
			else {
				AddHotKey(MOD_ALT, 'D', getSelectedText);
			}
		}
		string s;
		cout << "press anykey to quit" << endl;
		cin >> s;
		FreeLibrary(hDLL);
		
	}
}

