{
  description = "wsserver flake: build a WebSocket server library with CMake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        wsserverRelease = pkgs.stdenv.mkDerivation {
          pname = "wsserver-release";
          version = "2.1.3";
          src = self;
          nativeBuildInputs = [ pkgs.cmake pkgs.gnumake ];

          buildPhase = ''
            cd $TMPDIR
            cmake -S $src -B build -DCMAKE_BUILD_TYPE=Release
            cmake --build build
          '';

          installPhase = ''
            mkdir -p $out/lib/ $out/include/
            cp $TMPDIR/build/lib/libwsserver.a $out/lib/
            cp $src/src/websocket/websocket.h $out/include/
          '';

          meta = with pkgs.lib; {
            description = "WebSocket server library";
            license = licenses.mit;
            platforms = platforms.linux;
          };
        };

        wsserverDebug = pkgs.stdenv.mkDerivation {
          pname = "wsserver-debug";
          version = "2.1.3";
          src = self;
          nativeBuildInputs = [ pkgs.cmake pkgs.gnumake ];

          buildPhase = ''
            cd $TMPDIR
            cmake -S $src -B build -DCMAKE_BUILD_TYPE=Debug
            cmake --build build
          '';

          installPhase = ''
            mkdir -p $out/lib/ $out/include/
            cp $TMPDIR/build/lib/libwsserver.a $out/lib/
            cp $src/src/websocket/websocket.h $out/include/
          '';

          meta = with pkgs.lib; {
            description = "WebSocket server library (debug)";
            license = licenses.mit;
            platforms = platforms.linux;
          };
        };
      in {
        packages.release = wsserverRelease;
        packages.debug = wsserverDebug;
        packages.default = wsserverRelease;

        devShell = pkgs.mkShell {
          buildInputs = [ pkgs.cmake pkgs.gnumake ];
        };
      }
    );
}

