#!/bin/bash
set -e

BUILD_DIR="build"
INSTALL_PREFIX="$HOME/warcraft-server"

case "$1" in
  clean)
    rm -rf "$BUILD_DIR"
    ;;

  configure)
    mkdir -p "$BUILD_DIR"

    if [ "$2" = "--debug" ]; then
      BUILD_TYPE=Debug
      C_FLAGS="-march=native"
      CXX_FLAGS="-march=native"
    else
      BUILD_TYPE=Release
      C_FLAGS="-march=native -O3"
      CXX_FLAGS="-march=native -O3"
    fi

    cmake -S . -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_C_FLAGS="$C_FLAGS" \
      -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      -DTOOLS=0
    ;;

  build)
    if [ ! -d "$BUILD_DIR" ]; then
      echo "Error: build directory '$BUILD_DIR' does not exist. Run 'configure' first." >&2
      exit 1
    fi
    cmake --build "$BUILD_DIR" --target worldserver authserver -j"$(nproc)"
    ;;

  install)
    mkdir -p "$INSTALL_PREFIX/bin"
    if [ -f "$INSTALL_PREFIX/bin/worldserver" ]; then
      mv "$INSTALL_PREFIX/bin/worldserver" "$INSTALL_PREFIX/bin/worldserver_old"
      echo "Backed up worldserver -> worldserver_old"
    fi
    if [ -f "$INSTALL_PREFIX/bin/authserver" ]; then
      mv "$INSTALL_PREFIX/bin/authserver" "$INSTALL_PREFIX/bin/authserver_old"
      echo "Backed up authserver -> authserver_old"
    fi
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX"
    ;;

  restore)
    [ -f "$INSTALL_PREFIX/bin/worldserver_old" ] && mv -f "$INSTALL_PREFIX/bin/worldserver_old" "$INSTALL_PREFIX/bin/worldserver"
    [ -f "$INSTALL_PREFIX/bin/authserver_old" ]  && mv -f "$INSTALL_PREFIX/bin/authserver_old" "$INSTALL_PREFIX/bin/authserver"
    ;;

  *)
    echo "Usage: $0 {configure|build|clean|install|restore}"
    echo "  configure [--debug]  - configure build tree (Release by default, Debug with --debug)"
    echo "  build                - compile worldserver + authserver (run configure first)"
    echo "  clean                - remove build directory"
    echo "  install              - install built binaries to $INSTALL_PREFIX (saves old binaries as *_old)"
    echo "  restore              - restore previous binaries from *_old backups"
    ;;
esac
