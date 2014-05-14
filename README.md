# Installation and configuration into source tree

## Get Slurm sources

## Get plugin sources

checkout repository into `src/plugins/job_submit`:

    cd src/plugins/job_submit ; git clone ...

## Update Slurm Source tree

Add new plugin directory name to the list of `SUBDIRS` in `src/plugins/job_submit/Makefile.am`

Add full path to plugin makefiles to `AC_CONFIG_FILES` in `configure.ac`

## Reconfigure and build

    autoreconf
    ./configure --host=x86_64-linux-gnu --build=x86_64-linux-gnu --prefix=/usr --mandir=/share/man --infodir=/share/info --sysconfdir=/etc/slurm-llnl --with-munge --localstatedir=/var/run/slurm-llnl --with-blcr --libexecdir=/usr/share --enable-pam --without-rpath --enable-multiple-slurmd

Make as per usual.  Plugins must be manually installed.


