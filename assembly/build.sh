#!/usr/bin/env bash
set -ex

script_dir="$(dirname "$0")"
toplvl_dir="$(realpath "$script_dir")"
rom_dir="$toplvl_dir/roms"
tool_dir="$toplvl_dir/scripts"

scripts/build.py --compile-c

if [[ "$OSTYPE" == "msys"* ]]; then
    cmd //c $tool_dir/MakePPF3 c $rom_dir/base.z64 $rom_dir/patched.z64 $rom_dir/redux.ppf
fi
