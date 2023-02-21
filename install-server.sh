#!/bin/bash
SGX_PATH=$1
CPP_HTTPLIB_PATH=$2
SERVER_URL=$3
SERVER_PORT=$4

g++ -c use_case/server/server_database_config.cpp -o use_case/server/server_database_config.o
g++ use_case/server/server_database_config.o -l sqlite3 -o use_case/server/server_database_config
./use_case/server/server_database_config taciot.db

source $SGX_PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/sample_libcrypto/

cp config_macros_template config_macros.h
python3 setup_macros_file.py "(ROOT)" "$PWD"
python3 setup_macros_file.py "(HTTPLIB)" "$CPP_HTTPLIB_PATH"
python3 setup_macros_file.py "(SERVERURL)" "$SERVER_URL"
python3 setup_macros_file.py "(SERVERPORT)" "$SERVER_PORT"

make -f build.make \
server_enclave.signed.so \
Server \
HTTPLIB_DIR=$CPP_HTTPLIB_PATH 
