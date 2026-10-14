/*
 * File:	wx_print.h
 * Purpose:	Printing-related classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_printh
#define wx_printh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_canvs.h"
#include "wx_dialg.h"
#include "wx_panel.h"
#include "wx_panel.h"
#include "wx_buttn.h"
#include "wx_messg.h"
#include "wx_choic.h"
#include "wx_rbox.h"
#include "wx_check.h"
#include "wx_txt.h"
#include "wx_lay.h"
#include "wx_dcmem.h"
#include "wx_dcps.h"

#ifdef IN_CPROTO
typedef       void    *wxPrintDialog ;
typedef       void    *wxPrinter ;
typedef       void    *wxPrintout ;
typedef       void    *wxPreviewCanvas ;
typedef       void    *wxPreviewControlBar ;
typedef       void    *wxPreviewFrame ;
typedef       void    *wxPrintPreview ;
#else

class wxDC;
class wxPrintout;
class wxPrinter;
class wxPrintDialog;
class wxPrintPreview;
class wxPreviewCanvas;
class wxPreviewControlBar;
class wxPreviewFrame;

/*
 * wxPrintData
 * Encapsulates information displayed and edited in the printer dialog box.
 */

#ifndef wxPORTRAIT
#define wxPORTRAIT      1
#define wxLANDSCAPE     2
#endif
 
class WXDLLEXPORT wxPrintData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintData)

 public:
#ifdef wx_msw
  void *printData;
#endif
  int printFromPage;
  int printToPage;
  int printMinPage;
  int printMaxPage;
  int printNoCopies;
  int printOrientation;
  Bool printAllPages;
  Bool printCollate;
  Bool printToFile;
  Bool printEnableSelection;
  Bool printEnablePageNumbers;
  Bool printEnableHelp;
  Bool printEnablePrintToFile;
  Bool printSetupDialog;

  wxPrintData(void);
  ~wxPrintData(void);

  inline int GetFromPage(void) { return printFromPage; };
  inline int GetToPage(void) { return printToPage; };
  inline int GetMinPage(void) { return printMinPage; };
  inline int GetMaxPage(void) { return printMaxPage; };
  inline int GetNoCopies(void) { return printNoCopies; };
  inline Bool GetAllPages(void) { return printAllPages; };
  inline Bool GetCollate(void) { return printCollate; };
  inline Bool GetPrintToFile(void) { return printToFile; };
  inline Bool GetSetupDialog(void) { return printSetupDialog; };
  inline int  GetOrientation(void) { return printOrientation; };

  inline void SetFromPage(int v) { printFromPage = v; };
  inline void SetToPage(int v) { printToPage = v; };
  inline void SetMinPage(int v) { printMinPage = v; };
  inline void SetMaxPage(int v) { printMaxPage = v; };
  inline void SetNoCopies(int v) { printNoCopies = v; };
  inline void SetAllPages(Bool flag) { printAllPages = flag; };
  inline void SetCollate(Bool flag) { printCollate = flag; };
  inline void SetPrintToFile(Bool flag) { printToFile = flag; };
  inline void SetSetupDialog(Bool flag) { printSetupDialog = flag; };
  inline void SetOrientation(int orient) { printOrientation = orient; };

  inline void EnablePrintToFile(Bool flag) { printEnablePrintToFile = flag; };
  inline void EnableSelection(Bool flag) { printEnableSelection = flag; };
  inline void EnablePageNumbers(Bool flag) { printEnablePageNumbers = flag; };
  inline void EnableHelp(Bool flag) { printEnableHelp = flag; };

  inline Bool GetEnablePrintToFile(void) { return printEnablePrintToFile; };
  inline Bool GetEnableSelection(void) { return printEnableSelection; };
  inline Bool GetEnablePageNumbers(void) { return printEnablePageNumbers; };
  inline Bool GetEnableHelp(void) { return printEnableHelp; };

  void operator=(const wxPrintData& data);

#ifdef wx_msw
  // Convert to/from the PRINTDLG structure
  void ConvertToNative(void);
  void ConvertFromNative(void);
  void SetOwnerWindow(wxWindow* win);
  inline void* GetNativeData(void) { return printData; }
#endif
};

/*
 * This is the data used (and returned) by the wxPageSetupDialog.
 */

class WXDLLEXPORT wxPageSetupData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupData)

 public:
#if WIN95
  void*     m_pageSetupData;
