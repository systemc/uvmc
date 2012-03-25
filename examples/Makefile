#
# //------------------------------------------------------------//
# //   Copyright 2009-2012 Mentor Graphics Corporation          //
# //   All Rights Reserved Worldwid                             //
# //                                                            //
# //   Licensed under the Apache License, Version 2.0 (the      //
# //   "License"); you may not use this file except in          //
# //   compliance with the License.  You may obtain a copy of   //
# //   the License at                                           //
# //                                                            //
# //       http://www.apache.org/licenses/LICENSE-2.0           //
# //                                                            //
# //   Unless required by applicable law or agreed to in        //
# //   writing, software distributed under the License is       //
# //   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR   //
# //   CONDITIONS OF ANY KIND, either express or implied.  See  //
# //   the License for the specific language governing          //
# //   permissions and limitations under the License.           //
# //------------------------------------------------------------//

check_uvm_home:
ifndef UVM_HOME
	@echo "ERROR: UVM_HOME environment variable is not defined"
	abort-mission
endif
	

UVMC_HOME ?= ..

UVM_LIB  ?= $(UVMC_HOME)/lib/uvmc_lib
UVMC_LIB ?= $(UVMC_HOME)/lib/uvmc_lib

VLOG_ARGS ?= 
SCCOM_ARGS ?= 
LINK_ARGS ?= 
VSIM_ARGS ?= 


VERBOSE=+UVM_VERBOSITY=UVM_MEDIUM

VLOG  = vlog -sv +incdir+$(UVM_HOME)/src -L $(UVM_LIB) -L $(UVMC_LIB) +define+QUESTA $(VLOG_ARGS)
SCCOM = sccom -DSC_INCLUDE_DYNAMIC_PROCESSES -DQUESTA -I$(UVMC_HOME)/src/connect/sc $(SCCOM_ARGS)
LINK  = sccom -link -DSC_INCLUDE_DYNAMIC_PROCESSES -lib $(UVMC_LIB) -lib work $(LINK_ARGS)
VSIM  = vsim  -c $(VERBOSE) -do "run -all; quit -f" -t 1ps -L $(UVM_LIB) -L $(UVMC_LIB) $(VSIM_ARGS)


