#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#ifdef _POSIX_MESSAGE_PASSING
#include <mqueue.h>
#endif

#include "pdsdata/xtc/DetInfo.hh"
#include "pdsdata/xtc/ProcInfo.hh"
#include "pdsdata/xtc/XtcIterator.hh"
#include "pdsdata/xtc/Dgram.hh"

#define  MAGIC  (0xFACE0FFF)
#define PERMS (S_IRUSR|S_IRUSR|S_IRUSR|S_IROTH|S_IROTH|S_IROTH|S_IRGRP|S_IRGRP|S_IRGRP| \
               S_IWUSR|S_IWUSR|S_IWUSR|S_IWOTH|S_IWOTH|S_IWOTH|S_IWGRP|S_IWGRP|S_IWGRP)
#define OFLAGS (O_CREAT|O_RDWR)

void usage(char* progname) {
  fprintf(stderr,"Usage: %s -f <filename> -n <numberOfBuffers> -s <sizeOfBuffers> [-r <ratePerSec>] [-p <partitionTag>] [-l] [-h]\n", progname);
}

char toMonQname[128] = "/PdsToMonitorMsgQueue_";
char fromMonQname[128] = "/PdsFromMonitorMsgQueue_";
char shmName[128] = "/PdsMonitorSharedMemory_";
bool loop = false;
bool L1AcceptOnly = false;

void sigfunc(int sig_no) {
   printf("Unlinking ... \n");
   if (mq_unlink(toMonQname) == (mqd_t)-1) perror("mq_unlink To Monitor");
   if (mq_unlink(fromMonQname) == (mqd_t)-1) perror("mq_unlink From Monitor");
   shm_unlink(shmName);
   printf("Finished. %d\n", sig_no);
   exit(EXIT_SUCCESS);
}

class Msg {
  public:
    Msg() {}
    Msg(int bufferIndex) {_bufferIndex = bufferIndex;}
    ~Msg() {};
    Msg* bufferIndex(int b) {_bufferIndex=b; return this;}
    int bufferIndex() {return _bufferIndex;}
    void numberOfBuffers(int n) {_numberOfBuffers = n;}
    void sizeOfBuffers(int s) {_sizeOfBuffers = s;}
  private:
    int _bufferIndex;
    int _numberOfBuffers;
    unsigned _sizeOfBuffers;
};

long long int timeDiff(struct timespec* end, struct timespec* start) {
  long long int diff;
  diff =  (end->tv_sec - start->tv_sec) * 1000000000;
  diff += end->tv_nsec;
  diff -= start->tv_nsec;
  return diff;
}

Dgram* next(FILE* _file, unsigned _maxDgramSize, char* _bufferP) {
  Dgram& dg = *(Dgram*)_bufferP;
  unsigned header = sizeof(dg);
  fread(&dg, header, 1, _file);
  if (feof(_file)) {
   if (loop) {
     rewind(_file);
     return next(_file, _maxDgramSize, _bufferP);
   } else {
     return 0;
   }
  }
  unsigned payloadSize = dg.xtc.sizeofPayload();
  if ((payloadSize+header)>_maxDgramSize) {
      printf("Datagram size 0x%x larger than maximum: 0x%x\n",(unsigned)payloadSize+(unsigned)sizeof(dg), _maxDgramSize); 
      return 0;
   }
   fread(_bufferP+header, payloadSize, 1, _file);
   if (feof(_file)) {
     if (loop) {
       rewind(_file);
       return next(_file, _maxDgramSize, _bufferP);
     }
     else {
       return 0;
     }
   }
   return &dg;
}


