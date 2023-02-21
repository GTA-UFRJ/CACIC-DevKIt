#!/bin/bash
CPP_HTTPLIB_PATH=$1
AP_URL=$2
AP_PORT=$3
SERVER_URL=$4
SERVER_PORT=$5

g++ -c use_case/client/client_permdb_config.cpp -o use_case/client/client_permdb_config.o
g++ use_case/client/client_permdb_config.o -l sqlite3 -o use_case/client/client_permdb_config
.use_case/client/client_permdb_config default_permissions.db

cp config_macros_template config_macros.h
python3 setup_macros_file.py "(ROOT)" "$PWD"
python3 setup_macros_file.py "(HTTPLIB)" "$CPP_HTTPLIB_PATH"
python3 setup_macros_file.py "(APURL)" "$AP_URL"
python3 setup_macros_file.py "(APPORT)" "$AP_PORT"
python3 setup_macros_file.py "(SERVERURL)" "$SERVER_URL"
python3 setup_macros_file.py "(SERVERPORT)" "$SERVER_PORT"

root=$PWD
cd use_case/graphical-client

qmake -project
sed -i "19 i QT += core gui widgets" graphical-client.pro 
sed -i "38 i HEADERS += ${root}/core/client/client_publish.h ${root}/core/client/client_query.h ${root}/core/client/client.h ${root}/use_case/client/client_permdb_manager.h ${root}/use_case/client/client_key_manager.h ${root}/use_case/client/client_apnet.h ${root}/use_case/client/client_uenet.h ${root}/core/client/client_register.h ${root}/use_case/client/client_generic_interface.h ${root}/core/utils/utils.h ${root}/core/utils/encryption.h ${root}/core/utils/errors.h ${root}/benchmark/timer.h ${root}/config_macros.h" graphical-client.pro 
echo "SOURCES += ${root}/core/client/client_publish.cpp ${root}/core/client/client_query.cpp ${root}/core/client/client.cpp ${root}/use_case/client/client_permdb_manager.cpp ${root}/use_case/client/client_key_manager.cpp ${root}/use_case/client/client_apnet.cpp ${root}/use_case/client/client_uenet.cpp ${root}/core/client/client_register.cpp ${root}/use_case/client/client_generic_interface.cpp ${root}/core/utils/utils.cpp ${root}/core/utils/encryption.cpp ${root}/core/utils/errors.cpp ${root}/benchmark/timer.cpp" >> graphical-client.pro 

qmake
sed -i "20 i CXXFLGAS += -shared" Makefile
sed -i "22 i INCPATH += -I${root} -I${root}/core/client -I${root}/use_case/client -I${root}/core/utils -I${root}/cpp-httplib -I${root}/benchmark" Makefile
sed -i "46 i LIBS += -lsqlite3 -lcrypto" Makefile
sed -i "88 i SOURCES += ${root}/core/client/client.cpp ${root}/core/client/client_publish.cpp ${root}/core/client/client_query.cpp ${root}/use_case/client/client_permdb_manager.cpp ${root}/use_case/client/client_key_manager.cpp ${root}/use_case/client/client_apnet.cpp ${root}/use_case/client/client_uenet.cpp ${root}/core/client/client_register.cpp ${root}/use_case/client/client_generic_interface.cpp ${root}/core/utils/utils.cpp ${root}/core/utils/encryption.cpp ${root}/utils/errors.cpp ${root}/benchmark/timer.cpp" Makefile

make \
HTTPLIB_DIR=$CPP_HTTPLIB_PATH 

mv graphical-client ${root}/CACIC-GUI