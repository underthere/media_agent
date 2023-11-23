#!/bin/bash
script_dir="$(dirname "${BASH_SOURCE[0]}")"
CURRENT_DIR=$(cd "$script_dir"/.. || exit; pwd)

# 默认值
PLATFORM="linux/amd64"
IMAGE_NAME="media-agent-dev:latest"
ROCKCHIP=""
DIST=""

# 使用 getopts 解析命令行选项和参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --platform)
            shift
            PLATFORM="$1"
            shift
            ;;
        --image-name)
            shift
            IMAGE_NAME="$1"
            shift
            ;;
        --rockchip)
            ROCKCHIP="1"
            shift
            ;;
        --dist-name)
            shift
            DIST="$1"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if test -z "$HTTP_PROXY"; then
    D_HTTP_PROXY=""
else
    D_HTTP_PROXY="--build-arg D_HTTP_PROXY=$HTTP_PROXY"
fi

if test -z "$HTTPS_PROXY"; then
    D_HTTPS_PROXY=""
else
    D_HTTPS_PROXY="--build-arg D_HTTPS_PROXY=$HTTPS_PROXY"
fi

pushd "$CURRENT_DIR" || exit
docker buildx build ${D_HTTP_PROXY} ${D_HTTPS_PROXY} --build-arg ROCKCHIP="${ROCKCHIP}" --platform ${PLATFORM} -t ${IMAGE_NAME} -f docker/Dockerfile .
popd || exit
