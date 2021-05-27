#!/bin/bash
usage() { 
        echo "Usage: $0 -r /path_to_the_buildroot_root"; 
        exit 1; 
}

while getopts "b:r:x:" o; do
    case "${o}" in
        r)
            buildroot=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${buildroot}" ]; then
    usage
fi

buildroot=$(realpath $buildroot)

export CC=arm-buildroot-linux-uclibcgnueabihf-gcc
export CFLAGS="-I. -I.. -Wall -Wextra"
export CXX=arm-buildroot-linux-uclibcgnueabihf-g++
export LD=arm-buildroot-linux-uclibcgnueabihf-ld
export AR=arm-buildroot-linux-uclibcgnueabihf-ar
export OBJCOPY=arm-buildroot-linux-uclibcgnueabihf-objcopy
export SIZE=arm-buildroot-linux-uclibcgnueabihf-size
export PATH=$buildroot/output/host/bin:$PATH

make -j `nproc`
