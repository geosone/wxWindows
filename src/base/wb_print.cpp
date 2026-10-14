/*
 * File:        wx_print.cc
 * Purpose:     Printing framework implementation
 * Author:      Julian Smart
 * Created:     1995
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation "wx_print.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx_defs.h"

#if USE_PRINTING_ARCHITECTURE
#if USE_COMMON_DIALOGS

#define WINDOWS_PRINTING    (wxTheApp->GetPrintMode() == wxPRINT_WINDOWS)

#ifndef WX_PRECOMP
#include "wx_utils.h"
#include "wx_dc.h"
#include "wx_main.h"
#include "wx_cmdlg.h"
#include "wx_frame.h"
#include "wx_messg.h"
#endif

#include "wx_print.h"

#include <stdlib.h>

#ifdef wx_msw

#include <commdlg.h>

#ifndef WIN32
#include <print.h>
#endif

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
#endif
 
#ifdef WIN32
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif
 
#if !defined(WIN32)	// 3.x uses FARPROC for dialogs
#define DLGPROC FARPROC
#endif

LONG APIENTRY _EXPORT wxAbortProc(HDC hPr, int Code);

/*
#ifdef GNUWIN32
#include "gnuwin32\extra.h"
#endif
*/

#endif
 // End wx_msw

#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

wxPrintPaperDatabase *wxThePrintPaperDatabase = NULL;

#endif

/*
 * wxPrintDialog
 */

wxPrintDialog::wxPrintDialog(void):
 wxDialogBox()
{
  dialogParent = NULL;
  printerDC = NULL;
  destroyDC = TRUE;
  deviceName = NULL;
  driverName = NULL;
  portName = NULL;
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintData *data):
 wxDialogBox()
{
  Create(p, data);
}

Bool wxPrintDialog::Create(wxWindow *p, wxPrintData *data)
{
  dialogParent = p;
  printerDC = NULL;
  destroyDC = TRUE;
  deviceName = NULL;
  driverName = NULL;
  portName = NULL;

  if (data)
    printData = (*data);

#ifdef wx_msw
  printData.SetOwnerWindow(p);
#endif

  return TRUE;
}

wxPrintDialog::~wxPrintDialog(void)
{
  if (destroyDC && printerDC)
    delete printerDC;
  if (deviceName) delete[] deviceName;
  if (driverName) delete[] driverName;
  if (portName) delete[] portName;
}

Bool wxPrintDialog::Show(Bool flag)
{
  if (!flag)
    return FALSE;
#ifdef wx_msw    
  if (WINDOWS_PRINTING)
  {
    printData.ConvertToNative();

#if 0
    int sz = sizeof(PRINTDLG);
    char buf[400];
    sprintf(buf, "Size of PRINTDLG = %d", sz);
    wxMessageBox(buf);
#endif

    /* Problem with GnuWin32: returns an error indicating an incorrect
     * structure size (extended error 1).
     * Gnu PRINTDLG = 68 bytes
     * MS VC++ PRINTDLG = 66
     * MS PRINTDLG as compiled by GnuWin32 = 68 bytes also.
     */

    Bool ret = PrintDlg( (PRINTDLG *)printData.printData ) ;
    if ( ret != FALSE && ((PRINTDLG *)printData.printData)->hDC)
    {
      wxPrinterDC *pdc = new wxPrinterDC(((PRINTDLG *)printData.printData)->hDC);
      printerDC = pdc;
      printData.ConvertFromNative();
      return TRUE;
    }
    else
    {
/*
      DWORD exError = CommDlgExtendedError();
      sprintf(buf, "ret = %d, ex error = %d", (int) ret, (int) exError);
      wxMessageBox(buf);
*/
      return FALSE;
    }
  }
  else
#endif
#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
  {
    if (printData.GetSetupDialog())
    {
      wxGenericPrintSetupDialog *genericPrintSetupDialog =
          new wxGenericPrintSetupDialog(dialogParent, wxThePrintSetupData);
      genericPrintSetupDialog->Show(TRUE);
      if (wxGenericPrintSetupDialog::printSetupDialogCancelled)
      {
        genericPrintSetupDialog->Close(TRUE);
        return FALSE;
      }
      else
      {
        *wxThePrintSetupData = genericPrintSetupDialog->GetPrintData();

        genericPrintSetupDialog->Close(TRUE);
        return TRUE;
      }
    }
    else
    {
      wxGenericPrintDialog *genericPrintDialog = new wxGenericPrintDialog(dialogParent, & printData);
      genericPrintDialog->Show(TRUE);
      if (wxGenericPrintDialog::printDialogCancelled)
      {
        genericPrintDialog->Close(TRUE);
        return FALSE;
      }
      else
      {
        printData = genericPrintDialog->printData;
        genericPrintDialog->Close(TRUE);
        return TRUE;
      }
    }
  }
#endif
  return FALSE;
}

wxDC *wxPrintDialog::GetPrintDC(void)
{
#ifdef wx_msw
  if (WINDOWS_PRINTING)
  {
    if (printerDC)
    {
      destroyDC = FALSE;
      return printerDC;
    }
    else
      return NULL;
  }
  else
#endif
#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
  {
    return new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), FALSE, NULL);
  }
#else
  return NULL;
#endif
}

/*
 * Print data
 */

wxPrintData::wxPrintData(void)
{
#ifdef wx_msw
    printData = NULL;
#endif
    printOrientation = wxPORTRAIT;
    printFromPage = 0;
    printToPage = 0;
    printMinPage = 0;
    printMaxPage = 0;
    printNoCopies = 1;
    printAllPages = FALSE;
    printCollate = FALSE;
    printToFile = FALSE;
    printEnableSelection = FALSE;
    printEnablePageNumbers = TRUE;
    printEnablePrintToFile = TRUE;
    printEnableHelp = FALSE;
    printSetupDialog = FALSE;
}

wxPrintData::~wxPrintData(void)
{
#ifdef wx_msw
  if (WINDOWS_PRINTING)
  {
    PRINTDLG *pd = (PRINTDLG *)printData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd )
        delete pd;
  }
#endif
}

#ifdef wx_msw
void wxPrintData::ConvertToNative(void)
{
    PRINTDLG *pd = (PRINTDLG*) printData;
    if ( pd == NULL )
    {
        pd = new PRINTDLG;
        printData = (void*) pd;

        // GNU-WIN32 has the wrong size PRINTDLG - can't work out why.
//        pd->lStructSize    = sizeof(PRINTDLG);
        pd->lStructSize    = 66 ;
        pd->hwndOwner      = (HWND)NULL;
        pd->hDevMode       = NULL; // Will be created by PrintDlg
        pd->hDevNames      = NULL; // Ditto

#if 0
        pd->Flags          = PD_RETURNDEFAULT;
        pd->nCopies        = 1;

        // Fill out the DEVMODE structure
        // so we can use it as input in the 'real' PrintDlg
        if (!PrintDlg(pd))
        {
            if ( pd->hDevMode )
                GlobalFree(pd->hDevMode);
            if ( pd->hDevNames )
                GlobalFree(pd->hDevNames);
            pd->hDevMode = NULL;
            pd->hDevNames = NULL;
        }
        else
        {
            if ( pd->hDevNames )
                GlobalFree(pd->hDevNames);
            pd->hDevNames = NULL;
        }
#endif
    }

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        devMode->dmOrientation = printOrientation;
        devMode->dmFields = DM_ORIENTATION;
        GlobalUnlock(pd->hDevMode);
    }
    pd->hDC = (HDC) NULL;
    pd->nFromPage = (UINT)printFromPage;
    pd->nToPage = (UINT)printToPage;
    pd->nMinPage = (UINT)printMinPage;
    pd->nMaxPage = (UINT)printMaxPage;
    pd->nCopies = (UINT)printNoCopies;

    pd->Flags = PD_RETURNDC ;
//    pd->lStructSize = sizeof( PRINTDLG );
    pd->lStructSize = 66 ;
    pd->hwndOwner=(HANDLE)NULL;
    pd->hDevNames=(HANDLE)NULL;
    pd->hInstance=(HINSTANCE)NULL;
    pd->lCustData = (LPARAM) NULL;
    pd->lpfnPrintHook = NULL;
    pd->lpfnSetupHook = NULL;
    pd->lpPrintTemplateName = NULL;
    pd->lpSetupTemplateName = NULL;
    pd->hPrintTemplate = (HGLOBAL) NULL;
    pd->hSetupTemplate = (HGLOBAL) NULL;

    if ( printAllPages )
        pd->Flags |= PD_ALLPAGES;
    if ( printCollate )
        pd->Flags |= PD_COLLATE;
    if ( printToFile )
        pd->Flags |= PD_PRINTTOFILE;
    if ( !printEnablePrintToFile )
        pd->Flags |= PD_DISABLEPRINTTOFILE;
    if ( !printEnableSelection )
      pd->Flags |= PD_NOSELECTION;
    if ( !printEnablePageNumbers )
      pd->Flags |= PD_NOPAGENUMS;
    if ( printEnableHelp )
      pd->Flags |= PD_SHOWHELP;
    if ( printSetupDialog )
      pd->Flags |= PD_PRINTSETUP;
}

