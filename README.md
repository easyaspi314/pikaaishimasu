# pikaaishimasu

This is a decompilation of *Pikachu ga Pokeboru o Aishimasu* (ピカチュウがポケボルをアイします, Pikachu
Loves Poké Balls), one of the oldest homebrew games.

It doesn't function properly on any accurate emulator or hardware, due to a bad header checksum and
trying to display sprites in bitmap Mode 4.

It (should) build the rom with the following SHA1 checksum, but it currently doesn't match.

`d29656289680e245a32484f28872851c26167d43 pokeaishimasu.gba`

### Building

To build this, you must have the standard build tools, the Windows release of ARM SDT version 2.51, Wine 
(if not on Windows), and devkitARM (Technically only the libgba headers, an objcopy that supports ARM
binaries, and gbafix are required).

For legal reasons, the ARM SDT cannot be provided.

```sh
# Should be a folder with
#  Bin
#    armasm.exe
#    armcc.exe
#    armlink.exe
#  Include
#    (standard C89 headers)
#  Lib
#    armlib_cn.32l
export ARMSDT=/c/ARM251
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
make
```