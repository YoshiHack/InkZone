# InkZone

InkZone is an ESP32-powered color e-paper sports display. It emphasizes favorite
teams, selects an appropriate view automatically, and keeps the last useful
screen visible when no refresh is needed.

## Firmware components

- PlatformIO project configured for a generic ESP32 development board
- Arduino framework selected
- Serial monitor configured for 115200 baud
- Minimal boot firmware in `src/main.cpp`
- Provider-independent sports data models
- Automatic screen-selection and refresh-scheduling logic
- Debounced short-press and long-press button interpretation

See `docs/ARCHITECTURE.md` for the current boundaries and deferred hardware
decisions.

## Open in VS Code

1. Install the **PlatformIO IDE** extension in VS Code if it is not installed.
2. Open this folder in VS Code.
3. Connect the ESP32 with a USB data cable.
4. Use PlatformIO's **Build**, **Upload**, and **Serial Monitor** actions.

Expected serial output:

```text
InkZone starting...
Serial connection ready.
```

## Hardware assumption

The initial environment uses PlatformIO's `esp32dev` board definition. This is
only a placeholder until the exact ESP32 board is selected. Do not connect the
e-paper display until its panel, driver board, voltage, and pinout are confirmed.
