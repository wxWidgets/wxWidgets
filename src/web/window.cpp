// Copyright (c) 2007 John Wilmes

#include "wx/window.h"

IMPLEMENT_ABSTRACT_CLASS(wxWindowWeb, wxWindowBase)

wxWindow* wxWindowBase::DoFindFocus() {
    //TODO
    return (wxWindow*)NULL;
}

wxWindow* wxWindowBase::GetCapture() {
    return (wxWindow*)NULL;
}

wxWindowWeb::wxWindowWeb() {
}

wxWindowWeb::wxWindowWeb(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    Create(parent, id, pos, size, style, name);
}

wxWindowWeb::~wxWindowWeb() {
}

bool wxWindowWeb::Create(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name) {
    return true;
}

bool wxWindowWeb::Reparent(wxWindowBase *newParent) {
    return true;
}

void wxWindowWeb::SetFocus()
{
}

bool wxWindowWeb::Show(bool show)
{
    return true;
}

void wxWindowWeb::Clear() {
}

void wxWindowWeb::Raise()
{
}

void wxWindowWeb::Lower()
{
}

void wxWindowWeb::Freeze() {
}

void wxWindowWeb::Thaw() {
}

void wxWindowWeb::SetLabel(const wxString& WXUNUSED(label))
{
}

WXWidget wxWindowWeb::GetHandle() const {
    return 0;
}

wxString wxWindowWeb::GetLabel() const
{
    return wxEmptyString;
}

int wxWindowWeb::GetScrollPos(int orient) const {
    return -1;
}

int wxWindowWeb::GetScrollRange(int orient) const {
    return -1;
}

int wxWindowWeb::GetScrollThumb(int orient) const {
    return -1;
}

void wxWindowWeb::DoCaptureMouse()
{
}

void wxWindowWeb::DoReleaseMouse()
{
}

bool wxWindowWeb::SetFont(const wxFont& font)
{
    return true;
}

bool wxWindowWeb::SetCursor(const wxCursor& cursor)
{
    return true;
}

void wxWindowWeb::SetScrollbar(int orient, int pos, int thumbVisible,
                               int range, bool refresh) {
}

void wxWindowWeb::SetScrollPos(int orient, int pos, bool refresh) {
}

void wxWindowWeb::WarpPointer (int x, int y)
{
}

void wxWindowWeb::ScrollWindow(int dx, int dy, const wxRect *rect)
{
}

void wxWindowWeb::DoGetSize(int *x, int *y) const
{
   *x = 0;
   *y = 0; 
}

void wxWindowWeb::DoGetPosition(int *x, int *y) const
{
    *x = 0;
    *y = 0;
}

void wxWindowWeb::DoScreenToClient(int *x, int *y) const
{
    *x = 0;
    *y = 0;
}

void wxWindowWeb::DoClientToScreen(int *x, int *y) const
{
    *x = 0;
    *x = 0;
}


void wxWindowWeb::DoGetClientSize(int *x, int *y) const
{
    *x = 0;
    *x = 0;
}

void wxWindowWeb::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
}

void wxWindowWeb::DoSetClientSize(int width, int height)
{
}

void wxWindowWeb::DoMoveWindow(int x, int y, int width, int height)
{
}

int wxWindowWeb::GetCharHeight() const
{
    return 0;
}

int wxWindowWeb::GetCharWidth() const
{
    return 0;
}

void wxWindowWeb::GetTextExtent(const wxString& string,
                                int *x, int *y,
                                int *descent, int *externalLeading,
                                const wxFont *theFont) const
{
    *x = 0;
    *y = 0;
}

void wxWindowWeb::Refresh(bool eraseBack, const wxRect *rect)
{
}

void wxWindowWeb::Update()
{
}
