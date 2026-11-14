# Configuration

Version 2.0.0 does not expose a `WindhawkModSettings` block and has no user-adjustable options.

Windhawk may display **No settings available** for this mod. This is expected and does not mean that the mod failed to load.

The behavior is controlled by the source code:

- The taskbar visibility hooks are enabled while the mod is active.
- The background worker checks the taskbar every 300 milliseconds.
- The work area is expanded to the primary monitor bounds.
- The previous work area is saved during initialization and restored during shutdown.

Windows taskbar auto-hide should be enabled in Windows Settings, as described in the installation guide. Other taskbar-visibility mods should be disabled to avoid conflicts.

No configuration UI is intentionally provided in this version.
