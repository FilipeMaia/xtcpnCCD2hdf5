//
//  Class for Process Variable Monitoring
//
#ifndef PdsData_PVMonitor_hh
#define PdsData_PVMonitor_hh

#include <stdint.h>

namespace Pds {

  namespace ControlData {

    class PVMonitor {
    public:
      enum { NameSize=32 };
    public:
      PVMonitor();
      PVMonitor(const char* pvname, unsigned index, double loValue, double hiValue);
      PVMonitor(const PVMonitor&);
      ~PVMonitor();
    public:
      bool operator<(const PVMonitor&) const;
    public:
      const char* name            () const;
      unsigned    index           () const;
      double      loValue         () const;
      double      hiValue         () const;
    private:
      char     _name[NameSize];
      uint32_t _index;
      double   _loValue;
      double   _hiValue;
    };

  };

};

#endif
