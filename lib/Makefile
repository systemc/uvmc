#
#----------------------------------------------------------------------
#   Copyright 2009-2012 Mentor Graphics Corporation
#   All Rights Reserved Worldwide
#
#   Licensed under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in
#   compliance with the License.  You may obtain a copy of
#   the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in
#   writing, software distributed under the License is
#   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
#   CONDITIONS OF ANY KIND, either express or implied.  See
#   the License for the specific language governing
#   permissions and limitations under the License.
#----------------------------------------------------------------------


UVMC_HOME ?= ..
UVMC_LIB ?= $(UVMC_HOME)/lib/uvmc_lib
UVM_LIB  ?= $(UVMC_HOME)/lib/uvmc_lib


BITS = 32

all: check_uvm_home lib32 lib64

check_uvm_home:
ifndef UVM_HOME
	@echo "ERROR: UVM_HOME environment variable is not defined"
endif
	
lib32:
	$(MAKE) BITS=32 lib

lib64:
	$(MAKE) BITS=64 lib

lib:
	-vlib $(UVMC_LIB);
	-vlib $(UVM_LIB);
	vlog  -$(BITS) -work $(UVM_LIB) -incr +incdir+$(UVM_HOME)/src $(UVM_HOME)/src/uvm_pkg.sv $(UVM_HOME)/src/dpi/uvm_dpi.cc; \
        vlog  -$(BITS) -work $(UVMC_LIB) -incr +incdir+$(UVM_HOME)/src +incdir+$(UVMC_HOME)/src/connect/sv -L $(UVM_LIB) $(UVMC_HOME)/src/connect/sv/uvmc_pkg.sv; \
        sccom -$(BITS) -work $(UVMC_LIB) -incr -g -I$(UVMC_HOME)/src/connect/sc $(UVMC_HOME)/src/connect/sc/uvmc.cpp; \
        sccom -$(BITS) -work $(UVMC_LIB) -g -I$(UVMC_HOME)/src/connect/sc -lib $(UVMC_LIB) -DSC_INCLUDE_DYNAMIC_PROCESSES -linkshared;

clean:
	-rm -rf $(UVM_LIB)/_* $(UVM_LIB)/uvm*pkg
	-rm -rf $(UVMC_LIB)/_* $(UVMC_LIB)/uvm*pkg


