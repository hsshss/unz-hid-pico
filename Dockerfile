FROM ubuntu:22.04

SHELL ["/bin/bash", "-c"]

ENV APP_ROOT /app
ENV PICO_SDK_PATH /pico-sdk

RUN apt-get update && \
    apt-get install -y \
      git \
      python3 \
      cmake \
      build-essential \
      gcc-arm-none-eabi \
      libnewlib-arm-none-eabi && \
    rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --single-branch --branch 1.5.0 https://github.com/raspberrypi/pico-sdk.git /pico-sdk && \
    cd ${PICO_SDK_PATH} && \
    git submodule update --init --recommend-shallow --depth 1

WORKDIR ${APP_ROOT}

COPY . ${APP_ROOT}
