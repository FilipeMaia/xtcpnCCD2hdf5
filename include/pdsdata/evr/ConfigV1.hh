//
//  Class for configuration of the Event Receiver
//
#ifndef Evr_ConfigV1_hh
#define Evr_ConfigV1_hh

#include "pdsdata/xtc/TypeId.hh"
#include <stdint.h>

namespace Pds {
  namespace EvrData {

    class PulseConfig {
    public:
      PulseConfig ();
      PulseConfig (unsigned pulse,                 // Pulse ID
		   int      trigger,               // Pulse input control
		   int      set,
		   int      clear,
		   bool     polarity,              // Pulse output control
		   bool     map_set_enable,
		   bool     map_reset_enable,
		   bool     map_trigger_enable,
		   unsigned prescale,              // Pulse parameters
		   unsigned delay,
		   unsigned width);
    public:
      //  internal pulse generation channel
      unsigned pulse  () const;

      //  id of generated pulse for each mode (edge/level)
      int trigger() const;
      int set    () const;
      int clear  () const;

      //  positive(negative) level
      bool polarity          () const;
      //  enable pulse generation masks
      bool map_set_enable    () const;
      bool map_reset_enable  () const;
      bool map_trigger_enable() const;

      //  pulse event prescale
      unsigned prescale() const;
      //  delay in 119MHz clks
      unsigned delay   () const;
      //  width in 119MHz clks
      unsigned width   () const;
    private:
      uint32_t _pulse;
      uint32_t _input_control;
      uint32_t _output_control;
      uint32_t _prescale;
      uint32_t _delay;
      uint32_t _width;
    };

    class OutputMap {
    public:
      enum Source { Pulse, DBus, Prescaler, Force_High, Force_Low };
      enum Conn { FrontPanel, UnivIO };
    public:
      OutputMap ();
      OutputMap ( Source, unsigned source_id, 
		  Conn  , unsigned conn_id );
    public:
      //  source (generated pulse) of output generation
      Source   source   () const;
      unsigned source_id() const;
      //  connector for output destination
      Conn     conn     () const;
      unsigned conn_id  () const;
    public:
      //  encoded source value
      unsigned map      () const;
    private:
      uint32_t _v;
    };

    class ConfigV1 {
    public:
      enum { Version=1 };
      ConfigV1 ();
      ConfigV1 (unsigned npulses,
		const PulseConfig* pulses,
		unsigned noutputs,
		const OutputMap* outputs);

      //  pulse configurations appended to this structure
      unsigned npulses() const;
      const PulseConfig& pulse(unsigned) const;

      //  output configurations appended to this structure
      unsigned noutputs() const;
      const OutputMap& output_map(unsigned) const;

      //  size including appended PulseConfig's and OutputMap's
      unsigned size() const;
    private:
      uint32_t _npulses;
      uint32_t _noutputs;
    };
  };
};
#endif
