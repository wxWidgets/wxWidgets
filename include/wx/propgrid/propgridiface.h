/////////////////////////////////////////////////////////////////////////////
// Name:        wx/propgeid/propgridiface.h
// Purpose:     wxPropertyGridInterface class
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-24
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_PROPGRID_PROPGRIDIFACE_H__
#define __WX_PROPGRID_PROPGRIDIFACE_H__

#if wxUSE_PROPGRID

#include "wx/propgrid/property.h"
#include "wx/propgrid/propgridpagestate.h"

// -----------------------------------------------------------------------

#ifndef SWIG

/** @section wxPGPropArgCls

    Most property grid functions have this type as their argument, as it can
    convey a property by either a pointer or name.
*/
class WXDLLIMPEXP_PROPGRID wxPGPropArgCls
{
public:
    wxPGPropArgCls() { }
    wxPGPropArgCls( const wxPGProperty* property )
    {
        m_ptr.property = (wxPGProperty*) property;
        m_flags = IsProperty;
    }
    wxPGPropArgCls( const wxString& str )
    {
        m_ptr.stringName = &str;
        m_flags = IsWxString;
    }
    wxPGPropArgCls( const wxPGPropArgCls& id )
    {
        m_ptr = id.m_ptr;
        m_flags = id.m_flags;
    }
    // This is only needed for wxPython bindings
    wxPGPropArgCls( wxString* str, bool WXUNUSED(deallocPtr) )
    {
        m_ptr.stringName = str;
        m_flags = IsWxString | OwnsWxString;
    }
    ~wxPGPropArgCls()
    {
        if ( m_flags & OwnsWxString )
            delete m_ptr.stringName;
    }
    wxPGProperty* GetPtr() const
    {
        wxCHECK( m_flags == IsProperty, NULL );
        return m_ptr.property;
    }
    wxPGPropArgCls( const char* str )
    {
        m_ptr.charName = str;
        m_flags = IsCharPtr;
    }
#if wxUSE_WCHAR_T
    wxPGPropArgCls( const wchar_t* str )
    {
        m_ptr.wcharName = str;
        m_flags = IsWCharPtr;
    }
#endif
    /** This constructor is required for NULL. */
    wxPGPropArgCls( int )
    {
        m_ptr.property = (wxPGProperty*) NULL;
        m_flags = IsProperty;
    }
    wxPGProperty* GetPtr( wxPropertyGridInterface* iface ) const;
    wxPGProperty* GetPtr( const wxPropertyGridInterface* iface ) const
    {
        return GetPtr((wxPropertyGridInterface*)iface);
    }
    wxPGProperty* GetPtr0() const { return m_ptr.property; }
    bool HasName() const { return (m_flags != IsProperty); }
    const wxString& GetName() const { return *m_ptr.stringName; }
private:

    enum
    {
        IsProperty      = 0x00,
        IsWxString      = 0x01,
        IsCharPtr       = 0x02,
        IsWCharPtr      = 0x04,
        OwnsWxString    = 0x10,
    };

    union
    {
        wxPGProperty* property;
        const char* charName;
#if wxUSE_WCHAR_T
        const wchar_t* wcharName;
#endif
        const wxString* stringName;
    } m_ptr;
    unsigned char m_flags;
};

#endif

typedef const wxPGPropArgCls& wxPGPropArg;

// -----------------------------------------------------------------------

WXDLLIMPEXP_PROPGRID
void wxPGTypeOperationFailed( const wxPGProperty* p,
                              const wxString& typestr,
                              const wxString& op );
WXDLLIMPEXP_PROPGRID
void wxPGGetFailed( const wxPGProperty* p, const wxString& typestr );

// -----------------------------------------------------------------------

// Helper macro that does necessary preparations when calling
// some wxPGProperty's member function.
#define wxPG_PROP_ARG_CALL_PROLOG_0(PROPERTY) \
    PROPERTY *p = (PROPERTY*)id.GetPtr(this); \
    if ( !p ) return;

#define wxPG_PROP_ARG_CALL_PROLOG_RETVAL_0(PROPERTY, RETVAL) \
    PROPERTY *p = (PROPERTY*)id.GetPtr(this); \
    if ( !p ) return RETVAL;

#define wxPG_PROP_ARG_CALL_PROLOG() \
    wxPG_PROP_ARG_CALL_PROLOG_0(wxPGProperty)

#define wxPG_PROP_ARG_CALL_PROLOG_RETVAL(RVAL) \
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL_0(wxPGProperty, RVAL)

#define wxPG_PROP_ID_CONST_CALL_PROLOG() \
    wxPG_PROP_ARG_CALL_PROLOG_0(const wxPGProperty)

#define wxPG_PROP_ID_CONST_CALL_PROLOG_RETVAL(RVAL) \
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL_0(const wxPGProperty, RVAL)

// -----------------------------------------------------------------------


/** @class wxPropertyGridInterface

    Most of the shared property manipulation interface shared by wxPropertyGrid,
    wxPropertyGridPage, and wxPropertyGridManager is defined in this class.

    @remarks
    - In separate wxPropertyGrid component this class was known as
    wxPropertyContainerMethods.

    @library{wxpropgrid}
    @category{propgrid}
*/
class WXDLLIMPEXP_PROPGRID wxPropertyGridInterface
{
public:

