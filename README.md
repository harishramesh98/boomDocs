# BOOM Docs
Documentation for BOOM Research in UPenn ACG Group

Welcome to BOOM Docs! 

## Inportant Links
- [Chipyard Github](https://github.com/ucb-bar/chipyard)  
- [Chipyard Docs](https://chipyard.readthedocs.io/en/stable/)
- [Chipyard Mailing List](https://groups.google.com/g/riscv-boom)
- [Boom Website](https://boom-core.org/)
- [Boom Github](https://github.com/riscv-boom/riscv-boom)
- [Boom Docs](https://docs.boom-core.org/en/latest/)
- [RISCV Code Constructor - for reference](https://github.com/abejgonzalez/riscv-code-constructor.git)

## Installing Conda
Conda is a package manager that will allow us to install all the necessary packages for our project.

```
conda --version # must be version 4.12.0 or higher
```

If you don't have conda installed, follow the instructions here: https://github.com/conda-forge/miniforge/#download

or run the following command:

```
wget "https://github.com/conda-forge/miniforge/releases/latest/download/Mambaforge-$(uname)-$(uname -m).sh"

bash Mambaforge-$(uname)-$(uname -m).sh
```

Follow the instructions and make sure to say yes to the prompts in the installer script.

Run the following command to make sure conda is installed correctly:

```
conda install -n base conda-lock=1.4.0

conda activate base
```
Make sure that it is version 1.4.0. The newer versions of conda-lock throw errors while setting the chipyard up.
Restart your terminal, you should see (base) next to your peompt.

## Cloning Chipyard

Now we need to clone the chipyard repository. This is the repository that contains the rocket-chip generator and the boom core generator.

```
git clone https://github.com/ucb-bar/chipyard.git

cd chipyard

git checkout 1.9.0
```

Now, we need to build the RISCV toolchain. This will take a while.

```
./build-setup.sh riscv-tools # or esp-tools
```


## Scripts

Your `.bashrc` file should look like the file `bashrc4acggrid` in this repository. Copy the contents of that file into your .bashrc file. Note that the conda setup should be done by the miniforge installer script.

Use the boom alias to setup the environment for the chipyard project. This will activate the conda environment and setup the path variables. Please change the command to the correct path to your chipyard directory.

The boom alias also ensures that we can talk to the floating license server. The license server does not change between acggrid and the intel nuc.

## Chipyard Troubleshooting

You need to set the ulimit to be 16384 or greater for open pages. This is because the chipyard build process opens a lot of files. To check if the ulimit is set correctly, run the following command:

```
ulimit -Sn # should be 16384 or greater
ulimit -Hn # should be 16384 or greater
```

If the ulimit is not set correctly, run the following command:

```
ulimit -n 16384
# This will set the ulimit to 16384 TEMPORARILY and in my experience is really unreliable.
```

To set the ulimit permanently, run the following command:

```
sudo vim /etc/security/limits.conf
```

Once the file is open, add the following lines to the end of the file:

```
* soft nofile 16384
* hard nofile 16384
```

Save the file and restart your terminal. Run the ulimit commands again to make sure the ulimit is set correctly.
I'm pretty sure that isn't true, at least not for the scp command provided by the OpenSSH included with most Linux distributions.
## Building the BOOM Core

First we need to source the FPGA init script. This will set up the environment variables for the FPGA tools. 

```
./scripts/init-fpga.sh
```

Now we can build the BOOM core. This will take a while.

```
cd fpga/
make SUB_PROJECT=vcu118 CONFIG=BoomVCU118Config bitstream
```

This is where you can face some issues. If you get an error about a missing file, check the current GitHub branch you are on. You should be on the 1.9.0 branch. If a ulimit error occurs check the setup for ulimit again. If the license check fails, check the setup for the license server again. 

The final binary will be located in the following directory:

```
chipyard/fpga/generated-src/chipyard.fpga.vcu118.VCU118FPGATestHarness.BoomVCU118Config/obj/*.bit
```

## Connect and Program the FPGA

Connect the FPGA to the computer using the USB cable. Both JTAG and UART will be connected through the USB cable. Power the FPGA using the power supply.

Now, we need to open the Vivado Hardware Manager with `vivado &`. This opens the GUI that will allow us to program the FPGA. Open Vivado and search for the Hardware Manager.

Once the Hardware Manager is open, click on Open Target. Select the FPGA device and click OK. 

Now, we need to add the bitstream to the FPGA. Click on "Program Device". Select the bitstream file and click OK.

The FPGA should now be programmed.

## Building the Linux Binary for BOOM

This is a complicated process. First we need to make the Linux binary. This will take a while.

We need to go to the firemarshal directory. 

```
cd software/firemarshal
```

By default, FireMarshal is setup to work with FireSim. Instead, we want to target the prototype platform. This is done by switching the FireMarshal “board” from “firechip” to “prototype” using the following command:

```
vim marshal-config.yaml # comment whatever is in there

echo "board-dir : 'boards/prototype'" >> $PATH_TO_FIREMARSHAL/marshal-config.yaml
```

Note: `$PATH_TO_FIREMARSHAL` is the path to the firemarshal directory and is not an already set environment variable.

Now we need to build the Linux binary.

```
./marshal -v -d build br-base.json # here the -d indicates --nodisk or initramfs
```

The last step to generate the proper binary is to flatten it. This is done by using FireMarshal’s install feature which will produce a *-flat binary in the `$PATH_TO_FIREMARSHAL/images` directory (in our case `br-base-bin-nodisk-flat`) from the previously built Linux binary (`br-base-bin-nodisk`).

```
./marshal -v -d install -t prototype br-base.json
```


## Setting up the SD Card

### Setup Script

You can find a setup script [here](https://github.com/audrey-yang/boomDocs/blob/main/sd-setup.sh). It assumes that the SD card is located at `/dev/sdb` but can be changed.

### Step-by-step

1. We need to clean the SD card. This is done by running the following command:

``` 
sudo gdisk /dev/sdX # where X is the letter of the SD card
```

If you don't know which letter to use, you can run `lsblk` to check.

Press x to get the expert menu, then press z to zap everything. This will delete all the partitions on the SD card. Press y to all prompts.

2. Now we need to partition the SD card. This is done by running the following command:

```
sudo gdisk /dev/sdX # where X is the letter of the SD card
```

Press o to create a new empty GUID partition table (GPT). This will create a new partition table on the SD card. Press y to all prompts.

3. Now we need to create the boot partition. This is done by running the following:

- Press x to get the expert menu
- Press l command to set the boot sector to 34

- Press m to return to the main menu
- Press n to create a new partition
- Press 1 to create the first partition
- Press enter to select the default first sector (should be 34)
- Put +1048576 as the size of the partition. This is the size of the boot partition in sectors. This is 512MB. Note it should be larger than the size of the linux binary.
- For the type, search for the APFS type and use the hex number given. If you don't see the APFS option, we've found that Apple UFS works as well.

4. Now we need to create the root partition. This is done by running the following command:

- Press n to create a new partition
- Follow the prompts to create the root partition. The size of the partition should be the size of the SD card minus the size of the boot partition. 
- For the type, search for the HFS and use the hex number given.
- All the selections should be the default.

5. Now we need to write the partition table to the SD card. This is done by running the following:

- Press w to write the partition table to the SD card.
- Press y to all prompts.

6. Now we need to setup the file system for the root partition. This is done by running the following command:

```
sudo mkfs.hfs -v "PrototypeData" /dev/sdx2 # where x is the letter of the SD card and 2 is the number of the root partition.
```

Note, mkfs.hfs is not installed by default. You need to install it. You can also use newfs_hfs instead of mkfs.hfs on Mac and Linux.

7. Now we need to copy the linux binary to the boot partition. This is done by running the following command:

```
sudo dd if=$PATH_TO_FIREMARSHAL/br-base-bin-nodisk-flat of=/dev/sdx1 # where x is the letter of the SD card
```

Note! The SD card is very finicky. If you get an error, try running the command again. If you get an error again, try reformatting the SD card and running the command again.

Note!! *ALWAYS ENSURE THAT YOU UNMOUNT THE SD CARD BEFORE REMOVING IT FROM THE COMPUTER.*

### Troubleshooting

If you find that the file system is not formatted correctly, try running the `mkfs.hfs` command again.

If you lack permissions for the file system:
1. See that `user` is in the `disk` group
2. Use `sudo` to copy/move/create files and directories into the file system

## Booting the FPGA

1. Connect the SD card to the FPGA. We use an SD Card over SPI interface. The SD card should be connected to the FPGA next to the power port. 

2. Connect the FPGA to the computer using the USB cable. Both JTAG and UART will be connected through the USB cable. Power the FPGA using the power supply.

3. Open a terminal and run the following command:

```
sudo screen /dev/ttyUSB0 115200
```

Note: In my experience the screen command is not the most reliable. You should use minicom if you face issues with screen.

```
sudo minicom
```

Also, the FPGA will have to be reprogrammed everytime you power it on. 

Another note, UART has 2 USB ports, one for debugging and one for programming. Make sure you are using the programming port. The texts will only show up on the programming port. There is no way to tell which port is which. You just have to try both.

4. Power on the FPGA. You should see Linux booting up. 

The default username is `root` and the default password is `fpga`.

## Linux on the FPGA

The Linux on the FPGA is a very barebones Linux. It does not have many features. It is also very slow. It takes a long time to boot up and it takes a long time to run commands. First you have to mount the SD card. This is done by running the following command:

```
mount /dev/mmcblk0p2 /mnt
```

Now you should see all the files on the /mnt/ directory. You can now run the binaries on the FPGA.

Note: It is very important to unmount the SD card before removing it from the FPGA. This is done by running the following command:
I'm pretty sure that isn't true, at least not for the scp command provided by the OpenSSH included with most Linux distributions.
```
umount /dev/mmcblk0p2
```

## Using QEMU

QEMU is a virtual machine. It allows us to run RISC-V Linux on our computer. We can use QEMU to run the Linux binary that we generated earlier. This is useful because it allows us to test the Linux binary without having to program the FPGA everytime. It also comes bundled with the chipyard toolchain.

### QEMU Through FireMarshal

Head over to the firesim directory.

```
cd $PATH_TO_FIRESIM
./init-submodules.sh
```

Change the marshal-config.yaml file to point to the `../../sim/*` directory. This should be the command you commented out before.

Run the following command to build the Linux binary for QEMU. This will take a while.

```
marshal build ./example-workloads/example-fed.json
```

Run the following command to run the binary binary in QEMU.
```
marshal launch ./example-workloads/example-fed.json
```

#### Troubleshooting

If you get an error, try removing the runOutput with

```
lsof +D /runOutput
```

If a pid is found, kill it.

If you find that you don't have enough space for QEMU, navigate to `./images/firechip/example-fed`. The image is `example-fed.img` and its default size is about 2GB. You can resize it with

```
dd if=/dev/zero of=example-fed.img seek=N obs=1MB count=0
```

where `N` is the size you wish to resize to and `obs` is the unit for N (e.g. `N=30 obs=1GB` resizes to 30GB). 

To fit QEMU to the new image size, log into the VM and run `parted` and follow the prompts to resize the file system (format ext4). Afterwards, run `resize2fs /dev/vda` and restart the VM.

### QEMU Through `qemu-system-riscv64`

It's easy to use QEMU through FireMarshal, but it does not have much flexibility. In the version we're using, Fedora is set to version 33. However, we may want to use a different version for `glibc` compatability with the binary for BOOM. You can set up Fedora 31 as follows:

- Download (or `wget`) `Fedora-Developer-Rawhide-20190703.n.0-sda.raw.xz` and `fw_payload-uboot-qemu-virt-smode.elf` from [the Fedora project](https://dl.fedoraproject.org/pub/alt/risc-v/disk-images/fedora/rawhide/20190703.n.0/Developer/). These will serve as the image and kernel, respectively.
- Place the files into a new directory at `$PATH_TO_FIRESIM`
- Run the following to boot the image with QEMU (replacing path names)
  ```
  ./init-submodules.sh
  /path/to/chipyard/.conda-env/bin/qemu-system-riscv64 -nographic -bios none -smp 4 -machine virt -m 16384 -kernel /path/to/fw_payload-uboot-qemu-virt-smode.elf -object rng-random,filename=/dev/urandom,id=rng0 -device virtio-rng-device,rng=rng0 -device virtio-net-device,netdev=usernet -netdev user,id=usernet,hostfwd=tcp::41893-:22 -device virtio-blk-device,drive=hd0 -drive file=/path/to/Fedora-Developer-Rawhide-20190703.n.0-sda.raw,format=raw,id=hd0 
  ```

  You can use the same methods for different versions of Fedora and more.

## Config Files

BOOM CONFIG FILE PATH: `chipyard/generators/boom/src/main/scala/common/config-mixins.scala` (This is where you can change the number of cores, L1 cache size, L2 cache size, etc.)

Comments are added to config.scala to explain what each parameter does. We are using the LargeBoomCore.

Performance Counters: `chipyard/generators/boom/src/main/scala/exu/core.scala`

Comments are added to core.scala to explain what each performance counter does. You can add more performance counters here.

## Installing the RISC-V Toolchain

For practical purposes, we need to use the riscv toolchain outside the chipyard directory. This is because the toolchain in chipyard is not compatible with the linux on the FPGA. It is mostly used for software simulation and not bare metral. 

More support at : https://github.com/riscv-collab/riscv-gnu-toolchain

To install the riscv toolchain, run the following commands:

```
git clone https://github.com/riscv/riscv-gnu-toolchain
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build
./configure --prefix=/opt/riscv
make
```

One that is done, you will have the unknown-elf-gcc compiler in the riscv-gnu-toolchain/riscv64-unknown-elf-gcc/bin directory. You can use this to compile your code.

To install the Linux GNU toolchain, run the following commands:

```
./configure --prefix=/opt/riscv
make linux
```

One that is done, you will have the linux-unknown-gcc compiler in the riscv-gnu-toolchain/riscv64-unknown-linux-gnu/bin directory. You can use this to compile your code.

Note: This toolchain will compile for a 64 bit RISC-V processor. It is fine as BOOM is a 64 bit processor.

## Compiling the RISC-V Toolchain

...

## Using Go

Go is statically compiled and can be run with BOOM.

You'll need to use QEMU to compile into RISC-V. To install go, you'll need to [cross-compile](https://go.dev/doc/install/source#bootstrapFromCrosscompiledSource) for RISC-V and Linux. You can do this as follows:
1. Install go on your local machine first to do this. You can acceess the downloads and instructions [here](https://go.dev/doc/install).
2. Follow the path to your go installation (`/usr/local/go` on my Mac) and run `cd src`.
3. Run `GOOS=linux GOARCH=riscv64 ./bootstrap.bash`. This will generate a cross-compiled toolchain in `../../go-linux-riscv64-bootstrap` and a compressed archive.
4. Copy the archive over to QEMU and extract the files. Wherever you have the directory, set `GOROOT_BOOTSTRAP` to that path. You may need to also set export `GOROOT` to that same path and update your path variable via `export PATH=$PATH:$GOROOT/bin`.
5. Verify your installation with `go version`.

## Preliminary performance counter monitoring and RISCV-CODE-Constructor
Once you set up the older version of QEMU running in the chipyard, we can experiment with the riscv-code-constructor. The constructor requires a working chipyard directory. After executing "```source ./env.sh``` " we can use the code constructor to generate riscv executables. The code constructor uses a scoped or modifies version of GLIBC that does not have all the functionality of gcc or glibc. It uses a riscv based elf compiler to compile programs and generate executables for BOOM or Rocket-chip running baremetal programs. This does not suit our purpose. However, the programming syle mentioned in the code-constructor helps in figuring out how to enable certain performance counters on BOOM. Firstly, at the time of writing we have difficulty accessing machine-level registers. We can access cycle and instret registers with comparative ease. Compiling this code from the code constructor in the QEMU machine can be done using gcc on QEMU.
It is useful to occassionally disassemble code and view the assembly code to make sense of what happens behind the scenes.
Use:
```
objdump -D <Executable> > <executable>.txt
```
## Setting up the Prototype Test Bench
Inside the QEMU, for the older fedora distribution, copy the files above in the TestBench directory. TestFileMain.c has the required setup that allows switching between various test cases, based on the integer argument entered along with the input. At the moment the test cases are as follows:
```
1: Reads Cycle and Instr registers while performing a mathematical operation. The operation currently in place is calculating the hypotenuse of a right angled triangle given two sides.
2: Runs the hpmTest code given in riscv-code-constructor
```

