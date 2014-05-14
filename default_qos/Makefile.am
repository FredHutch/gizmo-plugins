# Makefile for job_submit_default_qos_so

AUTOMAKE_OPTIONS = foreign

PLUGIN_FLAGS = -module -avoid-version --export-dynamic

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/common


pkglib_LTLIBRARIES = job_submit_default_qos.la

# Job submit partition plugin.
job_submit_default_qos_la_LIBADD = 	$(top_builddir)/src/db_api/libslurmdb.o $(DL_LIBS)
job_submit_default_qos_la_SOURCES = job_submit_default_qos.c
job_submit_default_qos_la_LDFLAGS = $(SO_LDFLAGS) $(PLUGIN_FLAGS)
