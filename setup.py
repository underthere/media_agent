import os
from pathlib import Path

CURRENT_FILE_PATH = Path(__file__).absolute()
CURRENT_DIR = CURRENT_FILE_PATH.parent


FFMPEG_REPO = CURRENT_DIR / "thirdparty/FFmpeg"
FFMPEG_BUILD_DIR = CURRENT_DIR / "thirdparty/ffmpeg_build"


def build_ffmpeg():
    os.system(
        f"cd {FFMPEG_REPO} && ./configure --enable-gpl --enable-nonfree --enable-shared --enable-pthreads --enable-libx264 --enable-libx265 --prefix={FFMPEG_BUILD_DIR} && make && make install",
    )

build_ffmpeg()

