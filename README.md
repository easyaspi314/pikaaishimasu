# pikaaishimasu

This is a decompilation of *Pikachu ga Pokeboru o Aishimasu* (ピカチュウがポケボルをアイします, Pikachu
Loves Poké Balls), one of the oldest GBA homebrew games.

It doesn't function properly on any accurate emulator or hardware, due to a bad header checksum and
trying to display low sprites in Mode 4. If compiled with `make BUGFIX=1` or `make MODERN=1`, these major
bugs (as well as some minor bugs) will be fixed.

It (should) build the rom with the following SHA1 checksum, but it currently doesn't match.

`d29656289680e245a32484f28872851c26167d43 pikaaishimasu.gba`

### Building

#### Requirements:
 - The standard Unix build tools
 - For a build that attempts to match:
   - ARM SDT 2.11a, 2.50 or 2.51 for Windows
   - On non-Windows platforms, Wine and Winetricks
 - For a modern build:
   - devkitARM

For legal reasons, the ARM SDT cannot be provided.

Before building, it is necessary to run
```sh
git submodule init
git submodule update
```
to download libgba.

#### Classic build (requires ARM SDT):

Change paths if necessary.
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
make
```

**Note:** If you get the following message when trying to build with wine and ARMSDT:
```
0024:err:module:import_dll Library MFC42.DLL (which is needed by L"...\\armcc.dll") not found
Can't load armcc.dll
```
you need to run `winetricks -q mfc42` to install the MSVC 6 runtime.

#### Modern build (does not require ARMSDT):

Change paths if necessary.
```sh
# if you didn't have them set already 
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
make MODERN=1
```
