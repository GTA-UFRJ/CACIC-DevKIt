Installation
===================================

There are three tools: the server, the command line client tool, and the graphical user 
interface client tool, which are built with the following commands:

.. code-block:: shell
        :name: installation_procedure_1
        
        bash install-server.sh [SGX include path] [httplib directory path] [Server IP] [Server port]
        bash install-cli.sh [httplib directory path] [AP IP] [AP port] [Server IP] [Server port]
        bash install-gui.sh [httplib directory path] [AP IP] [AP port] [Server IP] [Server port]

For uninstallation, run:

.. code-block:: shell
        :name: installation_procedure_2

        bash uninstall-server.sh
        bash uninstall-cli.sh
        bash uninstall-gui.sh
