#!/bin/bash
rebuild=${1:-false}
if [ "$rebuild" = "true" ]; then
    bash docker/build_image.sh linux/amd64 media-agent-dev
fi
docker rm -f media-agent-dev || true
docker run -it --name media-agent-dev --workdir=/workdir -v /dev:/dev --device /dev/dri/renderD128 -v $PWD:/workdir media-agent-dev
