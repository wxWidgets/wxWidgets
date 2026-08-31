///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/dataview.h
// Purpose:     Internal data view transaction helpers
// Author:      wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_DATAVIEW_H_
#define _WX_PRIVATE_DATAVIEW_H_

class wxDataViewCtrl;
class wxDataViewRendererBase;

namespace wxPrivate
{

using DataViewEditGeneration = unsigned long long;

// Reserve a single edit transaction for a control. Returning zero means that
// another start/finish transaction is still traversing application callbacks.
DataViewEditGeneration
BeginDataViewEdit(wxDataViewCtrl* ctrl, wxDataViewRendererBase* renderer);

bool IsCurrentDataViewEdit(wxDataViewCtrl* ctrl,
                           wxDataViewRendererBase* renderer,
                           DataViewEditGeneration generation);

DataViewEditGeneration
GetCurrentDataViewEditGeneration(wxDataViewCtrl* ctrl,
                                 wxDataViewRendererBase* renderer);

// Release only the specified generation. This makes cleanup from an outer
// transaction harmless when an EDITING_DONE handler already started a new one.
void EndDataViewEdit(wxDataViewCtrl* ctrl,
                     wxDataViewRendererBase* renderer,
                     DataViewEditGeneration generation);

// Called after validation and immediately before EDITING_DONE is emitted.
// Starting another editor from that event is part of the public contract.
void ReleaseDataViewEditForDone(wxDataViewCtrl* ctrl,
                                wxDataViewRendererBase* renderer,
                                DataViewEditGeneration generation);

} // namespace wxPrivate

#endif // _WX_PRIVATE_DATAVIEW_H_
