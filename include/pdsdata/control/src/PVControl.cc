#include "pdsdata/control/PVControl.hh"

using namespace Pds::ControlData;

#include <string.h>

PVControl::PVControl() {}

PVControl::PVControl(const char* pvname, unsigned index, double setValue) :
  _index(index   ),
  _value(setValue)
{
  strncpy(_name, pvname, NameSize);
}

PVControl::PVControl(const PVControl& c) :
  _index(c._index),
  _value(c._value)
{
  strncpy(_name, c._name, NameSize);
}

PVControl::~PVControl() {}

bool PVControl::operator<(const PVControl& m) const
{
  int nt = strncmp(_name, m._name, NameSize);
  return (nt<0) || (nt==0 && _index < m._index);
}

const char* PVControl::name() const { return _name; }

unsigned PVControl::index() const { return _index; }

double PVControl::value() const { return _value; }
