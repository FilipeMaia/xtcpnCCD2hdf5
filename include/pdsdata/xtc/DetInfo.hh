#ifndef Pds_DetInfo_hh
#define Pds_DetInfo_hh

#include <stdint.h>
#include "pdsdata/xtc/Src.hh"

namespace Pds {

  class Node;

  class DetInfo : public Src {
  public:

    enum Detector {NoDetector,AmoIms,AmoGasdet,AmoETof,AmoITof,AmoMbs,AmoIis,AmoBps,
                   Camp,NumDetector};
    enum Device   {NoDevice,Evr,Acqiris,Opal1000,TM6740,pnCCD,NumDevice};

    DetInfo() {}
    DetInfo(uint32_t processId,
            Detector det, uint32_t detId,
            Device dev,   uint32_t devId);

    uint32_t processId() const;
    Detector detector()  const;
    Device   device()    const;
    uint32_t detId()     const;
    uint32_t devId()     const;

    static const char* name(Detector);
    static const char* name(Device);
    static const char* name(const DetInfo&);
  };

}
#endif