    /** Destructor */
    virtual ~wxPropertyGridInterface() { }

    /**
        Appends property to the list.

        wxPropertyGrid assumes ownership of the object.
        Becomes child of most recently added category.
        @remarks
        - wxPropertyGrid takes the ownership of the property pointer.
        - If appending a category with name identical to a category already in
          the wxPropertyGrid, then newly created category is deleted, and most
          recently added category (under which properties are appended) is set
          to the one with same name. This allows easier adding of items to same
          categories in multiple passes.
        - Does not automatically redraw the control, so you may need to call
          Refresh when calling this function after control has been shown for
          the first time.
    */
    wxPGProperty* Append( wxPGProperty* property );

    wxPGProperty* AppendIn( wxPGPropArg id, wxPGProperty* newproperty );

    /**
        In order to add new items into a property with fixed children (for
        instance, wxFlagsProperty), you need to call this method. After
        populating has been finished, you need to call EndAddChildren.
    */
    void BeginAddChildren( wxPGPropArg id );

    /** Deletes all properties.
    */
    virtual void Clear() = 0;

    /** Deselect current selection, if any. Returns true if success
        (ie. validator did not intercept). */
    bool ClearSelection();

    /** Resets modified status of all properties.
    */
    void ClearModifiedStatus();

    /** Collapses given category or property with children.
        Returns true if actually collapses.
    */
    bool Collapse( wxPGPropArg id );

    /** Collapses all items that can be collapsed.

        @return
        Return false if failed (may fail if editor value cannot be validated).
    */
    bool CollapseAll() { return ExpandAll(false); }

    /**
        Changes value of a property, as if from an editor.
        Use this instead of SetPropertyValue() if you need the value to run
        through validation process, and also send the property change event.

        @return
        Returns true if value was successfully changed.
    */
    bool ChangePropertyValue( wxPGPropArg id, wxVariant newValue );

    /**
        Deletes a property by id. If category is deleted, all children are
        automatically deleted as well.
     */
    void DeleteProperty( wxPGPropArg id );

    /** Disables property. */
    bool DisableProperty( wxPGPropArg id ) { return EnableProperty(id,false); }

    /**
        Returns true if all property grid data changes have been committed.

        Usually only returns false if value in active editor has been
        invalidated by a wxValidator.
    */
    bool EditorValidate();

    /**
        Enables or disables property, depending on whether enable is true or
        false.
     */
    bool EnableProperty( wxPGPropArg id, bool enable = true );

    /** Called after population of property with fixed children has finished.
    */
    void EndAddChildren( wxPGPropArg id );

    /** Expands given category or property with children.
        Returns true if actually expands.
    */
    bool Expand( wxPGPropArg id );

    /** Expands all items that can be expanded.
    */
    bool ExpandAll( bool expand = true );

    /** Returns id of first child of given property.
        @remarks
        Does not return sub-properties!
    */
    wxPGProperty* GetFirstChild( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)

        if ( !p->GetChildCount() || p->HasFlag(wxPG_PROP_AGGREGATE) )
            return wxNullProperty;

        return p->Item(0);
    }

    //@{
    /** Returns iterator class instance.
        @param flags
        See @ref propgrid_iterator_flags. Value wxPG_ITERATE_DEFAULT causes
        iteration over everything except private child properties.
        @param firstProp
            Property to start iteration from. If NULL, then first child of root
            is used.
        @param startPos
            Either wxTOP or wxBOTTOM. wxTOP will indicate that iterations start
            from the first property from the top, and wxBOTTOM means that the
            iteration will instead begin from bottommost valid item.
    */
    wxPropertyGridIterator GetIterator( int flags = wxPG_ITERATE_DEFAULT,
                                        wxPGProperty* firstProp = NULL )
    {
        return wxPropertyGridIterator( m_pState, flags, firstProp );
    }

    wxPropertyGridConstIterator
    GetIterator( int flags = wxPG_ITERATE_DEFAULT,
                 wxPGProperty* firstProp = NULL ) const
    {
        return wxPropertyGridConstIterator( m_pState, flags, firstProp );
    }

    wxPropertyGridIterator GetIterator( int flags, int startPos )
    {
        return wxPropertyGridIterator( m_pState, flags, startPos );
    }

    wxPropertyGridConstIterator GetIterator( int flags, int startPos ) const
    {
        return wxPropertyGridConstIterator( m_pState, flags, startPos );
    }
    //@}

    /** Returns id of first item, whether it is a category or property.
        @param flags
        @link iteratorflags List of iterator flags@endlink
    */
    wxPGProperty* GetFirst( int flags = wxPG_ITERATE_ALL )
    {
        wxPropertyGridIterator it( m_pState, flags, wxNullProperty, 1 );
        return *it;
    }

    const wxPGProperty* GetFirst( int flags = wxPG_ITERATE_ALL ) const
    {
        return ((wxPropertyGridInterface*)this)->GetFirst(flags);
    }

    /**
        Returns id of property with given name (case-sensitive).

        If there is no property with such name, returned property id is invalid
        ( i.e. it will return false with IsOk method).
        @remarks
        - Sub-properties (i.e. properties which have parent that is not
          category or root) can not be accessed globally by their name.
          Instead, use "<property>.<subproperty>" in place of "<subproperty>".
    */
    wxPGProperty* GetProperty( const wxString& name ) const
    {
        return GetPropertyByName(name);
    }

    /** Returns map-like storage of property's attributes.
        @remarks
        Note that if extra style wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES is set,
        then builtin-attributes are not included in the storage.
    */
    const wxPGAttributeStorage& GetPropertyAttributes( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(*((const wxPGAttributeStorage*)NULL));
        return p->GetAttributes();
    }

    /** Adds to 'targetArr' pointers to properties that have given
        flags 'flags' set. However, if 'inverse' is set to true, then
        only properties without given flags are stored.
        @param flags
        Property flags to use.
        @param iterFlags
        Iterator flags to use. Default is everything expect private children.
    */
    void GetPropertiesWithFlag( wxArrayPGProperty* targetArr,
                                wxPGProperty::FlagType flags,
                                bool inverse = false,
                                int iterFlags = wxPG_ITERATE_PROPERTIES |
                                                wxPG_ITERATE_HIDDEN |
                                                wxPG_ITERATE_CATEGORIES) const;

    /** Returns value of given attribute. If none found, returns NULL-variant.
    */
    wxVariant GetPropertyAttribute( wxPGPropArg id,
                                    const wxString& attrName ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullVariant)
        return p->GetAttribute(attrName);
    }

    /** Returns pointer of property's nearest parent category. If no category
        found, returns NULL.
    */
    wxPropertyCategory* GetPropertyCategory( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_CONST_CALL_PROLOG_RETVAL(NULL)
        return m_pState->GetPropertyCategory(p);
    }

