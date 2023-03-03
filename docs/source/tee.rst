Basics of TEE
=================

Trusted Execution Environments (TEE) are technologies that leverage hardware resources 
to isolate the code execution from malicious software. The main TEE technology used 
in cloud computing is the Software Guard Extensions (SGX), from Intel. SGX is an
extension of the x86 and x86-64 instruction sets for creating isolated regions in the 
computer's main memory, called enclaves.

Enclaves Implementation
----------------------

Enclaves are implemented as shared objects (or dynamic libraries, in Windows OS) containing
code executed in a processor's reserved memory. The access to this reserved memory space 
from a code ruining outside of the enclave is denied at microarchitectural level. The main 
advantage of using enclaves is the protection against high privileged software, such as 
super-users or operating systems. The enclave code can only be called through special 
instructions (ECALL), which involves security checks.

Attestation
----------------------

The enclave initialization instructions measure the enclave state at microarchitectural level, 
such that no software can tamper with the measurement. This measurement is signed using a
special system enclave (the Quoting Enclave) using a key provided by Intel during a setup 
phase (or provisioning phase). The signed measurement is forwarded to the client, which can
verify if the enclave was correctly initialized in a genuine plattform. The enclave and the
client can perform a symetric key exchange for creating an encrypted channel. However, the 
client send its data only if the enclave authenticity was successfuly attested.

Sealing
----------------------

Enclave data is encrypted with a sealing key before beeing stored in the disk. This sealing key
is derived from a hardware-specific key, and only the enclave can use the sealing key. The key
is never accessible to other software.
