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
    memory management. Right now it has 3 direct descendants - 1
    for C-way, 1 for C++-way and 1 that will not throw on allocation.

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
    void *Alloc(size_t size) = 0;
   
    /**
        Default memory allocation function. This function should not throw.
        It may be called with NULL pointer
    */
    void Free(void *ptr) = 0;
};

/**
    This class represented memory management using C-way
    with malloc()/free() pair

    @since 3.1.3
*/
class wxMallocAllocator: public wxMemoryAllocator
{
public:
    void *Alloc(size_t size);
   
    void Free(void *ptr);

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

    void *Alloc(size_t size);

    void Free(void *ptr);

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

