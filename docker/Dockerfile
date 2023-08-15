ARG BASE_IMAGE=debian:11-slim
FROM ${BASE_IMAGE}

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
    libssl-dev \
    lsb-release \
    pkg-config \
    software-properties-common \
    gnupg \
    uuid-dev \
    zlib1g-dev \
    libssl-dev

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
RUN git clone -b ${WORKFLOW_VERSION} https://github.com/sogou/workflow.git && \
    pushd workflow && git submodule update --init --recursive && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) && \
    make install && popd && rm -rf workflow

# wfrest
ARG WFREST_VERSION=v0.9.3
RUN git clone -b ${WFREST_VERSION} https://github.com/wfrest/wfrest.git && \
    pushd wfrest && git submodule update --init --recursive && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) && \
    make install && popd && rm -rf wfrest

# async_simple
ARG ASYNC_SIMPLE_VERSION=1.1
RUN git clone -b ${ASYNC_SIMPLE_VERSION} https://github.com/alibaba/async_simple.git && \
    pushd async_simple && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DASYNC_SIMPLE_ENABLE_TESTS=OFF -DASYNC_SIMPLE_DISABLE_AIO=ON && \
    make -j $(nproc) &&  make install && popd && rm -rf async_simple

# spdlog
ARG SPDLOG_VERSION=v1.9.2
RUN git clone -b ${SPDLOG_VERSION} https://github.com/gabime/spdlog.git && \
    pushd spdlog && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) &&  make install && popd && rm -rf spdlog

# nlohmann_json
ARG NLOHMANN_JSON_VERSION=v3.11.2
RUN git clone -b ${NLOHMANN_JSON_VERSION} https://github.com/nlohmann/json.git && \
    pushd json && git submodule update --init --recursive && \
    mkdir build && cd build && \
    CXXFLAGS="" CXX=""  cmake .. -DCMAKE_BUILD_TYPE=Release && \
    CXXFLAGS="" CXX=""  make -j $(nproc) &&  make install && popd && rm -rf json

# fmt
ARG FMT_VERSION=10.0.0
RUN git clone -b ${FMT_VERSION} https://github.com/fmtlib/fmt.git && \
    pushd fmt && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) &&  make install && popd && rm -rf fmt

# tl::expected
ARG TL_EXPECTED_VERSION=v1.1.0
RUN git clone -b ${TL_EXPECTED_VERSION} https://github.com/TartanLlama/expected.git && \
    pushd expected && git submodule update --init --recursive && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j $(nproc) &&  make install && popd && rm -rf expected

COPY scripts scripts
RUN bash scripts/install_ffmpeg.sh

ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

ENV http_proxy=""
ENV https_proxy=""
ENV HTTP_PROXY=""
ENV HTTPS_PROXY=""

ENV IN_DOCKER=1
