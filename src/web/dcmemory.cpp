// Copyright (c) 2007 John Wilmes

#include "wx/dcmemory.h"

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxWindowDC)

wxMemoryDC::wxMemoryDC() {
}

wxMemoryDC::~wxMemoryDC() {
}

void wxMemoryDC::DoSelect(const wxBitmap& bmp) {
}
