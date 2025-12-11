#!/bin/bash

NOW=$(date '+%Y%m%d')
ENV="esp32s3"

mkdir -p ./release

# build firmware
pio run -e $ENV
# build filesystem
./create_www.sh && pio run -t buildfs -e $ENV

# copy to output directory
cp ./.pio/build/${ENV}/firmware.bin ./release/${NOW}-firmware.bin
cp ./.pio/build/${ENV}/firmware.elf ./release/${NOW}-firmware.elf
cp ./.pio/build/${ENV}/littlefs.bin ./release/${NOW}-littlefs.bin