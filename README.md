# InkZone

InkZone is an ESP32-powered color e-paper sports display. It emphasizes
favorite teams, selects an appropriate view automatically, and preserves the
last useful screen when the network or sports provider is unavailable.

## Development environment

- Visual Studio Code
- PlatformIO IDE extension
- Arduino framework
- Generic ESP32 Dev Module target until the exact controller is selected
- Serial monitor at 115200 baud

Use PlatformIO's **Build** action to compile the firmware. Uploading should wait
until the exact ESP32 board and USB connection are confirmed.

## Firmware modules

- `models`: provider-independent teams, games, and standings
- `device_status`: startup, setup, connection, ready, and offline states
- `settings`: favorite-team and refresh-interval configuration validation
- `data_cache`: preservation of the most recent valid sports snapshot
- `screen_selector`: automatic display priority decisions
- `refresh_scheduler`: activity-based refresh intervals and retry backoff
- `button_tracker`: debounced short-press and long-press events
- `main.cpp`: boot diagnostics and top-level device state

See `docs/ARCHITECTURE.md` for module boundaries.

## Startup behavior

A new configuration contains no favorite teams, so InkZone enters the
`needs setup` state. A valid saved configuration will proceed to
`connecting`.

Expected USB serial output begins with:

```text
InkZone starting...
Chip: ...
CPU: ... MHz
Flash: ... bytes
Configuration: no favorite leagues
State: needs setup
```

## Hardware cautions

Do not connect a raw e-paper panel directly to the ESP32. Confirm the selected
panel, driver board, voltage, waveform support, and SPI pinout first. ESP32 pin
assignments remain intentionally unset until the controller board is known.

Wi-Fi passwords must never be printed to serial output or rendered on the
display.


## Reliability features

- Data health tracking preserves the last valid update and applies progressively longer retry intervals after failures.
- Data freshness labels distinguish fresh, aging, stale, and unavailable sports data.
- The display refresh guard blocks overlapping e-paper updates and enforces a configurable minimum delay after each completed refresh.
- Favorite-team navigation safely wraps through configured teams and handles empty or single-team lists.
