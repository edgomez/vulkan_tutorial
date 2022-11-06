#!/bin/sh

sd="$(dirname "$0")"
sd="$(cd "$sd" && pwd -P)"
root="$(cd "$sd/.." && pwd -P)"

cd "$root"
cxx_sources="$(git ls-files | grep -E '\.cpp$|\.h$')"
echo "$cxx_sources" | xargs clang-format -i
echo "$cxx_sources" | xargs dos2unix -q

script_sources="$(git ls-files | grep -E 'CMakeLists\.txt$|\.cmake$|\.sh$')"
echo "$script_sources" | xargs dos2unix -q
