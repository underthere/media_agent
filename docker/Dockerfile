ARG BASE_IMAGE=debian:11-slim
FROM ${BASE_IMAGE} as builder

ARG D_HTTP_PROXY=""
ARG D_HTTPS_PROXY=""

ENV http_proxy=$D_HTTP_PROXY
ENV https_proxy=$D_HTTPS_PROXY
ENV HTTP_PROXY=$D_HTTP_PROXY
ENV HTTPS_PROXY=$D_HTTPS_PROXY

# Install tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libaio-dev \
    wget \
    libva-dev \
    yasm nasm \
    libx264-dev \
    libx265-dev \
    librtmp-dev \
    libssl-dev \
    lsb-release \
    pkg-config \
    software-properties-common \
    gnupg \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libdrm-dev \
    meson \
    ninja-build

# llvm
ARG LLVM_VERSION=16
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    yes | ./llvm.sh 16 all && \
    rm llvm.sh

ENV CC=clang-${LLVM_VERSION}
ENV CXX=clang++-${LLVM_VERSION}
ENV CXXFLAGS="-stdlib=libc++ -glldb"

SHELL ["/bin/bash", "-c"]

# dependencies
# workflow
ARG WORKFLOW_VERSION=v0.10.8
RUN git clone -b ${WORKFLOW_VERSION} --single-branch https://github.com/sogou/workflow.git && \
    pushd workflow && git submodule update --init --recursive && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) && \
    make install && popd && rm -rf workflow

# wfrest
ARG WFREST_VERSION=v0.9.3
RUN git clone -b ${WFREST_VERSION} --single-branch https://github.com/wfrest/wfrest.git && \
    pushd wfrest && git submodule update --init --recursive && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) && \
    make install && popd && rm -rf wfrest

# async_simple
ARG ASYNC_SIMPLE_VERSION=1.1
RUN git clone -b ${ASYNC_SIMPLE_VERSION} --single-branch https://github.com/alibaba/async_simple.git && \
    pushd async_simple && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DASYNC_SIMPLE_ENABLE_TESTS=OFF -DASYNC_SIMPLE_DISABLE_AIO=ON && \
    make -j $(nproc) &&  make install && popd && rm -rf async_simple

# spdlog
ARG SPDLOG_VERSION=v1.9.2
RUN git clone -b ${SPDLOG_VERSION} --single-branch https://github.com/gabime/spdlog.git && \
    pushd spdlog && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) &&  make install && popd && rm -rf spdlog

# nlohmann_json
ARG NLOHMANN_JSON_VERSION=v3.11.2
RUN git clone -b ${NLOHMANN_JSON_VERSION} --single-branch https://github.com/nlohmann/json.git && \
    pushd json && git submodule update --init --recursive && \
    mkdir build && cd build && \
    CXXFLAGS="" CXX=""  cmake .. -DCMAKE_BUILD_TYPE=Release -DJSON_BuildTests=OFF && \
    CXXFLAGS="" CXX=""  make -j $(nproc) &&  make install && popd && rm -rf json

# fmt
ARG FMT_VERSION=10.0.0
RUN git clone -b ${FMT_VERSION} --single-branch https://github.com/fmtlib/fmt.git && \
    pushd fmt && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DFMT_TEST=OFF -DFMT_FUZZ=OFF && \
    make -j $(nproc) &&  make install && popd && rm -rf fmt

# tl::expected
ARG TL_EXPECTED_VERSION=v1.1.0
RUN git clone -b ${TL_EXPECTED_VERSION} --single-branch https://github.com/TartanLlama/expected.git && \
    pushd expected && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DEXPECTED_BUILD_TESTS=OFF && \
    make -j $(nproc) &&  make install && popd && rm -rf expected

ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# rockchip multimedia libs
ARG ROCKCHIP
ENV ROCKCHIP=${ROCKCHIP}
RUN if [ "${ROCKCHIP}" == "1" ]; then \
        git clone --single-branch https://github.com/jjm2473/librga.git && \
        pushd librga && \
        meson build && \
        cd build && \
        ninja install && \
        popd  && rm -rf librga; \
    fi
