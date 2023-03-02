Isolated Processing Tasks
=========================

The next step to developing an IoT system using CACIc-DevKit is programming the 
processing tasks. Each :doc:`publication request type </client>` can have its own 
processing task, which defines what kind of processing will be applied once the 
publication message is received. The developer can program C/C++ codes with the 
following structure:

.. code-block:: c++
        :name: piece of programmable_calls/server_tasks_calls.h
        
        server_error_t task_name
        (char* time,
        char* pk,
        char* payload,
        uint32_t payload_size,
        uint8_t* client_key,
        uint8_t* storage_key,
        uint8_t* result,
        uint32_t* p_result_size);

The parameters are the following:

* ``time``: date and time at which the publication request was sent. 
* ``pk``: identification of the sending client.
* ``payload``: buffer containing the publication request itself. It can be a sensor 
  data or metadatas concerning the processing task. The developer is free to choose 
  the payload format of each publication request type. 
* ``payload_size``: in number of bytes in the array.
* ``client_key``: 16 bytes array containing the AES communication key used to encrypt 
  messages exchanged between client and server. See :doc:`About CACIC </about>` section 
  for more details.
* ``storage_key``: 16 bytes array containing the AES storage key used to encrypt stored 
  data. see :doc:`About CACIC </about>` section for more details.
* ``result``: buffer containing the processing result, that will be encrypted and written 
  in the database accordingly to the ``publish_db()`` function. See 
  :doc:`Pluggable Database </database>` section for more details.
* ``result_size``: in number of bytes in the array.

The return value is an ``enum`` type defined in ``core/utils/errors.h`` file. 

In the smart grid use case, we have defined a task called ``aggregation()``, which is 
called when the server receives a publication request of type ``aggregate (type code 555555)``. 
The ``payload`` is an SQLite statement for querying the result of 
``smart-meter-sample (type code 123456)`` requests, that were previously published and 
aggregating the numeric data. The publication request of type ``smart-meter-sample (type code 123456)`` 
does not have an associated processing task.

For associating data types with processing tasks, the developer must access the file 
``programmable_calls/server_tasks_calls.cpp`` and modify the following line:

.. code-block:: c++
        :name: piece of programmable_calls/server_tasks_calls.cpp
        
        std::vector<task_function_t> tasks{NULL, &aggregation};

The first i-th vector represents a pointer to a function that implements the processing task 
corresponding to the i-th publication request type in the ``types`` vector, defined in the 
section :doc:`Publication Requests Types </types>`. You can use ``NULL`` when you do not want 
to associate the publication request type with a processing task. In that case, the received  
``payload`` will just be internally copied to the ``result`` buffer. 

The ``aggregation()`` function is implemented in the file ``programmable_calls/server_tasks_calls.cpp``. 
The file ``core/server/server_enclave/server_enclave_auxiliary.h`` defines some functions 
that can be used in the processing tasks. As an example, we may use the following function in  
``aggregation()``:

.. code-block:: c++
        :name: piece_of_programmable_calls/server_tasks_calls_2.cpp
        
        server_error_t enclave_multi_query_db(
        char* pk,
        uint8_t* key,
        char* command, 
        char** datas, 
        uint32_t* datas_sizes, 
        uint32_t* p_data_count);


This function exits the enclave, executes the ``multi_query_db()`` function to query some data 
from the database, decrypt the data, verify access permissions, perform some security checks 
behind the scenes, and returns the decrypted data to the enclave. 
