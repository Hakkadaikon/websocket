{ pkgs ? import <nixpkgs> {}, debug ? false }:

let
in pkgs.stdenv.mkDerivation {
  pname = "libwsserver";
  version = "1.0.0";

  src = ./.;

  buildInputs = [
  ];

  buildPhase = if debug then ''
    make native-debug-build
  '' else ''
    make native-build
  '';

  installPhase = ''
    mkdir -p $out/lib
    cp native/lib/$pname.a $out/lib/
  '';

  meta = with pkgs.lib; {
    description = "websocket server library";
    homepage = "https://nostter.app/hakkadaikon@iris.to";
    license = licenses.mit;
    maintainers = with maintainers; [ Hakkadaikon ];
  };
}
