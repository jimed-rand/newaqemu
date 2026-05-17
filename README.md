# newaqemu

**newaqemu** is a Qt6 graphical frontend for QEMU, forked from [AQEMU](https://github.com/tobimensch/aqemu) for **modern Linux** and **Windows** hosts.

## What newaqemu adds (1.0.0 → 1.5.0)

### 1.0.0 — Modern port baseline

- Rebrand from AQEMU (`newaqemu` binary, `~/.config/newaqemu/`)
- Qt6 / C++17 build
- **Hybrid backends:** ordinary VMs use direct QEMU; GPU passthrough VMs use **libvirt** (`virsh`)
- Host integration for **IOMMU / Intel iGPU SR-IOV / vfio-pci / kvmfr / Looking Glass** ([workflow reference](https://pen.waltuh.cyou/yonle/iommu-with-sr-iov-gpu-accelerated-windows-vm-on-a-linux-laptop-that-has-only))
- Legacy AQEMU `.aqemu` VM files and settings import

### 1.1.0 — Emulator control and tray

- **System tray:** minimize or close the main window to the tray (Advanced Settings)
- **Emulator control window** remembers size/position
- **USB disconnect** from the running VM (monitor `usb_del` with automatic bus address lookup)
- Emulator control updates: mouse grab (VNC), Qt6 screen sizing, reloadable removable-device menu

### 1.2.0 — Embedded display

- **VNC** embedded display: improved reconnect behaviour
- **SPICE** display: launches `remote-viewer` / `virt-viewer` when SPICE is enabled on the VM (`-DWITH_SPICE_DISPLAY=on`, default)

### 1.3.0 — Sound

- **Host audio driver** selection in Advanced Settings (probed from `qemu -audiodev help`)
- QEMU **8+** VMs use `-audiodev` + HDA/AC97 devices when supported; older QEMU keeps `-soundhw`

### 1.4.0 — Tooltips

- Help tooltips on main window, advanced settings, and emulator control (extensible via `UiTooltips`)

### 1.5.0 — Windows

- **Windows** build support: USB host device enumeration (SetupAPI), tray, paths
- GPU passthrough / host-setup remain **Linux-only**

## Dependencies

- Qt6 (Core, Widgets, Network, Test, PrintSupport)
- CMake ≥ 3.16
- QEMU ≥ 6.x (8.x+ recommended)
- **libvirt** + `virsh` (Linux passthrough profile only)
- Optional: **LibVNCServer** (embedded VNC, `-DWITHOUT_EMBEDDED_DISPLAY=on` to disable)
- Optional: **virt-viewer** or **remote-viewer** (SPICE display)
- Optional: **looking-glass-client**, **kvmfr**, **i915-sriov-dkms** (Intel iGPU VF passthrough, Linux)

## Build (Linux)

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DWITHOUT_EMBEDDED_DISPLAY=on
cmake --build build
sudo cmake --install build
```

With embedded VNC + SPICE helper:

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
```

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

## Versioning

- **1.0.x** — Qt6 rebrand and passthrough baseline
- **1.1.x–1.5.x** — legacy AQEMU TODO features (tray, USB, display, sound, tooltips, Windows)

## License

GPL-2.0 (same as AQEMU). See [COPYING](COPYING).
