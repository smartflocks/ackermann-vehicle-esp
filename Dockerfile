FROM espressif/idf:release-v5.1

ENV DEBIAN_FRONTEND noninteractive

RUN echo "Set disable_coredump false" >> /etc/sudo.conf

RUN apt update -q && \
    apt install -yq sudo lsb-release gosu nano && \
    rm -rf /var/lib/apt/lists/*

