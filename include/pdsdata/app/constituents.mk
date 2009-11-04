tgtnames = xtcreader xtcmonserver xtcmonclient xtcmonclientexample

ifneq ($(findstring x86_64-linux,$(tgt_arch)),)
syslibdir := /usr/lib64
else
syslibdir := /usr/lib
endif

tgtsrcs_xtcreader := xtcreader.cc
tgtlibs_xtcreader := pdsdata/xtcdata pdsdata/acqdata
tgtslib_xtcreader := $(syslibdir)/rt

tgtsrcs_xtcmonserver := xtcmonserver.cc
tgtlibs_xtcmonserver := pdsdata/xtcdata pdsdata/acqdata
tgtslib_xtcmonserver := $(syslibdir)/usr/lib/rt

tgtsrcs_xtcmonclient := xtcmonclient.cc XtcMonitorClient.cc
tgtlibs_xtcmonclient := pdsdata/xtcdata pdsdata/acqdata
tgtslib_xtcmonclient := $(syslibdir)/usr/lib/rt

tgtsrcs_xtcmonclientexample := xtcMonClientExample.cc XtcMonitorClient.cc
tgtlibs_xtcmonclientexample := pdsdata/xtcdata pdsdata/acqdata
tgtslib_xtcmonclientexample := $(syslibdir)/usr/lib/rt

tgtsrcs_agatfile := agatfile.cc
tgtlibs_agatfile := pdsdata/xtcdata pdsdata/acqdata
tgtslib_agatfile := $(syslibdir)/rt
