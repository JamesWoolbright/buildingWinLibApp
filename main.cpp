#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

//Global variables
HWND g_hListBox;
HWND g_hTextBox;
HWND g_hCompleteBox;

enum FileOperationMode {
    Save,
    Load,
};

void AddTask(const std::string& task) {
    SendMessageA(g_hListBox, LB_ADDSTRING, 0, (LPARAM)task.c_str());
}

void FileOperations(FileOperationMode mode) {
    std::fstream outputFile("data.txt", std::ios::in | std::ios::out);
    
    if (mode == Save) {
        outputFile.close(); outputFile.open("data.txt", std::ios::in | std::ios::out | std::ios::trunc);
    }

    if (outputFile.is_open() && mode == Save) {
        int ListCount = SendMessageA(g_hListBox, LB_GETCOUNT, 0, 0);
        for (int i = 0; i < ListCount; i++) {
            std::string CurrentMessage;
            CurrentMessage.resize(256);
            SendMessageA(g_hListBox, LB_GETTEXT, i, (LPARAM)CurrentMessage.data());
            outputFile << CurrentMessage << "\n";
        }
        outputFile.close();
    }

    if (outputFile.is_open() && mode == Load) {
        std::string line;
        while(std::getline(outputFile, line)) {
            AddTask(line);
        }
    }
}
// Sends a delete message code to the window
void RemoveTask(int index) {
    SendMessageA(g_hListBox, LB_DELETESTRING, index, 0);
}

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParameter, LPARAM lParameter) {
    switch (Message) {
        case WM_CREATE: {
            g_hTextBox = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 200, 25, Window, NULL, NULL, NULL);
            //I HAVE NO IDEA WHAT (HMENU)1 does
            CreateWindowExW(0, L"BUTTON", L"Add Task", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 10, 100, 25, Window, (HMENU)1, NULL, NULL);

            g_hListBox = CreateWindowExW(0 ,L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | LBS_STANDARD, 10, 50, 310, 200, Window, NULL, NULL, NULL);

            g_hCompleteBox = CreateWindowExW(0, L"BUTTON", L"Complete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 30, 100, 25, Window, (HMENU)2 , NULL, NULL);

            CreateWindowExW(0, L"BUTTON", L"Remove", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 65, 100, 25, Window, (HMENU)3 , NULL, NULL);

            CreateWindowExW(0, L"BUTTON", L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 100, 100, 25, Window, (HMENU)4 , NULL, NULL);

            CreateWindowExW(0, L"BUTTON", L"Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 135, 100, 25, Window, (HMENU)5 , NULL, NULL);

            CreateWindowExW(0, L"BUTTON", L"Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 340, 170, 100, 25, Window, (HMENU)6 , NULL, NULL);
            
        } break;
        case WM_COMMAND: {
            if (LOWORD(wParameter) == 1) { // Add Task Button Clicked
                std::string buffer;
                buffer.resize(256);
                GetWindowTextA(g_hTextBox, buffer.data(), sizeof(buffer));
                std::string task(buffer);
                if (!task.empty()) {
                    AddTask(task);
                    SetWindowTextA(g_hTextBox, "");
                }
            } 
            
            if (LOWORD(wParameter) == 2) { // Complete Box Clicked
                int index = SendMessageA(g_hListBox, LB_GETCURSEL, 0, 0);
                if (index != LB_ERR) {
                    char Message[256];
                    SendMessageA(g_hListBox, LB_GETTEXT, 0, (LPARAM)Message);

                    if (strstr(Message, "[Complete]") == NULL) {
                        strcat(Message, " [Complete]");
                        SendMessageA(g_hListBox, LB_DELETESTRING, index, 0);
                        SendMessageA(g_hListBox, LB_INSERTSTRING, index, (LPARAM)Message);
                    } else {
                        char* position = strstr(Message, " [Complete]");
                        if (position != NULL) {
                            *position = '\0';
                            SendMessageA(g_hListBox, LB_DELETESTRING, index, 0);
                            SendMessageA(g_hListBox, LB_INSERTSTRING, index, (LPARAM)Message);
                        }
                    }
                }
            }


            if (LOWORD(wParameter) == 3) {
                int index = SendMessageA(g_hListBox, LB_GETCURSEL, 0, 0);
                if (index != LB_ERR) {
                    RemoveTask(index);
                }
            } 

            if (LOWORD(wParameter) == 4) {
                SendMessageA(g_hListBox, LB_RESETCONTENT, 0, 0);
            }

            if (LOWORD(wParameter) == 5) {
                FileOperations(Save);          
            }

            if (LOWORD(wParameter) == 6) {
                FileOperations(Load);
            } 
        } break;


        case WM_CLOSE: {
            DestroyWindow(g_hCompleteBox);
            DestroyWindow(g_hListBox);
            DestroyWindow(g_hTextBox);

            DestroyWindow(Window);

            PostQuitMessage(0);

        } break;
        case WM_DESTROY: {
            DestroyWindow(g_hCompleteBox);
            DestroyWindow(g_hListBox);
            DestroyWindow(g_hTextBox);

            DestroyWindow(Window);

            PostQuitMessage(0);
            
            } break;

        default:
            return DefWindowProc(Window, Message, wParameter, lParameter);
    }
    return 0;
}

//main window creation a.k.a entry point
int WINAPI WinMain(
 HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow) {
    const char CLASS_NAME[] = "BasicTaskChecker";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    HWND hWindow = CreateWindowExA(0, CLASS_NAME, "Basic Task Checker", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 600, 300, NULL, NULL, hInstance, NULL);
    
    if (hWindow == NULL) {return 0;}
    
    ShowWindow(hWindow, nCmdShow);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }


    return 0;
 }