#endif
  wxPoint   m_paperSize;
  wxPoint   m_minMarginTopLeft;
  wxPoint   m_minMarginBottomRight;
  wxPoint   m_marginTopLeft;
  wxPoint   m_marginBottomRight;
  int       m_orientation;

  // Flags
  Bool      m_defaultMinMargins;
  Bool      m_enableMargins;
  Bool      m_enableOrientation;
  Bool      m_enablePaper;
  Bool      m_enablePrinter;
  Bool      m_getDefaultInfo; // Equiv. to PSD_RETURNDEFAULT
  Bool      m_enableHelp;

  wxPageSetupData(void);
  ~wxPageSetupData(void);

  inline wxPoint GetPaperSize(void) { return m_paperSize; };
  inline wxPoint GetMinMarginTopLeft(void) { return m_minMarginTopLeft; };
  inline wxPoint GetMinMarginBottomRight(void) { return m_minMarginBottomRight; };
  inline wxPoint GetMarginTopLeft(void) { return m_marginTopLeft; };
  inline wxPoint GetMarginBottomRight(void) { return m_marginBottomRight; };
  inline int GetOrientation(void) { return m_orientation; };

  inline Bool GetDefaultMinMargins(void) { return m_defaultMinMargins; };
  inline Bool GetEnableMargins(void) { return m_enableMargins; };
  inline Bool GetEnableOrientation(void) { return m_enableOrientation; };
  inline Bool GetEnablePaper(void) { return m_enablePaper; };
  inline Bool GetEnablePrinter(void) { return m_enablePrinter; };
  inline Bool GetDefaultInfo(void) { return m_getDefaultInfo; };
  inline Bool GetEnableHelp(void) { return m_enableHelp; };

  inline void SetPaperSize(const wxPoint& pt) { m_paperSize = pt; };
  inline void SetMinMarginTopLeft(const wxPoint& pt) { m_minMarginTopLeft = pt; };
  inline void SetMinMarginBottomRight(const wxPoint& pt) { m_minMarginBottomRight = pt; };
  inline void SetMarginTopLeft(const wxPoint& pt) { m_marginTopLeft = pt; };
  inline void SetMarginBottomRight(const wxPoint& pt) { m_marginBottomRight = pt; };
  inline void SetOrientation(int orient) { m_orientation = orient; };
  inline void SetDefaultMinMargins(Bool flag) { m_defaultMinMargins = flag; };
  inline void SetDefaultInfo(Bool flag) { m_getDefaultInfo = flag; };

  inline void EnableMargins(Bool flag) { m_enableMargins = flag; };
  inline void EnableOrientation(Bool flag) { m_enableOrientation = flag; };
  inline void EnablePaper(Bool flag) { m_enablePaper = flag; };
  inline void EnablePrinter(Bool flag) { m_enablePrinter = flag; };
  inline void EnableHelp(Bool flag) { m_enableHelp = flag; };

#if WIN95
  // Convert to/from the PAGESETUPDLG structure
  void ConvertToNative(void);
  void ConvertFromNative(void);
  void SetOwnerWindow(wxWindow* win);
  inline void* GetNativeData(void) { return m_pageSetupData; }
#endif

  void operator=(const wxPageSetupData& data);
};

/*
 * wxPrinterDialog
 * The common dialog for printing.
 */
 
class WXDLLEXPORT wxPrintDialog: public wxDialogBox
{
  DECLARE_DYNAMIC_CLASS(wxPrintDialog)

 private:
  wxPrintData printData;
  wxDC *printerDC;
  Bool destroyDC;
  char *deviceName;
  char *driverName;
  char *portName;
  wxWindow *dialogParent;
 public:
  wxPrintDialog(void);
  wxPrintDialog(wxWindow *parent, wxPrintData *data = NULL);
  ~wxPrintDialog(void);

  Bool Create(wxWindow *parent, wxPrintData *data = NULL);
  virtual Bool Show(Bool flag);

  inline wxPrintData& GetPrintData(void) { return printData; }
  virtual wxDC *GetPrintDC(void);
};

/*
 * wxPageSetupDialog
 * The common dialog for page setup.
 */
 
class WXDLLEXPORT wxPageSetupDialog: public wxDialogBox
{
  DECLARE_DYNAMIC_CLASS(wxPageSetupDialog)

 private:
  wxPageSetupData   m_pageSetupData;
  wxWindow*         m_dialogParent;
 public:
  wxPageSetupDialog(void);
  wxPageSetupDialog(wxWindow *parent, wxPageSetupData *data = NULL);
  ~wxPageSetupDialog(void);

  Bool Create(wxWindow *parent, wxPageSetupData *data = NULL);
  virtual Bool Show(Bool flag);