int main(int argc, char* argv[]) {
  int c;
  char* xtcname=0;
  char partitionTag[80] = "";
  int rate = 1;
  int numberOfBuffers = 0;
  unsigned sizeOfBuffers = 0;
  unsigned sizeOfShm;
  char *bufferP;   //  pointer to the shared memory area being used
  char *myShm; // the pointer to start of shared memory
  FILE* file;
  mqd_t myOutputQueue;
  mqd_t myInputQueue;
  unsigned priority = 0;
  struct timespec start, now, sleepTime;
  unsigned pageSize = (unsigned)sysconf(_SC_PAGESIZE);
  (void) signal(SIGINT, sigfunc);
  Msg myMsg;

  //printf("sizeof timespec %d\n", sizeof(struct timespec));
  //printf("Pagesize=%lx\n", pageSize);

  while ((c = getopt(argc, argv, "hf:r:n:s:p:l")) != -1) {
    switch (c) {
    case 'h':
      usage(argv[0]);
      exit(0);
    case 'f':
      xtcname = optarg;
      break;
    case 'r':
      sscanf(optarg, "%d", &rate);
      break;
    case 'n':
      sscanf(optarg, "%d", &numberOfBuffers);
      myMsg.numberOfBuffers(numberOfBuffers);
      break;
    case 's':
      sizeOfBuffers = (unsigned) strtoul(optarg, NULL, 0);
      myMsg.sizeOfBuffers(sizeOfBuffers);
      break;
    case 'p':
      strcpy(partitionTag, optarg);
      strcat(toMonQname, partitionTag);
      strcat(fromMonQname, partitionTag);
      strcat(shmName, partitionTag);
      break;
    case 'l':
      loop = true;
      printf("Enabling infinite looping\n");
      break;
    default:
      fprintf(stderr, "I don't understand %c!\n", c);
      usage(argv[0]);
      exit(0);
    }
  }
  
  if (!xtcname || !sizeOfBuffers || !numberOfBuffers) {
    usage(argv[0]);
    printf("rate %d, numb %d, size %d, partition %s\n", rate, numberOfBuffers, sizeOfBuffers, partitionTag);
    exit(2);
  }

  sizeOfShm = numberOfBuffers * sizeOfBuffers;
  //pageSize *= 8;
  unsigned remainder;
  if ((remainder = sizeOfShm%pageSize)) {
    printf("sizeOfShm changed from 0x%x", sizeOfShm);
    sizeOfShm += pageSize - remainder;
    printf(" to 0x%x\n", sizeOfShm);
  }

  file = fopen(xtcname,"r");
  if (!file) {
    char s[120];
    sprintf(s, "Unable to open file %s ", xtcname);
    perror(s);
    exit(2);
  }

  long long int period = 1000000000 / rate;
  sleepTime.tv_sec = 0;
  long long int busyTime;

  struct mq_attr mymq_attr;
  mymq_attr.mq_maxmsg = numberOfBuffers+4;
  mymq_attr.mq_msgsize = (long int)sizeof(Msg);
  mymq_attr.mq_flags = 0L;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  if (!shm_unlink(shmName)) perror("shm_unlink found a remnant of previous lives");
  int shm = shm_open(shmName, OFLAGS, PERMS);
  if (shm < 0) perror("shm_open");

  if ((ftruncate(shm, sizeOfShm))<0) perror("ftruncate");

  myShm = (char*)mmap(NULL, sizeOfShm, PROT_READ|PROT_WRITE, MAP_SHARED, shm, 0);
  //myShm = (char*)mmap(NULL, 0x08000, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_LOCKED, shm, 0);
  if (myShm == MAP_FAILED) perror("mmap");
  else printf("Shared memory at %p\n", (void*)myShm);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
  printf("Opening shared memory took %lld nanonseconds.\n", timeDiff(&now, &start));

  if (mq_unlink(toMonQname) != (mqd_t)-1) perror("mq_unlink To Monitor found a remnant of previous lives");
  if (mq_unlink(fromMonQname) != (mqd_t)-1) perror("mq_unlink From Monitor found a remnant of previous lives");
  myOutputQueue = mq_open(toMonQname, O_CREAT|O_RDWR, PERMS, &mymq_attr);
  if (myOutputQueue == (mqd_t)-1) perror("mq_open output");
  myInputQueue = mq_open(fromMonQname, O_CREAT|O_RDWR, PERMS, &mymq_attr);
  if (myInputQueue == (mqd_t)-1) perror("mq_open input");

  // prestuff the input queue which doubles as the free list
  for (int i=0; i<numberOfBuffers; i++) {
    if (mq_send(myInputQueue, (const char *)myMsg.bufferIndex(i), sizeof(Msg), 0)) perror("mq_send inQueueStuffing");
  }

  Dgram* dg;
  do {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    if (mq_receive(myInputQueue, (char*)&myMsg, sizeof(myMsg), &priority) < 0) perror("mq_receive buffer");
    else {
      bufferP = myShm + (sizeOfBuffers * myMsg.bufferIndex());
      if ((dg = next(file, sizeOfBuffers, bufferP))) {
	if ((dg->seq.service()==TransitionId::L1Accept) && loop) L1AcceptOnly = true;
	else if (L1AcceptOnly) {
	  while (dg->seq.service()!=TransitionId::L1Accept) {
	    dg = next(file, sizeOfBuffers, bufferP);
	  }
	}
	if (mq_send(myOutputQueue, (const char *)&myMsg, sizeof(myMsg), 0)) perror("mq_send buffer");
	printf("%s transition: time 0x%x/0x%x, payloadSize 0x%x, using buffer %d, spareTime %lld\n",TransitionId::name(dg->seq.service()),
		 dg->seq.stamp().fiducials(),dg->seq.stamp().ticks(),dg->xtc.sizeofPayload(), myMsg.bufferIndex(), period - busyTime);
      }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    busyTime = timeDiff(&now, &start);
    if (period > busyTime) {
      sleepTime.tv_nsec = period - busyTime;
      if (nanosleep(&sleepTime, &now)<0) perror("nanosleep");
    }
  } while (dg);

  fclose(file);
  sigfunc(0);
  return 0;
}
