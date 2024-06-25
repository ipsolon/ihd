FROM ubuntu:22.04
LABEL maintainer="Joshua Meyers"

RUN apt-get update

# else it will output an error about Gtk namespace not found
RUN apt-get install -y gir1.2-gtk-3.0

# to have add-apt-repository available
RUN apt-get install -y software-properties-common

# install sudo
RUN apt-get install -y sudo

# installing other packages needed for downloading and installing OOT modules
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y gnuradio gnuradio-dev cmake git libboost-all-dev libcppunit-dev liblog4cpp5-dev python3-pygccxml pybind11-dev liborc-dev python3-setuptools

# Install ettus uhd usrp drivers
RUN apt-get install -y libuhd-dev uhd-host

# Install x11 apps to troubleshoot issues
RUN apt-get install -y x11-apps xterm

# of course, nothing useful can be done without vim
RUN apt-get install -y vim

# Install Wireshark (it asks for a 'yes or no', let non-root users run wireshark
RUN echo "wireshark-common wireshark-common/install-setuid boolean true" | debconf-set-selections
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y wireshark wireshark-dev

# Dev libraries needed by UHD (found when compiling wireshark disector)
RUN apt-get install -y libgcrypt-dev zlib1g-dev libgnutls28-dev

# Install debugger
RUN apt-get install -y gdb

#install ifconfig
RUN apt-get install -y net-tools

#install the wireshark dissector for CHDR/RFnoc headers
RUN git clone https://github.com/EttusResearch/uhd.git && \
    cd uhd/tools/dissectors && \
    git checkout v4.1.0.5 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && make install

# Setup entry point script to take care of USERID, GROUPID, and whatever else needs to happen on startup
RUN apt-get -y install gosu
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

ENV PYTHONPATH "${PYTHONPATH}:/usr/local/lib/python3/dist-packages"

CMD bash
