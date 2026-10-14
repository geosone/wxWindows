/*
 * File:	wb_gdi.h
 * Purpose:	Declaration of various graphics objects - fonts, pens, icons etc.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_gdi.h	1.2 5/9/94" */

#ifndef wxb_gdih
#define wxb_gdih

#include "wx_obj.h"
#include "wx_list.h"
#include "wx_hash.h"
#include "wx_setup.h"

#ifdef __GNUG__
#pragma interface
#endif

// Standard cursors
typedef enum {
 wxCURSOR_ARROW =  1,
 wxCURSOR_BULLSEYE,
 wxCURSOR_CHAR,
 wxCURSOR_CROSS,
 wxCURSOR_HAND,
 wxCURSOR_IBEAM,
 wxCURSOR_LEFT_BUTTON,
 wxCURSOR_MAGNIFIER,
 wxCURSOR_MIDDLE_BUTTON,
 wxCURSOR_NO_ENTRY,
 wxCURSOR_PAINT_BRUSH,
 wxCURSOR_PENCIL,
 wxCURSOR_POINT_LEFT,
 wxCURSOR_POINT_RIGHT,
 wxCURSOR_QUESTION_ARROW,
 wxCURSOR_RIGHT_BUTTON,
 wxCURSOR_SIZENESW,
 wxCURSOR_SIZENS,
 wxCURSOR_SIZENWSE,
 wxCURSOR_SIZEWE,
 wxCURSOR_SIZING,
 wxCURSOR_SPRAYCAN,
 wxCURSOR_WAIT,
 wxCURSOR_WATCH,
 wxCURSOR_BLANK
#ifdef wx_x
  /* Not yet implemented for Windows */
  , wxCURSOR_CROSS_REVERSE,
  wxCURSOR_DOUBLE_ARROW,
  wxCURSOR_BASED_ARROW_UP,
  wxCURSOR_BASED_ARROW_DOWN
#endif
} _standard_cursors_t;

#ifdef IN_CPROTO
typedef       void *wxbFont;
typedef       void *wxColour;
typedef       void *wxPoint;
typedef       void *wxbPen;
typedef       void *wxbBrush;
typedef       void *wxPenList;
typedef       void *wxBrushList;
typedef       void *wxColourDatabase;
typedef       void *wxGDIList;
typedef       void *wxDash ;
#else

#ifdef wx_msw
typedef    DWORD  wxDash ;
#endif

#ifdef wx_x
typedef    char wxDash ;
#endif

/*
 * Part of the new GDI resource caching scheme
 */

class WXDLLEXPORT wxGDIObject: public wxObject
{
 public:
  int usageCount;
  inline wxGDIObject(void) { usageCount = 0; };
  inline ~wxGDIObject(void) {};
#ifdef wx_msw
  void IncrementResourceUsage(void);
  void DecrementResourceUsage(void);
  inline int GetResourceUsage(void) { return usageCount; };

  // Creates the resource
  virtual Bool RealizeResource(void) { return FALSE; };

  // Frees the resource
  virtual Bool FreeResource(void) { return FALSE; };

  // Uses the resource (increments count and Realizes if necessary)
  virtual Bool UseResource(void) { return FALSE; }

  // Un-uses the resource (decrements count but doesn't free just yet)
  virtual Bool ReleaseResource(void) { return FALSE; }

  virtual Bool IsFree(void) { return FALSE; };

  // Returns handle.
  virtual HANDLE GetResourceHandle(void) { return 0; }
#endif
};

// Font
class wxFont;
class WXDLLEXPORT wxbFont: public wxGDIObject
{
 protected:
  Bool temporary;   // If TRUE, the pointer to the actual font
                    // is temporary and SHOULD NOT BE DELETED by
                    // destructor
  int point_size;
  int family;
  int fontid; /* MATTHEW: New font system */
  int style;
  int weight;
  Bool underlined;
  char *faceName;
 public:
  wxbFont(void);
  wxbFont(int PointSize, int Family, int Style, int Weight, Bool underline = FALSE, const char *face = NULL);
  ~wxbFont(void);

  inline int GetPointSize(void) { return point_size; }
  inline int GetFamily(void) { return family; }
  inline int GetFontId(void) { return fontid; } /* New font system */
  inline int GetStyle(void) { return style; }
  inline int GetWeight(void) { return weight; }
  virtual char *GetFamilyString(void);
  virtual char *GetFaceName(void);
  virtual char *GetStyleString(void);
  virtual char *GetWeightString(void);
  virtual inline Bool GetUnderlined(void) { return underlined; }
};

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)

