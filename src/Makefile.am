# Makefile for job_submit/default_account plugin

AUTOMAKE_OPTIONS = foreign

PLUGIN_FLAGS = -module -avoid-version --export-dynamic

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/common

pkglib_LTLIBRARIES = job_submit_default_account.la

# Job submit partition plugin.
job_submit_default_account_la_SOURCES = job_submit_default_account.c
job_submit_default_account_la_LDFLAGS = $(SO_LDFLAGS) $(PLUGIN_FLAGS)
