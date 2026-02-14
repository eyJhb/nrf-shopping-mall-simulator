{
  pkgs ?
    import
      (fetchTarball {
        url = "https://github.com/NixOS/nixpkgs/archive/2343bbb58f99267223bc2aac4fc9ea301a155a16.tar.gz";
        sha256 = "sha256-LovWTGDwXhkfCOmbgLVA10bvsi/P8eDDpRudgk68HA8=";
      })
      {
        config.allowUnfree = true;
        config.segger-jlink.acceptLicense = true;
        config.permittedInsecurePackages = [
          "segger-jlink-qt4-874"
        ];
      },
}:

pkgs.mkShell {
  packages = with pkgs; [
    cmake
    ninja

    # for flashing
    nrfutil
    nrfconnect
    nrf-command-line-tools
    nrf5-sdk

    (python3.withPackages (
      ps: with ps; [
        west

        # needed to build
        pyelftools
        jsonschema

        # needed for flash
        intelhex
      ]
    ))
  ];

  GNUARMEMB_TOOLCHAIN_PATH = pkgs.gcc-arm-embedded;
  ZEPHYR_TOOLCHAIN_VARIANT = "gnuarmemb";

  # export PATH="$ZEPHYR_BASE/scripts:$PATH"
  shellHook = ''
    export ZEPHYR_BASE="$(pwd)/zephyr"

    echo Welcome to the nRF clean shell!
  '';
}
