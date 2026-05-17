#!/usr/bin/env bash
# Install native packages needed to compile newaqemu (CMake + Qt6 + toolchain).
# Usage: install-build-deps.sh [--full] [--runtime]
#   --full     Also install LibVNCServer (for "make full")
#   --runtime  Also install QEMU (to run and test VMs after build)

set -euo pipefail

FULL=0
RUNTIME=0
for arg in "$@"; do
	case "$arg" in
		--full) FULL=1 ;;
		--runtime) RUNTIME=1 ;;
		-h|--help)
			echo "Usage: $0 [--full] [--runtime]"
			exit 0
			;;
		*)
			echo "Unknown option: $arg" >&2
			exit 1
			;;
	esac
done

run_privileged() {
	if [ "${EUID:-$(id -u)}" -eq 0 ]; then
		"$@"
	elif command -v sudo >/dev/null 2>&1; then
		sudo "$@"
	else
		echo "Need root or sudo to install packages." >&2
		exit 1
	fi
}

install_apt() {
	local pkgs=(
		cmake
		ninja-build
		build-essential
		qt6-base-dev
		qt6-tools-dev
		bzip2
		libvirt-dev
	)
	[ "$FULL" -eq 1 ] && pkgs+=(libvncserver-dev)
	[ "$RUNTIME" -eq 1 ] && pkgs+=(qemu-system-x86 qemu-utils)
	run_privileged apt-get update
	run_privileged apt-get install -y "${pkgs[@]}"
}

install_dnf() {
	local pkgs=(
		cmake
		ninja-build
		gcc-c++
		qt6-qtbase-devel
		qt6-qttools-devel
		bzip2
		libvirt-devel
	)
	[ "$FULL" -eq 1 ] && pkgs+=(libvncserver-devel)
	[ "$RUNTIME" -eq 1 ] && pkgs+=(qemu-kvm)
	run_privileged dnf install -y "${pkgs[@]}"
}

install_pacman() {
	local pkgs=(
		cmake
		ninja
		gcc
		qt6-base
		qt6-tools
		bzip2
		libvirt
	)
	[ "$FULL" -eq 1 ] && pkgs+=(libvncserver)
	[ "$RUNTIME" -eq 1 ] && pkgs+=(qemu-desktop)
	run_privileged pacman -Syu --needed --noconfirm "${pkgs[@]}"
}

install_zypper() {
	local pkgs=(
		cmake
		ninja
		gcc-c++
		qt6-base-devel
		qt6-tools-devel
		bzip2
		libvirt-devel
	)
	[ "$FULL" -eq 1 ] && pkgs+=(libvncserver-devel)
	[ "$RUNTIME" -eq 1 ] && pkgs+=(qemu qemu-tools)
	run_privileged zypper --non-interactive refresh
	run_privileged zypper --non-interactive install -y "${pkgs[@]}"
}

install_emerge() {
	local useflags=""
	[ "$FULL" -eq 1 ] && useflags="net-libs/libvncserver"
	local pkgs=(
		"dev-build/cmake"
		"dev-build/ninja"
		"dev-qt/qtbase:6"
		"dev-qt/qttools:6"
		"app-arch/bzip2"
		"app-emulation/libvirt"
	)
	[ "$RUNTIME" -eq 1 ] && pkgs+=("app-emulation/qemu")
	run_privileged emerge -av1 "${pkgs[@]}" ${useflags:+$useflags}
}

detect_pm() {
	if command -v apt-get >/dev/null 2>&1; then
		echo apt
	elif command -v dnf >/dev/null 2>&1; then
		echo dnf
	elif command -v zypper >/dev/null 2>&1; then
		echo zypper
	elif command -v pacman >/dev/null 2>&1; then
		echo pacman
	elif command -v emerge >/dev/null 2>&1; then
		echo emerge
	else
		echo unknown
	fi
}

pm="$(detect_pm)"
echo "==> Detected package manager: $pm"
[ "$FULL" -eq 1 ] && echo "==> Profile: full (embedded VNC)"
[ "$RUNTIME" -eq 1 ] && echo "==> Also installing QEMU (runtime)"

case "$pm" in
	apt) install_apt ;;
	dnf) install_dnf ;;
	zypper) install_zypper ;;
	pacman) install_pacman ;;
	emerge) install_emerge ;;
	*)
		cat >&2 <<'EOF'
Could not detect a supported package manager (apt, dnf, zypper, pacman, emerge).

Install manually, then run: make

  Debian/Ubuntu:
    sudo apt install cmake ninja-build build-essential qt6-base-dev qt6-tools-dev bzip2 libvirt-dev
    # full: libvncserver-dev
    # test: qemu-system-x86

  Fedora:
    sudo dnf install cmake ninja-build gcc-c++ qt6-qtbase-devel qt6-qttools-devel bzip2 libvirt-devel
    # full: libvncserver-devel

  openSUSE:
    sudo zypper install cmake ninja gcc-c++ qt6-base-devel qt6-tools-devel bzip2 libvirt-devel
    # full: libvncserver-devel

  Arch:
    sudo pacman -S cmake ninja gcc qt6-base qt6-tools bzip2 libvirt
EOF
		exit 1
		;;
esac

if ! command -v cmake >/dev/null 2>&1 && command -v cmake3 >/dev/null 2>&1; then
	echo "==> Note: use CMAKE=cmake3 if 'cmake' is not on PATH"
fi

echo "==> Build dependencies installed."
echo "    Next: make          # minimal build"
echo "           make full     # with embedded VNC (after --full deps)"
