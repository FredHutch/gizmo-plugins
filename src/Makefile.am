# Makefile for job_submit/gizmo_defacct plugin

AUTOMAKE_OPTIONS = foreign

PLUGIN_FLAGS = -module -avoid-version --export-dynamic

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/common

pkglib_LTLIBRARIES = job_submit_gizmo_defacct.la

# Job submit partition plugin.
job_submit_gizmo_defacct_la_SOURCES = job_submit_gizmo_defacct.c
job_submit_gizmo_defacct_la_LDFLAGS = $(SO_LDFLAGS) $(PLUGIN_FLAGS)
