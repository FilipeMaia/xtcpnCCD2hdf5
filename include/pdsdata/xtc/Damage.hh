#ifndef Pds_Damage_hh
#define Pds_Damage_hh

#include <stdint.h>

namespace Pds {

  class Damage {
  public:
    enum Value {
      DroppedContribution    = 1,
      OutOfOrder             = 12,
      OutOfSynch             = 13,
      UserDefined            = 14,
      IncompleteContribution = 15
    };
    Damage(uint32_t v) : _damage(v) {}
    uint32_t  value() const             { return _damage; }
    void     increase(Damage::Value v)  { _damage |= (1<<v); }
    void     increase(uint32_t v)       { _damage |= v; }
    
  private:
    uint32_t _damage;
  };
}

#endif
