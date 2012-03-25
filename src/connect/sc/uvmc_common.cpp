//
//------------------------------------------------------------//
//   Copyright 2009-2012 Mentor Graphics Corporation          //
//   All Rights Reserved Worldwid                             //
//                                                            //
//   Licensed under the Apache License, Version 2.0 (the      //
//   "License"); you may not use this file except in          //
//   compliance with the License.  You may obtain a copy of   //
//   the License at                                           //
//                                                            //
//       http://www.apache.org/licenses/LICENSE-2.0           //
//                                                            //
//   Unless required by applicable law or agreed to in        //
//   writing, software distributed under the License is       //
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR   //
//   CONDITIONS OF ANY KIND, either express or implied.  See  //
//   the License for the specific language governing          //
//   permissions and limitations under the License.           //
//------------------------------------------------------------//

#include "uvmc_common.h"

namespace uvmc {

string uvmc_legal_path(const string path) {
  string str(path);
  for (int i=0; i<str.length(); i++) {
    if (str[i] == '/')
      str[i] = '_';
  }
  return str;
}


const char* uvmc_mask_to_string(const int mask) {
  switch(mask) {
    case TLM_B_PUT_MASK        : return "tlm_blocking_put-or-tlm_fw_nonblocking_transport(tlm2)";
    case TLM_NB_PUT_MASK       : return "tlm_nonblocking_put";
    case TLM_PUT_MASK          : return "tlm_put";

    case TLM_B_GET_MASK        : return "tlm_blocking_get-or-tlm_bw_nonblocking_transport(tlm2)";
    case TLM_NB_GET_MASK       : return "tlm_nonblocking_get";
    case TLM_GET_MASK          : return "tlm_get";

    case TLM_B_PEEK_MASK       : return "tlm_blocking_peek-or-tlm_blocking_transport(tlm2)";
    case TLM_NB_PEEK_MASK      : return "tlm_nonblocking_peek";
    case TLM_PEEK_MASK         : return "tlm_peek";

    case TLM_ANALYSIS_MASK     : return "tlm_analysis";

    //case TLM_B_TRANSPORT_MASK  : return "tlm_blocking_transport";
    //case TLM_NB_TRANSPORT_MASK : return "tlm_nonblocking_transport";
    case TLM_TRANSPORT_MASK    : return "tlm_transport";

    case TLM_B_GET_PEEK_MASK   : return "tlm_blocking_get_peek";
    case TLM_NB_GET_PEEK_MASK  : return "tlm_nonblocking_get_peek";
    case TLM_GET_PEEK_MASK     : return "tlm_get_peek";

    case TLM_B_MASTER_MASK     : return "tlm_blocking_master";
    case TLM_NB_MASTER_MASK    : return "tlm_nonblocking_master";
    case TLM_MASTER_MASK       : return "tlm_master";

    case TLM_B_SLAVE_MASK      : return "tlm_blocking_slave";
    case TLM_NB_SLAVE_MASK     : return "tlm_nonblocking_slave";
    case TLM_SLAVE_MASK        : return "tlm_slave";


    default: return "<unknown>";

  }
}

};