#ifndef SWIG
    /** Returns client data (void*) of a property. */
    void* GetPropertyClientData( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(NULL)
        return p->GetClientData();
    }
#endif

    /**
        Returns first property which label matches given string.

        NULL if none found. Note that this operation is extremely slow when
        compared to GetPropertyByName().
    */
    wxPGProperty* GetPropertyByLabel( const wxString& label ) const;

    /** Returns property with given name. NULL if none found.
    */
    wxPGProperty* GetPropertyByName( const wxString& name ) const;

    /** Returns child property 'subname' of property 'name'. Same as
        calling GetPropertyByName("name.subname"), albeit slightly faster.
    */
    wxPGProperty* GetPropertyByName( const wxString& name,
                                     const wxString& subname ) const;

    /** Returns property's editor. */
    const wxPGEditor* GetPropertyEditor( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(NULL)
        return p->GetEditorClass();
    }

    /** Returns help string associated with a property. */
    wxString GetPropertyHelpString( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(m_emptyString)
        return p->GetHelpString();
    }

    /** Returns property's custom value image (NULL of none). */
    wxBitmap* GetPropertyImage( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(NULL)
        return p->GetValueImage();
    }

    /** Returns label of a property. */
    const wxString& GetPropertyLabel( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(m_emptyString)
        return p->GetLabel();
    }

    /** Returns name of a property, by which it is globally accessible. */
    wxString GetPropertyName( wxPGProperty* property )
    {
        return property->GetName();
    }

    /** Returns parent item of a property. */
    wxPGProperty* GetPropertyParent( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)
        return p->GetParent();
    }

#if wxUSE_VALIDATORS
    /** Returns validator of a property as a reference, which you
        can pass to any number of SetPropertyValidator.
    */
    wxValidator* GetPropertyValidator( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(NULL)
        return p->GetValidator();
    }
#endif

    /** Returns value as wxVariant. To get wxObject pointer from it,
        you will have to use WX_PG_VARIANT_TO_WXOBJECT(VARIANT,CLASSNAME) macro.

        If property value is unspecified, Null variant is returned.
    */
    wxVariant GetPropertyValue( wxPGPropArg id )
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxVariant())
        return p->GetValue();
    }

    wxString GetPropertyValueAsString( wxPGPropArg id ) const;
    long GetPropertyValueAsLong( wxPGPropArg id ) const;
    unsigned long GetPropertyValueAsULong( wxPGPropArg id ) const
    {
        return (unsigned long) GetPropertyValueAsLong(id);
    }
#ifndef SWIG
    int GetPropertyValueAsInt( wxPGPropArg id ) const
        { return (int)GetPropertyValueAsLong(id); }
#endif
    bool GetPropertyValueAsBool( wxPGPropArg id ) const;
    double GetPropertyValueAsDouble( wxPGPropArg id ) const;

#define wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(TYPENAME, DEFVAL) \
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(DEFVAL) \
    wxString typeName(wxS(TYPENAME)); \
    wxVariant value = p->GetValue(); \
    if ( value.GetType() != typeName ) \
    { \
        wxPGGetFailed(p, typeName); \
        return DEFVAL; \
    }

#define wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL_WFALLBACK(TYPENAME, DEFVAL) \
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(DEFVAL) \
    wxVariant value = p->GetValue(); \
    if ( value.GetType() != wxS(TYPENAME) ) \
        return DEFVAL; \

    wxArrayString GetPropertyValueAsArrayString( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL("arrstring",
                                                   wxArrayString())
        return value.GetArrayString();
    }

