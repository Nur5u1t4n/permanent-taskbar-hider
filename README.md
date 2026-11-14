# Permanent Taskbar Hider

A Windhawk mod for Windows 11 that keeps the taskbar hidden and expands the desktop work area so maximized windows can reach the bottom edge of the display.

> **Project status:** Personal project. Version 2.0.0 was reported working by the author on one Windows 11 monitor. It has not been independently tested across Windows builds or multi-monitor configurations.

## Features

- Hides the primary Windows taskbar window.
- Intercepts common Windows API calls that would show, reposition, or activate the taskbar.
- Periodically checks that the taskbar remains hidden.
- Expands the primary desktop work area to the full monitor bounds.
- Restores the saved work area and shows the taskbar when the mod is disabled.
- Keeps the Start menu outside the taskbar-hiding logic.

## Compatibility

- Windows 11
- Windhawk
- x86-64
- Verified configuration reported by the author: one monitor

Multi-monitor support is not claimed. The current implementation uses the primary monitor and the system work-area API.

## Installation

1. Install [Windhawk](https://windhawk.net/).
2. Open **Create New Mod** in Windhawk.
3. Open [src/permanent-taskbar-hider.wh.cpp](src/permanent-taskbar-hider.wh.cpp), copy its entire contents, and paste them into the Windhawk editor.
4. Compile the mod and enable it.
5. In Windows taskbar settings, enable **Automatically hide the taskbar** as used by the tested setup.
6. Disable other Windhawk mods that also control taskbar visibility, such as **Taskbar auto-hide when maximized**, to avoid competing hooks.

See [Installation](docs/installation.md) for more detail.

## Configuration

Version 2.0.0 has no user-configurable settings. Windhawk may therefore show that no settings are available; this is expected. See [Configuration](docs/configuration.md).

## Recovery and removal

Disable the mod in Windhawk. The mod attempts to show the taskbar and restore the work area saved during initialization. If the taskbar does not return, restart Windows Explorer from Task Manager or reboot Windows.

## Source layout

```text
.
├── README.md
├── README.ru.md
├── LICENSE
├── .gitignore
├── src/
│   └── permanent-taskbar-hider.wh.cpp
└── docs/
    ├── changelog.md
    ├── configuration.md
    ├── configuration.ru.md
    ├── installation.md
    └── installation.ru.md
```

## License

This project is distributed under the [MIT License](LICENSE).

## Author

Nursultan (GitHub: [@Nur5u1t4n](https://github.com/Nur5u1t4n))

---

Русская версия: [README.ru.md](README.ru.md).