/* MATTHEW: wxFontNameDirectory and wxTheFontNameDirectory */

class WXDLLEXPORT wxFontNameDirectory : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxFontNameDirectory)
  wxHashTable *table;
  int nextFontId;
 public:
  wxFontNameDirectory(void);
  ~wxFontNameDirectory();
  void Initialize(void);
  void CleanUp(void);
  char *GetScreenName(int fontid, int weight, int style);
  char *GetPostScriptName(int fontid, int weight, int style);
  char *GetAFMName(int fontid, int weight, int style);

  void Initialize(int fontid, int family, const char *name);/* MATTHEW: [4] New font system */
  int GetNewFontId(void);
  
  int FindOrCreateFontId(const char *name, int family); /* MATTHEW: [4] New font system */

  int GetFontId(const char *name);/* MATTHEW: [4] const */
  char *GetFontName(int fontid);
  int GetFamily(int fontid); /* MATTHEW: [4] New font system */
};

extern wxFontNameDirectory WXDLLEXPORT_DATA wxTheFontNameDirectory;
#endif

// Colour
class WXDLLEXPORT wxColour: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxColour)
 private:
  Bool isInit;
  unsigned char red;
  unsigned char blue;
  unsigned char green;
 public:
#ifdef wx_x
  int pixel;
#endif
#ifdef wx_msw
  COLORREF pixel ;
#endif

  wxColour(void);
  wxColour(const unsigned char r, const unsigned char g, const unsigned char b);
  wxColour(const wxColour& col);
  wxColour(const char *col);
  ~wxColour(void) ;
  wxColour& operator =(const wxColour& src) ;
  wxColour& operator =(const char *src) ;
  inline int Ok(void)
#ifdef wx_x
//    { return (isInit && (pixel != -1)) ; } // Can't be right -- pixel can't
                                             // always be initialized!
    { return (isInit) ; }
#else
    { return (isInit) ; }
#endif

  void Set(unsigned char r, unsigned char g, unsigned char b);
  void Get(unsigned char *r, unsigned char *g, unsigned char *b);

  inline unsigned char Red(void) { return red; }
  inline unsigned char Green(void) { return green; }
  inline unsigned char Blue(void) { return blue; }
};

#define wxColor wxColour

class wxColourMap;


// Point
#if (!USE_TYPEDEFS)
class WXDLLEXPORT wxPoint: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPoint)
 public:
  float x;
  float y;
  wxPoint(void);
  wxPoint(float the_x, float the_y);
  ~wxPoint(void);
};
#else
typedef struct {
                float x ;
                float y ;
               } wxPoint ;
#endif

#if (!USE_TYPEDEFS)
class WXDLLEXPORT wxIntPoint: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxIntPoint)
 public:
  int x;
  int y;
  wxIntPoint(void);
  wxIntPoint(int the_x, int the_y);
  ~wxIntPoint(void);
};
#else
typedef struct {
                int x ;
                int y ;
               } wxIntPoint ;
#endif


// Pen
class wxPen;
class wxBitmap;
class WXDLLEXPORT wxbPen: public wxGDIObject
{
 protected:
  int width;
  int style;
  int join ;
  int cap ;
  wxBitmap *stipple ;
 public:
  int nb_dash ;
  wxDash *dash ;

  wxColour colour;

  wxbPen(void);
  wxbPen(wxColour& col, int width, int style);
  wxbPen(const char *col, int width, int style);
  ~wxbPen(void);

  virtual void SetColour(wxColour& col) ;
  virtual void SetColour(const char *col)  ;
  virtual void SetColour(char r, char g, char b)  ;

  virtual void SetWidth(int width)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(wxBitmap *stipple)  ;
  virtual void SetDashes(int nb_dashes, wxDash *dash)  ;
  virtual void SetJoin(int join)  ;
  virtual void SetCap(int cap)  ;

  virtual wxColour &GetColour(void);
  virtual int GetWidth(void);
  virtual int GetStyle(void);
  virtual int GetJoin(void);
  virtual int GetCap(void);
  virtual int GetDashes(wxDash **dash);
  virtual wxBitmap *GetStipple(void);
};

