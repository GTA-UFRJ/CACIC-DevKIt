#!/bin/bash
DIR="./backup/"
if [ ! -d "$DIR" ]; then
    echo "Creating backup directory"
    mkdir ./backup
else
    echo "Backup directory alredy exists"
fi

rm CACIC-GUI
rm config_macros.h
mv ./use_case/database/default_permissions.db "./backup/default_permissions-$(date '+%F-%H-%M-%S').db"
rm ./use_case/client/client_permdb_config
rm ./use_case/client/client_permdb_config.o

cd use_case/graphical-client
make clean 
rm .qmake.stash graphical-client.pro qrc_Resources.cpp moc_*
rm Makefile
rm *.o 