FROM debian:bookworm-slim AS build-dev
WORKDIR /opt/websocket
COPY . /opt/websocket
RUN \
  apt update && \
  apt install -y \
  cmake \
  make
RUN \
  cmake -S /opt/websocket -B /opt/websocket/build -DCMAKE_BUILD_TYPE=Release && \
  cmake --build /opt/websocket/build && \
  make BUILD=release -C /opt/websocket/examples/echoback

FROM scratch AS runtime
WORKDIR /opt/websocket

COPY --from=build-dev /opt/websocket/examples/echoback/bin/wsserver /opt/websocket/examples/echoback/bin/

ENTRYPOINT ["/opt/websocket/examples/echoback/bin/wsserver"]
