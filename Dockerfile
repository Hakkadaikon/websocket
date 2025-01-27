FROM debian:bookworm AS build-dev
WORKDIR /opt/websocket
RUN apt update && apt install -y cmake make git
COPY . /opt/websocket
RUN make native-build
RUN make -C examples/echoback

ENTRYPOINT ["/opt/websocket/examples/echoback/bin/wsserver"]
