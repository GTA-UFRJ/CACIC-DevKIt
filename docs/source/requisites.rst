Requisites
===============

For start using CACIC-DevKit, you must follow the procedures described here.

.. warning:: Although some programs are available in other Linux platforms with x86 
    and x86-64 architectures, all the procedures were only tested on Ubuntu 20.04.


Install SGX
--------------
The Intel Software Guard Extensions (SGX) resources installation and configuration 
can be found in the `installation guide <https://download.01.org/intel-sgx/sgx-linux/2.12/docs/Intel_SGX_Installation_Guide_Linux_2.12_Open_Source.pdf>`_. 
Note that this project is developed using Revision 2.12 of Intel SGX Linux Release. 
Correct operation is not ensured in other versions. 

For using the CACIC-DevKit, the following steps needs to be ensured:

* SGX SDK (C/C++ Software Development Kit) is installed
* SGX device is enabled in BIOS (and in software, if necessary)
* SGX Driver and SGX PSW (Plattform Software) are installed

Install other tools
---------------

Ensure that the following tools are installed:

* `g++ compiler <https://gcc.gnu.org/>`_
* `Make <https://www.gnu.org/software/make/>`_
* `Git and GitHub <https://git-scm.com/>`_

Some of these tools are usually installed by default in Linux distributions or can 
be easily installed using the package manager. You will also need to to clone the 
following GitHub repositories:

* `CACIC-DevKit <https://github.com/GTA-UFRJ/CACIC-Dev-KIt>`_
* `cpp-httplib <https://github.com/yhirose/cpp-httplib>`_

For running the :doc:`</docs/source/demonstration>`, you may also install:

* `SQLite 3 <https://www.sqlite.org/download.html>`_
* `CMake <https://cmake.org/download/>`_
* `Qt GUI library <https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4>`_. 
  The demo is compatible with version 6.
