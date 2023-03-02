Build Setup
===================================

For building both server and client, the developer can customize the ``build.make`` 
file at the root of the repository. There are some important macros that must be 
edited for you to compile your application, as follows:

* ``Server_Cpp_Files``: files used to make the server work outside of the enclave. 
  For the smart grid use case, we modified this macro in line 108
* ``Server_Include_Paths``: directories containing the header files. For the smart 
  grid use case, we modified this macro in line121.
* ``Server_C_Flags``: flags used to compile .cpp files and generate .o files.
* ``Server_Link_Flags``: flags used to linkedit .o files and generate the Server 
  executable application. For the smart grid use case, we modified this macro in line 149.
* ``Client_Cpp_Files``: files used to make the client work. For the smart grid use case, 
  we modified this macro in lines 172 to 177.
* ``Client_Include_Paths``: directories containing the header files. For the smart grid 
  use case, we modified this macro in line 186.
* ``Client_C_Flags``: flags used to compile .cpp files and generate .o files.
* ``Client_Link_Flags``: flags used to linkedit .o files and generate the Client executable 
  application. For the smart grid use case, we modified this macro in line 200.


As you can see, the developer can implement the use case by modularizing the application 
using its own files. In that case, it should define rules to compile these files. For the 
smart grid use case, some receipts were added to the build.make file, as follows:

.. code-block:: makefile
				:name: piece of build.make

				use_case/server/server_database_manager.o: use_case/server/server_database_manager.cpp core/utils/utils.cpp
					@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Server_Cpp_Flags) -c $< -o $@
					@echo "CXX  <=  $<"

				use_case/client/%.o: use_case/client/%.cpp  
					@$(CXX) $(Client_Cpp_Flags) -c $< -o $@
					@echo "CXX  <=  $<"

.. note:: this section does not describes the graphical user interface building procedure. 
    It involves using cmake to generate a .pro file and a Makefile. This is the default 
    procedure for deploying applications using Qt Framework and is outside of the scope of 
    this documentation.