  inline wxPageSetupData& GetPageSetupData(void) { return m_pageSetupData; }
};

/*
 * Represents the printer: manages printing a wxPrintout object
 */
 
class WXDLLEXPORT wxPrinter: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrinter)

 private:
  wxPrintData printData;
  wxPrintout *currentPrintout;
#ifdef wx_msw
  FARPROC lpAbortProc;
#endif
 public:
  static wxWindow *abortWindow;
  static Bool abortIt;

  wxPrinter(wxPrintData *data = NULL);
  ~wxPrinter(void);

  virtual Bool Print(wxWindow *parent, wxPrintout *printout, Bool prompt = TRUE);
  virtual Bool PrintDialog(wxWindow *parent);
  virtual wxWindow *CreateAbortWindow(wxWindow *parent, wxPrintout *printout);
  virtual Bool Setup(wxWindow *parent);
  virtual void ReportError(wxWindow *parent, wxPrintout *printout, char *message);
  virtual wxPrintData &GetPrintData(void);
  virtual inline Bool Abort(void) { return abortIt; }
};

/*
 * wxPrintout
 * Represents an object via which a document may be printed.
 * The programmer derives from this, overrides (at least) OnPrintPage,
 * and passes it to a wxPrinter object for printing, or a wxPrintPreview
 * object for previewing.
 */
 
class WXDLLEXPORT wxPrintout: public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxPrintout)

 private:
   char *printoutTitle;
   wxDC *printoutDC;

   int pageWidthPixels;
   int pageHeightPixels;

   int pageWidthMM;
   int pageHeightMM;

   int PPIScreenX;
   int PPIScreenY;
   int PPIPrinterX;
   int PPIPrinterY;

   Bool isPreview;
 public:
  wxPrintout(char *title = "Printout");
  ~wxPrintout(void);

  virtual Bool OnBeginDocument(int startPage, int endPage);
  virtual void OnEndDocument(void);
  virtual void OnBeginPrinting(void);
  virtual void OnEndPrinting(void);

  // Guaranteed to be before any other functions are called
  inline virtual void OnPreparePrinting(void) { }

  virtual Bool HasPage(int page);
  virtual Bool OnPrintPage(int page) = 0;
  virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

  inline virtual char *GetTitle(void) { return printoutTitle; }

  inline wxDC *GetDC(void) { return printoutDC; }
  inline void SetDC(wxDC *dc) { printoutDC = dc; }
  inline void SetPageSizePixels(int w, int  h) { pageWidthPixels = w; pageHeightPixels = h; }
  inline void GetPageSizePixels(int *w, int  *h) { *w = pageWidthPixels; *h = pageHeightPixels; }
  inline void SetPageSizeMM(int w, int  h) { pageWidthMM = w; pageHeightMM = h; }
  inline void GetPageSizeMM(int *w, int  *h) { *w = pageWidthMM; *h = pageHeightMM; }

  inline void SetPPIScreen(int x, int y) { PPIScreenX = x; PPIScreenY = y; }
  inline void GetPPIScreen(int *x, int *y) { *x = PPIScreenX; *y = PPIScreenY; }
  inline void SetPPIPrinter(int x, int y) { PPIPrinterX = x; PPIPrinterY = y; }
  inline void GetPPIPrinter(int *x, int *y) { *x = PPIPrinterX; *y = PPIPrinterY; }

  inline virtual Bool IsPreview(void) { return isPreview; }

  inline virtual void SetIsPreview(Bool p) { isPreview = p; }
};

/*
 * wxPreviewCanvas
 * Canvas upon which a preview is drawn.
 */
 
class WXDLLEXPORT wxPreviewCanvas: public wxCanvas
{
  DECLARE_CLASS(wxPreviewCanvas)

 private:
  wxPrintPreview *printPreview;
  static wxBrush *previewBackgroundBrush;
 public:
  wxPreviewCanvas(wxPrintPreview *preview, wxWindow *parent, int x = -1, int y = -1, int w = -1, int h = -1,
    long style = 0, char *name = "canvas");
  ~wxPreviewCanvas(void);

  void OnPaint(void);
  void OnEvent(wxMouseEvent& event);
  void OnChar(wxKeyEvent& event);
  void OnSize(int w, int h);
  void OnScroll(wxCommandEvent& event);

  inline wxPrintPreview* GetPrintPreview(void) { return printPreview; }
};

#ifdef wx_msw
#define USE_BITMAP_PREVIEW 0
#else
#define USE_BITMAP_PREVIEW 1
#endif

