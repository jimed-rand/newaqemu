# newaqemu manual test matrix

## Build

- [ ] `make deps` installs CMake/Qt6 toolchain on this distro
- [ ] `make` (minimal) and `build/newaqemu --version` prints `newaqemu 1.0.0`
- [ ] `make help` lists targets
- [ ] `cmake -B build -DWITHOUT_EMBEDDED_DISPLAY=on && cmake --build build` (or `make` only)
- [ ] Full build: `make full` (LibVNCServer + `WITH_SPICE_DISPLAY=on`)

## Rebrand / migration

- [ ] Fresh start uses `~/.config/newaqemu/newaqemu.conf`
- [ ] Legacy `~/.config/aqemu/AQEMU.conf` import prompt works
- [ ] Loads existing `.aqemu` VM XML (root tag `AQEMU`)
- [ ] Saves new VMs with root tag `newaqemu`

## Direct QEMU VM

- [ ] Create typical Linux VM, start/stop via QEMU
- [ ] KVM module check still works on Linux
- [ ] USB Connect → Disconnect from emulator menu (no manual bus address)
- [ ] Emulator control window restores geometry after restart
- [ ] Minimize to tray / close to tray (Advanced Settings)

## Embedded display

- [ ] VNC VM: embedded display scales and reconnects
- [ ] SPICE VM: “Open SPICE viewer” launches remote-viewer

## Sound

- [ ] Advanced Settings → host sound driver list populated from QEMU probe
- [ ] QEMU 8+ command line includes `-audiodev` when HDA enabled

## Tooltips

- [ ] Hover main window actions and advanced settings tray/audio options

## GPU passthrough (Linux)

- [ ] Advanced Settings → GPU passthrough shows IOMMU/KVM/kvmfr status
- [ ] Copy diagnostics to clipboard
- [ ] `pkexec newaqemu-host-setup --gpu-bdf …` writes templates (requires root)
- [ ] Wizard “GPU passthrough” creates `Backend=libvirt_passthrough` VM
- [ ] Start passthrough VM calls `virsh define` + `virsh start` (requires libvirt)
- [ ] Looking Glass client launch on start when enabled

## Windows

- [ ] Builds with Qt6 MSVC or MinGW
- [ ] Wizard + start/stop VM
- [ ] USB device list in emulator control
- [ ] Tray minimize/close
- [ ] Embedded VNC (optional LibVNC build)

## Regression

- [ ] Translations compile when `UPDATE_TRANSLATIONS=off`
