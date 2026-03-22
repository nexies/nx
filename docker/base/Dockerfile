FROM ubuntu:20.04
LABEL authors="nexie"

RUN apt update && \
    apt install tzdata

# Настройка временной зоны
RUN echo "Etc/UTC" > /etc/timezone && \
    dpkg-reconfigure -f noninteractive tzdata

RUN apt install -y \
    g++ \
    make \
    binutils \
    build-essential \
    cmake \
    git \
    libboost-system-dev \
    libssl-dev \
    zlib1g-dev \
    libcurl4-openssl-dev \
    libfmt-dev \
    libspdlog-dev

RUN apt clean

# tgbot-cpp
RUN git clone https://github.com/reo7sp/tgbot-cpp /tgbot-cpp && \
    cd /tgbot-cpp && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr . && \
    cmake --build . -j`nproc` && \
    cmake --install .

## spdlog
#RUN git clone https://github.com/gabime/spdlog.git && \
#    cmake -DCMAKE_INSTALL_PREFIX=/usr . && \
#    cmake --build . -j`nproc` && \
#    cmake --install .

# Установка рабочего каталога
WORKDIR /tgbot-cpp
