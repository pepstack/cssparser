{
  description = "A pure-C CSS parser forked from katana-parser.";

  outputs = { self, nixpkgs, ... }:
    let
      supportedSystems = [
        "aarch64-linux"
        "i686-linux"
        "riscv64-linux"
        "x86_64-linux"
        "x86_64-darwin"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in {
          default = pkgs.stdenv.mkDerivation {
            name = "cssparser";
            src = self;

            outputs = [ "out" "dev" ];
            enableParallelBuilding = true;
            nativeBuildInputs = with pkgs; [ autoconf automake libtool gnumake autoreconfHook ];

            meta = with pkgs.lib; {
              homepage = "https://github.com/pepstack/cssparser";
              license = with licenses; [ mit ];
              maintainers = [ "Tristan Ross" "pepstack" ];
            };
          };
        });

      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            packages = with pkgs; [ autoconf automake libtool gnumake autoreconfHook ];
          };
        });
    };
}
