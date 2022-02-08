# Dockerfile for ngrest

This directory provides a docker file for building a container capable of compiling and running ngrest applications.

## Building and developing

The file [dockerBuild.sh](dockerBuild.sh) provides the command for building the docker container, while [dockerRun.sh](dockerRun.sh) provides instructions for starting the container.

The directory `/usr/src/projects` should be used as base for your ngrest application.
Special attention has to be given to given to the `-v` option.
This option specifies the source ngrest directory mounted into the docker container.
Just replace `/media/lukas/LUKAS_ESSD_EXT4/programming/cpp/unmangler` with the path to your source code directory on your host machine.

Once you start your container, a shell should open and you should be inside the directory `/usr/src/projects`.
From there you can execute the usual `ngrest` commands.

Your application should be reachable via `http://localhost:9098/<insert your application name>/<insert your endpoint>`.

## Production environment

This container can be used as base for your application's production container. An example docker file [can be found here.](https://github.com/agentS/UnmanglerBackend/blob/master/Dockerfile)

The line `FROM ngrest` makes the production container inherit from the base container.

The following lines copy the source code into the production container as well as build and deploy the application.
    COPY . /usr/src/projects
    RUN ngrest clean
    RUN ngrest build
    RUN cp .ngrest/local/build/services/unmangler.so /root/.ngrest/ngrest-build/deploy/share/ngrest/services

The line `ENTRYPOINT [ "/usr/sbin/apache2ctl", "-D", "FOREGROUND" ]` starts the Apache 2 web server in the foreground of the container.
This prevents docker from immediately stopping the container once the web server was started.