RUN if [ "${ROCKCHIP}" == "1" ]; then \
        git clone --single-branch https://github.com/rockchip-linux/mpp.git && \
        mkdir mpp/cmake-build && \
        pushd mpp/cmake-build && \
        cmake .. && make -j`nproc` && make install && \
        popd && rm -rf mpp; \
    fi

# ffmpeg
ARG FFMPEG_GIT_TAG=release/5.1
ARG FFMPEG_RK_GIT_TAG=enc-5.1

RUN if [ "${ROCKCHIP}"  == "1" ]; then \
    git clone -b $FFMPEG_RK_GIT_TAG --single-branch https://github.com/jjm2473/ffmpeg-rk.git ffmpeg && \
    pushd ffmpeg && git submodule update --init --recursive && \
    ./configure --enable-nonfree --enable-gpl --enable-version3 --enable-libx264 --enable-libdrm --enable-rkmpp --enable-librtmp --enable-shared --enable-static --enable-librga --enable-libx265 --prefix=/usr/local && \
    make -j`nproc` && make install && \
    popd && rm -rf ffmpeg; \
else \
    git clone -b $FFMPEG_GIT_TAG --single-branch https://github.com/FFmpeg/FFmpeg.git ffmpeg && \
    pushd ffmpeg && git submodule update --init --recursive && \
    ./configure --enable-gpl --enable-nonfree --enable-libdrm --enable-version3 --enable-librtmp --enable-static --enable-shared --enable-pthreads --enable-libx264 --enable-libx265 --prefix=/usr/local && \
    make -j`nproc` && make install && \
    popd && rm -rf ffmpeg; \
fi

# libdatachannel
ARG LIBDATACHANNEL_GIT_TAG=v0.18.5
RUN git clone -b $LIBDATACHANNEL_GIT_TAG --single-branch https://github.com/paullouisageneau/libdatachannel.git && \
    pushd libdatachannel && git submodule update --init --recursive --depth 1 && \
    pushd deps/usrsctp && git checkout master && popd && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_GNUTLS=0 -DUSE_NICE=0 && \
    make -j $(nproc) &&  make install && popd && rm -rf libdatachannel

RUN ln -s /usr/bin/clangd-16 /usr/bin/clangd

ENV http_proxy=""
ENV https_proxy=""
ENV HTTP_PROXY=""
ENV HTTPS_PROXY=""

ENV IN_DOCKER=1

# compile && install media agent
COPY .. /opt/media_agent
RUN cmake -S /opt/media_agent -B /opt/media_agent/build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build /opt/media_agent/build --target install && \
    rm -rf /opt/media_agent


FROM ${BASE_IMAGE} as dist
ENV http_proxy=$D_HTTP_PROXY
ENV https_proxy=$D_HTTPS_PROXY
ENV HTTP_PROXY=$D_HTTP_PROXY
ENV HTTPS_PROXY=$D_HTTPS_PROXY

# Install tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libaio-dev \
    wget \
    libva-dev \
    yasm nasm \
    libx264-dev \
    libx265-dev \
    librtmp-dev \
    libssl-dev \
    lsb-release \
    pkg-config \
    software-properties-common \
    gnupg \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libdrm-dev \
    meson \
    ninja-build

COPY --from=builder /usr/local /usr/local
COPY --from=builder /usr/lib/llvm-16/lib/libc++* /usr/local/lib/
COPY --from=builder /usr/lib/llvm-16/lib/libunwind* /usr/local/lib/
ENV http_proxy=""
ENV https_proxy=""
ENV HTTP_PROXY=""
ENV HTTPS_PROXY=""


ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
ENV IN_DOCKER=1

CMD /usr/local/bin/media_agent --legacy-preload /kd_config/preload.json
