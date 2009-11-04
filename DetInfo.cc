
#include "pdsdata/xtc/DetInfo.hh"
#include "pdsdata/xtc/Level.hh"
#include <stdint.h>
#include <stdio.h>

using namespace Pds;

DetInfo::DetInfo(uint32_t processId,
         Detector det, uint32_t detId,
         Device dev,   uint32_t devId) : Src(Level::Source) {
  _log |= processId&0x00ffffff;
  _phy = ((det&0xff)<<24) | ((detId&0xff)<<16) | ((dev&0xff)<<8) |(devId&0xff);
}

uint32_t DetInfo::processId() const { return _log&0xffffff; }

DetInfo::Detector DetInfo::detector() const {return (Detector)((_phy&0xff000000)>>24);}
DetInfo::Device   DetInfo::device()   const {return (Device)((_phy&0xff00)>>8);}
uint32_t          DetInfo::detId()    const {return (_phy&0xff0000)>>16;}
uint32_t          DetInfo::devId()    const {return _phy&0xff;}
    
const char* DetInfo::name(Detector det){
  static const char* _detNames[] = {
    "NoDetector",
    "AmoIms",
    "AmoPem",
    "AmoETof",
    "AmoITof",
    "AmoMbs",
    "AmoIis",
    "AmoBps",
    "Camp"
  };
  return (det < NumDetector ? _detNames[det] : "-Invalid-");
}

const char* DetInfo::name(Device dev) {
  static const char* _devNames[] = {
    "NoDevice",
    "Evr",
    "Acqiris",
    "Opal1000",
    "TM6740",
    "pnCCD",
    "NumDevice"
  };
  return (dev < NumDevice ? _devNames[dev] : "-Invalid-");
}

const char* DetInfo::name(const DetInfo& src) {
  const int MaxLength=32;
  static char _name[MaxLength];
  snprintf(_name, MaxLength, "%s-%d|%s-%d", 
	   name(src.detector()), src.detId(),
	   name(src.device  ()), src.devId());
  return _name;
}
