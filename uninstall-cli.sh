#!/bin/bash
DIR="./backup/"
if [ ! -d "$DIR" ]; then
    echo "Creating backup directory"
    mkdir ./backup
else
    echo "Backup directory alredy exists"
fi

make -f build.make clean_client
rm config_macros.h
mv ./use_case/database/default_permissions.db "./backup/default_permissions-$(date '+%F-%H-%M-%S').db"
rm ./use_case/client/client_permdb_config
rm ./use_case/client/client_permdb_config.o