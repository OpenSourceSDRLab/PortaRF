# Licences and third-party notices

The PortaRF repository combines OpenSourceSDRLab's own hardware files and documentation with open-source firmware from the Mayhem and HackRF projects.

## Files under the Apache License 2.0

The Apache License 2.0 in [LICENSE.Apache-2.0](LICENSE.Apache-2.0) applies **only** to these files created by OpenSourceSDRLab:

- `hardware/PortaRF/` (PortaRF R1 schematic, PCB layout, Gerber and drill files)
- `docs/Quick-Start-Guide.md`, `docs/Quick-Start-Guide.pdf`
- `images/`, `README.md`, `NOTICE.md`

The PortaRF hardware is derived from the HackRF One design by Great Scott Gadgets, which is licensed under the CERN Open Hardware Licence Version 2 – Permissive (CERN-OHL-P), see `hackrf/hardware/hackrf-one/LICENSE`:

```
Copyright (c) 2012-2022, Great Scott Gadgets <info@greatscottgadgets.com>
Licensed under the CERN-OHL-P v2.
```

## Firmware (GNU GPL)

The firmware source code keeps the licences of the projects it comes from. The PortaRF changes to these files are released under the same licences.

| Component | Files | Owner | Licence |
|---|---|---|---|
| Mayhem firmware for PortaPack, version 2.3.2 | `firmware/`, `sdcard/`, `flashing/`, `hardware/portapack_*`, `docs/` (Mayhem website files), build files in the repository root | Jared Boone / ShareBrained Technology, Furrtek, the Mayhem contributors (<https://github.com/portapack-mayhem/mayhem-firmware>) and others as stated in each file | GPL-2.0-or-later and GPL-3.0, see [LICENSE](LICENSE) and [LICENSE.GPL-2.0-or-later](LICENSE.GPL-2.0-or-later); third-party parts (ChibiOS, FatFs, libopencm3 and others) as stated in their own files |
| HackRF firmware and host tools | `hackrf/` | Great Scott Gadgets, Jared Boone, Michael Ossmann and the HackRF contributors (<https://github.com/greatscottgadgets/hackrf>) | GPL-2.0, see `hackrf/COPYING`; hardware designs CERN-OHL-P |

Files added or changed by OpenSourceSDRLab for the PortaRF (released under the licence of the firmware they belong to):

- 2 MB flash image: `firmware/common/memory_map.hpp`, `firmware/tools/make_spi_image.py`, `firmware/chibios-portapack/os/ports/GCC/ARMCMx/LPC43xx_M0/ld/LPC43xx_M0.ld`, `firmware/application/external/external.ld`, `firmware/application/external/external.cmake`, `firmware/application/CMakeLists.txt`, `firmware/baseband/CMakeLists.txt`, `firmware/baseband/proc_flash_utility.cpp`, `firmware/application/apps/ui_flash_utility.hpp`
- MP3 player and WAV viewer: `firmware/application/apps/ui_mp3player.*`, `firmware/application/external/sd_mp3player/`, `firmware/application/external/wav_view/ui_view_wav.cpp`, `firmware/application/audio.*`, `firmware/common/wm8731.*`, `firmware/common/ak4951.*`, `firmware/application/bitmap.hpp`
- AI MDK voice commands and I2C module support: `firmware/application/usb_serial_shell.cpp`, `firmware/application/ui_navigation.*`, `firmware/application/apps/analog_audio_app.*`, `firmware/application/apps/ui_esp32update.*`, `firmware/common/i2cdevmanager.*`
- Low-power mode: `firmware/application/apps/ui_low_battery.*`
- Other: `firmware/application/apps/ble_tx_app.*`, `firmware/application/apps/ui_debug.*`, `firmware/baseband/sd_over_usb/`, version files in `.github/workflows/`
- HackRF firmware for the PortaRF board: files in `hackrf/firmware/common/` (PortaPack interface, SPI flash, mixer and tuning)

## Release files

| File | Contents | Licence |
|---|---|---|
| `portapack-mayhem-firmware.bin`, `portapack-mayhem_OCI.ppfw.tar`, `*.ppma` applications | built from this repository | GPL, as above |
| `utils/hackrf_one_usb.*` | HackRF firmware built from `hackrf/` | GPL-2.0 |
| `utils/hackrf_*.exe`, `libhackrf.dll` | HackRF host tools, release 2024.02.1 (<https://github.com/greatscottgadgets/hackrf>) | GPL-2.0 |
| `utils/dfu-util-static.exe` | dfu-util | GPL-2.0 |
| `utils/libusb-1.0.dll` | libusb | LGPL-2.1 |
| `driver/`, `utils/msvc*.dll` | Windows USB driver installer and Microsoft Visual C++ runtime, as distributed with Mayhem | owners' redistribution terms |
| SD-card data (`ADSB/`, `AIS/`, samples and lists) | as distributed with Mayhem | as stated in the Mayhem repository |

"HackRF" is a trademark of Great Scott Gadgets. "PortaPack" and other names are trademarks of their respective owners. They are used here only to describe compatibility and do not imply endorsement.
