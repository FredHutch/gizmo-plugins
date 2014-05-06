PACKAGE    ?= slurm-gizmo-plugins

LIBNAME    ?= lib$(shell uname -m | grep -q x86_64 && echo 64)
LIBDIR     ?= /usr/$(LIBNAME)
BINDIR     ?= /usr/bin
SBINDIR    ?= /sbin
LIBEXECDIR ?= /usr/libexec

SLURM_SOURCE ?= /home/mrg/build/slurm-llnl/2.6.2/slurm-2.6.2

export LIBNAME LIBDIR BINDIR SBINDIR LIBEXECDIR PACKAGE

CFLAGS   = -Wall -ggdb -I.. -I$(SLURM_SOURCE)

PLUGINS = \
	job_submit_gizmo_defacct.so

LIBRARIES = 

SUBDIRS = 

all: $(PLUGINS) $(LIBRARIES) subdirs

.SUFFIXES: .c .o .so

.c.o: 
	$(CC) $(CFLAGS) -o $@ -fPIC -c $<
.o.so: 
	$(CC) -shared -o $*.so $< $(LIBS)

job_submit_gizmo_defacct:
	$(CC) -shared -o $*.so $< -ldl 
