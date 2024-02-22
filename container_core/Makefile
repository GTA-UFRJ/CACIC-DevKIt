# PyTorch and the pre-trained model must be installed on the system. See README for details.

ARCH_LIBDIR ?= /lib/$(shell $(CC) -dumpmachine)
DATABASE_PATH ?= /home/guiaraujo/CACIC-DevKIt/use_case/database
SEALS_PATH ?= /home/guiaraujo/CACIC-DevKIt/core/server/resources
ROOT_PATH ?= /home/guiaraujo/CACIC-DevKIt/container_core
SCRIPT_PATH ?= /home/guiaraujo/CACIC-DevKIt/container_core/scacic_frontend.py

DEBUG=1

ifeq ($(DEBUG),1)
GRAMINE_LOG_LEVEL = debug
else
GRAMINE_LOG_LEVEL = error
endif

.PHONY: all
all: python.manifest
ifeq ($(SGX),1)
all: python.manifest.sgx python.sig 
endif

python.manifest: python.manifest.template
	gramine-manifest \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		-Ddatabase_path=$(DATABASE_PATH) \
		-Dseals_path=$(SEALS_PATH) \
		-Droot_path=$(ROOT_PATH) \
		-Dscript_path=$(SCRIPT_PATH) \
		-Darch_libdir=$(ARCH_LIBDIR) \
		-Dentrypoint=$(realpath $(shell sh -c "command -v python3")) \
		$< > $@

# Make on Ubuntu <= 20.04 doesn't support "Rules with Grouped Targets" (`&:`),
# for details on this workaround see
# https://github.com/graminepython/gramine/blob/e8735ea06c/CI-Examples/helloworld/Makefile
python.manifest.sgx python.sig: sgx_sign
	@:

.INTERMEDIATE: sgx_sign
sgx_sign: python.manifest
	gramine-sgx-sign \
		--manifest $< \
		--output $<.sgx

.PHONY: clean
clean:
	$(RM) *.token *.sig *.manifest.sgx *.manifest

