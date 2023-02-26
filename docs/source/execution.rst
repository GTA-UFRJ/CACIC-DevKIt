Execution
===================================

To initialize the server process, run the command:

.. prompt:: bash $
./Server 


To open the client CLI tool help, run the command:

.. prompt:: bash $
./Client

The help includes examples for publishing data to and querying data from the server.  

To initialize an access point, run the command 
.. prompt:: bash $
    ./Client ap_init

To open the client GUI tool, run the command:
.. prompt:: bash $
    ./CACIC-GUI

Before sending publication and query messages, it is necessary to configure the client. 
The required steps can be done using the CLI or GUI tools, as follows:

* Choose a 8-characters (0-9 and a-f) Client Identificator (ID, e.g. 72d41281)
* Choose a 32-characters (0-9 and a-f) Client Communication Key (CK)
* With the server running, register the client 

.. note:: in a production environment, this key must be sent to the enclave after attestation 
    using an encrypted channel. Intel offers code samples for remote attestation. One can 
    attest a remote enclave, that can transfer the data to other local enclaves, for example. 
    For reference, access the SGX examples._
