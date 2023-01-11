
# KeyLogger useful MD

## Keyboard Input Model 

The keyboard device driver receives scan codes from the keyboard, the keyboard layout translate it to specific message and post it to the
appropriate window in the application.

-Assigned to each key on a keyboard a unique value called scan code
-The keyboard device driver interprets the scan code and translate it to a virtual key-code.
-Pressing a key cause a WM_KEYDOWN

## Hooks

There is a Hook procedure using SetWindowsHookEx to install the Hook chain. A Hook procedure
must have the  following syntax:

LRESULT CALLBACK HookProc( int nCode, WPARAM wPARAM wParam, LPARAM lParam){
    // Process the event
    return CallNextHookEx(NULL, nCode, wPARAM,lParam);
}

Notice:
-HookProc is a placeholder for the application defined name
-The nCOde parameter is a hook code that allow to determine the action to perform

The KBDLLHOOKSTRUCT structure store information about a low-level keyboard input event!

typedef struct tagKBDLLHOOKSTRUCT {
  DWORD     vkCode;
  DWORD     scanCode;
  DWORD     flags;
  DWORD     time;
  ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT, *LPKBDLLHOOKSTRUCT,*PKBDLLHOOKSTRUCT;

### Installing and releasing Hooks procedure

1.Install a hook procedure by calling the SetWindowsHook function

HHOOK SetWindowsHookExA(
  [in] int       idHook,
  [in] HOOKPROC  lpfn,
  [in] HINSTANCE hmod,
  [in] DWORD     dwThreadId
);

-idHook: The type of hook procedure to be installed. (13 for WH_KEYBOARD_LL)
-lpfn: a pointer to the hook procedure
-If the function succeeds, the return value is the handle to the hook procedure.

### The messages LOOP

The system generates a message at each input event. The system uses two methods to route messages to a window procedure: posting messages to a first-in, first-out queue called a message queue.
<https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#message-handling>

A simple message loop consists of one function call to each of these three functions: GetMessage, TranslateMessage, and DispatchMessage.

MSG msg;
BOOL bRet;

while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
{
    if (bRet == -1)
    {
        // handle the error and possibly exit
    }
    else
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

The GetMessage function retrieves a message from the queue and copies it to a structure of type MSG. It returns a nonzero value, unless it encounters the WM_QUIT message.

typedef struct tagMSG {
  HWND   hwnd;
  UINT   message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD  time;
  POINT  pt;
  DWORD  lPrivate;
} MSG, *PMSG,*NPMSG, *LPMSG;

### Translate Vk_code

Translates (maps) a virtual-key code into a scan code or character value, or translates a scan code into a virtual-key code. The function translates the codes using the input language and an input locale identifier. --> <https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapvirtualkeyexa>

UINT MapVirtualKeyExW(
  [in]                UINT uCode,
  [in]                UINT uMapType,
  [in, out, optional] HKL  dwhkl
);

1. uCode should be vkCode or scanCode
2. Choose the map type: in this case uMapType = MAPVK_VK_TO_CHAR
3. Set the keyboard layout ex: (GetKeyboardLayout(LANG_SYSTEM_DEFAULT) for the default OS kb layout)