#if !USE_BITMAP_PREVIEW
// This is a canvas which displays the zoomed page within a wxPreviewCanvas,
// in Windows environments.
// In non-Windows, a bitmap/wxMemoryDC is used to display the page, since canvas nesting is not
// allowed.
class WXDLLEXPORT wxPreviewPageCanvas: public wxCanvas
{
  DECLARE_CLASS(wxPreviewPageCanvas)

 private:
 public:
  wxPreviewPageCanvas(wxWindow *parent, int x = -1, int y = -1, int w = -1, int h = -1,
    long style = 0, char *name = "canvas");
  ~wxPreviewPageCanvas(void);

  void OnPaint(void);
  void OnEvent(wxMouseEvent& event);
  void OnChar(wxKeyEvent& event);
};
#endif

/*
 * wxPreviewFrame
 * Default frame for showing preview.
 */

class WXDLLEXPORT wxPreviewFrame: public wxFrame
{
  DECLARE_CLASS(wxPreviewFrame)

 protected:
  wxCanvas *previewCanvas;
#if !USE_BITMAP_PREVIEW
  wxCanvas *pagePreviewCanvas;
#endif

  wxPreviewControlBar *controlBar;
  wxPrintPreview *printPreview;
 public:
  wxPreviewFrame(wxPrintPreview *preview, wxFrame *parent, char *title = "Print Preview",
    int x = -1, int y = -1, int w = -1, int h = -1,
    long style = wxDEFAULT_FRAME|wxSDI, char *name = "frame");
  ~wxPreviewFrame(void);

  Bool OnClose(void);
  virtual void Initialize(void);
  virtual void CreateCanvas(void);
  virtual void CreateControlBar(void);
};

/*
 * wxPreviewControlBar
 * A panel with buttons for controlling a print preview.
 * The programmer may wish to use other means for controlling
 * the print preview.
 */

#define wxPREVIEW_PRINT        1
#define wxPREVIEW_PREVIOUS     2
#define wxPREVIEW_NEXT         4
#define wxPREVIEW_ZOOM         8

#define wxPREVIEW_DEFAULT      wxPREVIEW_PREVIOUS|wxPREVIEW_NEXT|wxPREVIEW_ZOOM

class WXDLLEXPORT wxPreviewControlBar: public wxPanel
{
  DECLARE_CLASS(wxPreviewControlBar)

 protected:
  wxPrintPreview *printPreview;
  wxButton *closeButton;
  wxButton *nextPageButton;
  wxButton *previousPageButton;
  wxButton *printButton;
  wxChoice *zoomControl;
  static wxFont *buttonFont;
  long buttonFlags;
 public:
  wxPreviewControlBar(wxPrintPreview *preview, long buttons,
    wxWindow *parent, int x = -1, int y = -1, int w = -1, int h = -1,
    long style = 0, char *name = "panel");
  ~wxPreviewControlBar(void);

  virtual void CreateButtons(void);
  virtual void SetZoomControl(int zoom);
  virtual int GetZoomControl(void);
  inline virtual wxPrintPreview *GetPrintPreview(void) { return printPreview; }
  void OnPaint(void);
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */
 
class WXDLLEXPORT wxPrintPreview: public wxObject
{
  DECLARE_CLASS(wxPrintPreview)

 private:
  wxPrintData printData;
  wxCanvas *previewCanvas;
#if !USE_BITMAP_PREVIEW
  wxCanvas *previewPageCanvas;
#endif
  wxFrame *previewFrame;
  wxBitmap *previewBitmap;
  wxPrintout *previewPrintout;
  wxPrintout *printPrintout;
  int currentPage;
  int currentZoom;
  float previewScale;
  int topMargin;
  int leftMargin;
  int pageWidth;
  int pageHeight;
/*
  int minPage;
  int maxPage;
*/
 protected:
  Bool isOk;
 public:
  wxPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxPrintPreview(void);

  virtual Bool SetCurrentPage(int pageNum);
  virtual inline int GetCurrentPage(void) { return currentPage; };

  virtual void SetPrintout(wxPrintout *printout);
  virtual inline wxPrintout *GetPrintout(void) { return previewPrintout; };
  virtual inline wxPrintout *GetPrintoutForPrinting(void) { return printPrintout; };

  virtual void SetFrame(wxFrame *frame);
  virtual void SetCanvas(wxCanvas *canvas);
#if !USE_BITMAP_PREVIEW
  inline void SetPageCanvas(wxCanvas* canvas) { previewPageCanvas = canvas; }
#endif

