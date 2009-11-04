# You need to change HDF5ROOT to the path where hdf5 is installed.
# If it's installed under /usr or /sw nothing should be necessary
HDF5ROOT=/usr

# The following is necessary with HDF5 1.8.x, and should be harmless otherwise
HDF5CFLAGS=-D H5Gcreate_vers=1 -D H5Dcreate_vers=1

# Add any extra include directories here
INCLUDES=-I${HDF5ROOT}/include -Iinclude -I/sw/include

# Add any extra linker stuff here
LDFLAGS=-L${HDF5ROOT}/lib -L/sw/lib -lhdf5

# Any other compiler flags go here
CXXFLAGS=-W -Wall $(INCLUDES) $(HDF5CFLAGS)

OBJS = xtcpnCCD2hdf5.o XtcIterator.o DetInfo.o TypeId.o ProcInfo.o \
       XtcFileIterator.o Src.o TransitionId.o TimeStamp.o Level.o \
       Sequence.o ClockTime.o

all: xtcpnCCD2hdf5

xtcpnCCD2hdf5: $(OBJS)
	g++ $^ $(LDFLAGS) -o $@

$(OBJS): %.o : %.cc
	g++ -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJS) *~ core include/*~ xtcpnCCD2hdf5

.PHONY: clean