{ pkgs ? import <nixpkgs> {} }:
let
  libs = with pkgs; [
    boost
    cmake
    clang
    clang-tools
    clang-tidy-sarif
    clang-analyzer
    gcc
    lld
    pkg-config
    gdb
    valgrind
  ];
  lib = libs;
in
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cppcheck
    clang-tools
    codespell
    conan
    doxygen
    lcov
  ];
  buildInputs = libs;
  # XDG_DATA_DIRS = builtins.getEnv "XDG_DATA_DIRS";
  # XDG_RUNTIME_DIRS = "/run/user/1001/";
  shellHook = ''
    # Ensure proper environment setup for GCC and glibc
    # Correct the CXXFLAGS by removing `.dev` from the GCC path
    export CXXFLAGS="-I${pkgs.gcc}/include/c++/${pkgs.gcc.version} -I${pkgs.glibc}/include"

    export CC=clang
    export CXX=clang++
  '';
}
