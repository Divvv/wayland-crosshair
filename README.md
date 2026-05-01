# wayland-crosshair

A minimal custom crosshair overlay for Wayland compositors (Hyprland, Sway, etc.).

Displays a small green dot (or custom .png) at the exact center of your screen as a fullscreen transparent overlay. Designed for gaming — it is completely invisible to the input system: no focus, no keyboard grabs, no mouse interception. Clicks and cursor movement pass straight through to the game underneath.

Optionally launches `wlsunset` alongside it for a gamma boost, killing it automatically on exit.

## Features

- Zero input interference — uses an empty Wayland input region so the overlay cannot be focused or clicked
- `wlr-layer-shell` overlay layer — renders above fullscreen games on Wayland
- True screen center — anchors to all four edges so the dot is always at exact pixel center regardless of resolution
- Custom PNG crosshair — supply any PNG via `--image` and it replaces the default green dot
- Optional gamma boost via `wlsunset` — starts and stops with the crosshair
- Negligible resource usage — static surface, redrawn once at startup

## Dependencies

- `gtk3`
- `gtk-layer-shell`
- `wlsunset` *(optional, for gamma boost)*

Install on Arch Linux:

```bash
sudo pacman -S gtk3 gtk-layer-shell
# optional
sudo pacman -S wlsunset
```

## Build

```bash
make
```

## Usage

```bash
# Default green dot crosshair
./crosshair

# Custom PNG crosshair
./crosshair --image /path/to/crosshair.png
./crosshair -i /path/to/crosshair.png

# Custom PNG + gamma boost
./crosshair -i /path/to/crosshair.png --gamma 1.3

# Gamma boost only (value is passed to wlsunset -g)
./crosshair --gamma 1.2   # subtle
./crosshair --gamma 1.3   # moderate
./crosshair --gamma 1.5   # strong
./crosshair 1.3           # positional form still works
```

| Flag | Short | Description |
|------|-------|-------------|
| `--image <path>` | `-i <path>` | PNG file to render as the crosshair (centered on screen) |
| `--gamma <value>` | `-g <value>` | Gamma multiplier passed to `wlsunset` (e.g. `1.2`–`1.5`) |

Kill it (and wlsunset) with:

```bash
pkill crosshair
```

## Why not a Python/Electron/etc. script?

Most simple overlay examples steal input focus because they don't set `keyboard_interactivity = NONE` on the layer shell surface, and don't set an empty `wl_input_region`. This tool sets both, making it safe to run during gameplay.

## License

MIT
