/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/memoryallocator.cpp
// Purpose:     wxMemoryAllocator, wxMallocAllocator, wxNewAllocator
// Author:      Aron Helser
// Created      
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MEMORYALLOCATOR_H_
#define _WX_MEMORYALLOCATOR_H_

#include "wx/defs.h"


//-----------------------------------------------------------------------------
// wxMemoryAllocator
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMemoryAllocator
{
public:
    virtual ~wxMemoryAllocator() { }
    
    // may either return NULL or throw std::bad_alloc on error
    virtual void *Alloc(size_t size) = 0;
   
    // Will not throw, may be called with NULL pointer
    virtual void Free(void *ptr) = 0;
};


//-----------------------------------------------------------------------------
// wxMallocAllocator
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMallocAllocator: public wxMemoryAllocator
{
public:
    // Will return NULL on error
    virtual void *Alloc(size_t size) wxOVERRIDE
	{
        return malloc(size);
    }
   
    // Will not throw, may be called with NULL pointer
    virtual void Free(void *ptr) wxOVERRIDE
	{
        free(ptr);
    }

    // singleton accessor:
    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewAllocator
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNewAllocator : public wxMemoryAllocator
{
public:
    virtual void *Alloc(size_t size) wxOVERRIDE
	{
        return new char[size];
    }
    virtual void Free(void *ptr) wxOVERRIDE
	{
        delete [] static_cast<char *>( ptr );
    }
    // singleton accessor:
    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewNothrowAllocator
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNewNothrowAllocator : public wxMemoryAllocator
{
public:
    virtual void *Alloc(size_t size) wxOVERRIDE
	{
        return new (std::nothrow) char[size];
    }
    virtual void Free(void *ptr) wxOVERRIDE
	{
        delete [] static_cast<char *>( ptr );
    }
    // singleton accessor:
    static wxMemoryAllocator *Get();
};


#endif
  // _WX_MEMORYALLOCATOR_H_
