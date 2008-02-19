/////////////////////////////////////////////////////////////////////////////
// Name:        trefcount
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page trefcount_overview Reference counting

 @ref refcount_overview
 @ref refcountequality_overview
 @ref refcountdestruct_overview
 @ref refcountlist_overview
 @ref object_overview


 @section refcount Why you shouldn't care about it

 Many wxWidgets objects use a technique known as @e reference counting, also known
 as @e copy on write (COW).
 This means that when an object is assigned to another, no copying really takes place:
 only the reference count on the shared object data is incremented and both objects
 share the same data (a very fast operation).
 But as soon as one of the two (or more) objects is modified, the data has to be
 copied because the changes to one of the objects shouldn't be seen in the
 others. As data copying only happens when the object is written to, this is
 known as COW.
 What is important to understand is that all this happens absolutely
 transparently to the class users and that whether an object is shared or not
 is not seen from the outside of the class - in any case, the result of any
 operation on it is the same.

 @section refcountequality Object comparison

 The == and != operators of @ref refcountlist_overview
 always do a @c deep comparison.
 This means that the equality operator will return @true if two objects are
 identic and not only if they share the same data.
 Note that wxWidgets follows the @e STL philosophy: when a comparison operator cannot
 be implemented efficiently (like for e.g. wxImage's == operator which would need to
 compare pixel-by-pixel the entire image's data), it's not implemented at all.
 That's why not all reference-counted wxWidgets classes provide comparison operators.
 Also note that if you only need to do a @c shallow comparison between two
 #wxObject-derived classes, you should not use the == and != operators
 but rather the wxObject::IsSameAs function.


 @section refcountdestruct Object destruction

 When a COW object destructor is called, it may not delete the data: if it's shared,
 the destructor will just decrement the shared data's reference count without destroying it.
 Only when the destructor of the last object owning the data is called, the data is really
 destroyed. As for all other COW-things, this happens transparently to the class users so
 that you shouldn't care about it.


 @section refcountlist List of reference-counted wxWidgets classes

 The following classes in wxWidgets have efficient (i.e. fast) assignment operators
 and copy constructors since they are reference-counted:
 #wxAcceleratorTable

 #wxAnimation

 #wxBitmap

 #wxBrush

 #wxCursor

 #wxFont

 #wxIcon

 #wxImage

 #wxMetafile

 #wxPalette

 #wxPen

 #wxRegion

 #wxString

 #wxVariant

 #wxVariantData
 Note that the list above reports the objects which are reference-counted in all ports of
 wxWidgets; some ports may use this tecnique also for other classes.

 @section wxobjectoverview Make your own reference-counted class

 Reference counting can be implemented easily using #wxObject
 and #wxObjectRefData classes. Alternatively, you
 can also use the #wxObjectDataPtrT template.
 First, derive a new class from #wxObjectRefData and
 put there the memory-consuming data.
 Then derive a new class from #wxObject and implement there
 the public interface which will be seen by the user of your class.
 You'll probably want to add a function to your class which does the cast from
 #wxObjectRefData to your class-specific shared data; e.g.:

 @code
 MyClassRefData *GetData() const   { return wx_static_cast(MyClassRefData*, m_refData); }
 @endcode

 in fact, all times you'll need to read the data from your wxObject-derived class,
 you'll need to call such function.
 Very important, all times you need to actually modify the data placed inside your
 wxObject-derived class, you must first call the wxObject::UnShare
 function to be sure that the modifications won't affect other instances which are
 eventually sharing your object's data.

 */


