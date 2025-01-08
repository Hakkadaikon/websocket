{ pkgs ? import <nixpkgs> {} }:

let
  openssl = pkgs.openssl;
in pkgs.stdenv.mkDerivation {
  pname = "ws-server";
  version = "1.0.0";

  src = ./.;

  buildInputs = [
    pkgs.openssl
  ];

  CFLAGS = "
    -Ofast \
    -mtune=native \
    -pthread \
    -fno-stack-protector \
    -fomit-frame-pointer \
    -funroll-loops \
    -flto \
    -fno-asynchronous-unwind-tables
  ";

  LDFLAGS = "
    -flto \
    -pthread
  ";

  LDLIBS = "
    -lpthread \
    -lssl \
    -lcrypto
  ";

  buildPhase = ''
    mkdir -p build
    gcc $CFLAGS \
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
    homepage = "https://example.com";
    license = licenses.mit;
    maintainers = with maintainers; [ your_github_handle ];
  };
}

