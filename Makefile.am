## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = po src setup

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS



# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

EXTRA_DIST = config.rpath
