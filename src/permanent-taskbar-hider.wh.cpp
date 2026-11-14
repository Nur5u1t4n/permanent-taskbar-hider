// ==WindhawkMod==
// @id permanent-taskbar-hider
// @name Permanent Taskbar Hider
// @description Completely hides the Windows 11 taskbar and expands the desktop work area.
// @description:ru-RU Полностью скрывает панель задач Windows 11 и расширяет рабочую область.
// @version 2.0.0
// @author Nursultan
// @include explorer.exe
// @architecture x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Permanent Taskbar Hider

Completely hides the Windows 11 taskbar and expands the desktop work area so maximized windows can use the full monitor height.

## Features

- Hides the primary Windows taskbar window.
- Blocks common attempts to show, reposition, or activate the taskbar.
- Rechecks the taskbar visibility every 300 milliseconds.
- Expands the primary desktop work area to the full monitor bounds.
- Attempts to restore the original work area and show the taskbar when disabled.
- Does not intentionally hide the Start menu.

## Installation

1. Install Windhawk.
2. Open Create New Mod.
3. Paste the complete source code.
4. Compile the mod and enable it.
5. Enable Windows taskbar auto-hide in Taskbar settings.
6. Disable other Windhawk mods that control taskbar visibility.

## Compatibility

- Windows 11
- x86-64
- Tested configuration reported by the author: one monitor

Multi-monitor support is not claimed.

## Configuration

This version has no user-configurable settings. Windhawk may show "No settings available"; this is expected.

## Recovery

Disable the mod in Windhawk. If the taskbar does not return, restart Windows Explorer or reboot Windows.

## Русский язык

### Возможности

- Полностью скрывает основное окно панели задач Windows 11.
- Блокирует распространённые попытки показать, переместить или активировать панель.
- Проверяет видимость панели каждые 300 миллисекунд.
- Расширяет рабочую область основного монитора до его полных границ.
- При отключении пытается восстановить исходную рабочую область и показать панель задач.
- Намеренно не скрывает меню «Пуск».

### Установка

1. Установите Windhawk.
2. Откройте Create New Mod.
3. Вставьте полный исходный код.
4. Скомпилируйте и включите мод.
5. Включите автоматическое скрытие панели задач в настройках Windows.
6. Отключите другие моды Windhawk, управляющие видимостью панели задач.

### Совместимость

- Windows 11
- x86-64
- Проверенная автором конфигурация: один монитор

Поддержка нескольких мониторов не заявляется.

### Настройки

В этой версии нет пользовательских настроек. Сообщение Windhawk "No settings available" является ожидаемым.

### Восстановление

Отключите мод в Windhawk. Если панель не появилась, перезапустите Проводник Windows или перезагрузите компьютер.

## Author

Nursultan
Version: 2.0.0
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>
#include <atomic>
#include <cwchar>

// ------------------------------------------------------------
// Global state
// ------------------------------------------------------------

static std::atomic<bool> g_running{false};
static std::atomic<bool> g_forceHidden{true};

static HANDLE g_workerThread = nullptr;

static RECT g_originalWorkArea{};
static bool g_workAreaSaved = false;

// ------------------------------------------------------------
// Original Windows API functions
// ------------------------------------------------------------

using ShowWindow_t = decltype(&ShowWindow);
using SetWindowPos_t = decltype(&SetWindowPos);
using SetForegroundWindow_t = decltype(&SetForegroundWindow);

static ShowWindow_t OriginalShowWindow = nullptr;
static SetWindowPos_t OriginalSetWindowPos = nullptr;
static SetForegroundWindow_t OriginalSetForegroundWindow = nullptr;

// ------------------------------------------------------------
// Find taskbar
// ------------------------------------------------------------

bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    wchar_t className[128]{};

    if (!GetClassNameW(hwnd, className, 128)) {
        return false;
    }

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}

// ------------------------------------------------------------
// Save and expand desktop work area
// ------------------------------------------------------------

void ExpandWorkArea() {
    if (!g_workAreaSaved) {
        if (SystemParametersInfoW(
                SPI_GETWORKAREA,
                0,
                &g_originalWorkArea,
                0)) {
            g_workAreaSaved = true;
        }
    }

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);

    HMONITOR monitor = MonitorFromPoint(
        POINT{0, 0},
        MONITOR_DEFAULTTOPRIMARY
    );

    if (!GetMonitorInfoW(monitor, &mi)) {
        Wh_Log(L"Failed to get monitor information");
        return;
    }

    RECT fullArea = mi.rcMonitor;

    if (!SystemParametersInfoW(
            SPI_SETWORKAREA,
            0,
            &fullArea,
            SPIF_SENDCHANGE | SPIF_UPDATEINIFILE)) {
        Wh_Log(L"Failed to expand desktop work area");
    }
}

