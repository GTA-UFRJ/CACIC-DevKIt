Pluggable Database
===============

The second step to developing an IoT system using CACIc-DevKit is programming 
the database interface. The developer can implement simple disk read/write 
operations or deploy commercial database solutions like SQLite. The CACIC 
server core can call the following four functions, defined in the file  
``programmable_calls/server_database_calls.h``:

.. code-block:: c++
        :caption: piece of programmable_calls/server_database_calls.h

        //read and write operations
        server_error_t publish_db(char* time, char* pk, char* type, char* data, uint32_t data_size);
        server_error_t query_db(char* command, uint32_t index, char* data, uint32_t* p_data_size);
        server_error_t multi_query_db(char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count);


The developer must program the four functions in the file 
``programmable_calls/server_database_calls.cpp`` accordingly to the use case 
specific database implementation. For means of exemplification, you can take 
a look at our implementation for the smart grid use case, which involves using 
a SQLite connection and calling more specific functions developed in  
``use_case/server/server_database_manager.h``.