#if wxUSE_LONGLONG_NATIVE
    wxLongLong_t GetPropertyValueAsLongLong( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL_WFALLBACK("wxLongLong",
                                             (long) GetPropertyValueAsLong(id))
        wxLongLong ll;
        ll << value;
        return ll.GetValue();
    }

    wxULongLong_t GetPropertyValueAsULongLong( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL_WFALLBACK("wxULongLong",
                                    (unsigned long) GetPropertyValueAsULong(id))
        wxULongLong ull;
        ull << value;
        return ull.GetValue();
    }
#endif

    wxArrayInt GetPropertyValueAsArrayInt( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL("wxArrayInt",
                                                   wxArrayInt())
        wxArrayInt arr;
        arr << value;
        return arr;
    }

#if wxUSE_DATETIME
    wxDateTime GetPropertyValueAsDateTime( wxPGPropArg id ) const
    {
        wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL("datetime",
                                                   wxDateTime())
        return value.GetDateTime();
    }
#endif

#ifndef SWIG
    /** Returns a wxVariant list containing wxVariant versions of all
        property values. Order is not guaranteed.
        @param flags
        Use wxPG_KEEP_STRUCTURE to retain category structure; each sub
        category will be its own wxVariantList of wxVariant.
        Use wxPG_INC_ATTRIBUTES to include property attributes as well.
        Each attribute will be stored as list variant named
        "@@<propname>@@attr."
        @remarks
    */
    wxVariant GetPropertyValues( const wxString& listname = wxEmptyString,
        wxPGProperty* baseparent = NULL, long flags = 0 ) const
    {
        return m_pState->DoGetPropertyValues(listname, baseparent, flags);
    }
#endif

    /** Returns currently selected property. */
    wxPGProperty* GetSelection() const
    {
        return m_pState->GetSelection();
    }

#ifndef SWIG
    wxPropertyGridPageState* GetState() const { return m_pState; }
#endif

    /** Similar to GetIterator(), but instead returns wxPGVIterator instance,
        which can be useful for forward-iterating through arbitrary property
        containers.

        @param flags
        See @ref propgrid_iterator_flags.
    */
    virtual wxPGVIterator GetVIterator( int flags ) const;

    /** Hides or reveals a property.
        @param hide
            If true, hides property, otherwise reveals it.
        @param flags
            By default changes are applied recursively. Set this paramter
            wxPG_DONT_RECURSE to prevent this.
    */
    bool HideProperty( wxPGPropArg id,
                       bool hide = true,
                       int flags = wxPG_RECURSE );

#if wxPG_INCLUDE_ADVPROPS
    /** Initializes *all* property types. Causes references to most object
        files in the library, so calling this may cause significant increase
        in executable size when linking with static library.
    */
    static void InitAllTypeHandlers();
#else
    static void InitAllTypeHandlers() { }
#endif

    //@{
    /** Inserts property to the property container.

        @param priorThis
        New property is inserted just prior to this. Available only
        in the first variant. There are two versions of this function
        to allow this parameter to be either an id or name to
        a property.

        @param newproperty
        Pointer to the inserted property. wxPropertyGrid will take
        ownership of this object.

        @param parent
        New property is inserted under this category. Available only
        in the second variant. There are two versions of this function
        to allow this parameter to be either an id or name to
        a property.

        @param index
        Index under category. Available only in the second variant.
        If index is < 0, property is appended in category.

        @return
        Returns id for the property,

        @remarks

        - wxPropertyGrid takes the ownership of the property pointer.

        - While Append may be faster way to add items, make note that when
          both types of data storage (categoric and
          non-categoric) are active, Insert becomes even more slow. This is
          especially true if current mode is non-categoric.

        Example of use:

        @code

            // append category
            wxPGProperty* my_cat_id = propertygrid->Append(
                new wxPropertyCategory("My Category") );

            ...

            // insert into category - using second variant
            wxPGProperty* my_item_id_1 = propertygrid->Insert(
                my_cat_id, 0, new wxStringProperty("My String 1") );

            // insert before to first item - using first variant
            wxPGProperty* my_item_id_2 = propertygrid->Insert(
                my_item_id, new wxStringProperty("My String 2") );

        @endcode

    */
    wxPGProperty* Insert( wxPGPropArg priorThis, wxPGProperty* newproperty );
    wxPGProperty* Insert( wxPGPropArg parent,
                          int index,
                          wxPGProperty* newproperty );
    //@}

    /** Returns true if property is a category. */
    bool IsPropertyCategory( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return p->IsCategory();
    }

    /** Returns true if property is enabled. */
    bool IsPropertyEnabled( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return (!(p->GetFlags() & wxPG_PROP_DISABLED))?true:false;
    }

    /**
        Returns true if given property is expanded.

        Naturally, always returns false for properties that cannot be expanded.
    */
    bool IsPropertyExpanded( wxPGPropArg id ) const;

    /**
        Returns true if property has been modified after value set or modify
        flag clear by software.
    */
    bool IsPropertyModified( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return ( (p->GetFlags() & wxPG_PROP_MODIFIED) ? true : false );
    }

    /**
        Returns true if property is shown (ie hideproperty with true not
        called for it).
     */
    bool IsPropertyShown( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return (!(p->GetFlags() & wxPG_PROP_HIDDEN))?true:false;
    }

    /** Returns true if property value is set to unspecified.
    */
    bool IsPropertyValueUnspecified( wxPGPropArg id ) const
    {
        wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
        return p->IsValueUnspecified();
    }

    /**
        Disables (limit = true) or enables (limit = false) wxTextCtrl editor of
        a property, if it is not the sole mean to edit the value.
    */
    void LimitPropertyEditing( wxPGPropArg id, bool limit = true );

    /** If state is shown in it's grid, refresh it now.
    */
    virtual void RefreshGrid( wxPropertyGridPageState* state = NULL );

