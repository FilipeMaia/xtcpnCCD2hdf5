#include "pdsdata/evr/ConfigV1.hh"

#include <string.h>

using namespace Pds;
using namespace EvrData;

enum { Trigger_Shift=0, Set_Shift=8, Clear_Shift=16 };
enum { Polarity_Shift=0, 
       Map_Set_Ena_Shift=1,
       Map_Reset_Ena_Shift=2,
       Map_Trigger_Ena_Shift=3 };

PulseConfig::PulseConfig () {}

PulseConfig::PulseConfig (unsigned pulse,
			  int      trigger,       // Pulse input control
			  int      set,
			  int      clear,
			  bool polarity,          // Pulse output control
			  bool map_set_ena,
			  bool map_reset_ena,
			  bool map_trigger_ena,
			  unsigned prescale,      // Pulse parameters
			  unsigned delay,
			  unsigned width) :
  _pulse         (pulse),
  _input_control ( ((trigger+1) << Trigger_Shift) |
		   (    (set+1) << Set_Shift    ) |
		   (  (clear+1) << Clear_Shift  ) ),
  _output_control( (       polarity ? (1<<Polarity_Shift       ) : 0) |
		   (  map_reset_ena ? (1<<Map_Reset_Ena_Shift  ) : 0) |
		   (map_trigger_ena ? (1<<Map_Trigger_Ena_Shift) : 0) ),
  _prescale      (prescale),
  _delay         (delay),
  _width         (width)
{
}

unsigned PulseConfig::pulse() const
{ return _pulse; }

int PulseConfig::trigger() const 
{ return ((_input_control >> Trigger_Shift) & 0xff)-1; }

int PulseConfig::set    () const
{ return ((_input_control >> Set_Shift) & 0xff)-1; }

int PulseConfig::clear  () const
{ return ((_input_control >> Clear_Shift) & 0xff)-1; }


bool PulseConfig::polarity          () const
{ return _output_control & (1<<Polarity_Shift); }

bool PulseConfig::map_set_enable  () const
{ return _output_control & (1<<Map_Set_Ena_Shift); }

bool PulseConfig::map_reset_enable  () const
{ return _output_control & (1<<Map_Reset_Ena_Shift); }

bool PulseConfig::map_trigger_enable() const
{ return _output_control & (1<<Map_Trigger_Ena_Shift); }


unsigned PulseConfig::prescale() const { return _prescale; }

unsigned PulseConfig::delay   () const { return _delay; }

unsigned PulseConfig::width   () const { return _width; }


OutputMap::OutputMap () {}

OutputMap::OutputMap (Source src , unsigned source_id,
		      Conn   conn, unsigned conn_id)
{
  _v = ((unsigned(src) << 0) | 
	(    source_id << 8) | 
	(unsigned(conn)<<16) | 
	(      conn_id <<24));
}

OutputMap::Source OutputMap::source() const
{ return (Source)(_v&0xff); }

unsigned OutputMap::source_id() const
{ return (_v>>8)&0xff; }

OutputMap::Conn OutputMap::conn() const
{ return (Conn)((_v>>16)&0xff); }

unsigned OutputMap::conn_id() const
{ return _v>>24; }

unsigned OutputMap::map() const
{
  enum { Pulse_Offset=0, 
	 DBus_Offset=32, 
	 Prescaler_Offset=40 }; 
  unsigned map=0;
  Source src    = source();
  unsigned    src_id =source_id();
  switch(src) {
  case Pulse     : map = (src_id + Pulse_Offset); break;
  case DBus      : map = (src_id + DBus_Offset); break;
  case Prescaler : map = (src_id + Prescaler_Offset); break;
  case Force_High: map = 62; break;
  case Force_Low : map = 63; break;
  }
  return map;
}



ConfigV1::ConfigV1 () {}

ConfigV1::ConfigV1 (unsigned npulses,
		    const PulseConfig* pulses,
		    unsigned noutputs,
		    const OutputMap* outputs) :
  _npulses (0),
  _noutputs(0)
{
  char* next = reinterpret_cast<char*>(this+1);
  memcpy(next, pulses, npulses*sizeof(PulseConfig));
  _npulses=npulses;
  next += npulses*sizeof(PulseConfig);
  memcpy(next, outputs, noutputs*sizeof(OutputMap));
  _noutputs=noutputs;
}


unsigned ConfigV1::npulses() const { return _npulses; }
const PulseConfig& ConfigV1::pulse(unsigned pulse) const
{
  const PulseConfig* p = reinterpret_cast<const PulseConfig*>(this+1);
  return p[pulse];
}

unsigned ConfigV1::noutputs() const { return _noutputs; }
const OutputMap& ConfigV1::output_map(unsigned output) const
{
  const OutputMap* m = reinterpret_cast<const OutputMap*>(&pulse(_npulses));
  return m[output];
}


unsigned ConfigV1::size() const
{ 
  return (sizeof(*this) + 
	  _npulses*sizeof(PulseConfig) + 
	  _noutputs*sizeof(OutputMap));
}