void wxPrintData::ConvertFromNative(void)
{
    PRINTDLG *pd = (PRINTDLG*) printData;
    if ( pd == NULL )
        return;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        printOrientation = devMode->dmOrientation;
        GlobalUnlock(pd->hDevMode);
    }
    printFromPage = pd->nFromPage ;
    printToPage = pd->nToPage ;
    printMinPage = pd->nMinPage ;
    printMaxPage = pd->nMaxPage ;
    printNoCopies = pd->nCopies ;

    printAllPages = ((pd->Flags & PD_ALLPAGES) == PD_ALLPAGES);
    printCollate = ((pd->Flags & PD_COLLATE) == PD_COLLATE);
    printToFile = ((pd->Flags & PD_PRINTTOFILE) == PD_PRINTTOFILE);
    printEnablePrintToFile = ((pd->Flags & PD_DISABLEPRINTTOFILE) != PD_DISABLEPRINTTOFILE);
    printEnableSelection = ((pd->Flags & PD_NOSELECTION) != PD_NOSELECTION);
    printEnablePageNumbers = ((pd->Flags & PD_NOPAGENUMS) != PD_NOPAGENUMS);
    printEnableHelp = ((pd->Flags & PD_SHOWHELP) == PD_SHOWHELP);
    printSetupDialog = ((pd->Flags & PD_PRINTSETUP) == PD_PRINTSETUP);
}

void wxPrintData::SetOwnerWindow(wxWindow* win)
{
  if (WINDOWS_PRINTING)
  {
    if ( printData == NULL )
        ConvertToNative();

    if ( printData != NULL && win != NULL)
    {
      PRINTDLG *pd = (PRINTDLG *) printData ;
      pd->hwndOwner=win->GetHWND();
    }
  }
}
#endif

void wxPrintData::operator=(const wxPrintData& data)
{
    printFromPage = data.printFromPage;
    printToPage = data.printToPage;
    printMinPage = data.printMinPage;
    printMaxPage = data.printMaxPage;
    printNoCopies = data.printNoCopies;
    printAllPages = data.printAllPages;
    printCollate = data.printCollate;
    printToFile = data.printToFile;
    printEnableSelection = data.printEnableSelection;
    printEnablePageNumbers = data.printEnablePageNumbers;
    printEnableHelp = data.printEnableHelp;
    printEnablePrintToFile = data.printEnablePrintToFile;
    printSetupDialog = data.printSetupDialog;
    printOrientation = data.printOrientation;
}

/*
 * wxPageSetupDialog
 */

wxPageSetupDialog::wxPageSetupDialog(void):
 wxDialogBox()
{
  m_dialogParent = NULL;
}

wxPageSetupDialog::wxPageSetupDialog(wxWindow *p, wxPageSetupData *data):
 wxDialogBox()
{
  Create(p, data);
}

Bool wxPageSetupDialog::Create(wxWindow *p, wxPageSetupData *data)
{
  m_dialogParent = p;

  if (data)
    m_pageSetupData = (*data);

#if WIN95
  if (WINDOWS_PRINTING)
    m_pageSetupData.SetOwnerWindow(p);
#endif
  return TRUE;
}

wxPageSetupDialog::~wxPageSetupDialog(void)
{
}

Bool wxPageSetupDialog::Show(Bool flag)
{
  if (!flag)
    return FALSE;
#if WIN95
  if (WINDOWS_PRINTING)
  {
    m_pageSetupData.ConvertToNative();
    if (PageSetupDlg( (PAGESETUPDLG *)m_pageSetupData.GetNativeData() ))
    {
      m_pageSetupData.ConvertFromNative();
      return TRUE;
    }
    else
      return FALSE;
  }
  else
#endif
  // Only Windows 95 has the native page setup dialog; emulate on other platforms.
#if !(WIN95 && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
  {
      wxGenericPageSetupDialog *genericPageSetupDialog = new wxGenericPageSetupDialog(GetParent(), & m_pageSetupData);
      genericPageSetupDialog->Show(TRUE);
      if (wxGenericPageSetupDialog::pageSetupDialogCancelled)
      {
        return FALSE;
      }
      else
      {
        m_pageSetupData = genericPageSetupDialog->GetPageSetupData();
        genericPageSetupDialog->Close(TRUE);
        return TRUE;
      }
  }
#endif
  return FALSE;
}

/*
 * wxPageSetupData
 */

wxPageSetupData::wxPageSetupData(void)
{
#if WIN95
  m_pageSetupData = NULL;
#endif
  m_paperSize = wxPoint(0, 0);
  m_minMarginTopLeft = wxPoint(0, 0);
  m_minMarginBottomRight = wxPoint(0, 0);
  m_marginTopLeft = wxPoint(0, 0);
  m_marginBottomRight = wxPoint(0, 0);
  m_orientation = wxPORTRAIT;

  // Flags
  m_defaultMinMargins = FALSE;
  m_enableMargins = TRUE;
  m_enableOrientation = TRUE;
  m_enablePaper = TRUE;
  m_enablePrinter = TRUE;
  m_enableHelp = FALSE;
  m_getDefaultInfo = FALSE;
}

wxPageSetupData::~wxPageSetupData(void)
{
#if WIN95
  if (WINDOWS_PRINTING)
  {
    PAGESETUPDLG *pd = (PAGESETUPDLG *)m_pageSetupData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd )
        delete pd;
  }
#endif
}

void wxPageSetupData::operator=(const wxPageSetupData& data)
{
  m_paperSize = data.m_paperSize;
  m_minMarginTopLeft = data.m_minMarginTopLeft;
  m_minMarginBottomRight = data.m_minMarginBottomRight;
  m_marginTopLeft = data.m_marginTopLeft;
  m_marginBottomRight = data.m_marginBottomRight;
  m_orientation = data.m_orientation;

  m_defaultMinMargins = data.m_defaultMinMargins;
  m_enableMargins = data.m_enableMargins;
  m_enableOrientation = data.m_enableOrientation;
  m_enablePaper = data.m_enablePaper;
  m_enablePrinter = data.m_enablePrinter;
  m_getDefaultInfo = data.m_getDefaultInfo;;
  m_enableHelp = data.m_enableHelp;
}

#if WIN95
void wxPageSetupData::ConvertToNative(void)
{
  if (WINDOWS_PRINTING)
  {
    PAGESETUPDLG *pd = (PAGESETUPDLG*) m_pageSetupData;
    if ( m_pageSetupData == NULL )
    {
      pd = new PAGESETUPDLG;
      pd->hDevMode = GlobalAlloc(GMEM_MOVEABLE, sizeof(DEVMODE));
      m_pageSetupData = (void *)pd;
    }
  
    pd->Flags = PSD_MARGINS|PSD_MINMARGINS;

    if ( m_defaultMinMargins )
        pd->Flags |= PSD_DEFAULTMINMARGINS;
    if ( !m_enableMargins )
        pd->Flags |= PSD_DISABLEMARGINS;
    if ( !m_enableOrientation )
        pd->Flags |= PSD_DISABLEORIENTATION;
    if ( !m_enablePaper )
        pd->Flags |= PSD_DISABLEPAPER;
    if ( !m_enablePrinter )
        pd->Flags |= PSD_DISABLEPRINTER;
    if ( m_getDefaultInfo )
        pd->Flags |= PSD_RETURNDEFAULT;
    if ( m_enableHelp )
        pd->Flags |= PSD_SHOWHELP;

    pd->lStructSize = sizeof( PAGESETUPDLG );
    pd->hwndOwner=(HANDLE)NULL;
    pd->hDevNames=(HWND)NULL;
    pd->hInstance=(HINSTANCE)NULL;

    pd->ptPaperSize.x = m_paperSize.x;
    pd->ptPaperSize.y = m_paperSize.y;

    pd->rtMinMargin.left = m_minMarginTopLeft.x;
    pd->rtMinMargin.top = m_minMarginTopLeft.y;
    pd->rtMinMargin.right = m_minMarginBottomRight.x;
    pd->rtMinMargin.bottom = m_minMarginBottomRight.y;

    pd->rtMargin.left = m_marginTopLeft.x;
    pd->rtMargin.top = m_marginTopLeft.y;
    pd->rtMargin.right = m_marginBottomRight.x;
    pd->rtMargin.bottom = m_marginBottomRight.y;

    pd->lCustData = 0;
    pd->lpfnPageSetupHook = NULL;
    pd->lpfnPagePaintHook = NULL;
    pd->hPageSetupTemplate = NULL;
    pd->lpPageSetupTemplateName = NULL;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        memset(devMode, 0, sizeof(DEVMODE));
        devMode->dmSize = sizeof(DEVMODE);
        devMode->dmOrientation = m_orientation;
        devMode->dmFields = DM_ORIENTATION;
        GlobalUnlock(pd->hDevMode);
    }
  }
}

