{ cmake, pkg-config, nix, boost, stdenv, nlohmann_json }:
stdenv.mkDerivation {
  name = "nix-build-results";
  src = ./.;
  nativeBuildInputs = [
    cmake
    pkg-config
  ];
  buildInputs = [
    nix
    boost
    nlohmann_json
  ];
}
