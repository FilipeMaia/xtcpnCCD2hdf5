
#include "pdsdata/xtc/Src.hh"
#include <stdint.h>

using namespace Pds;

Src::Src() : _log(-1L), _phy(-1L) {}
Src::Src(Level::Type level) {
  uint32_t temp = (uint32_t)level;
  _log=(temp&0xff)<<24;
}

uint32_t Src::log()   const { return _log; }
uint32_t Src::phy()   const { return _phy; }
Level::Type Src::level() const { return (Level::Type)((_log>>24)&0xff); }

bool Src::operator==(const Src& s) const { return _phy==s._phy && _log==s._log; }
