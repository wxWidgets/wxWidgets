// Include every directly dispatched public control header, not private WinUI
// headers. This also detects duplicate public definitions and include-order
// coupling when both __WXWINUI__ and __WXMSW__ are defined.
#include "wx/activityindicator.h"
#include "wx/bmpcbox.h"
#include "wx/button.h"
#include "wx/calctrl.h"
#include "wx/checkbox.h"
#include "wx/checklst.h"
#include "wx/choice.h"
#include "wx/clrpicker.h"
#include "wx/colordlg.h"
#include "wx/combobox.h"
#include "wx/datectrl.h"
#include "wx/gauge.h"
#include "wx/hyperlink.h"
#include "wx/infobar.h"
#include "wx/listbox.h"
#include "wx/msgdlg.h"
#include "wx/notebook.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/scrolbar.h"
#include "wx/slider.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/srchctrl.h"
#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/statline.h"
#include "wx/stattext.h"
#include "wx/statusbr.h"
#include "wx/textctrl.h"
#include "wx/textdlg.h"
#include "wx/tglbtn.h"
#include "wx/timectrl.h"
#include "wx/toolbar.h"
#include "wx/treectrl.h"
#include "contract.h"

#if WX_EXPECT_WINUI
    #if !defined(_WX_WINUI_RADIOBOX_H_) || \
        !defined(_WX_WINUI_SPINCTRL_H_) || !defined(_WX_WINUI_SPINBUTT_H_)
        #error WinUI dispatch was shadowed by MSW
    #endif
#else
    #if defined(_WX_WINUI_RADIOBOX_H_) || \
        defined(_WX_WINUI_SPINCTRL_H_) || defined(_WX_WINUI_SPINBUTT_H_)
        #error WinUI declarations leaked into the ordinary MSW toolkit
    #endif
#endif
