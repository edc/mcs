PACKAGEFOLDER = mcs
SRCDIR = $(PACKAGEFOLDER)/src
MODULENAME = mcs
CSOURCEDIR = ..
CSOURCES = $(CSOURCEDIR)/bipartite.h $(CSOURCEDIR)/bipartite.c $(CSOURCEDIR)/list.h $(CSOURCEDIR)/list.c $(CSOURCEDIR)/utils.c $(CSOURCEDIR)/vmax.h $(CSOURCEDIR)/vmaxs.c  $(CSOURCEDIR)/timed.c

all : $(SRCDIR)/r_wrap.c csource

swig : $(SRCDIR)/r_wrap.c

$(SRCDIR)/r_wrap.c : ../swig.i
	swig -module $(MODULENAME) -o $@ -r $^
	mv $(SRCDIR)/$(MODULENAME).R $(PACKAGEFOLDER)/R/$(MODULENAME)-core.R

csource : $(CSOURCES)
	cp $(CSOURCES) $(SRCDIR)

clean :
	rm -f $(SRCDIR)/r_wrap.c
	rm -f $(PACKAGEFOLDER)/R/$(MODULENAME)-core.R 
	(cd $(SRCDIR);\
	rm -f *.o *.so; \
	for i in $(CSOURCES); do \
	rm -f `basename $$i`;\
	done)
