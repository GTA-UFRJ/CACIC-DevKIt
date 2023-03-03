
Tool Organization
===============

The repository can be divided into four main parts: 

* **core**: directory containing the core functionality of CACIC, like the 
  :doc:`client publish and query interface for the client application to call </client>`, 
  the server HTTP interface, the enclave entry points, some files for the correct 
  server operation and some utility functions (for printing errors or performing encrypt).
* **programmable_calls**: directory containing some functions that the CACIC server’s 
  core will call. These functions are programmed accordingly to the smart grid use case 
  example. In the **raw_programmable_calls** you will find just the functions templates 
  for the users to create their own systems. These functions will perform 
  :doc:`processing tasks </processing>` and interact with the :doc:`database </database>`. 
* **use_case**: directory containing files that are only meaningful for the smart grid 
  use case example, like the user interfaces (CLI and Qt), the ESP32 firmware, the SQLite 
  database, and some auxiliary functions for the client.
* **other files**: the repository also contains configuration files and scripts for 
  installing and evaluating the use case application.