void wxPageSetupData::ConvertFromNative(void)
{
  if (WINDOWS_PRINTING)
  {
    PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData ;
    if ( !pd )
        return;

    pd->Flags = PSD_MARGINS|PSD_MINMARGINS;

    m_defaultMinMargins = ((pd->Flags & PSD_DEFAULTMINMARGINS) == PSD_DEFAULTMINMARGINS);
    m_enableMargins = ((pd->Flags & PSD_DISABLEMARGINS) != PSD_DISABLEMARGINS);
    m_enableOrientation = ((pd->Flags & PSD_DISABLEORIENTATION) != PSD_DISABLEORIENTATION);
    m_enablePaper = ((pd->Flags & PSD_DISABLEPAPER) != PSD_DISABLEPAPER);
    m_enablePrinter = ((pd->Flags & PSD_DISABLEPRINTER) != PSD_DISABLEPRINTER);
    m_getDefaultInfo = ((pd->Flags & PSD_RETURNDEFAULT) == PSD_RETURNDEFAULT);
    m_enableHelp = ((pd->Flags & PSD_SHOWHELP) == PSD_SHOWHELP);

    m_paperSize.x = pd->ptPaperSize.x ;
    m_paperSize.y = pd->ptPaperSize.y ;

    m_minMarginTopLeft.x = pd->rtMinMargin.left ;
    m_minMarginTopLeft.y = pd->rtMinMargin.top ;
    m_minMarginBottomRight.x = pd->rtMinMargin.right ;
    m_minMarginBottomRight.y = pd->rtMinMargin.bottom ;

    m_marginTopLeft.x = pd->rtMargin.left ;
    m_marginTopLeft.y = pd->rtMargin.top ;
    m_marginBottomRight.x = pd->rtMargin.right ;
    m_marginBottomRight.y = pd->rtMargin.bottom ;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        m_orientation = devMode->dmOrientation ;
        GlobalUnlock(pd->hDevMode);
    }
  }
}

void wxPageSetupData::SetOwnerWindow(wxWindow* win)
{
  if (WINDOWS_PRINTING)
  {
    if ( m_pageSetupData == NULL )
        ConvertToNative();

    if ( m_pageSetupData != NULL && win != NULL)
    {
      PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData ;
      pd->hwndOwner=win->GetHWND();
    }
  }
}
#endif

/*
 * Printer
 */
 
wxPrinter::wxPrinter(wxPrintData *data)
{
  currentPrintout = NULL;
  abortWindow = NULL;
  abortIt = FALSE;
  if (data)
    printData = (*data);
#ifdef wx_msw
  lpAbortProc = MakeProcInstance((FARPROC) wxAbortProc, wxhInstance);
#endif
}

wxWindow *wxPrinter::abortWindow = NULL;
Bool wxPrinter::abortIt = FALSE;

wxPrinter::~wxPrinter(void)
{
#ifdef wx_msw
  FreeProcInstance(lpAbortProc);
#endif
}

Bool wxPrinter::Print(wxWindow *parent, wxPrintout *printout, Bool prompt)
{
  abortIt = FALSE;
  abortWindow = NULL;

  if (!printout)
    return FALSE;
    
  printout->SetIsPreview(FALSE);
  printout->OnPreparePrinting();

  // Get some parameters from the printout, if defined
  int fromPage, toPage;
  int minPage, maxPage;
  printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

  if (maxPage == 0)
    return FALSE;

  printData.SetMinPage(minPage);
  printData.SetMaxPage(maxPage);
  if (fromPage != 0)
    printData.SetFromPage(fromPage);
  if (toPage != 0)
    printData.SetToPage(toPage);

  if (minPage != 0)
  {
//    printData.EnablePageNumbers(TRUE);

    if (printData.GetFromPage() < printData.GetMinPage())
      printData.SetFromPage(printData.GetMinPage());
    else if (printData.GetFromPage() > printData.GetMaxPage())
      printData.SetFromPage(printData.GetMaxPage());

    if (printData.GetToPage() > printData.GetMaxPage())
      printData.SetToPage(printData.GetMaxPage());
    else if (printData.GetToPage() < printData.GetMinPage())
      printData.SetToPage(printData.GetMinPage());
  }
/*
  else
    printData.EnablePageNumbers(FALSE);
*/
  
  // Create a suitable device context  
  wxDC *dc = NULL;
  if (prompt)
  {
    wxPrintDialog dialog(parent, &printData);
    if (dialog.Show(TRUE))
    {
      dc = dialog.GetPrintDC();
      printData = dialog.GetPrintData();
    }
  }
  else
  {
#ifdef wx_msw
    if (WINDOWS_PRINTING)
      dc = new wxPrinterDC(NULL, NULL, NULL, FALSE, printData.GetOrientation());
    else
#endif
#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
    {
      dc = new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), FALSE, NULL);
    }
#else
    return FALSE;
#endif
  }

  // May have pressed cancel.
  if (!dc || !dc->Ok())
  {
    if (dc) delete dc;
    return FALSE;
  }
  
  int logPPIScreenX = 0;
  int logPPIScreenY = 0;
  int logPPIPrinterX = 0;
  int logPPIPrinterY = 0;

#ifdef wx_msw
  if (WINDOWS_PRINTING)
  {
    HDC hdc = ::GetDC(NULL);
    logPPIScreenX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    logPPIScreenY = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(NULL, hdc);

    logPPIPrinterX = ::GetDeviceCaps(dc->cdc, LOGPIXELSX);
    logPPIPrinterY = ::GetDeviceCaps(dc->cdc, LOGPIXELSY);
    if (logPPIPrinterX == 0 || logPPIPrinterY == 0)
    {
      delete dc;
      return FALSE;
    }
  }
  else
#endif
  {
    // Correct values for X/PostScript?
    logPPIScreenX = 100;
    logPPIScreenY = 100;
    logPPIPrinterX = 100;
    logPPIPrinterY = 100;
  }

  printout->SetPPIScreen(logPPIScreenX, logPPIScreenY);
  printout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);

  // Set printout parameters  
  printout->SetDC(dc);

  float w, h;
  dc->GetSize(&w, &h);
  printout->SetPageSizePixels((int)w, (int)h);
  dc->GetSizeMM(&w, &h);
  printout->SetPageSizeMM((int)w, (int)h);

  // Create an abort window
  wxBeginBusyCursor();

#ifdef wx_msw
  wxWindow *win = CreateAbortWindow(parent, printout);
  wxYield();
  ::SetAbortProc(dc->cdc, lpAbortProc);
  
  if (!win)
  {
    wxEndBusyCursor();
    wxMessageBox("Sorry, could not create an abort dialog.", "Print Error", wxOK, parent);
    delete dc;
  }
  abortWindow = win;
  abortWindow->Show(TRUE);
  wxYield();
#endif

  printout->OnBeginPrinting();
  
  Bool keepGoing = TRUE;

  int copyCount;
  for (copyCount = 1; copyCount <= printData.GetNoCopies(); copyCount ++)
  {
    if (!printout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
    {
      wxEndBusyCursor();
      wxMessageBox("Could not start printing.", "Print Error", wxOK, parent);
      break;
    }
    if (abortIt)
      break;

    int pn;
    for (pn = printData.GetFromPage(); keepGoing && (pn <= printData.GetToPage()) && printout->HasPage(pn);
         pn++)
    {
      if (abortIt)
      {
        keepGoing = FALSE;
        break;
      }
      else
      {
#ifdef wx_msw
//        int dcID = ::SaveDC(dc->cdc);
#endif
        dc->StartPage();
        printout->OnPrintPage(pn);
        dc->EndPage();
#ifdef wx_msw
//        ::RestoreDC(dc->cdc, dcID);
#endif

        // Get some parameters again from the printout.
        // Printing this page might have changed toPage.
        printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

        printData.SetMinPage(minPage);
        printData.SetMaxPage(maxPage);
        if (fromPage != 0)
          printData.SetFromPage(fromPage);
        if (toPage != 0)
          printData.SetToPage(toPage);
      }
    }
    printout->OnEndDocument();
  }

  printout->OnEndPrinting();

#ifdef wx_msw
  if (abortWindow)
  {
    abortWindow->Show(FALSE);
    delete abortWindow;
    abortWindow = NULL;
  }
#endif
  
  wxEndBusyCursor();

  delete dc;
  
  return TRUE;
}

Bool wxPrinter::PrintDialog(wxWindow *parent)
{
  wxPrintDialog dialog(parent, &printData);
  return dialog.Show(TRUE);
}

static void wxAbortWindowCancel(wxButton& WXUNUSED(but), wxCommandEvent& WXUNUSED(event))
{
  wxPrinter::abortIt = TRUE;
  wxPrinter::abortWindow->Show(FALSE);
  delete wxPrinter::abortWindow;
  wxPrinter::abortWindow = NULL;
}

wxWindow *wxPrinter::CreateAbortWindow(wxWindow *parent, wxPrintout *WXUNUSED(printout))
{
  wxDialogBox *dialog = new wxDialogBox(parent, wxSTR_PRINTING, 0, 0, 400, 400);
  (void) new wxMessage(dialog, wxSTR_PRINTING_WAIT);
  dialog->NewLine();
  wxButton *button = new wxButton(dialog, (wxFunction) wxAbortWindowCancel, wxSTR_BUTTON_CANCEL);
  
  dialog->Fit();
  button->Centre(wxHORIZONTAL);

  dialog->Centre();
  return dialog;
}

