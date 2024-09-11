#!/bin/bash

MOUNT_OPT=""
if [ -z "$1" ]; then
    echo "Provide path to mount a directory on opt if desired"
else
  if [ -d "$1" ]; then
    MOUNT_OPT='--mount type=bind,source='$1',target=/opt'
    echo "Mount UHD source: $MOUNT_OPT"
  fi
fi

echo "The ostype is: $OSTYPE"
if [[ "$OSTYPE" == "msys"* ]]; then
  # Windows want winpty, else leave the variable as non-existent
  WINDOWS_RUN=winpty
fi

CONTAINER_NAME="gnuradio-releases-latest_$USER"
 CID=$(docker ps -aq -f name=^/${CONTAINER_NAME}$)
if [ ! "${CID}" ]; then
  echo "Container ${CONTAINER_NAME} does not exist, create it";
  $WINDOWS_RUN docker run --net=host --env="DISPLAY" -e LOCAL_UID=$(id -u $USER) -e LOCAL_GID=$(id -g $USER) \
    --volume="$HOME/.Xauthority:/root/.Xauthority:rw" --privileged --privileged -v /dev/bus/usb/:/dev/bus/usb/ \
    --mount type=bind,source="$(pwd)"/../../../ihd,target=/ihd \
    ${MOUNT_OPT} \
    --group-add=audio \
    --name $CONTAINER_NAME \
    -it ubuntu:gnuradio-releases-latest bash
else
  echo "Container ${CONTAINER_NAME} does exist, start and run it";
  docker start ${CONTAINER_NAME}
  docker attach ${CONTAINER_NAME}
fi
unset CID
unset CONTAINER_NAME
