#include <windows.h>
#include <iostream>
using namespace std;

int main() {
	HINSTANCE hDLL = LoadLibrary(TEXT("GetSelected.dll"));
	if (hDLL == NULL) {
		// DLL加载失败的处理代码
		cout << "加载dll失败";
	}
	else {
		typedef char* (*CopySelectedTextType)();  // 根据函数的参数和返回值类型定义函数指针类型

		CopySelectedTextType CopySelectedText = (CopySelectedTextType)GetProcAddress(hDLL, "CopySelectedText");
		if (CopySelectedText == NULL) {
			// 获取函数地址失败的处理代码
			cout << "获取函数CopySelectedText失败";
		}
		else {
			cout << "五秒后复制鼠标选中文本" << endl;
			Sleep(5000);
			char* selectedText = CopySelectedText();
			if (selectedText != NULL) {
				cout << "获取复制的内容:" << selectedText << endl;
			}
			else{
				cout << "没有复制文本" << endl;
			}
		}
		FreeLibrary(hDLL);
	}
}

