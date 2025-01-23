{ pkgs ? import <nixpkgs> {}, debug ? false }:

let
  isLinux = pkgs.stdenv.isLinux;
  isDarwin = pkgs.stdenv.isDarwin;

  compiler = 
    if isLinux then 
      pkgs.gcc 
    else if isDarwin then 
      pkgs.clang 
    else 
      pkgs.gcc;
in pkgs.stdenv.mkDerivation {
  pname = "ws-server";
  version = "1.0.0";

  src = ./.;

  buildInputs = [
    compiler
  ];

  CFLAGS =
    if debug then ''
      -O0
      -static-libasan
      -g
      -DLOG_LEVEL_DEBUG
    '' else ''
      -O3
      -mtune=native
      -ffast-math
      -fno-math-errno
      -falign-functions
      -flto=auto
      -DLOG_LEVEL_ERROR
    '';

  LDFLAGS = ''
    -flto
  '';

  LDLIBS = ''
  '';

  buildPhase = ''
    mkdir -p build
    ${compiler}/bin/cc $CFLAGS \
      src/main.c \
      src/websocket/server/loop/loop.c \
      src/websocket/server/init.c \
      src/websocket/server/handshake.c \
      src/websocket/parser.c \
      src/websocket/crypto.c \
      src/websocket/socket/accept.c \
      src/websocket/socket/listen.c \
      src/websocket/socket/recv.c \
      src/websocket/socket/send.c \
      src/websocket/socket/epoll.c \
      src/websocket/socket/close.c \
      src/websocket/log.c \
      src/http/http.c \
      src/util/signal.c \
      src/crypto/sha1.c \
      src/util/log.c \
      -o \
      build/$pname \
      $LDLIBS \
      $LDFLAGS
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp build/$pname $out/bin/
  '';

  meta = with pkgs.lib; {
    description = "websocket server";
    homepage = "https://nostter.app/hakkadaikon@iris.to";
    license = licenses.mit;
    maintainers = with maintainers; [ Hakkadaikon ];
  };
}

