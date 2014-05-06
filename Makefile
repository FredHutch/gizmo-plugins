PACKAGE    ?= slurm-fhcrc-plugins

LIBNAME    ?= lib$(shell uname -m | grep -q x86_64 && echo 64)
LIBNAME    ?= $(shell dpkg-architecture -qDEB_HOST_MULTIARCH)
LIBNAME    ?= lib
LIBDIR     ?= /usr/$(LIBNAME)
BINDIR     ?= /usr/bin
SBINDIR    ?= /sbin
LIBEXECDIR ?= /usr/libexec

export LIBNAME LIBDIR BINDIR SBINDIR LIBEXECDIR PACKAGE

CFLAGS   = -Wall -ggdb

PLUGINS = 

LIBRARIES = 

SUBDIRS = ./src/defacct

all: 
	make -C ./slurm-spank-x11

.SUFFIXES: .c .o .so

.c.o: 
	$(CC) $(CFLAGS) -o $@ -fPIC -c $<
.o.so: 
	$(CC) -shared -o $*.so $< $(LIBS)

subdirs: 
	@for d in $(SUBDIRS); do make -C $$d; done

clean: subdirs-clean
	rm -f *.so *.o lib/*.o

install:
	@mkdir -p --mode=0755 $(DESTDIR)$(LIBDIR)/slurm
	install --mode=0755 ./slurm-spank-plugins/tmpdir.so $(DESTDIR)$(LIBDIR)/slurm;
	make -C ./slurm-spank-plugins/use-env DESTDIR=$(DESTDIR) install; 
	make -C ./slurm-spank-x11 DESTDIR=$(DESTDIR) install; 

subdirs-clean:
	@for d in $(SUBDIRS); do make -C $$d clean; done

orig:
	cd .. && tar czf slurm-fhcrc-plugins_0.23.orig.tar.gz slurm-fhcrc-plugins/Makefile slurm-fhcrc-plugins/README.md slurm-fhcrc-plugins/slurm-spank-*