Bool wxPrinter::Setup(wxWindow *parent)
{
  wxPrintDialog dialog(parent, &printData);
  dialog.GetPrintData().SetSetupDialog(TRUE);
  return dialog.Show(TRUE);
}

void wxPrinter::ReportError(wxWindow *parent, wxPrintout *WXUNUSED(printout), char *message)
{
  wxMessageBox(message, "Printing Error", wxOK, parent);
}

wxPrintData &wxPrinter::GetPrintData(void)
{
  return printData;
}

/*
 * Printout class
 */
 
wxPrintout::wxPrintout(char *title)
{
  printoutTitle = title ? copystring(title) : NULL;
  printoutDC = NULL;
  pageWidthMM = 0;
  pageHeightMM = 0;
  pageWidthPixels = 0;
  pageHeightPixels = 0;
  PPIScreenX = 0;
  PPIScreenY = 0;
  PPIPrinterX = 0;
  PPIPrinterY = 0;
  isPreview = FALSE;
}

wxPrintout::~wxPrintout(void)
{
  if (printoutTitle)
    delete[] printoutTitle;
}

Bool wxPrintout::OnBeginDocument(int WXUNUSED(startPage), int WXUNUSED(endPage))
{
  return GetDC()->StartDoc("Printing");
}

void wxPrintout::OnEndDocument(void)
{
  GetDC()->EndDoc();
}

void wxPrintout::OnBeginPrinting(void)
{
}

void wxPrintout::OnEndPrinting(void)
{
}

Bool wxPrintout::HasPage(int page)
{
  return (page == 1);
}

void wxPrintout::GetPageInfo(int *minPage, int *maxPage, int *fromPage, int *toPage)
{
  *minPage = 1;
  *maxPage = 32000;
  *fromPage = 1;
  *toPage = 1;
}

#ifdef wx_msw

/****************************************************************************

    FUNCTION: wxAbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

LONG APIENTRY _EXPORT wxAbortProc(HDC WXUNUSED(hPr), int WXUNUSED(Code))
{
    MSG msg;

    if (!wxPrinter::abortWindow)              /* If the abort dialog isn't up yet */
        return(TRUE);

    /* Process messages intended for the abort dialog box */

    while (!wxPrinter::abortIt && PeekMessage(&msg, NULL, NULL, NULL, TRUE))
        if (!IsDialogMessage(wxPrinter::abortWindow->GetHWND(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return (!wxPrinter::abortIt);
}
#endif

/*
 * Preview canvas
 */
 
wxPreviewCanvas::wxPreviewCanvas(wxPrintPreview *preview, wxWindow *parent, int x, int y, int w, int h,
    long style, char *name):
 wxCanvas(parent, x, y, w, h, style, name)
{
  printPreview = preview;
  if (!previewBackgroundBrush)
    previewBackgroundBrush = wxTheBrushList->FindOrCreateBrush("THISTLE", wxSOLID);
  SetBackground(previewBackgroundBrush);
  SetScrollbars(40, 40, 100, 100, 8, 8);
}

wxBrush *wxPreviewCanvas::previewBackgroundBrush = NULL;

wxPreviewCanvas::~wxPreviewCanvas(void)
{
}

void wxPreviewCanvas::OnPaint(void)
{
  wxCanvas::OnPaint();

#if USE_BITMAP_PREVIEW
  if (printPreview)
    printPreview->PaintPage(this);
#else
  printPreview->DrawBlankPage(this);
#endif
}

void wxPreviewCanvas::OnEvent(wxMouseEvent& WXUNUSED(event))
{
}

void wxPreviewCanvas::OnChar(wxKeyEvent& WXUNUSED(event))
{
}

void wxPreviewCanvas::OnSize(int w, int h)
{
#if !USE_BITMAP_PREVIEW
  if (printPreview && printPreview->GetPageCanvas())
  {
    float zoomScale = (float)((float)printPreview->GetZoom()/(float)100);
    float actualWidth = (float)(zoomScale*printPreview->GetPageWidth()*printPreview->GetPreviewScale());
    float actualHeight = (float)(zoomScale*printPreview->GetPageHeight()*printPreview->GetPreviewScale());

    float x = (float)((w - actualWidth)/2.0);
    if (x < printPreview->GetLeftMargin())
      x = (float)printPreview->GetLeftMargin();
    float y = (float)printPreview->GetTopMargin();

	int yPos = GetScrollPos(wxVERTICAL);
	int xPos = GetScrollPos(wxHORIZONTAL);

	int xPixels = - xPos * 40 + x;
	int yPixels = - yPos * 40 + y;

    printPreview->GetPageCanvas()->SetSize(xPixels, yPixels, actualWidth, actualHeight);
  }
#endif
}

void wxPreviewCanvas::OnScroll(wxCommandEvent& event)
{
	wxCanvas::OnScroll(event);
}

/*
 * Preview page canvas
 */

#if !USE_BITMAP_PREVIEW
wxPreviewPageCanvas::wxPreviewPageCanvas(wxWindow *parent, int x, int y, int w, int h,
    long style, char *name):
 wxCanvas(parent, x, y, w, h, style, name)
{
}

wxPreviewPageCanvas::~wxPreviewPageCanvas(void)
{
}

void wxPreviewPageCanvas::OnPaint(void)
{
  wxCanvas::OnPaint();

  wxPreviewCanvas* canvas = (wxPreviewCanvas*) GetParent();
  if (canvas->GetPrintPreview())
    canvas->GetPrintPreview()->PaintPage(canvas);
}

void wxPreviewPageCanvas::OnEvent(wxMouseEvent& WXUNUSED(event))
{
}

void wxPreviewPageCanvas::OnChar(wxKeyEvent& WXUNUSED(event))
{
}
#endif

/*
 * Preview control bar
 */
 
wxPreviewControlBar::wxPreviewControlBar(wxPrintPreview *preview, long buttons,
    wxWindow *parent, int x, int y, int w, int h,
    long style, char *name):
  wxPanel(parent, x, y, w, h, style, name)
{
  printPreview = preview;
  closeButton = NULL;
  nextPageButton = NULL;
  previousPageButton = NULL;
  printButton = NULL;
  zoomControl = NULL;
  buttonFlags = buttons;
}

wxFont *wxPreviewControlBar::buttonFont = NULL;

wxPreviewControlBar::~wxPreviewControlBar(void)
{
}

void wxPreviewControlBar::OnPaint(void)
{
  wxPanel::OnPaint();

  int w, h;
  GetSize(&w, &h);
  wxDC *dc = GetDC();
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxTRANSPARENT_BRUSH);
  DrawLine((float)0, (float)(h-1), (float)w, (float)(h-1));
}

static void wxPreviewCloseFunc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxPreviewControlBar *controlBar = (wxPreviewControlBar *)but.GetParent();
  wxPreviewFrame *frame = (wxPreviewFrame *)controlBar->GetParent();
  if (frame->GetEventHandler()->OnClose())
#ifdef wx_motif
    wxPostDelete(frame);
#else
    delete frame;
#endif
}

static void wxPreviewPrintFunc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxPreviewControlBar *controlBar = (wxPreviewControlBar *)but.GetParent();
  wxPrintPreview *preview = controlBar->GetPrintPreview();
  preview->Print(TRUE);
}

static void wxPreviewNextFunc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxPreviewControlBar *controlBar = (wxPreviewControlBar *)but.GetParent();
  wxPrintPreview *preview = controlBar->GetPrintPreview();
  if (preview)
  {
    int currentPage = preview->GetCurrentPage();

    int minPage, maxPage, pageFrom, pageTo;
    preview->GetPrintout()->GetPageInfo(&minPage, &maxPage, &pageFrom, &pageTo);

    if ((maxPage > 0) &&
        (currentPage < maxPage) &&
        preview->GetPrintout()->HasPage(currentPage + 1))
    {
      preview->SetCurrentPage(currentPage + 1);
    }
  }
}

static void wxPreviewPreviousFunc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxPreviewControlBar *controlBar = (wxPreviewControlBar *)but.GetParent();
  wxPrintPreview *preview = controlBar->GetPrintPreview();
  if (preview)
  {
    int currentPage = preview->GetCurrentPage();

    int minPage, maxPage, pageFrom, pageTo;
    preview->GetPrintout()->GetPageInfo(&minPage, &maxPage, &pageFrom, &pageTo);

    if ((minPage > 0) &&
        (currentPage > minPage) &&
        preview->GetPrintout()->HasPage(currentPage - 1))
    {
      preview->SetCurrentPage(currentPage - 1);
    }
  }
}

static void wxPreviewZoomFunc(wxChoice& but, wxCommandEvent& WXUNUSED(event))
{
  wxPreviewControlBar *controlBar = (wxPreviewControlBar *)but.GetParent();
  int zoom = controlBar->GetZoomControl();
  if (controlBar->GetPrintPreview())
    controlBar->GetPrintPreview()->SetZoom(zoom);
}

