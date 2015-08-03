/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        native_wrapper.mm
// Purpose:     Helper to compile native.cpp as Objective C++ code
// Author:      Vadim Zeitlin
// Created:     2015-08-02
// Copyright:   (c) 2015 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// As explained in widgets.bkl, we need to compile native.cpp using Objective
// C++ compiler, but there is no simple way to do it at the build system level,
// so we just include that file from this file, which has .mm extension
// ensuring that the correct compiler is used.
#include "native.cpp"
