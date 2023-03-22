{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-22.11";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = inputs: {
    overlays = {
      nix-build-results = final: prev: {
        nix-build-results = final.callPackage ./default.nix { };
      };
    };
    lib = {
      toDrvs = attrs: builtins.listToAttrs (
        map
          (name: let value = attrs.${name}.drvPath; in { inherit name value; })
          (builtins.attrNames attrs)
      );
    };
    overlay = inputs.self.overlays.nix-build-results;
  } // inputs.flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import inputs.nixpkgs {
        inherit system;
        overlays = [ inputs.self.overlay ];
      };
    in
    {
      legacyPackages = pkgs;
      packages = {
        inherit (pkgs) nix-build-results;
        drvs = inputs.self.lib.toDrvs {
          inherit (inputs.self.packages.${system}) test default;
        };
        default = pkgs.nix-build-results;
        test = pkgs.runCommand "test" { } ''
          cp ${./test.txt} $out
        '';
      };
      devShells = {
        default = pkgs.nix-build-results.overrideAttrs (old: {
          nativeBuildInputs = old.nativeBuildInputs ++ [
            pkgs.clang-tools
          ];
        });
      };
    });
}
