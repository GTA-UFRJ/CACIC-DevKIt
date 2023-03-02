Client API
===================================
CACIC Publication API
---------------------------------------

For sending a publication message to the server, one can use the following function, 
defined in ``core/client/client_publish.h`` and implemented in ``core/client/client_publish.cpp``:

.. code-block:: c++
        :name: piece of core/client/client_publish.h
        
        //sending publication message
        int client_publish(uint8_t* key, client_data_t data);


``client_data_t`` structure is defined in ``core/client/client.h``, as follows:

.. code-block:: c++
        :name: piece of core/client/client.h
        
        typedef struct client_data {
          char time[20];
          char pk[9];
          char type[7];
          char* payload;
          uint32_t permissions_count;
          char** permissions_list;
        } client_data_t;

The developer is free to choose the way the :doc:`ID </about>` (``pk`` field in 
the structure): and the :doc:`communication key </about>` (CK, ``key`` argument in 
the function) is stored. The use case gives an implementation example, based on using 
a file (``use_case/database/identity``) to store the concatenation between ID and CK.

The ``time`` field of the structure should not be ``NULL``. If the developer does not 
want to use any date and time, it should fill this field with some non-null characters 
(``xxxxxxxxxxxxxxxxxxxx``). The date and time can be useful in some processing tasks and 
can be useful to filter database requests. The developer can use the following function, 
defined in ``core/utils/utils.h`` and implemented in ``core/utils/utils.cpp``:

.. code-block:: c++
        :name: piece of core/utils/utils.cpp
        
        void get_time(char* );
 
The ``payload`` field depends on the 6-digit decimal ``type`` code, which depends on the 
use case.

The ``permissions_list`` field is a list of C strings (array of pointers to characters), 
in which each string is a :doc:`client ID </about>`. The number of elements in this array 
is defined by the ``permissions_count`` field. One specificity of the use case application 
is the usage of a database (``use_case/database/default_perms.db``) to store the default 
client IDs which access each published data type.

CACIC Query API
---------------------------------------

For querying some data from the server, the developer can use the following function, 
defined in ``core/client/client_query.h`` and implemented in ``core/client/client_query.cpp``:

.. code-block:: c++
        :name: piece of core/client/client_query.cpp
        
        int client_query(
            uint8_t* key, 
            uint8_t* data, 
            uint32_t data_index, 
            char* command, 
            uint32_t* data_size, 
            char* id);

The ``id`` and ``key`` arguments are respectively the client ID and CK, as described 
for the publication. The ``command`` and ``data_index`` are used to identify the data 
being queried in the database and depend on the use case. For the smart grid application, 
``command`` is a Structured Query Language (SQL) statement for selecting the data in the 
database, while ``data_index`` can be maintained zero. However, the client may send a 
statement (``command``) for querying multiple data. In such case, it can use the 
``data_index`` to select the data being queried.

CACIC Registration API
---------------------------------------

For sharing a client’s ID and CK with the server, the developer can use the following 
function, defined in ``core/client/client_register.h`` and implemented in 
``core/client/client_register.cpp``:

.. code-block:: c++
        :name: piece of core/client/client_register.h
        
        int client_register(client_identity_t rcv_id);

``client_data_t`` structure is defined in ``core/client/client.h``, as follows:

.. code-block:: c++
        :name: piece of core/client/client.h
        
        typedef struct client_identity {
          char pk[9];
          uint8_t comunication_key[16];
        } client_identity_t;

.. warning:: The current implementation of this API is insecure, since it does not perform 
    enclave attestation. This will be fixed in future versions.

