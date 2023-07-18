# Script for setting up SD card for BOOM
# Note: Assumes the SD card is /dev/sdb (run `lsblk` to verify)

# Zap everything
echo "ZAP"
sgdisk -Z /dev/sdb
echo

# Create new empty GPT
echo "Empty GPT"
sgdisk -o /dev/sdb 
echo 

# Create boot partition
echo "BOOT PARTITION"
sgdisk -a 34 -n 1:34:1048610 /dev/sdb
sgdisk -t 1:a800 /dev/sdb # set GUID to Apple UFS
echo

# Create root partition
echo "ROOT PARTITION"
sgdisk -a 34 -n 2:1048628:15118302 /dev/sdb
sgdisk -t 2:af00 /dev/sdb # set GUID to Apple HFS
echo 

# Set up file sstem for root partition
sudo mkfs.hfs -v "Prototype Data" /dev/sdb2

# Copy Linux binary to boot partition
sudo dd if=~/boom/br-base-bin-nodisk-flat of=/dev/sdb1
