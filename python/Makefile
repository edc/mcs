PACKAGEFOLDER = mcs
SRCDIR = src
MODULENAME = Cmcs
CSOURCEDIR = ..
CSOURCES = $(CSOURCEDIR)/bipartite.h $(CSOURCEDIR)/bipartite.c $(CSOURCEDIR)/list.h $(CSOURCEDIR)/list.c $(CSOURCEDIR)/utils.c $(CSOURCEDIR)/vmax.h $(CSOURCEDIR)/vmaxs.c $(CSOURCEDIR)/timed.c

all : $(SRCDIR)/py_wrap.c csource

swig : $(SRCDIR)/py_wrap.c

$(SRCDIR)/py_wrap.c : ../swig.i
	swig -module $(MODULENAME) -o $@ -python $^
	mv $(SRCDIR)/$(MODULENAME).py $(PACKAGEFOLDER)/

csource : $(CSOURCES)
	cp $(CSOURCES) $(SRCDIR)

dist : all
	rm -rf dist
	mkdir dist
	cp -r $(PACKAGEFOLDER) setup.py $(SRCDIR) README dist
	tar -czf py-$(PACKAGEFOLDER).tar.gz dist
	rm -rf dist

clean :
	rm -f $(SRCDIR)/py_wrap.c
	rm -f $(PACKAGEFOLDER)/$(MODULENAME).py $(PACKAGEFOLDER)/*.pyc
	rm -rf dist
	rm -rf build
	(cd $(SRCDIR);\
	for i in $(CSOURCES); do \
	rm -f `basename $$i`;\
	done)
