#!/bin/bash
usage() { 
        echo "Usage: $0 -k /path_to_kernel -v /path_to_vitis"; 
        exit 1; 
}

while getopts "k:v:" o; do
    case "${o}" in
        k)  kernel_path=${OPTARG}
            ;;
        v)
            vitis_path=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${vitis_path}" ] || [ -z "${kernel_path}"]; then
    usage
fi
vitis_path=$(realpath $vitis_path)
kernel_path=$(realpath $kernel_path)

source $vitis_path/settings64.sh
make KERNEL_SOURCE=$kernel_path ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
