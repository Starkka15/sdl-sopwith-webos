#!/bin/bash
# Configure script for SDL Sopwith webOS build with CodeSourcery toolchain

TOOLCHAIN_ROOT=/home/stark/arm-2011.03
PDK_ROOT=/home/stark/HPwebOS/PDK

export CC="${TOOLCHAIN_ROOT}/bin/arm-none-linux-gnueabi-gcc"
export CXX="${TOOLCHAIN_ROOT}/bin/arm-none-linux-gnueabi-g++"
export PKG_CONFIG_PATH=""
export PKG_CONFIG_LIBDIR=""

# SDL2 flags - adjust if you have SDL2 compat layer elsewhere
SDL_CFLAGS="-I${PDK_ROOT}/include -I${PDK_ROOT}/include/SDL"
SDL_LIBS="-L${PDK_ROOT}/device/lib -lSDL"

./configure \
    --host=arm-none-linux-gnueabi \
    --prefix=/media/internal/sopwith \
    --with-hiscores-path=/media/internal/sopwith \
    SDL_CFLAGS="${SDL_CFLAGS}" \
    SDL_LIBS="${SDL_LIBS}" \
    CFLAGS="-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=softfp -std=gnu89 -I${PDK_ROOT}/include -DWEBOS -D_GNU_SOURCE" \
    LDFLAGS="-L${PDK_ROOT}/device/lib -Wl,-rpath-link,${PDK_ROOT}/device/lib -Wl,--dynamic-linker=/lib/ld-linux.so.3 -lpdl -static-libgcc"
