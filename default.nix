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

  #buildInputs = [
  #  pkgs.openssl
  #];
  buildInputs = [
    compiler
  ];

  CFLAGS =
    if debug then ''
      -O0
      -pthread
      -static-libasan
      -g
    '' else ''
      -O3
      -pthread
      -mtune=native
      -ffast-math
      -fno-math-errno
      -falign-functions
      -flto=auto
    '';

  LDFLAGS = ''
    -flto
    -pthread
  '';

  LDLIBS = ''
    -lpthread
  '';

  buildPhase = ''
    mkdir -p build
    ${compiler}/bin/cc $CFLAGS \
      src/main.c \
      src/websocket/server/loop.c \
      src/websocket/server/init.c \
      src/websocket/server/handshake.c \
      src/websocket/server/func.c \
      src/websocket/parser.c \
      src/websocket/crypto.c \
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

