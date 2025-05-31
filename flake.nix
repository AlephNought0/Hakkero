# flake.nix (in your repo root)
{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = {
    self,
    nixpkgs,
  }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      nativeBuildInputs = with pkgs.buildPackages; [
        cmake
        ninja
        pkg-config
      ];

      buildInputs = with pkgs.buildPackages; [
        vulkan-headers
        vulkan-loader
        vulkan-tools
        vulkan-validation-layers
        glfw
        openal
        assimp
        entt
        glm
        shaderc
      ];

      LD_LIBRARY_PATH = "${pkgs.vulkan-loader}/lib";
      VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
    };
  };
}
