
FFMPEG_GIT_REPO=https://github.com/FFmpeg/FFmpeg.git
FFMPEG_GIT_TAG=n5.1.3
FFMPEG_RK_GIT_REPO=https://github.com/jjm2473/ffmpeg-rk.git
FFMPEG_RK_GIT_TAG=enc-5.1

if [ "$ROCKCHIP" == "1"]; then
    echo "not support rockchip"
else
    git clone -b $FFMPEG_GIT_TAG $FFMPEG_GIT_REPO ffmpeg
    pushd ffmpeg
    ./configure --enable-debug=3 --enable-gpl --enable-nonfree --enable-shared --enable-pthreads --enable-libx264 --enable-libx265
    make -j`nproc` && make install
    popd
    rm -rf ffmpeg
    echo "install ffmpeg success"
fi
