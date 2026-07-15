# Firmware architecture

InkZone is being built in layers so hardware details and sports providers do not
leak into the rest of the firmware.

## Current layers

### Standard models

`include/inkzone/models.h` defines the provider-independent `Team`, `Game`, and
`Standing` structures. A future provider will translate downloaded JSON into
these models. Screen code should never read provider JSON directly.

### Automatic screen selection

`screen_selector` applies the initial automatic-mode priorities:

1. A favorite team playing live
2. Another live game
3. A favorite game starting within one hour
4. A favorite game later in the next 24 hours
5. A league scoreboard
6. The idle dashboard when no game data is available

Manual override behavior will sit above this selector and temporarily replace
its decision.

### Refresh scheduling

`refresh_scheduler` owns the default update intervals. Network retries use
exponential backoff capped at five minutes. Display code must additionally
check the panel's BUSY signal before starting a physical refresh.

### Button interpretation

`button_tracker` debounces one raw button input and emits a single short-press
or long-press event. A future board-specific input layer will create three
instances for Previous, Select, and Next.

## Hardware-dependent components

- ESP32 pin assignments, until the exact controller board is selected
- E-paper driver integration, until the exact panel and driver are confirmed
- Wi-Fi credential storage and setup portal
- Live sports provider URLs and JSON parsing
- Timezone conversion and true local-calendar-day boundaries
- Rendering and six-color palette mapping

Keeping these items deferred prevents placeholder wiring or unofficial API
details from becoming accidental design commitments.
