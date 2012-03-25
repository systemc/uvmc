
Title: UVM Version Requirements

The UVM Connect library requires UVM 1.1a or later, which
adds a simple accessor method to uvm_port_base #(IF) for
getting the interface mask of the port:

376a374,377
>   function int m_get_if_mask();
>     return m_if_mask;
>   endfunction

To enable UVM 1.0p1 or UVM-1.1 to work with UVM Connect,
replace the .../src/base/uvm_port_base.svh file in either
of those releases with the one included in this directory.



| //------------------------------------------------------------//
| //   Copyright 2009-2012 Mentor Graphics Corporation          //
| //   All Rights Reserved Worldwid                             //
| //                                                            //
| //   Licensed under the Apache License, Version 2.0 (the      //
| //   "License"); you may not use this file except in          //
| //   compliance with the License.  You may obtain a copy of   //
| //   the License at                                           //
| //                                                            //
| //       http://www.apache.org/licenses/LICENSE-2.0           //
| //                                                            //
| //   Unless required by applicable law or agreed to in        //
| //   writing, software distributed under the License is       //
| //   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR   //
| //   CONDITIONS OF ANY KIND, either express or implied.  See  //
| //   the License for the specific language governing          //
| //   permissions and limitations under the License.           //
| //------------------------------------------------------------//