// ------------------------------------------------------------
// Restore original desktop work area
// ------------------------------------------------------------

void RestoreWorkArea() {
    if (!g_workAreaSaved) {
        return;
    }

    SystemParametersInfoW(
        SPI_SETWORKAREA,
        0,
        &g_originalWorkArea,
        SPIF_SENDCHANGE | SPIF_UPDATEINIFILE
    );

    g_workAreaSaved = false;
}

// ------------------------------------------------------------
// Hide taskbar
// ------------------------------------------------------------

void HideTaskbar() {
    HWND hwnd = FindTaskbar();

    if (!hwnd) {
        return;
    }

    if (IsWindowVisible(hwnd) && OriginalShowWindow) {
        OriginalShowWindow(hwnd, SW_HIDE);
    }
}

// ------------------------------------------------------------
// Hook: ShowWindow
// ------------------------------------------------------------

BOOL WINAPI HookShowWindow(HWND hwnd, int command) {
    if (g_forceHidden.load() &&
        IsTaskbarWindow(hwnd) &&
        command != SW_HIDE) {
        return FALSE;
    }

    return OriginalShowWindow(hwnd, command);
}

// ------------------------------------------------------------
// Hook: SetWindowPos
// ------------------------------------------------------------

BOOL WINAPI HookSetWindowPos(
    HWND hwnd,
    HWND insertAfter,
    int x,
    int y,
    int cx,
    int cy,
    UINT flags) {

    if (g_forceHidden.load() && IsTaskbarWindow(hwnd)) {
        flags &= ~SWP_SHOWWINDOW;
        flags |= SWP_HIDEWINDOW | SWP_NOACTIVATE;
    }

    return OriginalSetWindowPos(
        hwnd,
        insertAfter,
        x,
        y,
        cx,
        cy,
        flags
    );
}

// ------------------------------------------------------------
// Hook: SetForegroundWindow
// ------------------------------------------------------------

BOOL WINAPI HookSetForegroundWindow(HWND hwnd) {
    if (g_forceHidden.load() && IsTaskbarWindow(hwnd)) {
        return FALSE;
    }

    return OriginalSetForegroundWindow(hwnd);
}

// ------------------------------------------------------------
// Install hooks
// ------------------------------------------------------------

bool InstallHooks() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    if (!user32) {
        return false;
    }

    bool success = true;

    void* target = nullptr;

    target = (void*)GetProcAddress(user32, "ShowWindow");

    if (!target ||
        !Wh_SetFunctionHook(
            target,
            (void*)HookShowWindow,
            (void**)&OriginalShowWindow)) {
        Wh_Log(L"Failed to install ShowWindow hook");
        success = false;
    }

    target = (void*)GetProcAddress(user32, "SetWindowPos");

    if (!target ||
        !Wh_SetFunctionHook(
            target,
            (void*)HookSetWindowPos,
            (void**)&OriginalSetWindowPos)) {
        Wh_Log(L"Failed to install SetWindowPos hook");
        success = false;
    }

    target = (void*)GetProcAddress(user32, "SetForegroundWindow");

    if (!target ||
        !Wh_SetFunctionHook(
            target,
            (void*)HookSetForegroundWindow,
            (void**)&OriginalSetForegroundWindow)) {
        Wh_Log(L"Failed to install SetForegroundWindow hook");
        success = false;
    }

    return success;
}

// ------------------------------------------------------------
// Background worker
// ------------------------------------------------------------

DWORD WINAPI WorkerThread(LPVOID) {
    ExpandWorkArea();

    while (g_running.load()) {
        HideTaskbar();

        Sleep(300);
    }

    return 0;
}

// ------------------------------------------------------------
// Windhawk initialization
// ------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Permanent Taskbar Hider initializing");

    g_forceHidden.store(true);

    if (!InstallHooks()) {
        Wh_Log(L"Warning: Some hooks failed to install");
    }

    g_running.store(true);

    g_workerThread = CreateThread(
        nullptr,
        0,
        WorkerThread,
        nullptr,
        0,
        nullptr
    );

    if (!g_workerThread) {
        g_running.store(false);
        g_forceHidden.store(false);

        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    Wh_Log(L"Permanent Taskbar Hider enabled");

    return TRUE;
}

// ------------------------------------------------------------
// Windhawk shutdown
// ------------------------------------------------------------

void Wh_ModUninit() {
    Wh_Log(L"Permanent Taskbar Hider shutting down");

    g_running.store(false);

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    g_forceHidden.store(false);

    HWND hwnd = FindTaskbar();

    if (hwnd && OriginalShowWindow) {
        OriginalShowWindow(hwnd, SW_SHOW);
    }

    RestoreWorkArea();

    Wh_Log(L"Permanent Taskbar Hider disabled");
}
