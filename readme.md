#Buildroot settings and toolchain for Olimex a13-som module

This repo contains:
-	My buildroot build for a13-som with the sunxi-kernel.
-	The generated toolchain for cross-compiling
-	Some of my projet code

Usage:

```
cd buildroot
make a13_som_defconfig
make
```