#if wxPG_INCLUDE_ADVPROPS
    /**
        Initializes additional property editors (SpinCtrl etc.). Causes
        references to most object files in the library, so calling this may
        cause significant increase in executable size when linking with static
        library.
    */
    static void RegisterAdditionalEditors();
#else
    static void RegisterAdditionalEditors() { }
#endif

    /** Replaces property with id with newly created property. For example,
        this code replaces existing property named "Flags" with one that
        will have different set of items:
        @code
            pg->ReplaceProperty("Flags",
                wxFlagsProperty("Flags", wxPG_LABEL, newItems))
        @endcode
        For more info, see wxPropertyGrid::Insert.
    */
    wxPGProperty* ReplaceProperty( wxPGPropArg id, wxPGProperty* property );

    /** @anchor propgridinterface_editablestate_flags

        Flags for wxPropertyGridInterface::SaveEditableState() and
        wxPropertyGridInterface::RestoreEditableState().
    */
    enum EditableStateFlags
    {
        /** Include selected property. */
        SelectionState   = 0x01,
        /** Include expanded/collapsed property information. */
        ExpandedState    = 0x02,
        /** Include scrolled position. */
        ScrollPosState   = 0x04,
        /** Include selected page information.
            Only applies to wxPropertyGridManager. */
        PageState        = 0x08,
        /** Include splitter position. Stored for each page. */
        SplitterPosState = 0x10,
        /** Include description box size.
            Only applies to wxPropertyGridManager. */
        DescBoxState     = 0x20,

        /**
            Include all supported user editable state information.
            This is usually the default value. */
        AllStates        = SelectionState |
                           ExpandedState |
                           ScrollPosState |
                           PageState |
                           SplitterPosState |
                           DescBoxState
    };

    /**
        Restores user-editable state.

        See also wxPropertyGridInterface::SaveEditableState().

        @param src
            String generated by SaveEditableState.

        @param restoreStates
            Which parts to restore from source string. See @ref
            propgridinterface_editablestate_flags "list of editable state
            flags".

        @return
            False if there was problem reading the string.

        @remarks
        If some parts of state (such as scrolled or splitter position) fail to
        restore correctly, please make sure that you call this function after
        wxPropertyGrid size has been set (this may sometimes be tricky when
        sizers are used).
    */
    bool RestoreEditableState( const wxString& src,
                               int restoreStates = AllStates );

    /**
        Used to acquire user-editable state (selected property, expanded
        properties, scrolled position, splitter positions).

        @param includedStates
        Which parts of state to include. See @ref
        propgridinterface_editablestate_flags "list of editable state flags".
    */
    wxString SaveEditableState( int includedStates = AllStates ) const;

    /**
        Lets user to set the strings listed in the choice dropdown of a
        wxBoolProperty. Defaults are "True" and "False", so changing them to,
        say, "Yes" and "No" may be useful in some less technical applications.
    */
    static void SetBoolChoices( const wxString& trueChoice,
                                const wxString& falseChoice );

    /** Sets an attribute for this property.
        @param name
            Text identifier of attribute. See @ref propgrid_property_attributes.
        @param value
            Value of attribute.
        @param argFlags
            Optional. Use wxPG_RECURSE to set the attribute to child properties
            recursively.
    */
    void SetPropertyAttribute( wxPGPropArg id,
                               const wxString& attrName,
                               wxVariant value,
                               long argFlags = 0 )
    {
        DoSetPropertyAttribute(id,attrName,value,argFlags);
    }

    /** Sets property attribute for all applicapple properties.
        Be sure to use this method only after all properties have been
        added to the grid.
    */
    void SetPropertyAttributeAll( const wxString& attrName, wxVariant value );

    /** Sets text, bitmap, and colours for given column's cell.

        @remarks
        - You can set label cell by setting column to 0.
        - You can use wxPG_LABEL as text to use default text for column.
    */
    void SetPropertyCell( wxPGPropArg id,
                          int column,
                          const wxString& text = wxEmptyString,
                          const wxBitmap& bitmap = wxNullBitmap,
                          const wxColour& fgCol = wxNullColour,
                          const wxColour& bgCol = wxNullColour );

#ifndef SWIG
    /** Sets client data (void*) of a property.
        @remarks
        This untyped client data has to be deleted manually.
    */
    void SetPropertyClientData( wxPGPropArg id, void* clientData )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        p->SetClientData(clientData);
    }

    /** Sets editor for a property.

        @param editor
        For builtin editors, use wxPGEditor_X, where X is builtin editor's
        name (TextCtrl, Choice, etc. see wxPGEditor documentation for full
        list).

        For custom editors, use pointer you received from
        wxPropertyGrid::RegisterEditorClass().
    */
    void SetPropertyEditor( wxPGPropArg id, const wxPGEditor* editor )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        wxCHECK_RET( editor, wxT("unknown/NULL editor") );
        p->SetEditor(editor);
        RefreshProperty(p);
    }
