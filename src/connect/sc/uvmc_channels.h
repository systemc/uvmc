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

#ifndef UVMC_CHANNELS_H
#define UVMC_CHANNELS_H

//------------------------------------------------------------------------------
//
//
//                               SC ---> SV
//
//
//------------------------------------------------------------------------------

//typedef unsigned int bits_t;

#include "svdpi.h"

#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <systemc.h>
#include <tlm.h>


using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using std::string;
using std::map;
using std::vector;
using sc_core::sc_semaphore;

#include "uvmc_common.h"
#include "uvmc_convert.h"


//------------------------------------------------------------------------------
//
// Group- DPI-C export functions
//
// Not intended for public use.
//
// The following C2SV-prefixed functions are called by UVMC to transfer
// packed bits across the language boundary using standard DPI-C.
//
//------------------------------------------------------------------------------

extern "C"
{

void  C2SV_put      (int x_id, const bits_t *bits);
bool  C2SV_try_put  (int x_id, const bits_t *bits);
bool  C2SV_can_put  (int x_id);
void  C2SV_get      (int x_id);
bool  C2SV_try_get  (int x_id, bits_t *bits);
bool  C2SV_can_get  (int x_id);
void  C2SV_peek     (int x_id);
bool  C2SV_try_peek (int x_id, bits_t *bits);
bool  C2SV_can_peek (int x_id);
void  C2SV_write    (int x_id, const bits_t *bits);

void  C2SV_transport    (int x_id, bits_t *bits);
bool  C2SV_try_transport(int x_id, bits_t *bits);

int   C2SV_nb_transport_fw (int x_id,
                            bits_t *bits,
                            unsigned int *phase,
                            uint64 *delay);

int   C2SV_nb_transport_bw (int x_id,
                            bits_t *bits,
                            unsigned int *phase,
                            uint64 *delay);

void  C2SV_b_transport     (int x_id,
                            bits_t *bits,
                            uint64 delay);

void  SV2C_blocking_req_done(int x_id);
void  SV2C_blocking_rsp_done(int x_id, const bits_t *bits, uint64 delay);
}

namespace uvmc
{

//------------------------------------------------------------------------------
//
// CLASS- uvmc_channel_base
//
// Not intended for public use.
//
//
//------------------------------------------------------------------------------

class uvmc_channel_base : public virtual sc_prim_channel {

  public:
  uvmc_channel_base(const string name,
                    const string target,
                    const int mask,
                    uvmc_packer* packer=NULL);
  virtual ~uvmc_channel_base();

  int id();
  int mask();
  int x_id();
  void set_x_id(int id);
  virtual const char *name() const;
  virtual const char *kind() const;

  static bool resolve(const string name, const string target, const int mask,
                      const string type_name, const int sv_id, int *sc_id);

  protected:
  static map<string,int> x_channel_names;
  static vector<uvmc_channel_base*> x_channels;

  mutable bits_t m_bits[MAX_WORDS];

  protected:
  string m_name;
  int m_id;
  int m_mask;
  int m_x_id; // the id of our SV-counterpart
  uvmc_packer *m_packer;
  bool m_i_own_packer;
  bool m_locally_connected;

  void set_x_connected();
  sc_event m_x_connected_event;
  bool m_x_connected;
  sc_event m_never_triggered;


  private:
  explicit uvmc_channel_base();

  protected:
  mutable sc_semaphore m_sem;
  uint64 m_delay_bits;

  public:
  static uvmc_channel_base* get_x_port(const int id);

  sc_event m_blocking_op_done;
  virtual void blocking_req_done();
  virtual void blocking_rsp_done(const bits_t *bits, uint64 delay);

