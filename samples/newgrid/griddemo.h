// griddemo.h
//
//  grid test program
//


#ifndef griddemo_h
#define griddemo_h


class wxGrid;

class GridApp : public wxApp
{
  public:
    bool OnInit();
};


class GridFrame : public wxFrame
{
    wxGrid         *grid;
    wxTextCtrl     *logWin;
    wxLogTextCtrl  *logger;
    wxString       logBuf;

    void SetDefaults();
    void ToggleRowLabels( wxCommandEvent& );
    void ToggleColLabels( wxCommandEvent& );
    void ToggleControlPanel( wxCommandEvent& );
    void ToggleCellEdit( wxCommandEvent& );
    void SetLabelColour( wxCommandEvent& );
    void SetLabelTextColour( wxCommandEvent& );
    void SetRowLabelHorizAlignment( wxCommandEvent& );
    void SetRowLabelVertAlignment( wxCommandEvent& );
    void SetColLabelHorizAlignment( wxCommandEvent& );
    void SetColLabelVertAlignment( wxCommandEvent& );
    void SetGridLineColour( wxCommandEvent& );
    void ClearGrid( wxCommandEvent& );

    void About( wxCommandEvent& );
    
    void OnLabelLeftClick( wxGridEvent& );
    void OnCellLeftClick( wxGridEvent& );
    void OnRowSize( wxGridSizeEvent& );
    void OnColSize( wxGridSizeEvent& );
    void OnRangeSelected( wxGridRangeSelectEvent& );
    void OnCellValueChanged( wxGridEvent& );
    
  public:
    GridFrame();
    ~GridFrame();

    void OnSize( wxSizeEvent& );
    void OnQuit( wxCommandEvent& );

    enum { ID_TOGGLEROWLABELS = 100,
           ID_TOGGLECOLLABELS,
           ID_TOGGLECONTROLPANEL,
           ID_TOGGLECELLEDIT,
           ID_SETLABELCOLOUR,
           ID_SETLABELTEXTCOLOUR,
           ID_ROWLABELALIGN,
           ID_ROWLABELHORIZALIGN,
           ID_ROWLABELVERTALIGN,
           ID_COLLABELALIGN,
           ID_COLLABELHORIZALIGN,
           ID_COLLABELVERTALIGN,
           ID_GRIDLINECOLOUR,
           ID_CLEARGRID,
           ID_ABOUT,

           ID_TESTFUNC };
        
    DECLARE_EVENT_TABLE()
};


#endif

