{
  description = "wsserver flake: build a WebSocket server with CMake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        wsserver = pkgs.stdenv.mkDerivation {
          pname = "wsserver";
          version = "2.1.4";
          src = ./.;

          nativeBuildInputs = [ pkgs.cmake pkgs.gnumake ];

          buildPhase = ''
            rm -rf ./build
            make clean -C examples/echoback
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
            cmake --build build
            make BUILD=release -C examples/echoback
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp examples/echoback/bin/wsserver $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "WebSocket server built with CMake (static linking)";
            license = licenses.mit;
            platforms = platforms.linux;
          };
        };
      in {
        packages.default = wsserver;
        packages.wsserver = wsserver;

        devShell = pkgs.mkShell {
          buildInputs = [ pkgs.cmake pkgs.gnumake ];
        };
      }
    );
}