void wxPreviewControlBar::CreateButtons(void)
{
  SetSize(0, 0, 400, 40);

  SetLabelPosition(wxVERTICAL);
  if (!buttonFont)
    buttonFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxBOLD);
  SetButtonFont(buttonFont);

  int buttonWidth = 65;
  int buttonHeight = 25;

  closeButton = new wxButton(this, (wxFunction)wxPreviewCloseFunc, "Close", -1, -1, buttonWidth, buttonHeight);
  
  if (buttonFlags & wxPREVIEW_PRINT)
    printButton =  new wxButton(this, (wxFunction)wxPreviewPrintFunc, "Print...", -1, -1, buttonWidth, buttonHeight);
  if (buttonFlags & wxPREVIEW_PREVIOUS)
    previousPageButton = new wxButton(this, (wxFunction)wxPreviewPreviousFunc, "<<", -1, -1, buttonWidth, buttonHeight);
  if (buttonFlags & wxPREVIEW_NEXT)
    nextPageButton = new wxButton(this, (wxFunction)wxPreviewNextFunc, ">>", -1, -1, buttonWidth, buttonHeight);

  static char *choices[] = { "10%", "20%", "25%", "30%", "35%", "40%", "45%", "50%", "55%", "60%",
    "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%", "110%", "120%", "150%", "200%" };
  int n = 22;
  if (buttonFlags & wxPREVIEW_ZOOM)
  {
    zoomControl = new wxChoice(this, (wxFunction)wxPreviewZoomFunc, NULL, -1, -1, 100, -1, n, (char **)choices);
    SetZoomControl(printPreview->GetZoom());
  }

  closeButton->SetDefault();
}

void wxPreviewControlBar::SetZoomControl(int zoom)
{
  char buf[20];
  sprintf(buf, "%d%%", zoom);
  if (zoomControl)
    zoomControl->SetStringSelection(buf);
}

int wxPreviewControlBar::GetZoomControl(void)
{
  char buf[20];
  if (zoomControl && zoomControl->GetStringSelection())
  {
    strcpy(buf, zoomControl->GetStringSelection());
    buf[strlen(buf) - 1] = 0;
    return (int)atoi(buf);
  }
  else return 0;
}

/*
 * Preview frame
 */

wxPreviewFrame::wxPreviewFrame(wxPrintPreview *preview, wxFrame *parent, char *title,
    int x, int y, int w, int h, long style, char *name):
 wxFrame(parent, title, x, y, w, h, style, name)
{
  printPreview = preview;
  controlBar = NULL;
  previewCanvas = NULL;
#if !USE_BITMAP_PREVIEW
  pagePreviewCanvas = NULL;
#endif
}

wxPreviewFrame::~wxPreviewFrame(void)
{
}

Bool wxPreviewFrame::OnClose(void)
{
  MakeModal(FALSE);
  
  // Need to delete the printout and the print preview
  wxPrintout *printout = printPreview->GetPrintout();
  if (printout)
  {
    delete printout;
    printPreview->SetPrintout(NULL);
    printPreview->SetCanvas(NULL);
#if !USE_BITMAP_PREVIEW
    printPreview->SetPageCanvas(NULL);
#endif
    printPreview->SetFrame(NULL);
  }
  delete printPreview;
  return TRUE;
}

void wxPreviewFrame::Initialize(void)
{
  CreateStatusLine();
  
  CreateCanvas();
  CreateControlBar();

  printPreview->SetCanvas(previewCanvas);
#if !USE_BITMAP_PREVIEW
  printPreview->SetPageCanvas(pagePreviewCanvas);
#endif
  printPreview->SetFrame(this);

  // Set layout constraints here

  // Control bar constraints
  wxLayoutConstraints *c1 = new wxLayoutConstraints;
//  int w, h;
//  controlBar->GetSize(&w, &h);
  int h;
#ifdef wx_msw
  h = 40;
#else
  h = 60;
#endif

  c1->left.SameAs       (this, wxLeft);
  c1->top.SameAs        (this, wxTop);
  c1->right.SameAs      (this, wxRight);
  c1->height.Absolute   (h);

  controlBar->SetConstraints(c1);

  // Canvas constraints
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (this, wxLeft);
  c2->top.Below         (controlBar);
  c2->right.SameAs      (this, wxRight);
  c2->bottom.SameAs     (this, wxBottom);

  previewCanvas->SetConstraints(c2);

  SetAutoLayout(TRUE);

  MakeModal(TRUE);

#ifdef wx_x
  Layout();
#endif
}

void wxPreviewFrame::CreateCanvas(void)
{
  previewCanvas = new wxPreviewCanvas(printPreview, this);
#if !USE_BITMAP_PREVIEW
  pagePreviewCanvas = new wxPreviewPageCanvas(previewCanvas);
#endif
}

void wxPreviewFrame::CreateControlBar(void)
{
  long buttons = wxPREVIEW_DEFAULT;
  if (printPreview->GetPrintoutForPrinting())
    buttons |= wxPREVIEW_PRINT;
    
  controlBar = new wxPreviewControlBar(printPreview, buttons, this, 0, 0, 400, 40);
  controlBar->CreateButtons();
}
 
/*
 * Print preview
 */

wxPrintPreview::wxPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data)
{
  isOk = TRUE;
  previewPrintout = printout;
  if (previewPrintout)
    previewPrintout->SetIsPreview(TRUE);
    
  printPrintout = printoutForPrinting;
  if (data)
    printData = (*data);

  previewCanvas = NULL;
#if !USE_BITMAP_PREVIEW
  previewPageCanvas = NULL;
#endif
  previewFrame = NULL;
  previewBitmap = NULL;
  currentPage = 1;
  currentZoom = 30;
  topMargin = 40;
  leftMargin = 40;
  pageWidth = 0;
  pageHeight = 0;

  printout->OnPreparePrinting();

  // Get some parameters from the printout, if defined
  int minPage, maxPage;
  int selFrom, selTo;
  printout->GetPageInfo(&minPage, &maxPage, &selFrom, &selTo);
  printData.SetMinPage(minPage);
  printData.SetMaxPage(maxPage);

  DetermineScaling();
}

wxPrintPreview::~wxPrintPreview(void)
{
  if (previewPrintout)
    delete previewPrintout;
  if (previewBitmap)
    delete previewBitmap;
  if (printPrintout)
    delete printPrintout;
}

Bool wxPrintPreview::SetCurrentPage(int pageNum)
{
  if (currentPage == pageNum)
    return TRUE;

  currentPage = pageNum;

#if USE_BITMAP_PREVIEW
  if (previewBitmap)
  {
    delete previewBitmap;
    previewBitmap = NULL;
  }

  if (previewCanvas)
  {
    RenderPage(pageNum);
    previewCanvas->Refresh();
  }
#else
  if (previewCanvas)
    previewCanvas->Refresh(TRUE);
  if (previewPageCanvas)
	previewPageCanvas->Refresh(TRUE);
#endif

  return TRUE;
}

void wxPrintPreview::SetPrintout(wxPrintout *printout)
{
  previewPrintout = printout;
}

void wxPrintPreview::SetFrame(wxFrame *frame)
{
  previewFrame = frame;
}
  
void wxPrintPreview::SetCanvas(wxCanvas *canvas)
{
  previewCanvas = canvas;
}

Bool wxPrintPreview::PaintPage(wxCanvas *canvas)
{
#if USE_BITMAP_PREVIEW
  DrawBlankPage(canvas);

  if (!previewBitmap)
    RenderPage(currentPage);
    
  if (!previewBitmap)
    return FALSE;

  if (!canvas)
    return FALSE;

  int canvasWidth, canvasHeight;
  canvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
  float y = (float)topMargin;

  wxMemoryDC temp_dc;
  temp_dc.SelectObject(previewBitmap);
  canvas->GetDC()->Blit((float)x, (float)y, (float)previewBitmap->GetWidth(), (float)previewBitmap->GetHeight(), &temp_dc, (float)0, (float)0);

  temp_dc.SelectObject(NULL);

  return TRUE;
#else
  int canvasWidth, canvasHeight;
  canvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);
  float actualHeight = (float)(zoomScale*pageHeight*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
  float y = (float)topMargin;

  int yPos = canvas->GetScrollPos(wxVERTICAL);
  int xPos = canvas->GetScrollPos(wxHORIZONTAL);

  int xPixels = - xPos * 40 + x;
  int yPixels = - yPos * 40 + y;

  // Need to draw the blank page also

  previewPageCanvas->SetSize(xPixels, yPixels, actualWidth, actualHeight);
  RenderPage(currentPage);

  return TRUE;
#endif
}

