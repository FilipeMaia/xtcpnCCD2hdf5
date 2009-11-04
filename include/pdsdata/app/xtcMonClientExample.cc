#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pdsdata/xtc/DetInfo.hh"
#include "pdsdata/xtc/ProcInfo.hh"
#include "pdsdata/xtc/XtcIterator.hh"
#include "pdsdata/xtc/XtcFileIterator.hh"
#include "pdsdata/acqiris/ConfigV1.hh"
#include "pdsdata/acqiris/DataDescV1.hh"
#include "pdsdata/camera/FrameV1.hh"
#include "pdsdata/camera/FrameFexConfigV1.hh"
#include "pdsdata/camera/TwoDGaussianV1.hh"
#include "pdsdata/evr/ConfigV1.hh"
#include "pdsdata/opal1k/ConfigV1.hh"
#include "pdsdata/pnCCD/fformat.h"

#include "XtcMonitorClient.hh"

class myLevelIter : public XtcIterator {
public:
  enum {Stop, Continue};
  myLevelIter(Xtc* xtc, unsigned depth) : XtcIterator(xtc), _depth(depth) {}
  fileHeaderType fileHdrBuffer[1024];

  void process(const DetInfo& d, const Camera::FrameV1& f) {
    printf("*** Processing frame object\n");
  }
  void process(const DetInfo&, const Acqiris::DataDescV1&) {
    printf("*** Processing acqiris data object\n");
  }
  void process(const DetInfo&, const Acqiris::ConfigV1&) {
    printf("*** Processing Acqiris config object\n");
  }
  void process(const DetInfo&, const Opal1k::ConfigV1&) {
    printf("*** Processing Opal1000 config object\n");
  }
  void process(const DetInfo&, const Camera::FrameFexConfigV1&) {
    printf("*** Processing frame feature extraction config object\n");
  }
  void process(const DetInfo&, const EvrData::ConfigV1&) {
    printf("*** Processing EVR config object\n");
  }
  void process(const DetInfo&, const Camera::TwoDGaussianV1& o) {
    printf("*** Processing 2DGauss object\n");
  }
  void process(const DetInfo& di, fileHeaderType* FileHdr) {
    printf("*** Processing pnCCD config\n");
    printf("\tpnCCD File Header:\n");
    printf("\tmyLength  %d, fhLength %d, nCCDs %d, width %d, maxHeight %d, version %d\n\t dataSetID %s\n", 
			FileHdr->myLength, FileHdr->fhLength, FileHdr->nCCDs, FileHdr->width, 
			FileHdr->maxHeight, FileHdr->version, FileHdr->dataSetID);
    if (FileHdr->version > 5) {
      printf("\tthe_width %d, the_maxHeight %d\n", FileHdr->the_width, FileHdr->the_maxHeight);
    }
    memcpy((char*)fileHdrBuffer, (char*)FileHdr, 1024);
  }
  void process(const DetInfo& di, frameHeaderType* frh) {
    printf("*** Processing pnCCD frame data\n");
    printf("\tpnCCD Frame Header:\n");
    printf("\tstart %d, info %d, id %d, height %d, tv_sec %d, tv_usec %d, index %d, temp %lf\n", 
			frh->start, frh->info, frh->id, frh->height, 
			frh->tv_sec, frh->tv_usec, frh->index, frh->temp);
    if (fileHdrBuffer->version > 5) {
      printf("\tthe_start %d, the_height %d, external_id %d, aux_value %lf\n", 
			frh->the_start, frh->the_height, frh->external_id, frh->aux_value);
    }
  }
  int process(Xtc* xtc) {
    unsigned i=_depth; while (i--) printf("  ");
    Level::Type level = xtc->src.level();
    printf("%s level contains: %s: ",Level::name(level), TypeId::name(xtc->contains.id()));
    const DetInfo& info = *(DetInfo*)(&xtc->src);
    if (level==Level::Source) {
      printf("%s %d %s %d\n",
             DetInfo::name(info.detector()),info.detId(),
             DetInfo::name(info.device()),info.devId());
    } else {
      ProcInfo& info = *(ProcInfo*)(&xtc->src);
      printf("IpAddress 0x%x ProcessId 0x%x\n",info.ipAddr(),info.processId());
    }
    switch (xtc->contains.id()) {
    case (TypeId::Id_Xtc) : {
      myLevelIter iter(xtc,_depth+1);
      iter.iterate();
      break;
    }
    case (TypeId::Id_Frame) :
      process(info, *(const Camera::FrameV1*)(xtc->payload()));
      break;
    case (TypeId::Id_AcqWaveform) :
      process(info, *(const Acqiris::DataDescV1*)(xtc->payload()));
      break;
    case (TypeId::Id_AcqConfig) :
      {
        unsigned version = xtc->contains.version();
        switch (version) {
        case 1:
          process(info,*(const Acqiris::ConfigV1*)(xtc->payload()));
          break;
        default:
          printf("Unsupported acqiris configuration version %d\n",version);
          break;
        }
      }
      break;
    case (TypeId::Id_TwoDGaussian) :
      process(info, *(const Camera::TwoDGaussianV1*)(xtc->payload()));
      break;
    case (TypeId::Id_Opal1kConfig) :
      process(info, *(const Opal1k::ConfigV1*)(xtc->payload()));
      break;
    case (TypeId::Id_FrameFexConfig) :
      process(info, *(const Camera::FrameFexConfigV1*)(xtc->payload()));
      break;
    case (TypeId::Id_EvrConfig) :
      process(info, *(const EvrData::ConfigV1*)(xtc->payload()));
      break;
    case (TypeId::Id_pnCCDconfig) :
      process(info, (fileHeaderType*) xtc->payload());
      break;
    case (TypeId::Id_pnCCDframe) :
      process(info, (frameHeaderType*) xtc->payload());
      break;
    default :
      break;
    }
    return Continue;
  }
private:
  unsigned _depth;
};

class MyXtcMonitorClient : public XtcMonitorClient {
  public:
    MyXtcMonitorClient() {
    }
    virtual void processDgram(Dgram* dg) {
      printf("%s transition: time 0x%x/0x%x, payloadSize 0x%x\n",TransitionId::name(dg->seq.service()),
	     dg->seq.stamp().fiducials(),dg->seq.stamp().ticks(),dg->xtc.sizeofPayload());
      myLevelIter iter(&(dg->xtc),0);
      iter.iterate();
    };
};

void usage(char* progname) {
  fprintf(stderr,"Usage: %s [-t <partitionTag>] [-h]\n", progname);
}

int main(int argc, char* argv[]) {
  int c=0;
  char partitionTag[128] = "";
  MyXtcMonitorClient myClient;

  while ((c = getopt(argc, argv, "?ht:")) != -1) {
    switch (c) {
    case '?':
    case 'h':
      usage(argv[0]);
      exit(0);
    case 't':
      strcpy(partitionTag, optarg);
      // the run method will only return if it encounters an error
      fprintf(stderr, "myClient returned: %d\n", myClient.run(partitionTag));
      break;
    default:
      usage(argv[0]);
    }
  }
  if (c<1) usage(argv[0]);
  return 1;
}