// Brush
class wxBrush;
class WXDLLEXPORT wxbBrush: public wxGDIObject
{
 protected:
  int style;
  wxBitmap *stipple ;
 public:
  wxColour colour;
  wxbBrush(void);
  wxbBrush(wxColour& col, int style);
  wxbBrush(char *col, int style);
  ~wxbBrush(void);

  virtual void SetColour(wxColour& col)  ;
  virtual void SetColour(const char *col)  ;
  virtual void SetColour(char r, char g, char b)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(wxBitmap* stipple=NULL)  ;

  virtual wxColour &GetColour(void);
  virtual int GetStyle(void);
  virtual wxBitmap *GetStipple(void);
};

/*
 * Bitmap flags
 */

// Hint to discard colourmap if one is loaded
#define wxBITMAP_DISCARD_COLOURMAP      1

// Hint to indicate filetype
#define wxBITMAP_TYPE_BMP               2
#define wxBITMAP_TYPE_BMP_RESOURCE      4
#define wxBITMAP_TYPE_ICO               8
#define wxBITMAP_TYPE_ICO_RESOURCE      16
#define wxBITMAP_TYPE_CUR               32
#define wxBITMAP_TYPE_CUR_RESOURCE      64
#define wxBITMAP_TYPE_XBM               128
#define wxBITMAP_TYPE_XBM_DATA          256
#define wxBITMAP_TYPE_XPM               1024
#define wxBITMAP_TYPE_XPM_DATA          2048
#define wxBITMAP_TYPE_TIF               4096
#define wxBITMAP_TYPE_GIF               8192
#define wxBITMAP_TYPE_PNG               32768
#define wxBITMAP_TYPE_ANY               16384

#define wxBITMAP_TYPE_RESOURCE wxBITMAP_TYPE_BMP_RESOURCE

class wxBitmap;
class wxCursor;
class wxIcon;

// Management of pens, brushes and fonts
class WXDLLEXPORT wxPenList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxPenList)
 public:
  inline wxPenList(void)
    { }
  ~wxPenList(void);
  void AddPen(wxPen *pen);
  void RemovePen(wxPen *pen);
  wxPen *FindOrCreatePen(wxColour *colour, int width, int style);
  wxPen *FindOrCreatePen(char *colour, int width, int style);
};

class WXDLLEXPORT wxBrushList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxBrushList)
 public:
  inline wxBrushList(void)
    { }
  ~wxBrushList(void);
  void AddBrush(wxBrush *brush);
  void RemoveBrush(wxBrush *brush);
  wxBrush *FindOrCreateBrush(wxColour *colour, int style);
  wxBrush *FindOrCreateBrush(char *colour, int style);
};

class WXDLLEXPORT wxFontList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxFontList)
 public:
  inline wxFontList(void)
    { }
  ~wxFontList(void);
  void AddFont(wxFont *font);
  void RemoveFont(wxFont *font);
  wxFont *FindOrCreateFont(int PointSize, int Family, int Style, int Weight, Bool underline = FALSE, const char *Face = NULL);
  // Matthew: see comment in wb_gdi.cc
/*
  wxFont *FindOrCreateFont(int PointSize, const char *Face, int Family, int Style, 
			   int Weight, Bool underline = FALSE); 
*/
};

class WXDLLEXPORT wxColourDatabase: public wxList
{
  DECLARE_CLASS(wxColourDatabase)
 public:
  wxColourDatabase(int type);
  ~wxColourDatabase(void) ;
  wxColour *FindColour(const char *colour);
  char *FindName(wxColour& colour);
  void Initialize(void);
};

class WXDLLEXPORT wxGDIList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxGDIList)
 public:
   wxGDIList(void);
  ~wxGDIList(void);
};

// Lists of GDI objects
extern wxPenList   * WXDLLEXPORT_DATA wxThePenList;
extern wxBrushList * WXDLLEXPORT_DATA wxTheBrushList;
extern wxFontList   * WXDLLEXPORT_DATA wxTheFontList;
extern wxGDIList   * WXDLLEXPORT_DATA wxTheBitmapList;

// Stock objects
extern wxFont * WXDLLEXPORT_DATA wxNORMAL_FONT;
extern wxFont * WXDLLEXPORT_DATA wxSMALL_FONT;
extern wxFont * WXDLLEXPORT_DATA wxITALIC_FONT;
extern wxFont * WXDLLEXPORT_DATA wxSWISS_FONT;

