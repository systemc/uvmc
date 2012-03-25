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

#include "uvmc_channels.h"


//------------------------------------------------------------------------------
//
//
//                              SC ---> SV
//
//
//------------------------------------------------------------------------------

namespace uvmc
{

bool uvmc_wait_for_run_phase = 0;


//------------------------------------------------------------------------------
//
// CLASS- uvmc_channel_base
//
//------------------------------------------------------------------------------


uvmc_channel_base::uvmc_channel_base() :
      m_id(-1), m_mask(0), m_name(""), m_x_id(-1), m_sem(1)
{ }

// c'tor
// -----

uvmc_channel_base::uvmc_channel_base(const string name, const string target,
                                         const int mask, uvmc_packer *packer) : 
      m_id(-1), m_mask(mask), m_name(/*uvmc_legal_path*/(name)), m_x_id(0),
      m_sem(1), m_x_connected(0), m_packer(packer)
{

    if (x_channels.size()==0) // avoid a '0' id
      x_channels.push_back(NULL);

    int id(x_channels.size());

    map<string,int>::iterator iter;

    cout << "Registering SC-side '" << m_name;
    if (target != "")
      cout << " and lookup string '" << target << "'";
    cout << " for later connection with SV" << endl;
    // mask=" << hex << mask << dec << endl; //" id=" << id << endl;


    iter = x_channel_names.find(m_name);
    if (iter != x_channel_names.end()) {
      int local_id = (*iter).second;
      uvmc_channel_base* exist_chan = x_channels[local_id];
      if (exist_chan->m_x_id != 0) {
        uvmc_channel_base *bound_chan = x_channels[exist_chan->m_x_id];
        cerr << "Error: port/channel with name "
             << exist_chan->name() << " is already bound to"
             << bound_chan->name() << endl;
        return;
      }
      cout << "Warning-UVMC: Making local connection between existing " 
           << exist_chan->name() << " and " << m_name << endl;
      m_locally_connected = 1;
      m_x_id = local_id;
      x_channels.push_back(this);
      x_channel_names[m_name] = id;
      exist_chan->m_x_id = id;
      exist_chan->m_locally_connected = 1;
      return;
    }

    if (target.length() != 0) {
      iter = x_channel_names.find(target);
      if (iter != x_channel_names.end()) {
        int local_id = (*iter).second;
        uvmc_channel_base* exist_chan = x_channels[local_id];
        if (exist_chan->m_x_id != 0) {
          uvmc_channel_base *bound_chan = x_channels[exist_chan->m_x_id];
          cerr << "Error: port/channel with name "
               << exist_chan->name() << " is already bound to"
               << bound_chan->name() << " using lookup name " << target << endl;
          return;
        }
        cout << "Warning-UVMC: Making local connection between existing " 
             << exist_chan->name() << " and "
             << m_name << " using lookup string " << target << endl;
        m_locally_connected = 1;
        m_x_id = local_id;
        x_channels.push_back(this);
        x_channel_names[m_name] = id;
        exist_chan->m_x_id = id;
        exist_chan->m_locally_connected = 1;
        return;
      }
      x_channel_names[target] = id;
    }

    m_id = id;
    x_channel_names[m_name] = id;
    x_channels.push_back(this);


    if (m_packer!=NULL) {
      m_i_own_packer = 0;
    }
    else {
      m_i_own_packer = 1;
      m_packer = new uvmc_packer();
    }

}


// d'tor
// -----

uvmc_channel_base::~uvmc_channel_base() {
  for (int i=0; i < x_channels.size(); i++) {
    //cout << "***** DESTROYING UVMC CHANNEL " << i
    //     << " name=" << x_channels[i]->name() << endl;
    delete x_channels[i];
  }
  if (m_i_own_packer)
    delete m_packer;
}


// get_x_port (static)
// -----------

uvmc_channel_base* uvmc_channel_base::get_x_port(const int id)
{
  if (id < 0 || id >= uvmc_channel_base::x_channels.size()) {
    cout << "Fatal: SV2C_x_trans_done: invalid id, " << id << endl;
    return NULL;
  }
  return uvmc_channel_base::x_channels[id];
}


// resolve
// -------

bool uvmc_channel_base::resolve(const string name,
                                const string target,
                                const int mask,
                                const string type_name,
                                const int sv_id,
                                int *sc_id) 
{
  map<string,int>::const_iterator iter;
  uvmc_channel_base *channel;
  int id;
    
  iter = x_channel_names.find(name);
  if (iter != x_channel_names.end()) {
    id = (*iter).second;
  }
  else {
    iter = x_channel_names.find(target);
    if (iter != x_channel_names.end()) {
      id = (*iter).second;
    }
    else {
      cout << "Error: No SC-side proxy channel registered with name="
           << name << " or lookup=" << target <<  ". (mask="
           << hex << mask << dec << " sv_id=" << sv_id << ")" << endl;
      map<string,int>::const_iterator last = x_channel_names.end(); 
      cout << "  Registered SC-side UVMC channels are:" << endl;
      for (map<string,int>::const_iterator it = x_channel_names.begin();
           it != last; ++it)
      {
        string nm = (*it).first;
        cout << "    '" << nm << "'";
        if (nm == target)
          cout << "  MATCH!" << endl;
        else
          cout << endl;
        //cout << "    " << x_channels[ (*it).second ].name() << endl;
      }
      cout << endl;

      return 0;
    }
  }
  channel = x_channels[id];
 

  // check mask compatibility
  if ((mask & channel->mask()) != channel->mask()) {
    cout << "Error: SC-side UVMC Channel '" << channel->name()
         << "' with mask '" << hex << channel->mask() << dec
         << "' and id " << channel->id()
         << " not compatible with connected SV port '" << name
         << "' with mask '" << mask <<  dec << "' and id " << sv_id << endl;
    return 0;
  }

  // check transaction type compatibility
  /*
  if (channel->kind().compare(sv_trans_type)) {
    cout << "Error: Channel interface '" << channel->name()
         << "' carrying transaction type '" << channel->kind()
         << "' not compatible with connected SV port '" << port_name
         << "' carrying transaction type '" << trans_type << endl;
    return 0;
  }
  */

  // Successful connnection. Assign channel's id to SV-side id.
  // Then, all communication from channel will be delegated to
  // connected SV-side port. 
  channel->set_x_id(sv_id);
  *sc_id = channel->id();
  
  cout << "Connected SC-side '" << channel->name()
       << "' to SV-side '" << name << "'" << endl;

  if (!uvmc_wait_for_run_phase) {
    channel->set_x_connected();
  }

  return 1;

}

void uvmc_channel_base::blocking_req_done() {
  m_blocking_op_done.notify();
}

void uvmc_channel_base::blocking_rsp_done(const bits_t *bits, uint64 delay) {
  // TODO: use memcpy?
  for (int i=0; i<MAX_WORDS; i++) {
    m_bits[i] = bits[i];
  }
  m_delay_bits = delay;
  m_blocking_op_done.notify();
}



// name
// ----

const char *uvmc_channel_base::name() const {
  return m_name.c_str();
}


// kind
// ----

const char *uvmc_channel_base::kind() const {
  return "uvmc_channel_base";
}


// id
// --

int uvmc_channel_base::id()  {
    return m_id;
}


// mask
// ----

int uvmc_channel_base::mask() {
    return m_mask;
}

// x_id
// ----

int uvmc_channel_base::x_id()  {
    return m_x_id;
}


// set_x_id
// --------

void uvmc_channel_base::set_x_id(int id) {
    m_x_id = id;
}


// set_x_connected
// ---------------

void uvmc_channel_base::set_x_connected() {
  m_x_connected = 1;
  m_x_connected_event.notify();
}


// wait_x_connected
// ----------------

void uvmc_channel_base::wait_x_connected() {
  sc_process_handle proc_h = sc_get_current_process_handle();
  /*
  if (proc_h.proc_kind() == SC_METHOD_PROC_) {
    return;
  }
  else {
  */
    if (uvmc_wait_for_run_phase) {
      svSetScope(svGetScopeFromName("uvmc_pkg"));
      uvmc_wait_for_phase("run",UVM_PHASE_STARTED,UVM_GTE);
      if (!m_x_connected)
        cout << "Error: Port '" << name()
             << "' not connected to SV side, yet reached run phase." << endl;
    }
    if (m_x_connected==0)
      ::wait(m_x_connected_event);
  //}
}

// internal variables
map<string,int> uvmc_channel_base::x_channel_names;
vector<uvmc_channel_base*> uvmc_channel_base::x_channels;

}; // namespace uvmc



//------------------------------------------------------------------------------
//
// DPI IMPORT FUNC IMPLS
//
//------------------------------------------------------------------------------

using namespace uvmc;

void SV2C_blocking_req_done(int x_id)
{
  uvmc_channel_base* port;
  port = uvmc_channel_base::get_x_port(x_id);
  if (port)
    port->blocking_req_done();
}


void SV2C_blocking_rsp_done(int x_id,
                            const bits_t *bits,
                            uint64 delay)
{
  uvmc_channel_base* port;
  port = uvmc_channel_base::get_x_port(x_id);
  if (port) {
    port->blocking_rsp_done(bits,delay);
  }
}


extern "C"

/*
//extern "C" 
extern void global_stop_request();

void uvm_global_stop_request() {
  svSetScope(svGetScopeFromName("uvm_pkg"));
  cout << "Calling SV-UVM global_stop_request" << endl;
  global_stop_request();
}
*/

