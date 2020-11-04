#! env /bin/bash

sudo docker run --rm -it -p 80:80 -p 9098:9098 -v /media/lukas/LUKAS_ESSD_EXT4/programming/cpp/unmangler:/usr/src/projects --name=ngrest ngrest
