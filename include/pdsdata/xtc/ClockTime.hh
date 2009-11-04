#ifndef Pds_ClockTime_hh
#define Pds_ClockTime_hh

#include <stdint.h>

namespace Pds {
  class ClockTime {
  public:
    ClockTime();
    ClockTime(const ClockTime& t);
    ClockTime(unsigned sec, unsigned nsec);

  public:
    unsigned seconds    () const {return _high;}
    unsigned nanoseconds() const {return _low;}

  public:
    ClockTime& operator=(const ClockTime&);
    bool operator>(const ClockTime&) const; 

  private:
    uint32_t _low;
    uint32_t _high;
  };
}
#endif
