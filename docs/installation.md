# Installation

## Requirements

- Windows 11, x86-64
- [Windhawk](https://windhawk.net/)
- The current project source: [permanent-taskbar-hider.wh.cpp](../src/permanent-taskbar-hider.wh.cpp)

The author reported testing version 2.0.0 on a single-monitor Windows 11 setup. Other Windows builds and multi-monitor setups have not been verified.

## Before enabling the mod

1. Open **Settings → Personalization → Taskbar → Taskbar behaviors**.
2. Enable **Automatically hide the taskbar**. This was part of the configuration used when the working setup was tested.
3. In Windhawk, disable other mods that also change taskbar visibility, especially **Taskbar auto-hide when maximized**.

## Install

1. Open Windhawk.
2. Select **Create New Mod**.
3. Open the source file linked above and copy the complete file.
4. Replace the contents of the Windhawk editor with the copied source.
5. Select **Compile Mod**.
6. If compilation succeeds, enable the mod.
7. Check that the taskbar stays hidden when the pointer reaches the bottom edge and that maximized windows extend to the bottom of the display.

## Disable / recover

1. Disable **Permanent Taskbar Hider** in Windhawk.
2. The mod's shutdown callback attempts to show the taskbar and restore the work area saved at initialization.
3. If the taskbar is still missing, restart **Windows Explorer** from Task Manager. Rebooting Windows is another recovery option.

## Troubleshooting

- **The taskbar reappears or flickers:** check that another taskbar-related Windhawk mod is not enabled.
- **Maximized windows leave a gap at the bottom:** verify that Windows taskbar auto-hide is enabled, then restart or re-enable the mod.
- **Compilation fails:** capture the complete compiler output and compare it with the source in this repository. Do not mix fragments from different versions.

This project does not include a compiled installer or DLL. Windhawk compiles the source on the target machine.