extern wxPen * WXDLLEXPORT_DATA wxRED_PEN;
extern wxPen * WXDLLEXPORT_DATA wxCYAN_PEN;
extern wxPen * WXDLLEXPORT_DATA wxGREEN_PEN;
extern wxPen * WXDLLEXPORT_DATA wxBLACK_PEN;
extern wxPen * WXDLLEXPORT_DATA wxWHITE_PEN;
extern wxPen * WXDLLEXPORT_DATA wxTRANSPARENT_PEN;
extern wxPen * WXDLLEXPORT_DATA wxBLACK_DASHED_PEN;
extern wxPen * WXDLLEXPORT_DATA wxGREY_PEN;
extern wxPen * WXDLLEXPORT_DATA wxMEDIUM_GREY_PEN;
extern wxPen * WXDLLEXPORT_DATA wxLIGHT_GREY_PEN;

extern wxBrush * WXDLLEXPORT_DATA wxBLUE_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxGREEN_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxWHITE_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxBLACK_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxGREY_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxMEDIUM_GREY_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxLIGHT_GREY_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxTRANSPARENT_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxCYAN_BRUSH;
extern wxBrush * WXDLLEXPORT_DATA wxRED_BRUSH;

extern wxColour * WXDLLEXPORT_DATA wxBLACK;
extern wxColour * WXDLLEXPORT_DATA wxWHITE;
extern wxColour * WXDLLEXPORT_DATA wxRED;
extern wxColour * WXDLLEXPORT_DATA wxBLUE;
extern wxColour * WXDLLEXPORT_DATA wxGREEN;
extern wxColour * WXDLLEXPORT_DATA wxCYAN;
extern wxColour * WXDLLEXPORT_DATA wxLIGHT_GREY;

// Stock cursors types
extern wxCursor * WXDLLEXPORT_DATA wxSTANDARD_CURSOR;
extern wxCursor * WXDLLEXPORT_DATA wxHOURGLASS_CURSOR;
extern wxCursor * WXDLLEXPORT_DATA wxCROSS_CURSOR;

extern wxColourDatabase * WXDLLEXPORT_DATA wxTheColourDatabase;
WXDLLEXPORT extern void wxInitializeStockObjects(void);
WXDLLEXPORT extern void wxDeleteStockObjects(void);

WXDLLEXPORT extern Bool wxColourDisplay(void);

// Returns depth of screen
WXDLLEXPORT extern int wxDisplayDepth(void);

WXDLLEXPORT extern void wxDisplaySize(int *width, int *height);

WXDLLEXPORT extern void wxSetCursor(wxCursor *cursor);

// Return the system colour for a particular GUI element
WXDLLEXPORT wxColour wxGetSysColour(int sysColour);
#define wxGetSysColor wxGetSysColour

// These are the system colours
#define wxCOLOUR_SCROLLBAR	   0
#define wxCOLOUR_BACKGROUND	   1
#define wxCOLOUR_ACTIVECAPTION	   2
#define wxCOLOUR_INACTIVECAPTION   3
#define wxCOLOUR_MENU		   4
#define wxCOLOUR_WINDOW		   5
#define wxCOLOUR_WINDOWFRAME	   6
#define wxCOLOUR_MENUTEXT	   7
#define wxCOLOUR_WINDOWTEXT	   8
#define wxCOLOUR_CAPTIONTEXT  	   9
#define wxCOLOUR_ACTIVEBORDER	  10
#define wxCOLOUR_INACTIVEBORDER	  11
#define wxCOLOUR_APPWORKSPACE	  12
#define wxCOLOUR_HIGHLIGHT	  13
#define wxCOLOUR_HIGHLIGHTTEXT	  14
#define wxCOLOUR_BTNFACE          15
#define wxCOLOUR_BTNSHADOW        16
#define wxCOLOUR_GRAYTEXT         17
#define wxCOLOUR_BTNTEXT	  18
#define wxCOLOUR_INACTIVECAPTIONTEXT 19
#define wxCOLOUR_BTNHIGHLIGHT        20

// Useful macro for create icons portably

#ifdef wx_msw
# define wxICON(X) wxIcon(X##_icon);
#elif defined(wx_x)
# define wxICON(X) wxIcon(X##_bits, X##_width, X##_height);
#else
# define wxICON    wxIcon
#endif

/*-
  Example:
  #define wxbuild_icon "wxbuild"
 
  wxIcon *icon = new wxICON(wxbuild);
 */

 
#endif // IN_CPROTO
#endif // wxb_gdih
