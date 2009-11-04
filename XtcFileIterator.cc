
#include "pdsdata/xtc/XtcFileIterator.hh"
#include <new>
#include <stdio.h>
#include <stdlib.h>

using namespace Pds;

XtcFileIterator::XtcFileIterator(FILE* file, size_t maxDgramSize) :
  _file(file),_maxDgramSize(maxDgramSize),_buf(new char[maxDgramSize]) {}

XtcFileIterator::~XtcFileIterator() {
  delete[] _buf;
}

Dgram* XtcFileIterator::next() {
  Dgram& dg = *(Dgram*)_buf;
  fread(&dg, sizeof(dg), 1, _file);
  if (feof(_file)) return 0;
  size_t payloadSize = dg.xtc.sizeofPayload();
  if ((payloadSize+sizeof(dg))>_maxDgramSize) {
    printf("Datagram size %zu larger than maximum: %zu\n", payloadSize+sizeof(dg), _maxDgramSize);
    return 0;
  }
  fread(dg.xtc.payload(), payloadSize, 1, _file);
  return (feof(_file)) ? 0 : &dg;
}
