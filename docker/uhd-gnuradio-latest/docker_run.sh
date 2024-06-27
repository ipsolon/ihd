#!/bin/bash

MOUNT_UHD=""
if [ -z "$1" ]; then
    echo "Provide path to mount uhd source if desired"
else
  if [ -d "$1" ]; then
    MOUNT_UHD='--mount type=bind,source='$1',target=/opt/uhd'
    echo "Mount UHD source: $MOUNT_UHD"
  fi
fi

docker run --net=host --env="DISPLAY" -e LOCAL_UID=$(id -u $USER) -e LOCAL_GID=$(id -g $USER) \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw" --privileged --privileged -v /dev/bus/usb/:/dev/bus/usb/ \
  --device /dev/snd \
  --mount type=bind,source="$(pwd)"/../../../ihd,target=/ihd \
  ${MOUNT_UHD} \
  --group-add=audio \
  -p 49152:49152/udp \
  -it ubuntu:gnuradio-releases-latest bash
