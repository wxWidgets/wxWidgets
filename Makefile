# Top-level Makefile for wxGTK by Wolfram Gloger
# based on the version for wx-Xt by Martin Sperl

SHELL=/bin/sh

DIRS=src

#if DIRS are defind make only executes in these diretories
all::
	@if test "x$(DIRS)" = x; then \
	  for i in src samples utils user; do \
	    echo "entering directory $$i building $@"; \
	    (cd $$i; ${MAKE} -k $@); \
	  done; \
	else \
	  for i in $(DIRS) xxx; do \
	    if test "$$i" != xxx; then \
	      echo "entering directory $$i building $@"; \
	      (cd $$i; ${MAKE} -k $@); \
	    fi; \
	  done; \
	fi	    

# what to do if a target is not understood:
# pass it on to all the children...

.DEFAULT::
	@if test "x$(DIRS)" = x; then \
	  for i in src samples utils user; do \
	    echo "entering directory $$i building $@"; \
	    (cd $$i; ${MAKE} -k $@); \
	  done; \
	else \
	  for i in $(DIRS) xxx; do \
	    if test "$$i" != xxx; then \
	      echo "entering directory $$i building $@"; \
	      (cd $$i; ${MAKE} -k $@); \
	    fi; \
	  done; \
	fi

src::
	@echo "entering directory src building all"
	@cd src; ${MAKE} all

samples::
	@echo "entering directory samples building all"
	@cd samples; ${MAKE} all

utils::
	@echo "entering directory utils building all"
	@cd utils; ${MAKE} all

user::
	@echo "entering directory user building all"
	@cd user; ${MAKE} all

install::
	@echo "entering directory src for installing"
	@cd src; ${MAKE} install
#	@echo "entering directory utils for installing"
#	@cd utils; ${MAKE} install
	
# the following ones recreate all Makefiles.

makefiles:: recreate
Makefiles:: recreate
recreate::
	@setup/general/createall

# the following ones define what needs to be done to distribute the 
# library and its components

distribute:: distrib
distrib:: distrib_base distrib_user join_utils join_samples

distrib_samples::
	@echo "entering directory samples creating distribution files"
	@(cd samples; ${MAKE} -k distrib)

distrib_user::
	@echo "entering directory user creating distribution files"
	@(cd user; ${MAKE} -k distrib)

distrib_utils:: 
	@echo "entering directory utils creating distribution files"
	@(cd utils; ${MAKE} -k distrib)

join_utils:: distrib_utils
	@$(MAKE) join \
	    BASEDIR=utils \
	    FILES=`echo distrib/utils/*.tgz `

join_samples:: distrib_samples
	@$(MAKE) join \
	    BASEDIR=samples \
	    FILES=`echo distrib/samples/*.tgz `

join_user:: distrib_user
	@$(MAKE) join \
	    BASEDIR=user \
	    FILES=`echo distrib/user/*.tgz `

join::
	@# needed are BASEDIR and FILES
	@if test "x$$BASEDIR" = x; then\
	  echo "BASEDIR not specified.";\
	  exit -1;\
	fi
	@if test "x$$FILES" != x ; then \
	  echo "putting all seperate distribution files:";\
	  echo "$$FILES";\
	  echo "into distrib/$(BASEDIR).tgz";\
	  src/gtk/setup/general/jointar $(BASEDIR) $$FILES distrib/$(BASEDIR).tgz; \
	else \
	  echo "Nothing to join - deleting..."; \
	  echo "This may be the case, if you have not specified FILES."\
	  rm -f distrib/$(BASEDIR).tgz; \
	fi

distrib_base:: 
	@if test ! -d distrib ; then mkdir distrib; fi;
	@if test ! -f system.list ; then \
	  echo "dummy" > system.list;\
	fi
	@(curr=`pwd`; direc=`basename $$curr`;\
	 (cd ..; \
	  echo creating distrib/$$direc.tar from the current directory;\
	  tar -cf /tmp/$$direc.tar \
	    $$direc/COPYING\
	    $$direc/INSTALL\
	    $$direc/Makefile\
	    $$direc/template.mak\
	    $$direc/configure\
	    $$direc/configure.in\
	    $$direc/config.guess\
	    $$direc/config.sub\
	    $$direc/install-sh\
	    $$direc/user/Makefile \
	    $$direc/utils/Makefile \
	    $$direc/samples/Makefile \
	    ;\
	  sed "s|^\(.*\)$$|/\1/|g" $$direc/system.list \
	    | uniq > /tmp/$$direc.list; \
	  echo "/RCS/" >> /tmp/$$direc.list; \
	  for each in misc docs wx src setup; do \
	    tar -uf /tmp/$$direc.tar \
	      `\
	       find $$direc/$$each \( -type f -o -type l \) -print \
	       | fgrep -vf /tmp/$$direc.list \
	       | grep -v "[~#]$$" \
	      ` ;\
	  done; \
	  echo compressing $$direc.tar to $$direc.tgz;\
	  gzip -9 -c /tmp/$$direc.tar > $$direc/distrib/$$direc.tgz;\
	  rm /tmp/$$direc.tar /tmp/$$direc.list;\
	 )\
	)

# the following ones are only needed if configure.in has changed
# and needs to be updated...

config:: configure

configure::
	@autoconf
	@cat configure \
	  | sed "s/config.cache/\$$OSTYPE.config.cache/g" \
	  | sed "s/config.status/\$$OSTYPE.config.status/g" \
	  | sed "s/\*\*--/  --/g" \
	  > configure1
	@chmod a+x configure1
	@mv configure1 configure