  void wait_x_connected();


};

extern bool uvmc_wait_for_run_phase;


//------------------------------------------------------------------------------
//
// CLASS- uvmc_tlm1_channel_proxy
//
// Not intended as a user class.
// The <uvmc_connect> function is the user of this proxy. 
//
// The ~uvmc_tlm1_channel_proxy~ is used to connect with registered TLM1 ports
// so they pass connectivity checks during SystemC elaboration. It serves as a
// proxy to an export or imp residing in SystemVerilog.
//
//|                             bind
//|  sc_port<[TLM1 interface]>  ---->  uvmc_tlm1_channel_proxy
//
// The <uvmc_connect> function creates, binds, and returns an instance of this
// proxy.
//------------------------------------------------------------------------------

template <class T1, class T2=T1,
          class CVRT_T1=uvmc_converter<T1>,
          class CVRT_T2=uvmc_converter<T2> >
class uvmc_tlm1_channel_proxy
    : public virtual uvmc_channel_base,
      public virtual tlm_put_if<T1>,
      public virtual tlm_get_peek_if<T1>,
      public virtual tlm_master_if<T1,T2>,
      public virtual tlm_slave_if<T2,T1>,
      public virtual tlm_transport_if<T1,T2>
{
  public:
  typedef uvmc_tlm1_channel_proxy <T1,T2,CVRT_T1,CVRT_T2> this_type;

  public:
  // c'tor

  uvmc_tlm1_channel_proxy(const string name, const string lookup, const int mask,
     uvmc_packer *packer=NULL) : uvmc_channel_base(name, lookup, mask, packer) {
  }

 
  // B_PUT

  virtual void put (const T1 &t) {
    wait_x_connected();
    m_sem.wait();
    m_packer->init_pack(m_bits);
    CVRT_T1::do_pack(t,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_put(m_x_id,m_bits);
    wait(m_blocking_op_done);
    m_sem.post();
  }


  // NB_PUT

  virtual bool nb_put (const T1 &t) {
    if (!nb_can_put())
      return 0;
    m_packer->init_pack(m_bits);
    CVRT_T1::do_pack(t,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    return C2SV_try_put(m_x_id,m_bits);
  }

  virtual bool nb_can_put (tlm_tag<T1> *t=0) const {
    if (!m_x_connected)
      return 0;
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    return C2SV_can_put(m_x_id);
  }
   
  virtual const sc_event &ok_to_put (tlm_tag<T1> *t=0) const  {
    // return *something* non-null, effectively disabled.
    return m_never_triggered;
  }

  // B_GET

  virtual void get (T2 &t) {
    wait_x_connected();
    m_sem.wait();
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_get(m_x_id);
    wait(m_blocking_op_done);
    m_packer->init_unpack(m_bits);
    CVRT_T2::do_unpack(t,*m_packer);
    m_sem.post();
  }

  virtual T2 get (tlm_tag<T2> *t=0)  {
    T2 tr;
    get(tr);
    return tr;
  }
   
  // NB_GET

  virtual bool nb_get (T2 &t)  {
    bool result;
    if (!nb_can_get())
      return 0;
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    if (C2SV_try_get(m_x_id,m_bits)) {
      m_packer->init_unpack(m_bits);
      CVRT_T2::do_unpack(t,*m_packer);
      return 1;
    }
    return 0;
  }
   
  virtual bool nb_can_get (tlm_tag<T2> *t=0) const  {
    if (!m_x_connected)
      return 0;
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    return C2SV_can_get(m_x_id);
  }
   
  virtual const sc_event &ok_to_get (tlm_tag<T2> *t=0) const  {
    // return *something* non-null, effectively disabled.
    return m_never_triggered;
  }

  
  // B_PEEK

  virtual void peek (T2& t) const  {
    // ADAME: Note: wait methods are not const. Need to play trick.
    m_sem.wait();
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_peek(m_x_id);
    const_cast< this_type * >( this )->wait(m_blocking_op_done);
    m_packer->init_unpack(m_bits);
    CVRT_T2::do_unpack(t,*m_packer);
    m_sem.post();
  }
   
  virtual T2 peek (tlm_tag<T2> *t=0) const {
    T2 tr;
    peek(tr);
    return tr;
  }

  // NB_PEEK

  virtual bool nb_peek (T2 &t) const  {
    if (!nb_can_peek())
      return 0;
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    if (C2SV_try_peek(m_x_id,m_bits)) {
      m_packer->init_unpack(m_bits);
      CVRT_T2::do_unpack(t,*m_packer);
      return 1;
    }
    return 0;
  }
   
  virtual bool nb_can_peek (tlm_tag<T2> *t=0) const  {
    if (!m_x_connected)
      return 0;
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    return C2SV_can_peek(m_x_id);
  }
   
  virtual const sc_event &ok_to_peek (tlm_tag<T2> *t=0) const  {
    // return *something* non-null, effectively disabled.
    return m_never_triggered;
  }
   
  // TRANSPORT (TLM1)

  virtual void transport(const T1 &req, T2 &rsp)  {
    wait_x_connected();
    m_sem.wait();
    m_packer->init_pack(m_bits);
    CVRT_T1::do_pack(req,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_transport(m_x_id,m_bits);
    wait(m_blocking_op_done); 
    m_packer->init_unpack(m_bits);
    CVRT_T2::do_unpack(rsp,*m_packer);
    m_sem.post();
  }
   
  // tlm_core_ifs defines return type as return-by-value!;
  // RSP must define copy ctor!  Inefficient.
  virtual T2 transport(const T1 &req) {
    T2 rsp;
    transport(req,rsp);
    return rsp;
  }
};



//------------------------------------------------------------------------------
//
// CLASS- uvmc_tlm2_channel_proxy
//
// Not intended as a user class.
// The <uvmc_connect> function is the user of this proxy. 
//
// The ~uvmc_tlm2_channel_proxy~ is used to connect with registered TLM2 ports
// so they pass connectivity checks during SystemC elaboration. It serves as a
// proxy to an export or imp residing in SystemVerilog.
//
//|                             bind
//|  sc_port<[TLM2 interface]>  ---->  uvmc_tlm2_channel_proxy
//
// This proxy is also used as a proxy channel for connecting analysis ports, i.e.
// ~sc_port<tlm_analysis_if<T>~.
//
// The <uvmc_connect> function creates, binds, and returns an instance of
// this proxy.
//------------------------------------------------------------------------------


template <class T, class PHASE=tlm_phase, class CVRT=uvmc_converter<T> >
class uvmc_tlm2_channel_proxy
    : public uvmc_channel_base,
      public virtual tlm_blocking_transport_if<T>,
      public virtual tlm_fw_nonblocking_transport_if<T,PHASE>,
      public virtual tlm_bw_nonblocking_transport_if<T,PHASE>,
      public virtual tlm_analysis_if<T>,
      public virtual tlm_write_if<T>
{
  public:
  uvmc_tlm2_channel_proxy(const string name, const string lookup,
                            const int mask, uvmc_packer *packer=NULL) :
                        uvmc_channel_base(name, lookup, mask, packer) {
  }

  virtual ~uvmc_tlm2_channel_proxy() {
  }

  // B_TRANSPORT (TLM2)

  virtual void b_transport( T& trans, sc_time& t )
  {
    double delay_in_ps = t.to_seconds() * 1e12; 
    uint64* delay_in_bits = reinterpret_cast<uint64*>(&delay_in_ps);
    wait_x_connected();
    m_sem.wait();
    m_packer->init_pack(m_bits);
    CVRT::do_pack(trans,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_b_transport(m_x_id, m_bits, *delay_in_bits);
    wait(m_blocking_op_done);
    m_packer->init_unpack(m_bits);
    CVRT::do_unpack(trans,*m_packer);
    delay_in_ps = *(reinterpret_cast<double*>(&m_delay_bits));
    t = sc_time(delay_in_ps,SC_PS);
    m_sem.post();
  }

  // NB_TRANSPORT_FW

  virtual tlm::tlm_sync_enum nb_transport_fw( T& trans,
                                              PHASE& phase,
                                              sc_core::sc_time& t)
  {
    if (!m_x_connected) {
      cout << "ERROR: " << name()
           << ".nb_transport_fw called before SV-side was connected" << endl;
      // Todo- if TLM GP, default resp is TLM_INCOMPLETE_RESPONSE
      return TLM_COMPLETED;
    }
    int result;
    tlm_sync_enum sync;
    unsigned int ph = phase;
    double delay_in_ps = t.to_seconds() * 1e12; 
    uint64* delay_in_bits = reinterpret_cast<uint64*>(&delay_in_ps);
    m_packer->init_pack(m_bits);
    CVRT::do_pack(trans,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    result = C2SV_nb_transport_fw(m_x_id, m_bits, &ph, delay_in_bits);
    sync = static_cast<tlm_sync_enum>(result);
    phase = ph;
    m_packer->init_unpack(m_bits);
    CVRT::do_unpack(trans,*m_packer);
    delay_in_ps = *(reinterpret_cast<double*>(&delay_in_bits));
    t = sc_time(delay_in_ps,SC_PS);
    return sync;
  }

  // NB_TRANSPORT_BW

  virtual tlm::tlm_sync_enum nb_transport_bw( T& trans,
                                              PHASE& phase,
                                              sc_core::sc_time& t)
  {
    if (!m_x_connected) {
      cout << "ERROR: " << name()
           << ".nb_transport_bw called before SV-side was connected" << endl;
      return TLM_COMPLETED;
    }
    int result;
    tlm_sync_enum sync;
    unsigned int ph = phase;
    double delay_in_ps = t.to_seconds() * 1e12; 
    uint64* delay_in_bits = reinterpret_cast<uint64*>(&delay_in_ps);
    m_packer->init_pack(m_bits);
    CVRT::do_pack(trans,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    result = C2SV_nb_transport_bw(m_x_id, m_bits, &ph, delay_in_bits);
    sync = static_cast<tlm_sync_enum>(result);
    phase = ph;
    m_packer->init_unpack(m_bits);
    CVRT::do_unpack(trans,*m_packer);
    delay_in_ps = *(reinterpret_cast<double*>(&delay_in_bits));
    t = sc_time(delay_in_ps,SC_PS);
    return sync;
  }

  virtual int x_nb_transport_bw (bits_t *bits, uint32 *phase, uint64 delay) {
    cout << "ERROR: x_nb_transport_bw not implemented" << endl;
    return 0;
  }

  // WRITE (ANALYSIS)

  virtual void write(const T& t)  {
    if (!m_x_connected) {
      cout << "ERROR: " << name()
           << ".write called before SV-side was connected. "
           << "Transaction dropped." << endl;
      return;
    }
    m_packer->init_pack(m_bits);
    CVRT::do_pack(t,*m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    C2SV_write(m_x_id,m_bits);
  }


};


//------------------------------------------------------------------------------
// CLASS- uvmc_target_socket
//
// Not intended as a user class.
// The <uvmc_connect> function is the user of this proxy. 
//
// The ~uvmc_target_socket~ is used to connect with registered TLM2 initiator
// sockets so they pass connectivity checks during SystemC elaboration. It
// serves as a proxy to a TLM2 target socket residing in SystemVerilog.
//
//|                              bind
//|  tlm_initiator_socket<T,...> ---->  uvmc_target_socket
//
//------------------------------------------------------------------------------
// Implements fw interface; 

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm_base_protocol_types,
          int N = 1,
          sc_port_policy POL = SC_ONE_OR_MORE_BOUND,
          class CVRT=uvmc_converter<typename TYPES::tlm_payload_type> >

class uvmc_target_socket
    : public uvmc_tlm2_channel_proxy<typename TYPES::tlm_payload_type,
                                     typename TYPES::tlm_phase_type, CVRT>,
      public tlm_fw_transport_if<TYPES>
{
  public:
  typedef uvmc_tlm2_channel_proxy<typename TYPES::tlm_payload_type,
                                  typename TYPES::tlm_phase_type,
                                    CVRT> proxy_type;
  typedef tlm_target_socket <BUSWIDTH, TYPES, N, POL> target_type;

  target_type m_target;

  uvmc_target_socket(const string name, const string lookup, 
                       uvmc_packer *packer=NULL) :
           proxy_type(name, lookup, TLM_FW_NB_TRANSPORT_MASK |
                                    TLM_BW_NB_TRANSPORT_MASK |
                                    TLM_B_TRANSPORT_MASK, packer),
           m_target((string("UVMC_TARGET_SOCKET_FOR_") +
                    uvmc_legal_path(name)).c_str()) {
    m_target.bind( *this );
  }

  virtual int x_nb_transport_bw (bits_t *bits,
                                 uint32 *phase,
                                 uint64 *delay) {
    typename TYPES::tlm_payload_type x_trans;
    double* d = reinterpret_cast<double*>(&delay);
    sc_time x_delay = sc_time(*d,SC_PS);
    typename TYPES::tlm_phase_type x_phase = *phase;
    proxy_type::m_packer->init_unpack(bits);
    CVRT::do_unpack(x_trans,*proxy_type::m_packer);
    tlm_sync_enum result;
    result = m_target->nb_transport_bw(x_trans,x_phase,x_delay);
    *phase = x_phase;
    double delay_in_ps = x_delay.to_seconds() * 1e12; 
    delay = reinterpret_cast<uint64*>(&delay_in_ps);
    proxy_type::m_packer->init_pack(bits);
    CVRT::do_pack(x_trans,*proxy_type::m_packer);
    svSetScope(svGetScopeFromName("uvmc_pkg"));
    return result;
  }

  virtual bool get_direct_mem_ptr(typename TYPES::tlm_payload_type &trans,
                                  tlm::tlm_dmi& dmi_data) {
    //cout << "UVMC WARN: get_direct_mem_ptr not implemented in SV" << endl;
    return false;
  }

  virtual unsigned int transport_dbg(typename TYPES::tlm_payload_type &trans) {
    //cout << "UVMC WARN: transport_dbg not implemented in SV" << endl;
    return -1;
  }

  virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                         sc_dt::uint64 end_range) {
    //cout << "UVMC WARN: invalidate_direct_mem_ptr not implemented in SV" << endl;
  }

};


}; // namespace uvmc

#endif // UVMC_CHANNELS_H
