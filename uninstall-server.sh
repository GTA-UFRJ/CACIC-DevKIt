#!/bin/bash
DIR="./backup/"
if [ ! -d "$DIR" ]; then
    echo "Creating backup directory"
    mkdir ./backup
else
    echo "Backup directory alredy exists"
fi

make clean_server
rm config_macros.h
mv ./use_case/database/taciot.db "./backup/taciot-$(date '+%F-%H-%M-%S').db"
rm ./use_case/server/server_database_config
rm ./use_case/server/server_database_config.o
rm ./core/server/resources/ck_*