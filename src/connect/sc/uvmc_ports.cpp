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

#include "uvmc_ports.h"

//------------------------------------------------------------------------------
//
//
//                               SV ---> SC
//
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
// Group- DPI Imports
//
// Called by SV-side port
//------------------------------------------------------------------------------
//
// SV calls these methods with id, transaction bit stream, and size.
//
// id   - this is determined by SV2C_resolve_binding, another import function
//        called for each uvmc_channel instance on the SV side. 
//
// bits - a pointer to the data
//
//------------------------------------------------------------------------------

using uvmc::uvmc_port_base;
using uvmc::uint32;

#define GET_PORT_FROM_ID(NAME,RET) \
  uvmc_port_base* port; \
  if (id < 0 || id >= uvmc_port_base::x_ports.size()) { \
    cout << "Fatal: " << #NAME << ": invalid id, " << id << endl; \
    cout << "Possible ports are (list of " \
         << uvmc_port_base::x_ports.size() << " ports) :" << endl; \
    for (int i=0; i< uvmc_port_base::x_ports.size(); i++) { \
      cout << "  port[" << i << "]=" \
           << uvmc_port_base::x_ports[i]->name() << endl; \
    } \
    return RET; \
  } \
  port = uvmc_port_base::x_ports[id];

#define GET_PORT_FROM_ID_VOID(NAME) \
  GET_PORT_FROM_ID(NAME,)


// PUT 

extern "C" {

void SV2C_put (int id, const bits_t *bits) {
  GET_PORT_FROM_ID_VOID(SV2C_put)
  port->x_put(bits);
}
svBit SV2C_try_put (int id, const bits_t *bits) {
  GET_PORT_FROM_ID(SV2C_try_put,0)
  return port->x_try_put(bits);
}
svBit SV2C_can_put (int id) {
  GET_PORT_FROM_ID(SV2C_can_put,0)
  return port->x_can_put();
}


// WRITE

void SV2C_write (int id, const bits_t *bits) {
  GET_PORT_FROM_ID_VOID(SV2C_write)
  port->x_write(bits);
}

// GET

void SV2C_get (int id) {
  GET_PORT_FROM_ID_VOID(SV2C_get)
  port->x_get();
}
svBit SV2C_try_get (int id, bits_t *bits) {
  GET_PORT_FROM_ID(SV2C_try_get,0)
  return port->x_try_get(bits);
}
svBit SV2C_can_get (int id) {
  GET_PORT_FROM_ID(SV2C_can_get,0)
  return port->x_can_get();
}


// PEEK

void SV2C_peek (int id) {
  GET_PORT_FROM_ID_VOID(SV2C_peek)
  port->x_peek();
}
svBit SV2C_try_peek (int id, bits_t *bits) {
  GET_PORT_FROM_ID(SV2C_try_peek,0)
  return port->x_try_peek(bits);
}
svBit SV2C_can_peek (int id) {
  GET_PORT_FROM_ID(SV2C_can_peek,0)
  return port->x_can_peek();
}


// TRANSPORT

void SV2C_transport (int id, bits_t *bits) {
  GET_PORT_FROM_ID_VOID(SV2C_transport)
  port->x_transport(bits);
}
svBit SV2C_try_transport (int id, bits_t *bits) {
  GET_PORT_FROM_ID(SV2C_try_transport,0)
  //port->x_nb_transport(bits);
  return 0;
}


// TLM2
//

int SV2C_nb_transport_fw (int id, bits_t *bits, uint32 *phase, uint64 *delay) {
  GET_PORT_FROM_ID(SV2C_nb_transport_fw,tlm::TLM_COMPLETED)
  return port->x_nb_transport_fw(bits,phase,delay);
}

int SV2C_nb_transport_bw (int id, bits_t *bits, uint32 *phase, uint64 *delay) {
  GET_PORT_FROM_ID(SV2C_nb_transport_bw,tlm::TLM_COMPLETED)
  return port->x_nb_transport_bw(bits,phase,delay);
}

void  SV2C_b_transport (int id, bits_t* bits, uint64 delay) {
  GET_PORT_FROM_ID_VOID(SV2C_b_transport)
  port->x_b_transport(bits,delay);
}


/*
map<char*,sc_event*> uvmc_phase_begin;
map<char*,sc_event*> uvmc_phase_end;

void  SV2C_phase_notification(char *phase, svBit done) {
  if (!done) {
    uvmc_phase_begin[phase]->notify();
  }
  else {
    uvmc_phase_end[phase]->notify();
  }
}
*/

} // extern "C"



