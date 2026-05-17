# newaqemu

**newaqemu** is a Qt6 graphical frontend for QEMU, forked from [AQEMU](https://github.com/tobimensch/aqemu) for **modern Linux** and **Windows** hosts.

## Features

- Rebrand from AQEMU (`newaqemu` binary, `~/.config/newaqemu/`)
- Qt6 / C++17 build
- **Hybrid backends:** ordinary VMs use direct QEMU; GPU passthrough VMs use **libvirt** (`virsh`)
- Host integration for **IOMMU / Intel iGPU SR-IOV / vfio-pci / kvmfr / Looking Glass** ([workflow reference](https://pen.waltuh.cyou/yonle/iommu-with-sr-iov-gpu-accelerated-windows-vm-on-a-linux-laptop-that-has-only))
- Legacy AQEMU `.aqemu` VM files and settings import
- **System tray:** minimize or close the main window to the tray (Advanced Settings)
- **Emulator control window** remembers size/position; USB disconnect from a running VM; VNC mouse grab; removable-device menu
- **VNC** embedded display with improved reconnect; **SPICE** via `remote-viewer` / `virt-viewer` when enabled at build
- **Host audio driver** selection (probed from `qemu -audiodev help`); QEMU 8+ uses `-audiodev` when supported
- Help tooltips on main window, advanced settings, and emulator control
- **Windows** build: USB host enumeration (SetupAPI), tray, paths; GPU passthrough remains **Linux-only**

## Dependencies

- Qt6 (Core, Widgets, Network, Test, PrintSupport)
- CMake ≥ 3.16
- QEMU ≥ 6.x (8.x+ recommended)
- **libvirt** + `virsh` (Linux passthrough profile only)
- Optional: **LibVNCServer** (embedded VNC, `-DWITHOUT_EMBEDDED_DISPLAY=on` to disable)
- Optional: **virt-viewer** or **remote-viewer** (SPICE display)
- Optional: **looking-glass-client**, **kvmfr**, **i915-sriov-dkms** (Intel iGPU VF passthrough, Linux)

## Build (Linux)

On a fresh system, install compile dependencies first (uses apt, dnf, zypper, or pacman):

```bash
make deps          # CMake, Qt6, toolchain, bzip2, libvirt headers
make deps-runtime  # optional: QEMU for testing VMs
make
sudo make install
```

If `cmake` is not installed, `make` will try to run `make deps` for you (sudo may be required).

Embedded VNC + SPICE (requires LibVNCServer):

```bash
make deps-full
make full
sudo make install
```

Advanced / explicit CMake (packagers, custom prefixes):

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DWITHOUT_EMBEDDED_DISPLAY=on
cmake --build build
sudo cmake --install build
```

Full display via CMake:

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
```

Run `make help` for targets (`clean`, `distclean`, `debug`, `run`) and variables (`PREFIX`, `JOBS`, `CMAKE_ARGS`).

## Build (Windows)

Use Qt6 for Windows (MSVC or MinGW), then:

```bash
cmake -B build -G Ninja -DCMAKE_PREFIX_PATH=%Qt6_DIR%
cmake --build build
windeployqt build/newaqemu.exe
```

Use **TCP monitor** (default on Windows). User-mode NAT networking works out of the box; TAP bridging requires a separate TAP-Windows adapter (not automated).

## GPU passthrough (Linux only)

1. Open **Advanced Settings → GPU passthrough** and review host readiness.
2. Apply host templates: `pkexec newaqemu-host-setup --gpu-bdf 0000:00:02.1 --kvmfr-mb 32`
3. Configure kernel cmdline (`intel_iommu=on`, `i915.max_vfs=1`, …) and reboot — see in-app diagnostics.
4. Create a VM with **GPU passthrough** checked in the wizard.
5. Complete Windows install, Intel driver, Looking Glass host, VDD, then switch display to Looking Glass mode.

## License

GPL-2.0 (same as AQEMU). See [COPYING](COPYING).
