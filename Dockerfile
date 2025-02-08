FROM debian:bookworm AS build-dev
WORKDIR /opt/websocket
RUN apt update && apt install -y cmake make git
COPY . /opt/websocket
RUN cmake -S /opt/websocket -B build -DCMAKE_BUILD_TYPE=Release
RUN make BUILD=release -C /opt/websocket/examples/echoback

ENTRYPOINT ["/opt/websocket/examples/echoback/bin/wsserver"]
