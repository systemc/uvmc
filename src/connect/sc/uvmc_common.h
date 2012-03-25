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

#ifndef UVMC_COMMON_H
#define UVMC_COMMON_H


// PRIMITIVE INTERFACES
#define TLM_B_PUT_MASK        (1<<0)
#define TLM_B_GET_MASK        (1<<1)
#define TLM_B_PEEK_MASK       (1<<2)
#define TLM_TRANSPORT_MASK    (1<<3)
#define TLM_NB_PUT_MASK       (1<<4)
#define TLM_NB_GET_MASK       (1<<5)
#define TLM_NB_PEEK_MASK      (1<<6)
//#define TLM_NB_TRANSPORT_MASK (1<<7)
#define TLM_ANALYSIS_MASK     (1<<8)
#define TLM_MASTER_BIT_MASK   (1<<9)
#define TLM_SLAVE_BIT_MASK    (1<<10)

#define TLM_FW_NB_TRANSPORT_MASK  (1<<0)
#define TLM_BW_NB_TRANSPORT_MASK  (1<<1)
#define TLM_B_TRANSPORT_MASK      (1<<2)

// COMBINATION INTERFACES
#define TLM_PUT_MASK         (TLM_B_PUT_MASK       | TLM_NB_PUT_MASK)
#define TLM_GET_MASK         (TLM_B_GET_MASK       | TLM_NB_GET_MASK)
#define TLM_PEEK_MASK        (TLM_B_PEEK_MASK      | TLM_NB_PEEK_MASK)
#define TLM_B_GET_PEEK_MASK  (TLM_B_GET_MASK       | TLM_B_PEEK_MASK)
#define TLM_B_MASTER_MASK    (TLM_B_PUT_MASK       | TLM_B_GET_MASK | \
                              TLM_B_PEEK_MASK      | TLM_MASTER_BIT_MASK)
#define TLM_B_SLAVE_MASK     (TLM_B_PUT_MASK       | TLM_B_GET_MASK | \
                              TLM_B_PEEK_MASK      | TLM_SLAVE_BIT_MASK)
#define TLM_NB_GET_PEEK_MASK (TLM_NB_GET_MASK      | TLM_NB_PEEK_MASK)
#define TLM_NB_MASTER_MASK   (TLM_NB_PUT_MASK      | TLM_NB_GET_MASK | \
                              TLM_NB_PEEK_MASK     | TLM_MASTER_BIT_MASK)
#define TLM_NB_SLAVE_MASK    (TLM_NB_PUT_MASK      | TLM_NB_GET_MASK | \
                              TLM_NB_PEEK_MASK     | TLM_SLAVE_BIT_MASK)
#define TLM_GET_PEEK_MASK    (TLM_GET_MASK         | TLM_PEEK_MASK)
#define TLM_MASTER_MASK      (TLM_B_MASTER_MASK    | TLM_NB_MASTER_MASK)
#define TLM_SLAVE_MASK       (TLM_B_SLAVE_MASK     | TLM_NB_SLAVE_MASK)
//#define TLM_TRANSPORT_MASK   (TLM_B_TRANSPORT_MASK | TLM_NB_TRANSPORT_MASK)


// Must be same as max words in SV!
#ifndef MAX_WORDS
#define MAX_WORDS (4096/4)
#endif

#define UVM_PORT 0
#define UVM_EXPORT 1
#define UVM_IMP  2

#include <string>
using std::string;

typedef int unsigned bits_t;

namespace uvmc {

typedef  unsigned int uint32;

string uvmc_legal_path(const string path);

const char* uvmc_mask_to_string(const int mask);

};

#endif // UVMC_COMMON_H

