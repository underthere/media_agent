script_dir="$(dirname "${BASH_SOURCE[0]}")"

CURRENT_DIR=$(cd "$script_dir" || exit; pwd)

export DYLD_LIBRARY_PATH=$CURRENT_DIR/thirdparty/ffmpeg_build/lib:$DYLD_LIBRARY_PATH
export LD_LIBRARY_PATH=$CURRENT_DIR/thirdparty/ffmpeg_rk_build/lib:$LD_LIBRARY_PATH
export PATH=$CURRENT_DIR/thirdparty/ffmpeg_build/bin:$PATH
