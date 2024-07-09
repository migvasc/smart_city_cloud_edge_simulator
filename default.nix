{
  pkgs ? import (fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/63dacb46bf939521bdc93981b4cbb7ecb58427a0.tar.gz";
    sha256 = "sha256:1lr1h35prqkd1mkmzriwlpvxcb34kmhc9dnr48gkm8hh089hifmx";
  }) {}
}:

with pkgs;

let
  packages = rec {

    # The derivation for chord

    nlohmann_json = stdenv.mkDerivation rec {
    pname = "nlohmann_json";
      version = "0.0.in-tuto";
      src = fetchgit {
        url = "https://github.com/nlohmann/json";
        rev = "9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03";
        sha256 = "sha256-7F0Jon+1oWL7uqet5i1IgHX0fUw/+z0QwEcA3zs5xHg=";
      };
      
      buildInputs = [
        boost
        cmake
      ];

    };

    smart_city_edge_simul = stdenv.mkDerivation rec {
      pname = "smart_city_edge_simul";
      version = "0.0.in-tuto";
      src = fetchgit {
        url = "https://github.com/migvasc/smart_city_cloud_edge_simulator";
        rev = "49e8cdae9687636102e3308661d96dd2f8fc945b";
        sha256 = "sha256-Si5wmrUz4rrC29HOqtGPHTg/JGm3HDHLuk3ws4r/kok=";
      };

      buildInputs = [
      nlohmann_json
        simgrid
        boost
        cmake
      ];

    };

    # The shell of our experiment runtime environment
    expEnv = mkShell rec {
      name = "exp01Env";
      buildInputs = [
        smart_city_edge_simul
      ];
    };

  };
in
  packages