  inline virtual wxFrame *GetFrame(void) { return previewFrame; }
  inline virtual wxCanvas *GetCanvas(void) { return previewCanvas; }
#if !USE_BITMAP_PREVIEW
  inline virtual wxCanvas *GetPageCanvas(void) { return previewPageCanvas; }
#endif

  // The preview canvas should call this from OnPaint
  virtual Bool PaintPage(wxCanvas *canvas);

  // This draws a blank page onto the preview canvas
  virtual Bool DrawBlankPage(wxCanvas *canvas);

  // This is called by wxPrintPreview to render a page into
  // a wxMemoryDC.
  virtual Bool RenderPage(int pageNum);

  virtual wxPrintData &GetPrintData(void);
  virtual void SetZoom(int percent);
  virtual int GetZoom(void);

  // If we own a wxPrintout that can be used for printing, this
  // will invoke the actual printing procedure. Called
  // by the wxPreviewControlBar.
  virtual Bool Print(Bool interactive);

  virtual int GetMaxPage(void) { return printData.GetMaxPage(); }
  virtual int GetMinPage(void) { return printData.GetMinPage(); }

  inline  int GetPageWidth(void) { return pageWidth; }
  inline  int GetPageHeight(void) { return pageHeight; }
  inline  int GetTopMargin(void) { return topMargin; }
  inline  int GetLeftMargin(void) { return leftMargin; }

  inline  float GetPreviewScale(void) { return previewScale; }
  
  Bool Ok(void) { return isOk; }
  void SetOk(Bool ok) { isOk = ok; }

 protected:
 
  // Calculate scaling that needs to be done to get roughly
  // the right scaling for the screen pretending to be
  // the currently selected printer.
  virtual void DetermineScaling(void);
};

#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

/*
 * Simulated Print and Print Setup dialogs
 * for non-Windows platforms (and Windows using PostScript print/preview)
 */

class WXDLLEXPORT wxGenericPrintDialog: public wxDialogBox
{
  DECLARE_DYNAMIC_CLASS(wxGenericPrintDialog)

 public:
  wxMessage  *printerMessage;
  wxButton   *setupButton;
  wxButton   *helpButton;
  wxRadioBox *rangeRadioBox;
  wxText     *fromText;
  wxText     *toText;
  wxText     *noCopiesText;
  wxCheckBox *printToFileCheckBox;
  wxCheckBox *collateCopiesCheckBox;

  static Bool printDialogCancelled;
  
  wxPrintData printData;
  wxGenericPrintDialog(wxWindow *parent, wxPrintData *data = NULL);
  ~wxGenericPrintDialog(void);

  Bool OnClose(void);
  void ValuesToDialog(void);
  void DialogToValues(void);
  void OnSetup(void); // Setup button pressed 
  virtual wxPrintData& GetPrintData(void) { return printData; }
};

class WXDLLEXPORT wxGenericPrintSetupDialog: public wxDialogBox
{
  DECLARE_CLASS(wxGenericPrintSetupDialog)

 public:
  wxMessage  *printerMessage;
//  wxButton   *helpButton;
  wxRadioBox *orientationRadioBox;
  wxText     *printerCommandText;
  wxText     *printerOptionsText;
  wxCheckBox *colourCheckBox;
  wxChoice   *paperTypeChoice;

  static Bool printSetupDialogCancelled;
  
  wxPrintSetupData printData;
  wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data = NULL);
  ~wxGenericPrintSetupDialog(void);

  Bool OnClose(void);
  void ValuesToDialog(void);
  void DialogToValues(void);
  wxChoice *CreatePaperTypeChoice(void);
  inline wxPrintSetupData& GetPrintData(void) { return printData; }
};

class WXDLLEXPORT wxGenericPageSetupDialog: public wxDialogBox
{
  DECLARE_CLASS(wxGenericPageSetupDialog)

 public:
//  wxMessage  *printerMessage;
  wxButton   *printerButton;
  wxRadioBox *orientationRadioBox;
  wxText     *marginLeftText;
  wxText     *marginTopText;
  wxText     *marginRightText;
  wxText     *marginBottomText;
  wxChoice   *paperTypeChoice;

  static Bool pageSetupDialogCancelled;
  
  wxPageSetupData pageData;

  wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data = NULL);
  ~wxGenericPageSetupDialog(void);

  Bool OnClose(void);
  void ValuesToDialog(void);
  void DialogToValues(void);
  wxChoice *CreatePaperTypeChoice(void);
  inline wxPageSetupData& GetPageSetupData(void) { return pageData; }
};

#endif
  // USE_POSTSCRIPT_ARCHITECTURE_IN_MSW
#endif // IN_CPROTO
#endif // wx_printh