//------------------------------------------------------------------------------
//
// CLASS- uvmc_port_base
//
// Defines a "typeless" TLM API interface. Each uvmc_*_ports will inherit
// from this class, implementing only the methods pertaining to the port's
// interface.
//------------------------------------------------------------------------------



#define UVMCERR(N) \
  cout << "ERROR: Invalid call. " #N " must be implemented in derived class" << endl;

namespace uvmc
{

void uvmc_port_base::x_put (const bits_t *bits) {
  UVMCERR(x_put)
}

bool uvmc_port_base::x_try_put (const bits_t *bits) {
  UVMCERR(x_try_put)
  return 0;
}

bool uvmc_port_base::x_can_put () {
  UVMCERR(x_can_put)
  return 0;
}

void uvmc_port_base::x_get () {
  UVMCERR(x_get)
}

bool uvmc_port_base::x_try_get (bits_t *bits) {
  UVMCERR(x_try_get)
  return 0;
}

bool uvmc_port_base::x_can_get() {
  UVMCERR(x_can_get)
  return 0;
}

void uvmc_port_base::x_peek () {
  UVMCERR(x_peek)
}

bool uvmc_port_base::x_try_peek (bits_t *bits) {
  UVMCERR(x_try_peek)
  return 0;
}

bool uvmc_port_base::x_can_peek () {
  UVMCERR(x_can_peek)
  return 0;
}

void uvmc_port_base::x_write (const bits_t *bits) {
  UVMCERR(x_write)
}

void uvmc_port_base::x_transport (bits_t *bits) {
  UVMCERR(x_transport)
}

bool uvmc_port_base::x_nb_transport (bits_t *bits) {
  UVMCERR(x_nb_transport)
}


void uvmc_port_base::x_b_transport (bits_t *bits, uint64 delay ) {
  UVMCERR(x_b_transport);
}

int uvmc_port_base::x_nb_transport_fw (bits_t *bits, uint32 *phase, uint64 *delay ) {
  UVMCERR(x_nb_transport_fw);
  return tlm::TLM_COMPLETED;
}

int uvmc_port_base::x_nb_transport_bw (bits_t *bits, uint32 *phase, uint64 *delay ) {
  UVMCERR(x_nb_transport_bw);
  return tlm::TLM_COMPLETED;
}


// c'tor
// -----

uvmc_port_base::uvmc_port_base (string name,
                                string target,
                                int mask,
                                uvmc_packer *packer)
          : m_name(/*uvmc_legal_path*/(name)),
            m_id(0),
            m_mask(mask),
            m_x_id(-1),
            m_packer(packer)
{
    if (x_ports.size()==0) // avoid a '0' id
      x_ports.push_back(NULL);


    int id(x_ports.size());
    map<string,int>::iterator iter;

    cout << "Registering SC-side '" << m_name;
    if (target != "")
      cout << " and lookup string '" << target << "'";
    cout << " for later connection with SV" << endl;
    // mask=" << hex << mask << dec << endl; //" id=" << id << endl;

    iter = x_port_names.find(m_name);
    if (iter != x_port_names.end()) {
      int local_id = (*iter).second;
      uvmc_port_base* exist_port = x_ports[local_id];
      if (exist_port->m_x_id != 0) {
        uvmc_port_base *bound_port = x_ports[exist_port->m_x_id];
        cerr << "Error: port/channel with name " 
             << exist_port->name() << " is already bound to"
             << bound_port->name() << endl;
        return;
      }
      cout << "Warning-UVMC: Making local connection between existing " 
           << exist_port->name() << " and " << m_name << endl;
      m_locally_connected = 1;
      m_x_id = local_id;
      x_ports.push_back(this);
      x_port_names[m_name] = id;
      exist_port->m_x_id = id;
      exist_port->m_locally_connected = 1;
      return;
    }

    if (target.length() != 0) {
      iter = x_port_names.find(target);
      if (iter != x_port_names.end()) {
      iter = x_port_names.find(target);
        int local_id = (*iter).second;
        uvmc_port_base* exist_port = x_ports[local_id];
        if (exist_port->m_x_id != 0) {
          uvmc_port_base *bound_port = x_ports[exist_port->m_x_id];
          cerr << "Error: port/channel with name "
               << exist_port->name() << " is already bound to"
               << bound_port->name() << " using lookup name " << target << endl;
          return;
        }
        cout << "Warning-UVMC: Making local connection between existing " 
             << exist_port->name() << " and " << m_name
             << " using lookup string " << target << endl;
        m_locally_connected = 1;
        m_x_id = local_id;
        x_ports.push_back(this);
        x_port_names[m_name] = id;
        exist_port->m_x_id = id;
        exist_port->m_locally_connected = 1;
        return;
      }
      x_port_names[target] = id;
    }

  /* debug
  iter = x_port_names.find(m_name);
  if (iter != x_port_names.end()) {
    cerr << "Error: port with name " << m_name << " already exists" << endl;
    return;
  }

  if (target.length() != 0) {
    iter = x_port_names.find(target);
    if (iter != x_port_names.end()) {
      cerr << "Error: port with name " << m_name << " already exists" << endl;
      return;
    }
    x_port_names[target] = id;
  }
  */

  m_id = id;
  x_port_names[m_name] = id;
  x_ports.push_back(this);

  
  if (m_packer!=NULL) {
    m_i_own_packer = 0;
  }
  else {
    m_i_own_packer = 1;
    m_packer = new uvmc_packer();
  }
  
}

uvmc_port_base::uvmc_port_base() { }


// d'tor
// -----

uvmc_port_base::~uvmc_port_base() {
  for (int i=0; i < x_ports.size(); i++) {
    //cout << "***** DESTROYING UVMC PORT " << i
    //     << " name=" << x_ports[i]->name() << endl;

    //delete x_ports[i];
  }

  if (m_i_own_packer)
    delete m_packer;
  
}


// resolve
// -------

bool uvmc_port_base::resolve (string name,
                              string target,
                              int mask,
                              string type_name,
                              int sv_id,
                              int *sc_id) 
{
  map<string,int>::const_iterator iter;
  uvmc_port_base *port;
  int id;
    
  iter = x_port_names.find(name);
  if (iter != x_port_names.end()) {
    id = (*iter).second;
  }
  else {
    iter = x_port_names.find(target);
    if (iter != x_port_names.end())
      id = (*iter).second;
    else {
      cout << "Error: No SC-side proxy port registered with name="
           << name << " or lookup=" << target 
           <<  ". (mask=" << hex << mask
           << dec << " sv_id=" << sv_id << ")" << endl;
      return 0; // NO MATCH. ERROR OCCURS ON SV SIDE
    }
  }
  port = x_ports[id];
 

  // check mask compatibility
  if ((mask & port->mask()) != port->mask()) {
    cout << "Error: SC-side UVMC Port    '" << port->name()
            << "' with mask '" << hex << port->mask() << dec
         << "' and id " << port->id()
         << " not compatible with connected SV port '" << name
         << "' with mask '" << mask
         <<  dec << "' and id " << sv_id << endl;
    return 0;
  }

  // check transaction type compatibility
  // can't rely on this check; disabled
  /*
  if (port->kind().compare(sv_trans_type)) {
    cout << "Error: Channel interface '" << port->name()
         << "' carrying transaction type '" << port->kind()
         << "' not compatible with connected SV port '" << port_name
         << "' carrying transaction type '" << trans_type << endl;
    return 0;
  }
  */

  cout << "Connected SC-side '" << port->m_name
       << "' to SV-side '" << name << "'" << endl;


  // Successful connnection. Assign port's id to SV-side id.
  // Then, all communication to port will be delegated
  // from connected SV-side channel. 
  port->set_x_id(sv_id);

  // return our id back to SV side
  *sc_id = port->id();

  return 1;
}


// set_x_id
// ---------

void uvmc_port_base::set_x_id(int id)  {
  // TODO: validate before assigning
  m_x_id = id;
}


// x_id
// -----

int uvmc_port_base::x_id()  {
  return m_x_id;
}

// id
// --

int uvmc_port_base::id()  {
  return m_id;
}

// mask
// ----

int uvmc_port_base::mask() {
  return m_mask;
}


// name
// ----

const char *uvmc_port_base::name() const {
  return m_name.c_str();
}


// kind
// ----

const char *uvmc_port_base::kind() const {
  return "uvmc_port_base";
}


// static variables
// ----------------

map<string,int> uvmc_port_base::x_port_names;
vector<uvmc_port_base*> uvmc_port_base::x_ports;


} // namespace uvmc

using uvmc::uvmc_port_base;


