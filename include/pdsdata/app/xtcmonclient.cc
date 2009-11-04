#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "XtcMonitorClient.hh"

void usage(char* progname) {
  fprintf(stderr,"Usage: %s [-p <partitionTag>] [-h]\n", progname);
}


int main(int argc, char* argv[]) {
  int c;
  char partitionTag[128] = "";
  Pds::XtcMonitorClient myClient;

  while ((c = getopt(argc, argv, "?hp:")) != -1) {
    switch (c) {
    case '?':
    case 'h':
      usage(argv[0]);
      exit(0);
    case 'p':
      strcpy(partitionTag, optarg);
      fprintf(stderr, "myClient returned: %d\n", myClient.run(partitionTag));
      break;
    default:
      usage(argv[0]);
    }
  }
  return 1;
}