#endif

    /** Sets editor control of a property. As editor argument, use
        editor name string, such as "TextCtrl" or "Choice".
    */
    void SetPropertyEditor( wxPGPropArg id, const wxString& editorName )
    {
        SetPropertyEditor(id,GetEditorByName(editorName));
    }

    /** Sets label of a property.
    */
    void SetPropertyLabel( wxPGPropArg id, const wxString& newproplabel );

    /**
        Sets name of a property.

        @param id
            Name or pointer of property which name to change.

        @param newName
            New name for property.
    */
    void SetPropertyName( wxPGPropArg id, const wxString& newName )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        m_pState->DoSetPropertyName( p, newName );
    }

    /**
        Sets property (and, recursively, its children) to have read-only value.
        In other words, user cannot change the value in the editor, but they
        can still copy it.
        @remarks
        This is mainly for use with textctrl editor. Not all other editors fully
        support it.
        @param flags
        By default changes are applied recursively. Set this paramter
        wxPG_DONT_RECURSE to prevent this.
    */
    void SetPropertyReadOnly( wxPGPropArg id,
                              bool set = true,
                              int flags = wxPG_RECURSE )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        if ( flags & wxPG_RECURSE )
            p->SetFlagRecursively(wxPG_PROP_READONLY, set);
        else
            p->SetFlag(wxPG_PROP_READONLY);
    }

    /** Sets property's value to unspecified.
        If it has children (it may be category), then the same thing is done to
        them.
    */
    void SetPropertyValueUnspecified( wxPGPropArg id );

#ifndef SWIG
    /** Sets various property values from a list of wxVariants. If property with
        name is missing from the grid, new property is created under given
        default category (or root if omitted).
    */
    void SetPropertyValues( const wxVariantList& list,
                            wxPGPropArg defaultCategory = wxNullProperty )
    {
        wxPGProperty *p;
        if ( defaultCategory.HasName() ) p = defaultCategory.GetPtr(this);
        else p = defaultCategory.GetPtr0();
        m_pState->DoSetPropertyValues(list, p);
    }

    void SetPropertyValues( const wxVariant& list,
                            wxPGPropArg defaultCategory = wxNullProperty )
    {
        SetPropertyValues(list.GetList(),defaultCategory);
    }
#endif

    /** Associates the help string with property.
        @remarks
        By default, text is shown either in the manager's "description"
        text box or in the status bar. If extra window style
        wxPG_EX_HELP_AS_TOOLTIPS is used, then the text will appear as a
        tooltip.
    */
    void SetPropertyHelpString( wxPGPropArg id, const wxString& helpString )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        p->SetHelpString(helpString);
    }

    /** Set wxBitmap in front of the value.
        @remarks
        - Bitmap will be scaled to a size returned by
        wxPropertyGrid::GetImageSize();
    */
    void SetPropertyImage( wxPGPropArg id, wxBitmap& bmp )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        p->SetValueImage(bmp);
        RefreshProperty(p);
    }

    /** Sets max length of property's text.
    */
    bool SetPropertyMaxLength( wxPGPropArg id, int maxLen );

#if wxUSE_VALIDATORS
    /** Sets validator of a property.
    */
    void SetPropertyValidator( wxPGPropArg id, const wxValidator& validator )
    {
        wxPG_PROP_ARG_CALL_PROLOG()
        p->SetValidator(validator);
    }
#endif

#ifndef SWIG
    /** Sets value (long integer) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, long value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }

    /** Sets value (integer) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, int value )
    {
        wxVariant v((long)value);
        SetPropVal( id, v );
    }
    /** Sets value (floating point) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, double value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }
    /** Sets value (bool) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, bool value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }
#if wxUSE_WCHAR_T
    void SetPropertyValue( wxPGPropArg id, const wchar_t* value )
    {
        SetPropertyValueString( id, wxString(value) );
    }
#endif
    void SetPropertyValue( wxPGPropArg id, const char* value )
    {
        SetPropertyValueString( id, wxString(value) );
    }
    void SetPropertyValue( wxPGPropArg id, const wxString& value )
    {
        SetPropertyValueString( id, value );
    }

    /** Sets value (wxArrayString) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, const wxArrayString& value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }

#if wxUSE_DATETIME
    void SetPropertyValue( wxPGPropArg id, const wxDateTime& value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }
#endif

    /** Sets value (wxObject*) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, wxObject* value )
    {
        wxVariant v(value);
        SetPropVal( id, v );
    }

    void SetPropertyValue( wxPGPropArg id, wxObject& value )
    {
        wxVariant v(&value);
        SetPropVal( id, v );
    }

#if wxUSE_LONGLONG_NATIVE
    /** Sets value (wxLongLong&) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, wxLongLong_t value )
    {
        wxVariant v = WXVARIANT(wxLongLong(value));
        SetPropVal( id, v );
    }
    /** Sets value (wxULongLong&) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, wxULongLong_t value )
    {
        wxVariant v = WXVARIANT(wxULongLong(value));
        SetPropVal( id, v );
    }
#endif

    /** Sets value (wxArrayInt&) of a property.
    */
    void SetPropertyValue( wxPGPropArg id, const wxArrayInt& value )
    {
        wxVariant v = WXVARIANT(value);
        SetPropVal( id, v );
    }
