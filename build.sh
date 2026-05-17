#!/bin/bash
set -e

BUILD_DIR="build"
INSTALL_PREFIX="$HOME/warcraft-server"

case "$1" in
  clean)
    rm -rf "$BUILD_DIR"
    ;;

  build|debug)
    mkdir -p "$BUILD_DIR"

    if [ "$1" = "debug" ]; then
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

    cmake --build "$BUILD_DIR" --target worldserver authserver -j8
    # cmake --build "$BUILD_DIR" -j8
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
    echo "Usage: $0 {build|debug|clean|install}"
    echo "  build   - configure and compile worldserver + authserver (Release, -O3, march=native)"
    echo "  debug   - same but Debug mode (-O0 -g, march=native)"
    echo "  clean   - remove build directory"
    echo "  install - install built binaries to $INSTALL_PREFIX (saves old binaries as *_old)"
    echo "  restore - restore previous binaries from *_old backups"
    ;;
esac
