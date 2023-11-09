#!/bin/bash
bash uninstall-server.sh
bash uninstall-cli.sh
bash install-server.sh ~/sgxsdk/environment ~/cpp-httplib/ localhost 8080
bash install-cli.sh ~/cpp-httplib/ localhost 5555 localhost 8080