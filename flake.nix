{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-22.11";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = inputs: inputs.flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import inputs.nixpkgs {
        inherit system;
      };
      nix-build-results = pkgs.stdenv.mkDerivation {
        name = "nix-build-results";
        src = ./.;
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.pkg-config
        ];
        buildInputs = [
          pkgs.nix
          pkgs.boost
        ];
      };
    in
    {
      legacyPackages = pkgs;
      packages = {
        inherit nix-build-results;
        default = nix-build-results;
      };
      devShells = {
        default = nix-build-results.overrideAttrs (old: {
          nativeBuildInputs = old.nativeBuildInputs ++ [
            pkgs.clang-tools
          ];
        });
      };
    });
}
