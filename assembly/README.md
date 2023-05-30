# MM Redux Assembly

This directory contains the ASM & C source code used to build a binary blob which is allocated in memory, and the
patches required to make use of the created binary blob.

Download [armips](https://github.com/Kingcom/armips/releases/latest) and [MakePPF3](https://www.romhacking.net/utilities/353/) binaries and put them into the `scripts` directory.

## Building (Docker)

Currently [Docker] is used for building. It installs all required software onto a linux base image, and uses the
provided python scripts to build the following files:

- `rom_patch.bin`
- `symbols.json`

To build the binary blob, run the following command:

```sh
docker-compose up
```

This command will do the following:
- Provision the Docker image (this only needs to be done once and may take awhile).
- Build the binary blob files into `build/generated`.
- Copy these files into `MMR.Randomizer/Resources/asm`, where they are usable by the Randomizer.

[Docker]:https://www.docker.com/

## Building (MSYS2)

Download and install [MSYS2](https://www.msys2.org/). 

Run `MSYS2 MINGW32`:

```
pacman -S git python
git clone https://github.com/glankk/n64 n64
cd n64
./install_deps
./configure --prefix=/opt/n64 --enable-vc
make install-toolchain -j 10
make && make install
make install-sys
make install-local-exec
export PATH=$PATH:/opt/n64/bin
```

Open `.bashrc` in `MSYS2/home/USER`, add to file and save:

```
export PATH=$PATH:"/opt/n64/bin"
```

Re-open `MSYS2 MINGW32` and run:

```
./build.sh
```