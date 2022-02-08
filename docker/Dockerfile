FROM ubuntu:18.04

RUN apt-get update
RUN apt-get -y install build-essential
RUN apt-get -y install apache2 libapr1-dev apache2-dev
RUN apt-get -y install wget sudo
RUN wget -qO- http://bit.ly/ngrest | bash
RUN touch ~/.ngrest/ngrest/CMakeLists.txt; make -C ~/.ngrest/ngrest-build

RUN cp ~/.ngrest/ngrest/modules/apache2/conf/ngrest.* /etc/apache2/mods-available/
COPY ngrest.load /etc/apache2/mods-available/ngrest.load
COPY ngrest.conf /etc/apache2/mods-available/ngrest.conf
RUN a2enmod ngrest
COPY ports.conf /etc/apache2/ports.conf

RUN mkdir /usr/src/projects
WORKDIR /usr/src/projects
RUN apt-get -y install inotify-tools

RUN chmod go+rx /root
EXPOSE 80
EXPOSE 9098

RUN a2enmod headers

ENTRYPOINT [ "/bin/bash" ]
