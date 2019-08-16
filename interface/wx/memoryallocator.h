/////////////////////////////////////////////////////////////////////////////
// Name:        memoryallocator.h
// Purpose:     interface of wxMemoryAllocator
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryAllocator

    This is a little helper class that hides the difference between
    C++ operators new/delete and C way of memory management using
    malloc()/free(). It is currently being used in wxImage to do
    memory management.

    @since 3.1.1
*/
class wxMemoryAllocator
{
public:
    /**
        Destructor.
    */
    virtual ~wxMemoryAllocator();
    
    /**
        Default memory allocation function. This function may
        either return NULL or throw std::bad_alloc on error.
    */
    virtual void *Alloc(size_t size) = 0;
   
    /**
        Default memory allocation function. This function should not throw.
        It may be called with NULL pointer
    */
    virtual void Free(void *ptr) = 0;
};

/**
    This class implements memory management in a C-style
    with malloc()/free() pair

    @since 3.1.3
*/
class wxMallocAllocator: public wxMemoryAllocator
{
public:
    virtual void *Alloc(size_t size);
   
    virtual void Free(void *ptr);

    // singleton accessor:
    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewAllocator
//-----------------------------------------------------------------------------

/**
    This class represented memory management using C++-way
    with new/delete pair

    @since 3.1.3
*/
class wxNewAllocator : public wxMemoryAllocator
{
public:

    virtual void *Alloc(size_t size);

    virtual void Free(void *ptr);

    static wxMemoryAllocator *Get();
};

//-----------------------------------------------------------------------------
// wxNewNothrowAllocator
//-----------------------------------------------------------------------------

/**
    This class represented memory management using C++-way
    with new/delete pair, but its Alloc function will never
    throw an exception.

    @since 3.1.3
*/
class wxNewNothrowAllocator : public wxMemoryAllocator
{
public:
    virtual void *Alloc(size_t size);

    virtual void Free(void *ptr);

    static wxMemoryAllocator *Get();
};

