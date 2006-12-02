#Boa:FramePanel:LanguageSelectPanel
import os, sys
import wx
from wx.lib import langlistctrl
from Main import opj


# Normally you would just set _ to be a reference to the
# wx.GetTranslation function, and then wrap all you literal strings in
# _() function calls.  Then everytime you use one of your literals, it
# would first pass through the translation function and try to load a
# translated version of the string from the current message catalogs.
# For this example, since we are changinb language on the fly, and
# since we are only translating the label for one widget, we'll not do
# it the automatic way and we'll be more explicit.  See the setup in
# __init__() and the translation done in updateLanguage() below.

_ = wx.GetTranslation

exampleStrings = [
    'the quick brown fox jumps over the lazy dog',  # demo string
    'Tip of the Day',                               # wx built in translation
    'Warning',                                      # wx built in translation
]


[wxID_LANGUAGESELECTPANEL, wxID_LANGUAGESELECTPANELENGLISHBASECH, 
 wxID_LANGUAGESELECTPANELLANGCTRLCONTAINER, 
 wxID_LANGUAGESELECTPANELLANGFILTERRB, wxID_LANGUAGESELECTPANELSTATICLINE1, 
 wxID_LANGUAGESELECTPANELSTATICTEXT1, wxID_LANGUAGESELECTPANELSTATICTEXT2, 
 wxID_LANGUAGESELECTPANELSTATICTEXT3, wxID_LANGUAGESELECTPANELTRANSLATEDST, 
] = [wx.NewId() for _init_ctrls in range(9)]

