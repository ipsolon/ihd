docker run --net=host --env="DISPLAY" -e LOCAL_UID=$(id -u $USER) -e LOCAL_GID=$(id -g $USER) \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw" --privileged --privileged -v /dev/bus/usb/:/dev/bus/usb/ \
  --device /dev/snd --mount type=bind,source="$(pwd)"/../../../ihd,target=/ihd --group-add=audio \
  -it ubuntu:gnuradio-releases-latest bash