#endif  // !SWIG

    /** Sets value (wxString) of a property.

        @remarks
        This method uses wxPGProperty::SetValueFromString, which all properties
        should implement. This means that there should not be a type error,
        and instead the string is converted to property's actual value type.
    */
    void SetPropertyValueString( wxPGPropArg id, const wxString& value );

    /** Sets value (wxVariant&) of a property.

        @remarks
        Use wxPropertyGrid::ChangePropertyValue() instead if you need to run
        through validation process and send property change event.
    */
    void SetPropertyValue( wxPGPropArg id, wxVariant value )
    {
        SetPropVal( id, value );
    }

#ifndef SWIG
    /** Sets value (wxVariant&) of a property. Same as SetPropertyValue, but
        accepts reference. */
    void SetPropVal( wxPGPropArg id, wxVariant& value );
#endif

    /** Adjusts how wxPropertyGrid behaves when invalid value is entered
        in a property.
        @param vfbFlags
        See @link vfbflags list of valid flags values@endlink
    */
    void SetValidationFailureBehavior( int vfbFlags );

#ifdef SWIG
    %pythoncode {
        def MapType(class_,factory):
            "Registers Python type/class to property mapping.\n\nfactory: Property builder function/class."
            global _type2property
            try:
                mappings = _type2property
            except NameError:
                raise AssertionError("call only after a propertygrid or manager instance constructed")

            mappings[class_] = factory


        def DoDefaultTypeMappings(self):
            "Map built-in properties."
            global _type2property
            try:
                mappings = _type2property

                return
            except NameError:
                mappings = {}
                _type2property = mappings

            mappings[str] = StringProperty
            mappings[unicode] = StringProperty
            mappings[int] = IntProperty
            mappings[float] = FloatProperty
            mappings[bool] = BoolProperty
            mappings[list] = ArrayStringProperty
            mappings[tuple] = ArrayStringProperty
            mappings[wx.Font] = FontProperty
            mappings[wx.Colour] = ColourProperty
            "mappings[wx.Size] = SizeProperty"
            "mappings[wx.Point] = PointProperty"
            "mappings[wx.FontData] = FontDataProperty"

        def DoDefaultValueTypeMappings(self):
            "Map pg value type ids to getter methods."
            global _vt2getter
            try:
                vt2getter = _vt2getter

                return
            except NameError:
                vt2getter = {}
                _vt2getter = vt2getter

        def GetPropertyValues(self,dict_=None, as_strings=False, inc_attributes=False):
            "Returns values in the grid."
            ""
            "dict_: if not given, then a new one is created. dict_ can be"
            "  object as well, in which case it's __dict__ is used."
            "as_strings: if True, then string representations of values"
            "  are fetched instead of native types. Useful for config and such."
            "inc_attributes: if True, then property attributes are added"
            "  as @<propname>@<attr>."
            ""
            "Return value: dictionary with values. It is always a dictionary,"
            "so if dict_ was object with __dict__ attribute, then that attribute"
            "is returned."

            if dict_ is None:
                dict_ = {}
            elif hasattr(dict_,'__dict__'):
                dict_ = dict_.__dict__

            if not as_strings:
                getter = self.GetPropertyValue
            else:
                getter = self.GetPropertyValueAsString

            it = self.GetVIterator(PG_ITERATE_PROPERTIES)
            while not it.AtEnd():
                p = it.GetProperty()
                name = p.GetName()

                dict_[name] = getter(p)

                if inc_attributes:
                    attrs = p.GetAttributes()
                    if attrs and len(attrs):
                        dict_['@%s@attr'%name] = attrs

                it.Next()

            return dict_

        GetValues = GetPropertyValues


        def SetPropertyValues(self,dict_):
            "Sets property values from dict_, which can be either\ndictionary or an object with __dict__ attribute."
            ""
            "autofill: If true, keys with not relevant properties"
            "  are auto-created. For more info, see AutoFill."
            ""
            "Notes:"
            "  * Keys starting with underscore are ignored."
            "  * Attributes can be set with entries named @<propname>@<attr>."
            ""

            autofill = False

            if dict_ is None:
                dict_ = {}
            elif hasattr(dict_,'__dict__'):
                dict_ = dict_.__dict__

            attr_dicts = []

            def set_sub_obj(k0,dict_):
                for k,v in dict_.iteritems():
                    if k[0] != '_':
                        if k.endswith('@attr'):
                            attr_dicts.append((k[1:-5],v))
                        else:
                            try:
                                self.SetPropertyValue(k,v)
                            except:
                                try:
                                    if autofill:
                                        self._AutoFillOne(k0,k,v)
                                        continue
                                except:
                                    if isinstance(v,dict):
                                        set_sub_obj(k,v)
                                    elif hasattr(v,'__dict__'):
                                        set_sub_obj(k,v.__dict__)


            for k,v in attr_dicts:
                p = GetPropertyByName(k)
                if not p:
                    raise AssertionError("No such property: '%s'"%k)
                for an,av in v.iteritems():
                    p.SetAttribute(an, av)


            cur_page = False
            is_manager = isinstance(self,PropertyGridManager)

            try:
                set_sub_obj(self.GetGrid().GetRoot(),dict_)
            except:
                import traceback
                traceback.print_exc()

            self.Refresh()

        SetValues = SetPropertyValues

        def _AutoFillMany(self,cat,dict_):
            for k,v in dict_.iteritems():
                self._AutoFillOne(cat,k,v)


        def _AutoFillOne(self,cat,k,v):
            global _type2property

            factory = _type2property.get(v.__class__,None)

            if factory:
                self.AppendIn( cat, factory(k,k,v) )
            elif hasattr(v,'__dict__'):
                cat2 = self.AppendIn( cat, PropertyCategory(k) )
                self._AutoFillMany(cat2,v.__dict__)
            elif isinstance(v,dict):
                cat2 = self.AppendIn( cat, PropertyCategory(k) )
                self._AutoFillMany(cat2,v)
            elif not k.startswith('_'):
                raise AssertionError("member '%s' is of unregisted type/class '%s'"%(k,v.__class__))


        def AutoFill(self,obj,parent=None):
            "Clears properties and re-fills to match members and\nvalues of given object or dictionary obj."

            self.edited_objects[parent] = obj

            cur_page = False
            is_manager = isinstance(self,PropertyGridManager)

            if not parent:
                if is_manager:
                    page = self.GetCurrentPage()
                    page.Clear()
                    parent = page.GetRoot()
                else:
                    self.Clear()
                    parent = self.GetGrid().GetRoot()
            else:
                it = self.GetIterator(PG_ITERATE_PROPERTIES, parent)
                it.Next()  # Skip the parent
                while not it.AtEnd():
                    p = it.GetProperty()
                    if not p.IsSomeParent(parent):
                        break

                    self.DeleteProperty(p)

                    name = p.GetName()
                    it.Next()

            if not is_manager or page == self.GetCurrentPage():
                self.Freeze()
                cur_page = True

            try:
                self._AutoFillMany(parent,obj.__dict__)
            except:
                import traceback
                traceback.print_exc()

            if cur_page:
                self.Thaw()

        def RegisterEditor(self, editor, editorName=None):
            "Transform class into instance, if necessary."
            if not isinstance(editor, PGEditor):
                editor = editor()
            if not editorName:
                editorName = editor.__class__.__name__
            try:
                self._editor_instances.append(editor)
            except:
                self._editor_instances = [editor]
            RegisterEditor(editor, editorName)

        def GetPropertyClientData(self, p):
            if isinstance(p, basestring):
                p = self.GetPropertyByName(p)
            return p.GetClientData()

        def SetPropertyClientData(self, p, data):
            if isinstance(p, basestring):
                p = self.GetPropertyByName(p)
            return p.SetClientData(data)
    }
