/////////////////////////////////////////////////////////////////////////////
// Name:        wx/memoryallocator.h
// Purpose:     wxMemoryAllocator, wxMallocAllocator, wxNewAllocator
// Author:      Aron Helser
// RCS-ID:      $Id: $
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MEMORYALLOCATOR_H_
#define _WX_MEMORYALLOCATOR_H_

#include "wx/defs.h"


//-----------------------------------------------------------------------------
// wxMemoryAllocator
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMemoryAllocator
{
public:
    virtual ~wxMemoryAllocator() { }
    
    // may either return NULL or throw std::bad_alloc on error
    virtual void *Alloc(size_t size) = 0;
   
    // should not throw, may be called with NULL pointer
    virtual void Free(void *ptr) = 0;
};


//-----------------------------------------------------------------------------
// wxMallocAllocator
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMallocAllocator: public wxMemoryAllocator
{
public:
    // Will return NULL on error
    virtual void *Alloc(size_t size) {
        return malloc(size);
    }
   
    // should not throw, may be called with NULL pointer
    virtual void Free(void *ptr) {
        free(ptr);
    }

    // singleton accessor:
    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewAllocator
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxNewAllocator : public wxMemoryAllocator
{
public:
    // will trigger std::set_new_handler() first, then throw std::bad_alloc on
    // out-of-memory error
    virtual void *Alloc(size_t size) {
        return new char[size];
    }
    virtual void Free(void *ptr) {
        delete [] static_cast<char *>( ptr );
    }
    // singleton accessor:
    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewNothrowAllocator
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxNewNothrowAllocator : public wxMemoryAllocator
{
public:
    // will trigger std::set_new_handler() first, then return NULL
    // out-of-memory error
    virtual void *Alloc(size_t size) {
        return new (std::nothrow) char[size];
    }
    virtual void Free(void *ptr) {
        delete [] static_cast<char *>( ptr );
    }
    // singleton accessor:
    static wxMemoryAllocator *Get();
};


#endif
  // _WX_MEMORYALLOCATOR_H_
