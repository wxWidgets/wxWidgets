#the calling Makefile MUST set OS and RULE
#RULE can be one of the following:
# bin      for                             one local  binary 
# bin2     for                             two local  binaries
# gbin     for                             one global binary
# gbin2    for                             two global binaries
# lib      for a local         library
# libbin   for a local         library and one local  binary
# libgbin  for a local         library and one global binary
# glib     for a global        library
# glibbin  for a global        library and one local  binary
# glibgbin for a global        library and one global binary
# gslib    for a global shared library

# no need to edit below this line !!!!

RULES_DIR=$(WXBASEDIR)/setup/rules
RULES_GENERIC=$(RULES_DIR)/generic
SETUP_DIR=$(WXBASEDIR)/setup/$(OS)
SHARE_DIR=$(WXBASEDIR)/setup/shared

SRCDIR=$(WXBASEDIR)/src
UTILS=$(WXBASEDIR)/utils
SAMPLES=$(WXBASEDIR)/samples
OTHER=$(WXBASEDIR)/other

# now include the global setting
include $(SETUP_DIR)/maketmpl

# now include the rule needed
include $(RULES_DIR)/$(RULE)

# now include the global objects
include $(RULES_GENERIC)/globals

