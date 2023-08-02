import os
from pathlib import Path

CURRENT_FILE_PATH = Path(__file__).absolute()
CURRENT_DIR = CURRENT_FILE_PATH.parent


FFMPEG_REPO = CURRENT_DIR / "thirdparty/FFmpeg"
FFMPEG_BUILD_DIR = CURRENT_DIR / "thirdparty/ffmpeg_build"

FFMPEG_RK_REPO = CURRENT_DIR / "thirdparty/ffmpeg-rk"
FFMPEG_RK_BUILD_DIR = CURRENT_DIR / "thirdparty/ffmpeg_rk_build"


def build_ffmpeg():
    if (len(os.environ.get("ROCKCHIP", "")) > 0):
        ffmpeg_build_cmd = f"cd {FFMPEG_RK_REPO} && ./configure --enable-nonfree --enable-gpl --enable-version3 --enable-libx264 --enable-libdrm --enable-rkmpp --enable-librtmp --enable-shared --enable-static --enable-librga --enable-libx265 --prefix={FFMPEG_RK_BUILD_DIR} && make -j4 && make install"
    else:
        ffmpeg_build_cmd = f"cd {FFMPEG_REPO} && ./configure --enable-gpl --enable-nonfree --enable-shared --enable-pthreads --enable-libx264 --enable-libx265 --prefix={FFMPEG_BUILD_DIR} && make && make install"
    os.system(ffmpeg_build_cmd)

build_ffmpeg()

