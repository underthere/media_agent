bash docker/build_image.sh linux/amd64 media-agent-dev
docker rm -f media-agent-dev || true
docker run -it --name media-agent-dev --workdir=/workdir -v /dev:/dev --device /dev/dri/renderD128 -v $PWD:/workdir media-agent-dev
