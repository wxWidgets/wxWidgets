///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/dialogbuttonfactory.h
// Purpose:     Internal standard-dialog-button transaction composition
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_DIALOGBUTTONFACTORY_H_
#define _WX_PRIVATE_DIALOGBUTTONFACTORY_H_

#include "wx/defs.h"

#include <cstdint>

class wxDialogBase;

namespace wxPrivate
{

enum class DialogButtonWriterField
{
    PermanentDefault,
    TemporaryDefault,
    Focus,
    Affirmative
};

struct DialogButtonWriterSnapshot
{
    std::uint64_t permanentDefault { 0 };
    std::uint64_t temporaryDefault { 0 };
    std::uint64_t focus { 0 };
    std::uint64_t affirmative { 0 };
};

// These free functions deliberately keep all transaction state outside the
// public dialog/sizer layouts. The conditional restore is a compare-and-swap:
// a callback that published any newer writer always wins.
WXDLLIMPEXP_CORE DialogButtonWriterSnapshot
SnapshotDialogButtonWriters(wxDialogBase* dialog);

WXDLLIMPEXP_CORE void RestoreDialogButtonWriter(
    wxDialogBase* dialog,
    DialogButtonWriterField field,
    std::uint64_t expectedWriter,
    std::uint64_t previousWriter);

} // namespace wxPrivate

#endif // _WX_PRIVATE_DIALOGBUTTONFACTORY_H_