#endif

    // GetPropertyByName With nice assertion error message.
    wxPGProperty* GetPropertyByNameA( const wxString& name ) const;

    static wxPGEditor* GetEditorByName( const wxString& editorName );

    virtual void RefreshProperty( wxPGProperty* p ) = 0;

protected:

    /**
        In derived class, implement to set editable state component with
        given name to given value.
    */
    virtual bool SetEditableStateItem( const wxString& name, wxVariant value )
    {
        wxUnusedVar(name);
        wxUnusedVar(value);
        return false;
    }

    /**
        In derived class, implement to return editable state component with
        given name.
    */
    virtual wxVariant GetEditableStateItem( const wxString& name ) const
    {
        wxUnusedVar(name);
        return wxNullVariant;
    }

    // Returns page state data for given (sub) page (-1 means current page).
    virtual wxPropertyGridPageState* GetPageState( int pageIndex ) const
    {
        if ( pageIndex <= 0 )
            return m_pState;
        return NULL;
    }

    virtual bool DoSelectPage( int WXUNUSED(index) ) { return true; }

    // Default call's m_pState's BaseGetPropertyByName
    virtual wxPGProperty* DoGetPropertyByName( const wxString& name ) const;

#ifndef SWIG

    // Deriving classes must set this (it must be only or current page).
    wxPropertyGridPageState*         m_pState;

    // Intermediate version needed due to wxVariant copying inefficiency
    void DoSetPropertyAttribute( wxPGPropArg id,
                                 const wxString& name,
                                 wxVariant& value, long argFlags );

    // Empty string object to return from member functions returning const
    // wxString&.
    wxString                    m_emptyString;

private:
    // Cannot be GetGrid() due to ambiguity issues.
    wxPropertyGrid* GetPropertyGrid()
    {
        return m_pState->GetGrid();
    }

    // Cannot be GetGrid() due to ambiguity issues.
    const wxPropertyGrid* GetPropertyGrid() const
    {
        return (const wxPropertyGrid*) m_pState->GetGrid();
    }
#endif // #ifndef SWIG

    friend class wxPropertyGrid;
    friend class wxPropertyGridManager;
};

#endif // wxUSE_PROPGRID

#endif // __WX_PROPGRID_PROPGRIDIFACE_H__
