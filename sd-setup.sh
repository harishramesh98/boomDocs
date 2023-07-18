# Script for setting up SD card for BOOM
# Note: Assumes the SD card is /dev/sdb (run `lsblk` to verify)

# Zap everything
sgdisk -Z /dev/sdb

# Create new empty GPT
sgdisk -o /dev/sdb 

# Create boot partition
sgdisk -a 34 /dev/sdb
sgdisk -n 1:34:1048610 /dev/sdb
sgdisk -t 1:a800 /dev/sdb # set GUID to Apple UFS

# Create root partition
sgdisk -n 2:1048628:15118302 /dev/sdb
sgdisk -t 2:af00 /dev/sdb # set GUID to Apple HFS

# Set up file sstem for root partition
sudo mkfs.hfs -v "Prototype Data" /dev/sdb2

# Copy Linux binary to boot partition
sudo dd if=~/boom/br-base-bin-nodisk-flat of=/dev/sdb1

