# You need to change HDF5ROOT to the path where hdf5 is installed. If it's installed under /usr or /sw nothing should be necessary 
HDF5ROOT=/usr

all: xtcpnCCD2hdf5
CFLAGS=-std=c99 -Wall -W -Iinclude -I/sw/include
CXXFLAGS=-Iinclude -I/sw/include -I${HDF5ROOT}/include
LDFLAGS=-L/sw/lib -L${HDF5ROOT}/lib
LDLIBS= -lhdf5
objs = xtcpnCCD2hdf5.o XtcIterator.o DetInfo.o TypeId.o ProcInfo.o XtcFileIterator.o Src.o TransitionId.o TimeStamp.o Level.o Sequence.o ClockTime.o

# $^ corresponds to the left side of the : (xtcpnCCD2hdf5) and $@ to the right side ($objs)

xtcpnCCD2hdf5: $(objs)
	g++ $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
.PHONY: clean

clean:
	rm -f $(objs) *~ core include/*~ xtcpnCCD2hdf5
