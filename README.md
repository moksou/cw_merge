# cw_merge

It's a simple(?) C program to merge at least two PWAD's into one with all necessary resources (music, sounds, textures etc). 

# Usage
```sh
$ cw_merge wad_1_name.wad wad_2_name.wad result_name.wad
```

# Bugs/limitations
- Not tested in non-Linux OS.
- IWAD's are not supported. Entirely.
- There is not lump ordering support yet. Now the tool is good to merge multiple maps without custom textures, or maps with common texturepack.