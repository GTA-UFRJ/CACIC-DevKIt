Publication Requests Types
===============

The first step to developing an IoT system using CACIc-DevKit is identifying the 
publication request types. The server expects publication requests messages for 
it to process the received data and publish the result in the database. Each 
publication request type is identified by its **six-digit decimal code**. 

In the smart grid use case, we have defined two publication requests types:

* ``smart-meter-sample (123456)``: contains a numeric value representing the energy 
  consumption in Kws
* ``aggregate (555555)``: contains a database request for adding up the queried 
  energy consumption data.

For defining the publication types, open the file ``programmable_calls/server_tasks_calls.cpp`` 
and change the following line to include the numeric code of the types (the order will matter, 
as we will describe in :doc:`processing </processing>`):

.. code-block:: c++
        :name: piece of programmable_calls/server_tasks_calls.cpp

        //including the numeric code of the types
        std::vector<const char*> types{"123456", "555555"};