Bool wxPrintPreview::RenderPage(int pageNum)
{
#if USE_BITMAP_PREVIEW
  int canvasWidth, canvasHeight;
  if (!previewCanvas)
  {
    wxMessageBox("wxPrintPreview::RenderPage: must use wxPrintPreview::SetCanvas to let me know about the canvas!",
      "Print Preview Failure", wxOK);
    return FALSE;
  }
  previewCanvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);
  float actualHeight = (float)(zoomScale*pageHeight*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
//  float y = topMargin;

  if (!previewBitmap)
  {
    previewBitmap = new wxBitmap((int)actualWidth, (int)actualHeight);
    if (!previewBitmap || !previewBitmap->Ok())
    {
      if (previewBitmap)
        delete previewBitmap;
      previewBitmap = NULL;
      wxMessageBox("Sorry, not enough memory to create a preview.", "Print Preview Failure", wxOK);
      return FALSE;
    }
  }

  wxBeginBusyCursor();

  wxMemoryDC memoryDC;
  memoryDC.SelectObject(previewBitmap);

  memoryDC.Clear();

  previewPrintout->SetDC(&memoryDC);
  previewPrintout->SetPageSizePixels(pageWidth, pageHeight);
  previewPrintout->OnBeginPrinting();

  if (!previewPrintout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
  {
    wxMessageBox("Could not start document preview.", "Print Preview Failure", wxOK);
    memoryDC.SelectObject(NULL);
    delete previewBitmap;
    wxEndBusyCursor();

    return FALSE;
  }
  previewPrintout->OnPrintPage(pageNum);
  previewPrintout->OnEndDocument();
  previewPrintout->OnEndPrinting();
  previewPrintout->SetDC(NULL);
  memoryDC.SelectObject(NULL);

  // Get some parameters from the printout, if defined
  int minPage, maxPage;
  int selFrom, selTo;
  previewPrintout->GetPageInfo(&minPage, &maxPage, &selFrom, &selTo);
  if ( minPage != 0 )
    printData.SetMinPage(minPage);
  if ( maxPage != 0 )
    printData.SetMaxPage(maxPage);

  char buf[200];
  if (printData.GetMaxPage() != 0)
    sprintf(buf, "Page %d of %d", pageNum, printData.GetMaxPage());
  else
    sprintf(buf, "Page %d", pageNum);

  if (previewFrame)
    previewFrame->SetStatusText(buf);

  wxEndBusyCursor();

  return TRUE;
#else
  int canvasWidth, canvasHeight;
  if (!previewCanvas || !previewPageCanvas)
  {
    wxMessageBox("wxPrintPreview::RenderPage: must use wxPrintPreview::SetCanvas to let me know about the canvas!",
      "Print Preview Failure", wxOK);
    return FALSE;
  }
  previewCanvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);
  float actualHeight = (float)(zoomScale*pageHeight*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
//  float y = topMargin;

  wxBeginBusyCursor();

  wxDC& canvasDC = * previewPageCanvas->GetDC();

  canvasDC.BeginDrawing();

  canvasDC.SetBackground(wxWHITE_BRUSH);
  canvasDC.SetLogicalOrigin(0, 0);
  canvasDC.SetUserScale(1.0, 1.0);
  canvasDC.SetLogicalFunction(wxCOPY);
  canvasDC.SetBackgroundMode(wxSOLID);
  canvasDC.Clear();

  previewPrintout->SetDC(&canvasDC);
  previewPrintout->SetPageSizePixels(pageWidth, pageHeight);
  previewPrintout->OnBeginPrinting();

  if (!previewPrintout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
  {
	canvasDC.EndDrawing();
    wxMessageBox("Could not start document preview.", "Print Preview Failure", wxOK);
    wxEndBusyCursor();

    return FALSE;
  }
  previewPrintout->OnPrintPage(pageNum);
  previewPrintout->OnEndDocument();
  previewPrintout->OnEndPrinting();
  previewPrintout->SetDC(NULL);

  canvasDC.EndDrawing();

  // Get some parameters from the printout, if defined
  int minPage, maxPage;
  int selFrom, selTo;
  previewPrintout->GetPageInfo(&minPage, &maxPage, &selFrom, &selTo);
  if ( minPage != 0 )
    printData.SetMinPage(minPage);
  if ( maxPage != 0 )
    printData.SetMaxPage(maxPage);

  char buf[200];
  if (printData.GetMaxPage() != 0)
    sprintf(buf, "Page %d of %d", pageNum, printData.GetMaxPage());
  else
    sprintf(buf, "Page %d", pageNum);

  if (previewFrame)
    previewFrame->SetStatusText(buf);

  wxEndBusyCursor();

  return TRUE;

#endif
}

Bool wxPrintPreview::DrawBlankPage(wxCanvas *canvas)
{
  int canvasWidth, canvasHeight;
  canvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = zoomScale*pageWidth*previewScale;
  float actualHeight = zoomScale*pageHeight*previewScale;

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
  float y = (float)topMargin;

  wxDC *dc = canvas->GetDC();

  // Draw shadow, allowing for 1-pixel border AROUND the actual page
  int shadowOffset = 4;
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxBLACK_BRUSH);
  dc->DrawRectangle(x-1 + shadowOffset, y-1 + shadowOffset, actualWidth+2, actualHeight+2);

  // Draw blank page allowing for 1-pixel border AROUND the actual page
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxWHITE_BRUSH);
  dc->DrawRectangle(x-1, y-1, actualWidth+2, actualHeight+2);
  return TRUE;
}

wxPrintData &wxPrintPreview::GetPrintData(void)
{
  return printData;
}

void wxPrintPreview::SetZoom(int percent)
{
  if (currentZoom == percent)
    return;
    
  currentZoom = percent;

#if USE_BITMAP_PREVIEW
  if (previewBitmap)
  {
    delete previewBitmap;
    previewBitmap = NULL;
  }
  RenderPage(currentPage);

  if (previewCanvas)
  {
    previewCanvas->Clear();
    previewCanvas->Refresh();
  }
#else
  if (previewCanvas)
    previewCanvas->Refresh(TRUE);
  if (previewPageCanvas)
	previewPageCanvas->Refresh(TRUE);
#endif
}

int wxPrintPreview::GetZoom(void)
{
  return currentZoom;
}

Bool wxPrintPreview::Print(Bool interactive)
{
  if (!printPrintout)
    return FALSE;
  wxPrinter printer(&printData);
  return printer.Print(previewFrame, printPrintout, interactive);
}

void wxPrintPreview::DetermineScaling(void)
{
#ifdef wx_msw
  if (WINDOWS_PRINTING)
  {
    HDC dc = ::GetDC(NULL);
    int screenWidth = ::GetDeviceCaps(dc, HORZSIZE);
    int screenHeight = ::GetDeviceCaps(dc, VERTSIZE);
    int screenXRes = ::GetDeviceCaps(dc, HORZRES);
    int screenYRes = ::GetDeviceCaps(dc, VERTRES);
    int logPPIScreenX = ::GetDeviceCaps(dc, LOGPIXELSX);
    int logPPIScreenY = ::GetDeviceCaps(dc, LOGPIXELSY);
    previewPrintout->SetPPIScreen(logPPIScreenX, logPPIScreenY);

    ::ReleaseDC(NULL, dc);

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC(NULL, NULL, NULL, FALSE, printData.GetOrientation());

    int printerWidth = 150;
    int printerHeight = 250;
    int printerXRes = 1500;
    int printerYRes = 2500;

    if (printerDC.cdc)
    {
      printerWidth = ::GetDeviceCaps(printerDC.cdc, HORZSIZE);
      printerHeight = ::GetDeviceCaps(printerDC.cdc, VERTSIZE);
      printerXRes = ::GetDeviceCaps(printerDC.cdc, HORZRES);
      printerYRes = ::GetDeviceCaps(printerDC.cdc, VERTRES);

      int logPPIPrinterX = ::GetDeviceCaps(printerDC.cdc, LOGPIXELSX);
      int logPPIPrinterY = ::GetDeviceCaps(printerDC.cdc, LOGPIXELSY);

      previewPrintout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);
      previewPrintout->SetPageSizeMM(printerWidth, printerHeight);
      
      if (logPPIPrinterX == 0 || logPPIPrinterY == 0 || printerWidth == 0 || printerHeight == 0)
        isOk = FALSE;
    }
    else
      isOk = FALSE;

    pageWidth = printerXRes;
    pageHeight = printerYRes;

    // At 100%, the page should look about page-size on the screen.
    previewScale = (float)((float)screenWidth/(float)printerWidth);
    previewScale = previewScale * (float)((float)screenXRes/(float)printerYRes);
  }
  else
#endif
#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
  {
    char *paperType = wxThePrintSetupData->GetPaperName();
    if (!paperType)
      paperType = "A4 210 x 297 mm";

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
    if (!paper)
      paper = wxThePrintPaperDatabase->FindPaperType("A4 210 x 297 mm");
    if (paper)
    {
      previewPrintout->SetPPIScreen(100, 100);
      previewPrintout->SetPPIPrinter(100, 100);

      // If in landscape mode, we need to swap the width and height.
      if ( printData.GetOrientation() == wxLANDSCAPE )
      {
        pageWidth = paper->heightPixels;
        pageHeight = paper->widthPixels;
        previewPrintout->SetPageSizeMM(paper->heightMM, paper->widthMM);
        previewPrintout->SetPageSizePixels(paper->heightPixels, paper->widthPixels);
      }
      else
      {
        pageWidth = paper->widthPixels;
        pageHeight = paper->heightPixels;
        previewPrintout->SetPageSizeMM(paper->widthMM, paper->heightMM);
        previewPrintout->SetPageSizePixels(paper->widthPixels, paper->heightPixels);
      }

      // At 100%, the page should look about page-size on the screen.
      previewScale = (float)0.8;
//      previewScale = (float)((float)screenWidth/(float)printerWidth);
//      previewScale = previewScale * (float)((float)screenXRes/(float)printerYRes);
    }
  }
