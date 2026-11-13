#!/usr/bin/env bash
# Ubuntu 22.04: download/extract only the needed libraries, without sudo/install.
set -eu
dependency_directory=${1:?Usage: linux_private_deps.sh ABSOLUTE_DIRECTORY}
case "$dependency_directory" in /*) ;; *) echo 'Use an absolute directory' >&2; exit 2 ;; esac
mkdir -p "$dependency_directory/packages"
cd "$dependency_directory/packages"
apt-get download libopencv-core-dev libopencv-imgproc-dev libopencv-core4.5d libopencv-imgproc4.5d libtbb2
for package_file in ./*.deb; do dpkg-deb -x "$package_file" "$dependency_directory"; done
printf 'OpenCV prefix: %s/usr\nLoader path: %s/usr/lib/x86_64-linux-gnu\n' "$dependency_directory" "$dependency_directory"
