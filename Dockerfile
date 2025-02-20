FROM debian:bookworm AS build-dev
WORKDIR /opt/websocket
COPY . /opt/websocket
RUN \
  apt update && \
  apt install -y \
  cmake \
  make
RUN \
  rm -rf /opt/websocket/build && \
  make clean -C /opt/websocket/examples/echoback && \
  cmake -S /opt/websocket -B /opt/websocket/build -DCMAKE_BUILD_TYPE=Release && \
  cmake --build /opt/websocket/build && \
  make BUILD=release -C /opt/websocket/examples/echoback

ENTRYPOINT ["/opt/websocket/examples/echoback/bin/wsserver"]
