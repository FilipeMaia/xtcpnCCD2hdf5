/*
  Copyright (c) 2009 Filipe Maia (filipe _at_ xray.bmc.uu.se)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <hdf5.h>

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


class myLevelIter : public XtcIterator {
public:
  enum {Stop, Continue};
  fileHeaderType currentHeader;
  myLevelIter(Xtc* xtc, unsigned depth) : XtcIterator(xtc), _depth(depth) {}

  int write_hdf5(int width, int height, short * data, char * filename){
    hsize_t dims[2] = {width,height};
    hid_t file_id = H5Fcreate(filename,  H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t dataspace_id = H5Screate_simple( 2, dims, NULL);
    H5Gcreate(file_id,"data",0);
    hid_t dataset_id = H5Dcreate(file_id, "/data/data", H5T_NATIVE_SHORT,
			       dataspace_id, H5P_DEFAULT);
    if(H5Dwrite(dataset_id,H5T_NATIVE_SHORT , H5S_ALL, H5S_ALL,
		H5P_DEFAULT, data)< 0){
      return -1;
    }
    H5Dclose(dataset_id);
  }
  int write_frame(const frameHeaderType & pnCCDFrame,short * data){
    char filename[1024];
    sprintf(filename,"pnCCD-%d-%d.h5",pnCCDFrame.index,pnCCDFrame.id);
    int width = currentHeader.the_width;
    int height = pnCCDFrame.the_height;
    hsize_t dims[2] = {width,height};
    hid_t file_id = H5Fcreate(filename,  H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t dataspace_id = H5Screate_simple( 2, dims, NULL);
    H5Gcreate(file_id,"data",0);
    hid_t dataset_id = H5Dcreate(file_id, "/data/data", H5T_NATIVE_SHORT,
			       dataspace_id, H5P_DEFAULT);
    if(H5Dwrite(dataset_id,H5T_NATIVE_SHORT , H5S_ALL, H5S_ALL,
		H5P_DEFAULT, data)< 0){
      return -1;
    }
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);

    H5Gcreate(file_id,"pnCCD",0);
    H5Gcreate(file_id,"pnCCD/frame",0);
    H5Gcreate(file_id,"pnCCD/header",0);
    dims[0] = 1;
    dataspace_id = H5Screate_simple( 1, dims, NULL );
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/info", H5T_NATIVE_UCHAR,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UCHAR , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.info );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/id", H5T_NATIVE_UCHAR,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UCHAR , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.id );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/tv_sec", H5T_NATIVE_UINT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UINT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.tv_sec );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/tv_usec", H5T_NATIVE_UINT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UINT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.tv_usec );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/index", H5T_NATIVE_UINT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UINT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.index );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/the_start", H5T_NATIVE_USHORT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_USHORT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.the_start );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/the_height", H5T_NATIVE_USHORT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_USHORT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.the_height );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/external_id", H5T_NATIVE_UINT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UINT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.external_id );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/frame/aux_value", H5T_NATIVE_DOUBLE,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_DOUBLE , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&pnCCDFrame.aux_value );
    H5Dclose(dataset_id);

    dataset_id = H5Dcreate(file_id, "/pnCCD/header/nCCDs", H5T_NATIVE_USHORT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_USHORT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&currentHeader.nCCDs );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/header/the_width", H5T_NATIVE_USHORT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_USHORT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&currentHeader.the_width );
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/header/the_maxHeight", H5T_NATIVE_USHORT,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_USHORT , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&currentHeader.the_maxHeight);
    H5Dclose(dataset_id);
    dataset_id = H5Dcreate(file_id, "/pnCCD/header/version", H5T_NATIVE_UCHAR,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,H5T_NATIVE_UCHAR , H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,&currentHeader.version);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);

    dataspace_id = H5Screate(H5S_SCALAR);
    hid_t stype = H5Tcopy(H5T_C_S1);
    H5Tset_size(stype,80);
    dataset_id = H5Dcreate(file_id, "/pnCCD/header/dataSetID", stype,
			       dataspace_id, H5P_DEFAULT);
    H5Dwrite(dataset_id,stype, H5S_ALL, H5S_ALL,
	     H5P_DEFAULT,currentHeader.dataSetID);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
  }
  void process(const DetInfo&, const frameHeaderType & pnCCDFrame) {
    printf("*** Processing pnCCDframe object\n");
    printf("CCD ID = %d\n",pnCCDFrame.id);
    printf("Index = %d\n",pnCCDFrame.index);
    printf("Info = %d\n",pnCCDFrame.info);
    printf("lines in frame = %d\n",pnCCDFrame.the_height);
    printf("starting line in frame = %d\n",pnCCDFrame.the_start);
    printf("External ID = %d\n",pnCCDFrame.external_id);
    printf("Aux value = %f\n",pnCCDFrame.aux_value);
    short * data = (short *)((&pnCCDFrame)+1);
    char buffer[1024];
    sprintf(buffer,"pnCCD-%d.h5",pnCCDFrame.index);
    //    write_short_image(width,height,data,buffer);
    write_frame(pnCCDFrame,data);
    //    write_hdf5(width,height,data,buffer);
  }

  void process(const DetInfo&, const fileHeaderType & pnCCDHeader) {
    printf("*** Processing pnCCDconfig object\n");
    printf("dataset ID = %s\n",pnCCDHeader.dataSetID);
    printf("width = %d\n",pnCCDHeader.the_width);
    printf("max height = %d\n",pnCCDHeader.the_maxHeight);
    currentHeader = pnCCDHeader;
  }
  int process(Xtc* xtc) {
    unsigned i=_depth; while (i--) printf("  ");
    Level::Type level = xtc->src.level();
    printf("%s level contains: %s: ",Level::name(level), TypeId::name(xtc->contains.id()));
    const DetInfo& info = *(DetInfo*)(&xtc->src);
    if (level==Level::Source) {
      printf("%s,%d  %s,%d\n",
             DetInfo::name(info.detector()),info.detId(),
             DetInfo::name(info.device()),info.devId());
    } else {
      ProcInfo& info = *(ProcInfo*)(&xtc->src);
      printf("IpAddress 0x%x ProcessId 0x%x\n",info.ipAddr(),info.processId());
    }
    printf("---Contains ID = %d---\n",xtc->contains.id());
    switch (xtc->contains.id()) {
    case (TypeId::Id_Xtc) : {
      myLevelIter iter(xtc,_depth+1);
      iter.iterate();
      break;
    }
    case (TypeId::Id_pnCCDframe) :
      process(info, *(const frameHeaderType*)(xtc->payload()));
      break;
    case (TypeId::Id_pnCCDconfig) :
      process(info, *(const fileHeaderType*)(xtc->payload()));
      break;
    default :
      break;
    }
    return Continue;
  }
private:
  unsigned _depth;
};

void usage(char* progname) {
  fprintf(stderr,"Usage: %s -f <filename> [-h]\n", progname);
}

int main(int argc, char* argv[]) {
  int c;
  char* xtcname=0;
  int parseErr = 0;

  while ((c = getopt(argc, argv, "hf:")) != -1) {
    switch (c) {
    case 'h':
      usage(argv[0]);
      exit(0);
    case 'f':
      xtcname = optarg;
      break;
    default:
      parseErr++;
    }
  }
  
  if (!xtcname) {
    usage(argv[0]);
    exit(2);
  }

  FILE* file = fopen(xtcname,"r");
  if (!file) {
    perror("Unable to open file %s\n");
    exit(2);
  }

  XtcFileIterator iter(file,0x900000);
  Dgram* dg;
  while ((dg = iter.next())) {
    printf("%s transition: time 0x%x/0x%x, payloadSize 0x%x\n",TransitionId::name(dg->seq.service()),
           dg->seq.stamp().fiducials(),dg->seq.stamp().ticks(),dg->xtc.sizeofPayload());
    myLevelIter iter(&(dg->xtc),0);
    iter.iterate();
  }

  fclose(file);
  return 0;
}
