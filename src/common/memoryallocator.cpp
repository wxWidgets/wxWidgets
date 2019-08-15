/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/memoryallocator.cpp
// Purpose:     wxMemoryAllocator, wxMallocAllocator, wxNewAllocator
// Author:      Aron Helser
// RCS-ID:      $Id: $
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/memoryallocator.h"

//-----------------------------------------------------------------------------
// wxMallocAllocator
//-----------------------------------------------------------------------------

// singleton accessor:
wxMemoryAllocator *wxMallocAllocator::Get()
{
   // This must be a local static inside this method, otherwise we can't be sure
   // it is initialized before other non-local static objects that use it.
   static wxMallocAllocator s_alloc;
   return &s_alloc;
}

//-----------------------------------------------------------------------------
// wxNewAllocator
//-----------------------------------------------------------------------------

// singleton accessor:
wxMemoryAllocator *wxNewAllocator::Get()
{
   // This must be a local static inside this method, otherwise we can't be sure
   // it is initialized before other non-local static objects that use it.
   static wxNewAllocator s_alloc;
   return &s_alloc;
}

//-----------------------------------------------------------------------------
// wxNewNothrowAllocator
//-----------------------------------------------------------------------------

// singleton accessor:
wxMemoryAllocator *wxNewNothrowAllocator::Get()
{
   // This must be a local static inside this method, otherwise we can't be sure
   // it is initialized before other non-local static objects that use it.
   static wxNewNothrowAllocator s_alloc;
   return &s_alloc;
}
