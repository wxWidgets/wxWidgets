##############################################################################
# Name:        src/version.mak
# Purpose:     file defining wxWindows version used by all other makefiles
# Author:      Vadim Zeitlin
# Modified by:
# Created:     25.02.03
# RCS-ID:      $Id$
# Copyright:   (c) 2003 Vadim Zeitlin
# Licence:     wxWindows license
##############################################################################

wxMAJOR_VERSION=2
wxMINOR_VERSION=4
wxRELEASE_NUMBER=2

# release number if used in the DLL file names only for the unstable branch as
# for the stable branches the micro releases are supposed to be backwards
# compatible and so should have the same name or otherwise it would be
# impossible to use them without recompiling the applications (which is the
# whole goal of keeping them backwards compatible in the first place)
#
# As 2.4 is a stable branch, wxRELEASE_NUMBER_IFUNSTABLE should be empty
# so we end up with DLLs with only "24" in the name.  Unstable branches
# should set it to $wxRELEASE_NUMBER
wxRELEASE_NUMBER_IFUNSTABLE=
