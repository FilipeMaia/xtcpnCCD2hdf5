#include "pdsdata/control/PVMonitor.hh"

#include <string.h>

using namespace Pds::ControlData;

PVMonitor::PVMonitor() {}

PVMonitor::PVMonitor(const char* pvname, unsigned index, double loValue, double hiValue) :
  _index  (index  ),
  _loValue(loValue),
  _hiValue(hiValue)
{
  strncpy(_name, pvname, NameSize);
}

PVMonitor::PVMonitor(const PVMonitor& m) :
  _index  (m._index  ),
  _loValue(m._loValue),
  _hiValue(m._hiValue)
{
  strncpy(_name, m._name, NameSize);
}

PVMonitor::~PVMonitor() {}

bool PVMonitor::operator<(const PVMonitor& m) const
{
  int nt = strncmp(_name, m._name, NameSize);
  return (nt<0) || (nt==0 && _index < m._index);
}

const char* PVMonitor::name() const { return _name; }

unsigned PVMonitor::index() const { return _index; }

double PVMonitor::loValue() const { return _loValue; }

double PVMonitor::hiValue() const { return _hiValue; }
