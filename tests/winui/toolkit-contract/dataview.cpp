#include "wx/dataview.h"
#include "contract.h"

#if wxUSE_DATAVIEWCTRL

#if WX_EXPECT_WINUI && wxUSE_NATIVE_DATAVIEWCTRL
    #if !defined(wxHAS_WINUI_DATAVIEWCTRL) || \
        !defined(wxHAS_NATIVE_DATAVIEWCTRL) || \
        defined(wxHAS_GENERIC_DATAVIEWCTRL) || \
        !defined(_WX_WINUI_DATAVIEW_H_)
        #error The WinUI data view must select its XAML backend
    #endif
#else
    #ifdef wxHAS_WINUI_DATAVIEWCTRL
        #error The WinUI data view leaked into another backend
    #endif
#endif

static_assert(std::is_base_of<wxDataViewCtrlBase, wxDataViewCtrl>::value,
              "Portable model and selection API");
static_assert(std::is_base_of<wxDataViewRendererBase,
                              wxDataViewCustomRenderer>::value,
              "Public custom renderer contract");

void DataViewPortableContract(wxDataViewCtrl& view,
                             wxDataViewColumn& column,
                             const wxDataViewItem& item)
{
    view.Select(item);
    view.Unselect(item);
    view.Expand(item);
    view.Collapse(item);
    view.EnsureVisible(item, &column);
    view.EditItem(item, &column);
    column.SetWidth(120);
    column.SetSortOrder(true);
    (void)view.GetItemRect(item, &column);
    (void)view.GetModel();
}

#endif // wxUSE_DATAVIEWCTRL