class LanguageSelectPanel(wx.Panel):
    def _init_coll_boxSizer3_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.langCtrlContainer, 1, border=0, flag=wx.GROW)
        parent.AddSpacer(wx.Size(8, 8), border=0, flag=0)
        parent.AddWindow(self.langFilterRB, 0, border=0, flag=0)

    def _init_coll_flexGridSizer1_Growables(self, parent):
        # generated method, don't edit

        parent.AddGrowableRow(1)
        parent.AddGrowableCol(0)

    def _init_coll_boxSizer1_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText1, 0, border=8, flag=wx.ALL)
        parent.AddSizer(self.boxSizer3, 1, border=8, flag=wx.ALL | wx.GROW)
        parent.AddSizer(self.boxSizer2, 0, border=8, flag=wx.GROW | wx.ALL)

    def _init_coll_boxSizer2_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText2, 0, border=8, flag=wx.ALL)
        parent.AddWindow(self.englishBaseCh, 0, border=8, flag=wx.GROW | wx.ALL)
        parent.AddWindow(self.staticLine1, 0, border=8, flag=wx.GROW | wx.ALL)
        parent.AddWindow(self.staticText3, 0, border=8, flag=wx.ALL)
        parent.AddWindow(self.translatedST, 0, border=8, flag=wx.GROW | wx.ALL)

    def _init_sizers(self):
        # generated method, don't edit
        self.boxSizer1 = wx.BoxSizer(orient=wx.VERTICAL)

        self.flexGridSizer1 = wx.FlexGridSizer(cols=2, hgap=8, rows=0, vgap=8)

        self.boxSizer3 = wx.BoxSizer(orient=wx.HORIZONTAL)

        self.boxSizer2 = wx.BoxSizer(orient=wx.VERTICAL)

        self._init_coll_boxSizer1_Items(self.boxSizer1)
        self._init_coll_flexGridSizer1_Growables(self.flexGridSizer1)
        self._init_coll_boxSizer3_Items(self.boxSizer3)
        self._init_coll_boxSizer2_Items(self.boxSizer2)

        self.SetSizer(self.boxSizer1)

    def _init_ctrls(self, prnt):
        # generated method, don't edit
        wx.Panel.__init__(self, id=wxID_LANGUAGESELECTPANEL,
              name='LanguageSelectPanel', parent=prnt, 
              style=wx.RESIZE_BORDER | wx.DEFAULT_DIALOG_STYLE)
        
        self.staticText1 = wx.StaticText(id=wxID_LANGUAGESELECTPANELSTATICTEXT1,
              label='Choose a language that will be used for example translation.',
              name='staticText1', parent=self, style=0)

        self.langCtrlContainer = wx.Panel(id=wxID_LANGUAGESELECTPANELLANGCTRLCONTAINER,
              name='langCtrlContainer', parent=self, style=wx.TAB_TRAVERSAL)
        self.langCtrlContainer.SetBackgroundColour(wx.Colour(255, 255, 255))
        self.langCtrlContainer.Bind(wx.EVT_SIZE, self.OnLangCtrlContainerSize)

        self.langFilterRB = wx.RadioBox(choices=['Translated example languages',
              'Available languages on your system', 'All languages'],
              id=wxID_LANGUAGESELECTPANELLANGFILTERRB, label='Filter',
              majorDimension=1, name='langFilterRB', parent=self,
              style=wx.RA_SPECIFY_COLS)
        self.langFilterRB.Bind(wx.EVT_RADIOBOX, self.OnLangFilterRBRadiobox,
              id=wxID_LANGUAGESELECTPANELLANGFILTERRB)

        self.staticText2 = wx.StaticText(id=wxID_LANGUAGESELECTPANELSTATICTEXT2,
              label='English Text:', name='staticText2', parent=self,
              style=0)

        self.staticText3 = wx.StaticText(id=wxID_LANGUAGESELECTPANELSTATICTEXT3,
              label='Translated Text:', name='staticText3', parent=self,
              style=0)

        self.englishBaseCh = wx.Choice(choices=self.choices,
              id=wxID_LANGUAGESELECTPANELENGLISHBASECH, name='englishBaseCh',
              parent=self, style=0)
        self.englishBaseCh.Bind(wx.EVT_CHOICE, self.OnLangSelectAndTranslate,
              id=wxID_LANGUAGESELECTPANELENGLISHBASECH)

        self.staticLine1 = wx.StaticLine(id=wxID_LANGUAGESELECTPANELSTATICLINE1,
              name='staticLine1', parent=self, style=0)

        self.translatedST = wx.StaticText(id=wxID_LANGUAGESELECTPANELTRANSLATEDST,
              label='', name='translatedST', parent=self, style=0)

        self._init_sizers()

    def __init__(self, parent, log):
        self.choices = []
        self.choices = exampleStrings
        
        self._init_ctrls(parent)

        self.log = log

        lang = wx.LANGUAGE_DEFAULT
        filter = 'demo'
        langs = (wx.LANGUAGE_AFRIKAANS, wx.LANGUAGE_ENGLISH, wx.LANGUAGE_DEFAULT, 
                 wx.LANGUAGE_SPANISH, wx.LANGUAGE_GERMAN, wx.LANGUAGE_ITALIAN,
                 wx.LANGUAGE_FRENCH)


        # usually you would define wx.Locale in your wx.App.OnInit class.
        # for the demo we just define it in this module
        self.locale = None
        wx.Locale.AddCatalogLookupPathPrefix(opj('data/locale'))
        self.updateLanguage(wx.LANGUAGE_DEFAULT)

        
        self.filterMap = {'demo': langlistctrl.LC_ONLY, 
                          'available': langlistctrl.LC_AVAILABLE, 
                          'all': langlistctrl.LC_ALL}
                        
        self.filterIdxMap = {0: 'demo', 
                             1: 'available', 
                             2: 'all'}
        self.langs = langs
        self.langCtrl = langlistctrl.LanguageListCtrl(self.langCtrlContainer, -1, 
              filter=self.filterMap[filter], only=langs, select=lang)
        
        self.langCtrl.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnLangSelectAndTranslate)
        self.langCtrl.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.OnClearTranslatedText)
              
        self.OnLangCtrlContainerSize()
        
        self.englishBaseCh.Select(0)
        self.OnLangSelectAndTranslate()


    def updateLanguage(self, lang):
        # Make *sure* any existing locale is deleted before the new
        # one is created.  The old C++ object needs to be deleted
        # before the new one is created, and if we just assign a new
        # instance to the old Python variable, the old C++ locale will
        # not be destroyed soon enough, likely causing a crash.
        if self.locale:
            assert sys.getrefcount(self.locale) <= 2
            del self.locale
        
        # create a locale object for this language
        self.locale = wx.Locale(lang)
        self.locale.AddCatalog('wxpydemo')

    def translateExample(self):
        self.translatedST.SetLabel(_(self.englishBaseCh.GetStringSelection()))

    def OnLangCtrlContainerSize(self, event=None):
        if event: event.Skip()
        self.langCtrl.SetSize(self.langCtrlContainer.GetSize())

    def OnLangFilterRBRadiobox(self, event):
        self.langCtrl.SetUpFilter(
            self.filterMap[self.filterIdxMap[self.langFilterRB.GetSelection()]],
            self.langs)

    def OnLangSelectAndTranslate(self, event=None):
        lang = self.langCtrl.GetLanguage()
        
        if lang is not None:
            # set to the selected language
            self.updateLanguage(lang)

            self.translateExample()
            
            # set back to default
            self.updateLanguage(wx.LANGUAGE_DEFAULT)

    def OnClearTranslatedText(self, event):
        self.translatedST.SetLabel('')        


def runTest(frame, nb, log):
    win = LanguageSelectPanel(nb, log) 
    return win

#-------------------------------------------------------------------------------

overview = """<html><body>
<h2>Internationalization (I18N)</h2>
<p>
This demo demonstrates how to setup and use the wx.Locale object to translate text.
<p>
It also shows the langlistctrl.LanguageListCtrl that can be used to display 
languages with their associated countries flags, e.g. for setting the language
in your application.

</body></html>
"""
  
if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