#else
  return;
#endif
}

#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
/*
 * Generic print dialog for non-Windows printing use.
 */

void wxGenericPrintOkProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintDialog *dialog = (wxGenericPrintDialog *)but.GetParent();
  dialog->DialogToValues();

  // There are some interactions between the global setup data
  // and the standard print dialog. The global printing 'mode'
  // is determined by whether the user checks Print to file
  // or not.
  if (dialog->printData.GetPrintToFile())
  {
    wxThePrintSetupData->SetPrinterMode(PS_FILE);

    char *f = wxFileSelector("PostScript file",
        wxPathOnly(wxThePrintSetupData->GetPrinterFile()),
        wxFileNameFromPath(wxThePrintSetupData->GetPrinterFile()),
        "ps", "*.ps", 0, dialog);
    if (f)
      wxThePrintSetupData->SetPrinterFile(f);
    else
      return;
  }
  else
    wxThePrintSetupData->SetPrinterMode(PS_PRINTER);
  
  dialog->printDialogCancelled = FALSE;
  dialog->Show(FALSE);
}

void wxGenericPrintCancelProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintDialog *dialog = (wxGenericPrintDialog *)but.GetParent();
  dialog->printDialogCancelled = TRUE;
  dialog->Show(FALSE);
}

void wxGenericPrintSetupProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintDialog *dialog = (wxGenericPrintDialog *)but.GetParent();
  dialog->OnSetup();
}

void wxGenericPrintRangeProc(wxRadioBox& rbox, wxCommandEvent& event)
{
  wxGenericPrintDialog *dialog = (wxGenericPrintDialog *)rbox.GetParent();
  if (event.commandInt == 0)
  {
    dialog->fromText->Enable(FALSE);
    dialog->toText->Enable(FALSE);
  }
  else if (event.commandInt == 1)
  {
    dialog->fromText->Enable(TRUE);
    dialog->toText->Enable(TRUE);
  }
}

wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent, wxPrintData* data):
  wxDialogBox(parent, "Print", TRUE, 0, 0, 600, 600)
{
  if ( data )
    printData = *data;

  printDialogCancelled = FALSE;
  
  SetLabelPosition(wxHORIZONTAL);

  int buttonWidth = 65;
  int buttonHeight = 25;

  wxButton *okButton = new wxButton(this, (wxFunction)wxGenericPrintOkProc, "OK", -1, -1, buttonWidth, buttonHeight);
  (void) new wxButton(this, (wxFunction)wxGenericPrintCancelProc, "Cancel", -1, -1, buttonWidth, buttonHeight);

//  if (printData.GetEnableHelp())
//  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPrintHelpProc, "Help", -1, -1, buttonWidth, buttonHeight);

  setupButton = new wxButton(this, (wxFunction)wxGenericPrintSetupProc, "Setup...", -1, -1, buttonWidth, buttonHeight);

  NewLine();
  okButton->SetDefault();
  okButton->SetFocus();

  printerMessage = new wxMessage(this, "PostScript printer");
  NewLine();
  char *choices[2];
  choices[0] = "All";
  choices[1] = "Pages";
  rangeRadioBox = new wxRadioBox(this, (wxFunction)wxGenericPrintRangeProc, "Print Range",
    -1, -1, -1, -1, 2, (char **)choices, 2);
  rangeRadioBox->SetSelection(1);

  NewLine();

  fromText = new wxText(this, (wxFunction)NULL, "From:", "", -1, -1, 100, -1);
  toText = new wxText(this, (wxFunction)NULL, "To:", "", -1, -1, 100, -1);
  noCopiesText = new wxText(this, (wxFunction)NULL, "Copies:", "", -1, -1, 100, -1);
    
  NewLine();

  printToFileCheckBox = new wxCheckBox(this, (wxFunction)NULL, "Print to File");
//  collateCopiesCheckBox = new wxCheckBox(this, (wxFunction)NULL, "Collate Copies");
  
  Fit();
  Centre(wxBOTH);

  ValuesToDialog();
}

Bool wxGenericPrintDialog::printDialogCancelled = FALSE;

wxGenericPrintDialog::~wxGenericPrintDialog(void)
{
}

Bool wxGenericPrintDialog::OnClose(void)
{
  printDialogCancelled = TRUE;
  return TRUE;
}

void wxGenericPrintDialog::OnSetup(void)
{
  wxGenericPrintSetupDialog *genericPrintSetupDialog =
          new wxGenericPrintSetupDialog(this, wxThePrintSetupData);
  genericPrintSetupDialog->Show(TRUE);
  if (wxGenericPrintSetupDialog::printSetupDialogCancelled)
  {
  }
  else
  {
    *wxThePrintSetupData = genericPrintSetupDialog->printData;

    // Dialog not deleted if OK pressed, to give us the opportunity
    // to copy the printData.

    delete genericPrintSetupDialog;
  }
}

void wxGenericPrintDialog::ValuesToDialog(void)
{
  char buf[10];
  if (printData.GetEnablePageNumbers())
  {
    fromText->Enable(TRUE);
    toText->Enable(TRUE);

    sprintf(buf, "%d", printData.GetFromPage());
    fromText->SetValue(buf);
    sprintf(buf, "%d", printData.GetToPage());
    toText->SetValue(buf);
    
    if (printData.GetAllPages())
      rangeRadioBox->SetSelection(0);
    else
      rangeRadioBox->SetSelection(1);
  }
  else
  {
    fromText->Enable(FALSE);
    toText->Enable(FALSE);
    rangeRadioBox->SetSelection(0);
    rangeRadioBox->wxRadioBox::Enable(1, FALSE);
  }
  sprintf(buf, "%d", printData.GetNoCopies());
  noCopiesText->SetValue(buf);

  printToFileCheckBox->SetValue(printData.GetPrintToFile());
  printToFileCheckBox->Enable(printData.GetEnablePrintToFile());

//  collateCheckBox->SetValue(printData.GetCollate());
//  collateCheckBox->Enable(printData.GetEnableCollate());

}

void wxGenericPrintDialog::DialogToValues(void)
{
  if (printData.GetEnablePageNumbers())
  {
    printData.SetFromPage(atoi(fromText->GetValue()));
    printData.SetToPage(atoi(toText->GetValue()));
  }
  if (rangeRadioBox->GetSelection() == 0)
    printData.SetAllPages(TRUE);
  else
    printData.SetAllPages(FALSE);
  printData.SetNoCopies(atoi(noCopiesText->GetValue()));
  printData.SetPrintToFile(printToFileCheckBox->GetValue());
}

/*
 * Generic print setup dialog
 */

void wxGenericPrintSetupOkProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintSetupDialog *dialog = (wxGenericPrintSetupDialog *)but.GetParent();
  dialog->printSetupDialogCancelled = FALSE;
  dialog->DialogToValues();
  dialog->Show(FALSE);
}

void wxGenericPrintSetupCancelProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintSetupDialog *dialog = (wxGenericPrintSetupDialog *)but.GetParent();
  dialog->printSetupDialogCancelled = TRUE;
  dialog->Show(FALSE);
}

wxGenericPrintSetupDialog::wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data):
  wxDialogBox(parent, "Print Setup", TRUE, 0, 0, 600, 600)
{
  if ( data )
    printData = *data;
  printSetupDialogCancelled = FALSE;
  
  SetLabelPosition(wxHORIZONTAL);

  int buttonWidth = 65;
  int buttonHeight = 25;

  wxButton *okButton = new wxButton(this, (wxFunction)wxGenericPrintSetupOkProc, "OK", -1, -1, buttonWidth, buttonHeight);
  (void) new wxButton(this, (wxFunction)wxGenericPrintSetupCancelProc, "Cancel", -1, -1, buttonWidth, buttonHeight);

//  if (printData.GetEnableHelp())
//  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPrintSetupHelpProc, "Help", -1, -1, buttonWidth, buttonHeight);

  NewLine();
  okButton->SetDefault();
  okButton->SetFocus();

  printerMessage = new wxMessage(this, "PostScript print setup");
  NewLine();
  paperTypeChoice = CreatePaperTypeChoice();
  colourCheckBox = new wxCheckBox(this, (wxFunction)NULL, "Print in colour");

  NewLine();

  char *choices[2];
  choices[0] = "Portrait";
  choices[1] = "Landscape";
  orientationRadioBox = new wxRadioBox(this, (wxFunction)NULL, "Orientation",
    -1, -1, -1, -1, 2, (char **)choices, 2);
  orientationRadioBox->SetSelection(0);

  NewLine();

  printerCommandText = new wxText(this, (wxFunction)NULL, "Printer command:", "", -1, -1, 220, -1);
  printerOptionsText = new wxText(this, (wxFunction)NULL, "Printer options:", "", -1, -1, 220, -1);
  NewLine();

  Fit();
  Centre(wxBOTH);

  ValuesToDialog();
}

Bool wxGenericPrintSetupDialog::printSetupDialogCancelled = FALSE;

wxGenericPrintSetupDialog::~wxGenericPrintSetupDialog(void)
{
}

Bool wxGenericPrintSetupDialog::OnClose(void)
{
  printSetupDialogCancelled = TRUE;
  return TRUE;
}

void wxGenericPrintSetupDialog::ValuesToDialog(void)
{
  if (printerCommandText && printData.GetPrinterCommand())
    printerCommandText->SetValue(printData.GetPrinterCommand());
  if (printerOptionsText && printData.GetPrinterOptions())
    printerOptionsText->SetValue(printData.GetPrinterOptions());
  if (colourCheckBox)
    colourCheckBox->SetValue(printData.GetColour());
  if (orientationRadioBox)
  {
    if (printData.GetPrinterOrientation() == PS_PORTRAIT)
      orientationRadioBox->SetSelection(0);
    else
      orientationRadioBox->SetSelection(1);
  }
}

void wxGenericPrintSetupDialog::DialogToValues(void)
{
  if (printerCommandText)
    printData.SetPrinterCommand(printerCommandText->GetValue());
  if (printerOptionsText)
    printData.SetPrinterOptions(printerOptionsText->GetValue());
  if (colourCheckBox)
    printData.SetColour(colourCheckBox->GetValue());
  if (orientationRadioBox)
  {
    int sel = orientationRadioBox->GetSelection();
    if (sel == 0)
      printData.SetPrinterOrientation(PS_PORTRAIT);
    else
      printData.SetPrinterOrientation(PS_LANDSCAPE);
  }
  if (paperTypeChoice)
  {
    char *val = paperTypeChoice->GetStringSelection();
    if (val && *val)
      printData.SetPaperName(val);
  }
}

wxChoice *wxGenericPrintSetupDialog::CreatePaperTypeChoice(void)
{
  if (!wxThePrintPaperDatabase)
  {
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();
  }
  int n = wxThePrintPaperDatabase->Number();
  char **choices = new char *[n];
  int sel = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
    choices[i] = (char *)paper->pageName;
    if (printData.GetPaperName() && strcmp(choices[i], printData.GetPaperName()) == 0)
      sel = i;
  }
  wxChoice *choice = new wxChoice(this, (wxFunction)NULL, "Paper type", -1, -1, 300, -1, n,
    choices);
  delete[] choices;

  choice->SetSelection(sel);
  return choice;
}

/*
 * Generic page setup dialog
 */

void wxGenericPageSetupOkProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPageSetupDialog *dialog = (wxGenericPageSetupDialog *)but.GetParent();
  dialog->pageSetupDialogCancelled = FALSE;
  dialog->DialogToValues();
  dialog->Show(FALSE);
}

void wxGenericPageSetupCancelProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPageSetupDialog *dialog = (wxGenericPageSetupDialog *)but.GetParent();
  dialog->pageSetupDialogCancelled = TRUE;
  dialog->Show(FALSE);
}

void wxGenericPageSetupPrinterProc(wxButton& but, wxCommandEvent& WXUNUSED(event))
{
  wxGenericPageSetupDialog *dialog = (wxGenericPageSetupDialog *)but.GetParent();

  if (wxTheApp->GetPrintMode() == wxPRINT_POSTSCRIPT)
  {
    wxGenericPrintSetupDialog *genericPrintSetupDialog =
          new wxGenericPrintSetupDialog(dialog, wxThePrintSetupData);
    genericPrintSetupDialog->Show(TRUE);
    if (wxGenericPrintSetupDialog::printSetupDialogCancelled)
    {
      genericPrintSetupDialog->Close(TRUE);
    }
    else
    {
      *wxThePrintSetupData = genericPrintSetupDialog->GetPrintData();

      genericPrintSetupDialog->Close(TRUE);
    }
  }
#ifdef wx_msw
  else
  {
    wxPrintData data;
    data.SetSetupDialog(TRUE);
    wxPrintDialog printDialog(dialog, & data);
    printDialog.Show(TRUE);
  }
#endif
}

wxGenericPageSetupDialog::wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data):
  wxDialogBox(parent, "Page Setup", TRUE, 0, 0, 600, 600)
{
  if ( data )
    pageData = *data;
  pageSetupDialogCancelled = FALSE;
  
  SetLabelPosition(wxHORIZONTAL);

  int buttonWidth = 75;
  int buttonHeight = 25;

  wxButton *okButton = new wxButton(this, (wxFunction)wxGenericPageSetupOkProc, "OK", -1, -1, buttonWidth, buttonHeight);
  (void) new wxButton(this, (wxFunction)wxGenericPageSetupCancelProc, "Cancel", -1, -1, buttonWidth, buttonHeight);
  printerButton = new wxButton(this, (wxFunction)wxGenericPageSetupPrinterProc, "Printer...", -1, -1, buttonWidth, buttonHeight);

  if ( !pageData.GetEnablePrinter() )
    printerButton->Enable(FALSE);

//  if (printData.GetEnableHelp())
//  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPageSetupHelpProc, "Help", -1, -1, buttonWidth, buttonHeight);

  NewLine();
  okButton->SetDefault();
  okButton->SetFocus();

//  printerMessage = new wxMessage(this, "PostScript page setup");
  NewLine();
  NewLine();
  paperTypeChoice = CreatePaperTypeChoice();

  NewLine();

  char *choices[2];
  choices[0] = "Portrait";
  choices[1] = "Landscape";
  orientationRadioBox = new wxRadioBox(this, (wxFunction)NULL, "Orientation",
    -1, -1, -1, -1, 2, (char **)choices, 2);
  orientationRadioBox->SetSelection(0);

  NewLine();

  marginLeftText = new wxText(this, (wxFunction)NULL, "Left margin (mm):", "", -1, -1, 150, -1);
  marginRightText = new wxText(this, (wxFunction)NULL, "Right margin (mm):", "", -1, -1, 150, -1);
  NewLine();
  marginTopText = new wxText(this, (wxFunction)NULL, "Top margin (mm):", "", -1, -1, 150, -1);
  marginBottomText = new wxText(this, (wxFunction)NULL, "Bottom margin (mm):", "", -1, -1, 150, -1);
  NewLine();

  Fit();
  Centre(wxBOTH);

  ValuesToDialog();
}

Bool wxGenericPageSetupDialog::pageSetupDialogCancelled = FALSE;

wxGenericPageSetupDialog::~wxGenericPageSetupDialog(void)
{
}

Bool wxGenericPageSetupDialog::OnClose(void)
{
  pageSetupDialogCancelled = TRUE;
  return TRUE;
}

void wxGenericPageSetupDialog::ValuesToDialog(void)
{
  if (marginLeftText)
    marginLeftText->SetValue(IntToString((int) pageData.GetMarginTopLeft().x));
  if (marginTopText)
    marginTopText->SetValue(IntToString((int) pageData.GetMarginTopLeft().y));
  if (marginRightText)
    marginRightText->SetValue(IntToString((int) pageData.GetMarginBottomRight().x));
  if (marginBottomText)
    marginBottomText->SetValue(IntToString((int) pageData.GetMarginBottomRight().y));

  if (orientationRadioBox)
  {
    if (pageData.GetOrientation() == wxPORTRAIT)
      orientationRadioBox->SetSelection(0);
    else
      orientationRadioBox->SetSelection(1);
  }
}

void wxGenericPageSetupDialog::DialogToValues(void)
{
  if (marginLeftText && marginTopText)
    pageData.SetMarginTopLeft(wxPoint(atoi(marginLeftText->GetValue()),atoi(marginTopText->GetValue())));
  if (marginRightText && marginBottomText)
    pageData.SetMarginBottomRight(wxPoint(atoi(marginRightText->GetValue()),atoi(marginBottomText->GetValue())));

  if (orientationRadioBox)
  {
    int sel = orientationRadioBox->GetSelection();
    if (sel == 0)
    {
      wxThePrintSetupData->SetPrinterOrientation(wxPORTRAIT);
      pageData.SetOrientation(wxPORTRAIT);
    }
    else
    {
      wxThePrintSetupData->SetPrinterOrientation(wxLANDSCAPE);
      pageData.SetOrientation(wxLANDSCAPE);
    }
  }
  if (paperTypeChoice)
  {
    char *val = paperTypeChoice->GetStringSelection();
    if (val && *val)
    {
        wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(val);
        if ( paper )
        {
            pageData.SetPaperSize(wxPoint(paper->widthMM, paper->heightMM));
        }
    }
  }
}

wxChoice *wxGenericPageSetupDialog::CreatePaperTypeChoice(void)
{
  if (!wxThePrintPaperDatabase)
  {
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();
  }
  int n = wxThePrintPaperDatabase->Number();
  char **choices = new char *[n];
  int sel = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
    choices[i] = (char *)paper->pageName;
    if (pageData.GetPaperSize().x == paper->widthMM && pageData.GetPaperSize().y == paper->heightMM)
      sel = i;
  }
  wxChoice *choice = new wxChoice(this, (wxFunction)NULL, "Paper size", -1, -1, 300, -1, n,
    choices);
  delete[] choices;

  choice->SetSelection(sel);
  return choice;
}

#endif
  // PostScript print/preview in MSW

#endif
  // USE_COMMON_DIALOGS
#endif
  // End USE_PRINTING_ARCHITECTURE
