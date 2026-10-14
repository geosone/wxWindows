/*
 * File:	wx_tbar.cc
 * Purpose:
 * Authors:     Julian Smart & Vitaly Prokopenko
 * Created:	1993
 * Updated: Chris Breeze 2/10/97, changed OnEvent to produce more Win95-like
 *          behaviour.
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation "wx_tbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if USE_TOOLBAR

#include "wx_tbar.h"

static wxPen * white_pen = NULL,
             * dark_grey_pen = NULL,
             * black_pen = NULL,
             * thick_black_pen;

// Keep a list of all toolbars created, so you can tell whether a toolbar
// is still valid: a tool may have quit the toolbar.
static wxList toolBars;

// class wxToolBarTool

wxToolBarTool::wxToolBarTool(int theIndex,
                    wxBitmap *theBitmap1, wxBitmap *theBitmap2, Bool toggle, wxCanvas *WXUNUSED(canvas),
                    float xPos, float yPos, char *helpS1, char *helpS2)
{
  toolStyle = wxTOOL_STYLE_BUTTON;
  clientData = NULL;
  index = theIndex;
  isToggle = toggle;
  toggleState = FALSE;
  enabled = TRUE;
  bitmap1 = theBitmap1;
  bitmap2 = theBitmap2;
  disabledBitmap = 0;
  x = xPos;
  y = yPos;
  width = height = (float)0.0;
  deleteSecondBitmap = FALSE;
  if (bitmap1)
  {
    width = (float)(bitmap1->GetWidth()+2);
    height = (float)(bitmap1->GetHeight()+2);
  }
  if (helpS1)
    shortHelpString = copystring(helpS1);
  else
    shortHelpString = NULL;
  if (helpS2)
    longHelpString = copystring(helpS2);
  else
    longHelpString = NULL;
}

wxToolBarTool::~wxToolBarTool(void)
{
  if (deleteSecondBitmap && bitmap2)
    delete bitmap2;
  if (shortHelpString)
    delete[] shortHelpString;
  if (longHelpString)
    delete[] longHelpString;
  delete disabledBitmap;
}

// Chris Breeze: new method which decides which bitmap should
// be displayed. If the button is disabled then disabledBitmap
// is returned otherwise bitmap1 is returned if the button is
// up or bitmap2 if it is pressed.
//
// disabledBitmap is created from bitmap1 if it is not already
// defined: The top left pixel of bitmap1 is assumed to be the
// background colour and GREY is used as the foreground colour.
// disabledBitmap is created the same size and depth as bitmap1.
// bitmap1 is scanned and each pixel whose "intensity" is greater
// than 3*128 is set to the bg colour, the rest are set to the
// fg colour.
// This sounds crude but produces reasonable results. It is not
// very efficient on X but works well in practice.
//
wxBitmap* wxToolBarTool::GetBitmap()
{
  wxBitmap* bmap = 0;
  if (enabled)
  {
	bmap = toggleState ? bitmap2 : bitmap1;
  }
  else
  {
	// tool is disabled
	if (!disabledBitmap)
	{
		int width = bitmap1->GetWidth();
		int height = bitmap1->GetHeight();
		disabledBitmap = new wxBitmap(
							width,
							height,
							bitmap1->GetDepth()
							);
		wxMemoryDC* srcDC = new wxMemoryDC;
		wxMemoryDC* destDC = new wxMemoryDC;
		srcDC->SelectObject(bitmap1);
		destDC->SelectObject(disabledBitmap);

		wxColour bgCol;
		srcDC->GetPixel(0, 0, &bgCol);
		wxPen bgPen(bgCol, 1, wxSOLID);
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				wxColour col;
				srcDC->GetPixel(x, y, &col);
				if (col.Red() + col.Blue() + col.Green() > 3 * 128)
				{
					destDC->SetPen(&bgPen);
				}
				else
				{
					destDC->SetPen(wxGREY_PEN);
				}
				destDC->DrawPoint(x, y);
			}
		}

		srcDC->SelectObject(0);
		destDC->SelectObject(0);
		delete srcDC;
		delete destDC;
	}
	bmap = disabledBitmap;
  }
  return bmap;
}

// class wxToolBar

wxToolBar::wxToolBar(void)
{
  WXSET_TYPE(wxToolBar, wxTYPE_TOOLBAR)
  
  tilingDirection = wxVERTICAL;
  rowsOrColumns = 0;
  maxWidth = (float)0.0;
  maxHeight = (float)0.0;
  xMargin = 0;
  yMargin = 0;
  toolPacking = 1;
  toolSeparation = 5;
  currentTool = 0;
  toolBars.Append(this);
}

wxToolBar::wxToolBar( wxWindow *parent, int x, int y, int w, int h, long style,
  int direction, int RowsOrColumns, Constdata char *name ):
#ifdef wx_xview
  wxCanvas(parent, x, y, w, h, style, name), tools(wxKEY_INTEGER)
#else
  wxPanel(parent, x, y, w, h, style, name), tools(wxKEY_INTEGER)
#endif
{
  WXSET_TYPE(wxToolBar, wxTYPE_TOOLBAR)
  
//  info_frame = (wxFrame*)parent;
  SetBackground(wxGREY_BRUSH);
  if ( white_pen == 0 )
  {
    white_pen = new wxPen;
    white_pen->SetColour( "WHITE" );
  }
  if ( dark_grey_pen == 0 )
  {
    dark_grey_pen = new wxPen;
    dark_grey_pen->SetColour( 85,85,85 );
  }
  if ( black_pen == 0 )
  {
    black_pen = new wxPen;
    black_pen->SetColour( "BLACK" );
  }
  if ( thick_black_pen == 0 )
  {
    thick_black_pen = new wxPen("BLACK", 3, wxSOLID);
  }
  tilingDirection = direction;
  rowsOrColumns = RowsOrColumns;
  if ( tilingDirection == wxVERTICAL )
    { lastX = (float)3; lastY = (float)7; }
  else
    { lastX = (float)7; lastY = (float)3; }
  maxWidth = maxHeight = 0;
  pressedTool = currentTool = 0;
  xMargin = 0;
  yMargin = 0;
  toolPacking = 1;
  toolSeparation = 5;
  
  toolBars.Append(this);
}

wxToolBar::~wxToolBar ()
{
  toolBars.DeleteObject(this);
  
  for ( wxNode *node = tools.First(); node; node = node->Next() )
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    delete tool;
  }
}

void wxToolBar::OnPaint ()
{
  static int count = 0;
  // Prevent reentry of OnPaint which would cause wxMemoryDC errors.
  if ( count > 0 )
    return;
  count++;

  wxMemoryDC mem_dc(GetDC());

  for ( wxNode *node = tools.First(); node; node = node->Next() )
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->toolStyle == wxTOOL_STYLE_BUTTON)
      DrawTool(mem_dc, tool);
  }

  int w, h;
  GetClientSize( &w, &h );
  wxCanvasDC *dc = GetDC();
  wxPen * old_pen = dc->GetPen();
  dc->SetPen( white_pen );
  dc->DrawLine((float)0,(float)0,(float)w,(float)0);
  dc->SetPen( black_pen );
  dc->DrawLine((float)0,(float)(h-1),(float)w,(float)(h-1));
  dc->SetPen( old_pen );

  count--;
}

void wxToolBar::OnSize ( int w, int h )
{
  // Motif can inherit from a panel, but
  // because XView doesn't allow scrolling panels,
  // a toolbar must inherit from a canvas instead.
#ifdef wx_xview
  wxCanvas::OnSize(w, h);
#else
  wxPanel::OnSize(w, h);
#endif
}

void wxToolBar::OnKillFocus ()
{
	// Chris Breeze: I don't know why these should be set to 0, it causes
	// problems on Motif. Commenting out this line seems to work.
	//pressedTool = currentTool = 0;
	OnMouseEnter(-1);
}

// Chris Breeze: rewritten to produce more Win95-like behaviour.
// When a button is pressed CaptureMouse() is called to grab all mouse
// events until the button is released. The button is pressed when the
// mouse enters it and released when it leaves. All other buttons are
// ignored until the mouse button is released (when ReleaseMouse() is called).
// OnLeftClick is only called if the mouse button is released over the
// original button.
void wxToolBar::OnEvent ( wxMouseEvent & event )
{
	float x, y;
	event.Position(&x, &y);
	wxToolBarTool *tool = FindToolForPosition(x, y);

	if (event.LeftDown())
	{
		// Left button pressed
		if (tool && tool->enabled)
		{
			// mouse button pressed over an enabled button - CaptureMouse
			// and track events until it is released.
			pressedTool = tool;
			tool->toggleState = !tool->toggleState;
			wxMemoryDC *dc2 = new wxMemoryDC(GetDC());
			DrawTool(*dc2, tool);
			delete dc2;
			CaptureMouse();
			OnMouseEnter(tool->index);
		}
	}
	else if (event.LeftUp())
	{
		// Left button released
		if (pressedTool)
		{
			ReleaseMouse();

			if (tool == pressedTool)
			{
				// clear pressedTool otherwise other events may modify it
				// and since this is the button up event the tool is no
				// longer pressed
				pressedTool = 0;

				// mouse was released over pressedTool
				if (!tool->isToggle) tool->toggleState = FALSE;
				
				if (!OnLeftClick(tool->index, tool->toggleState) &&
					tool->isToggle)
				{
					// Tool is toggle but OnLeftClick says No Toggle allowed,
					// so change it back
					tool->toggleState = !tool->toggleState;
				}

				wxMemoryDC *dc2 = new wxMemoryDC(GetDC());
				DrawTool(*dc2, tool);
				delete dc2;
			}
			else
			{
				OnMouseEnter(tool ? tool->index : -1);
			}
			pressedTool = 0;
		}
	}
	else if (pressedTool)
	{
		if (pressedTool == currentTool)
		{
			// last event was in pressedTool, test for leaving...
			if (tool != pressedTool)
			{
				pressedTool->toggleState = !pressedTool->toggleState;
				wxMemoryDC *dc2 = new wxMemoryDC(GetDC());
				DrawTool(*dc2, pressedTool);
				delete dc2;
			}
		}
		else
		{
			// last event was outside pressedTool, test for entering...
			if (tool == pressedTool)
			{
				pressedTool->toggleState = !pressedTool->toggleState;
				wxMemoryDC *dc2 = new wxMemoryDC(GetDC());
				DrawTool(*dc2, pressedTool);
				delete dc2;
			}
		}
	}
	else if (event.RightDown())
	{
		if (tool)
		{
			// Right click callback
			OnRightClick(tool->index, x, y);
		}
	}
	else
	{
		if (tool != currentTool)
		{
			OnMouseEnter(tool ? tool->index : -1);
		}
	}
	currentTool = tool;
}

void wxToolBar::DrawTool(wxMemoryDC& memDC, wxToolBarTool *tool)
{
  wxCanvasDC *dc = GetDC();
  wxBitmap *bitmap = tool->GetBitmap();

  if (bitmap)
  {
    if (bitmap->GetColourMap())
      memDC.SetColourMap(bitmap->GetColourMap());

    int ax = (int)tool->x,
        ay = (int)tool->y,
        bx = (int)(tool->x+tool->GetWidth()),
        by = (int)(tool->y+tool->GetHeight());
    memDC.SelectObject(bitmap);
    if (windowStyle & wxTB_3DBUTTONS)
    {
      dc->SetClippingRegion((float)ax, (float)ay, (float)(bx-ax+1), (float)(by-ay+1));
      dc->Blit((float)(ax+1), (float)(ay+1), (float)(bx-ax-2), (float)(by-ay-2), &memDC, (float)0, (float)0);
      wxPen * old_pen = dc->GetPen();
      dc->SetPen( white_pen );
      dc->DrawLine((float)ax,(float)(by-1),(float)ax,(float)ay);
      dc->DrawLine((float)ax,(float)ay,(float)(bx-1),(float)ay);
      dc->SetPen( dark_grey_pen );
      dc->DrawLine((float)(bx-1),(float)(ay+1),(float)(bx-1),(float)(by-1));
      dc->DrawLine((float)(bx-1),(float)(by-1),(float)(ax+1),(float)(by-1));
      dc->SetPen( black_pen );
      dc->DrawLine((float)bx,(float)ay,(float)bx,(float)by);
      dc->DrawLine((float)bx,(float)by,(float)ax,(float)by);
      dc->SetPen( old_pen );
      dc->DestroyClippingRegion();
      // Select bitmap out of the DC
    }
    else
    {
      dc->Blit(tool->x, tool->y,
            (float)bitmap->GetWidth(), (float)bitmap->GetHeight(),
            &memDC, (float)0, (float)0);
    }
    memDC.SelectObject(NULL);
    memDC.SetColourMap(NULL);
  }
  // No second bitmap, so draw a thick line around bitmap, or invert if mono
  else if (tool->toggleState)
  {
    Bool drawBorder = FALSE;
    #ifdef wx_x // X doesn't invert properly on colour
    drawBorder = wxColourDisplay();
    #else       // Inversion works fine under Windows
    drawBorder = FALSE;
    #endif

    if (!drawBorder)
    {
      memDC.SelectObject(tool->bitmap1);
      dc->Blit(tool->x, tool->y, tool->GetWidth(), tool->GetHeight(),
               &memDC, 0, 0, wxSRC_INVERT);
      memDC.SelectObject(NULL);
    }
    else
    {
      if (windowStyle & wxTB_3DBUTTONS)
      {
        int ax = (int)tool->x,
            ay = (int)tool->y,
            bx = (int)(tool->x+tool->GetWidth()),
            by = (int)(tool->y+tool->GetHeight());

        memDC.SelectObject(tool->bitmap1);
        dc->SetClippingRegion((float)ax, (float)ay, (float)(bx-ax+1), (float)(by-ay+1));
        dc->Blit((float)(ax+2), (float)(ay+2), (float)(bx-ax-2), (float)(by-ay-2), &memDC, (float)0, (float)0);
        wxPen * old_pen = dc->GetPen();
        dc->SetPen( black_pen );
        dc->DrawLine((float)ax,(float)(by-1),(float)ax,(float)ay);
        dc->DrawLine((float)ax,(float)ay,(float)(bx-1),(float)ay);
        dc->SetPen( dark_grey_pen );
        dc->DrawLine((float)(ax+1),(float)(by-2),(float)(ax+1),(float)(ay+1));
        dc->DrawLine((float)(ax+1),(float)(ay+1),(float)(bx-2),(float)(ay+1));
        dc->SetPen( white_pen );
        dc->DrawLine((float)bx,(float)ay,(float)bx,(float)by);
        dc->DrawLine((float)bx,(float)by,(float)ax,(float)by);
        dc->SetPen( old_pen );
        dc->DestroyClippingRegion();
        memDC.SelectObject(NULL);
      }
      else
      {
        float x = tool->x;
        float y = tool->y;
        float w = (float)tool->bitmap1->GetWidth();
        float h = (float)tool->bitmap1->GetHeight();

        memDC.SelectObject(tool->bitmap1);
        dc->SetClippingRegion(tool->x, tool->y, w, h);
        dc->Blit(tool->x, tool->y, w, h,
                 &memDC, (float)0, (float)0);
        dc->SetPen(thick_black_pen);
        dc->SetBrush(wxTRANSPARENT_BRUSH);
        dc->DrawRectangle(x, y, w-1, h-1);
        dc->DestroyClippingRegion();
        memDC.SelectObject(NULL);
      }
    }
  }
}

// Only allow toggle if returns TRUE
Bool wxToolBar::OnLeftClick(int WXUNUSED(toolIndex), Bool WXUNUSED(toggleDown))
{
  return TRUE;
}

/*
void wxToolBar::OnMenuTool( int menuIndex ) {}
*/

// Called when the mouse cursor enters a tool bitmap (no button pressed).
// Argument is -1 if mouse is exiting the toolbar.
void wxToolBar::OnMouseEnter ( int WXUNUSED(toolIndex) )
{
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.
wxToolBarTool *wxToolBar::AddTool(int index, wxBitmap *bitmap, wxBitmap *pushedBitmap,
             Bool toggle, float xPos, float yPos, wxObject *clientData,
             char *helpString1, char *helpString2)
{
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, pushedBitmap, toggle, this, xPos, yPos, helpString1, helpString2);
  tool->clientData = clientData;

  if (xPos > -1)
    tool->x = xPos;
  else
    tool->x = xMargin;

  if (yPos > -1)
    tool->y = yPos;
  else
    tool->y = yMargin;
  
  // Calculate reasonable max size in case Layout() not called
  if ((tool->x + bitmap->GetWidth() + xMargin) > maxWidth)
    maxWidth = (tool->x + bitmap->GetWidth() + xMargin);

  if ((tool->y + bitmap->GetHeight() + yMargin) > maxHeight)
    maxHeight = (tool->y + bitmap->GetHeight() + yMargin);

  tools.Append((long)index, tool);
  return tool;
}

void wxToolBar::AddSeparator ()
{
  wxToolBarTool *tool = new wxToolBarTool;
  tool->toolStyle = wxTOOL_STYLE_SEPARATOR;
  tools.Append(tool);
}

void wxToolBar::ClearTools(void)
{
  pressedTool = currentTool = 0;
  wxNode *node = tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    wxNode *nextNode = node->Next();
    delete tool;
    delete node;
    node = nextNode;
  }
}

void wxToolBar::EnableTool(int index, Bool enable)
{
  wxNode *node = tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      Bool oldState = tool->enabled;
      tool->enabled = enable;

      if (oldState != enable)
      {
        wxMemoryDC memDC(GetDC());
        DrawTool(memDC, tool);
      }
    }
  }
}

void wxToolBar::ToggleTool(int index, Bool toggle)
{
  wxNode *node = tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool && tool->isToggle)
    {
      Bool oldState = tool->toggleState;
      tool->toggleState = toggle;

      if (oldState != toggle)
      {
        wxMemoryDC memDC(GetDC());
        DrawTool(memDC, tool);
      }
    }
  }
}

void wxToolBar::SetToggle(int index, Bool value)
{
  wxNode *node=tools.Find((long)index);
  if (node){
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    tool->isToggle = value;
  }
}

Bool wxToolBar::GetToolState(int index)
{
  wxNode *node = tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->toggleState;
    }
    else return FALSE;
  }
  else return FALSE;
}

Bool wxToolBar::GetToolEnabled(int index)
{
  wxNode *node = tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->enabled;
    }
    else return FALSE;
  }
  else return FALSE;
}

wxObject *wxToolBar::GetToolClientData(int index)
{
  wxNode *node = tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool)
    {
      return tool->clientData;
    }
    else return NULL;
  }
  else return NULL;
}

void wxToolBar::SetToolShortHelp(int index, char *helpString)
{
  wxNode *node=tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->shortHelpString == helpString)
      return;
    if (tool->shortHelpString)
      delete[] tool->shortHelpString;
    if (helpString)
      tool->shortHelpString = copystring(helpString);
    else
      tool->shortHelpString = NULL;
  }
}

char *wxToolBar::GetToolShortHelp(int index)
{
  wxNode *node=tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    return tool->shortHelpString;
  }
  else
    return NULL;
}

void wxToolBar::SetToolLongHelp(int index, char *helpString)
{
  wxNode *node=tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->longHelpString == helpString)
      return;
    if (tool->longHelpString)
      delete[] tool->longHelpString;
    if (helpString)
      tool->longHelpString = copystring(helpString);
    else
      tool->longHelpString = NULL;
  }
}

char *wxToolBar::GetToolLongHelp(int index)
{
  wxNode *node=tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    return tool->longHelpString;
  }
  else
    return NULL;
}

wxToolBarTool *wxToolBar::FindToolForPosition(float x, float y)
{
  wxNode *node = tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if ((x >= tool->x) && (y >= tool->y) &&
        (x <= (tool->x + tool->GetWidth())) &&
        (y <= (tool->y + tool->GetHeight())))
      return tool;

    node = node->Next();
  }
  return NULL;
}

void wxToolBar::GetMaxSize ( float * width, float * height )
{
/* Why + 7??
  *width = maxWidth+7;
  *height = maxHeight+7;
 */
  *width = maxWidth;
  *height = maxHeight;
}

void wxToolBar::SetMargins(int x, int y)
{
  xMargin = x;
  yMargin = y;
}

void wxToolBar::SetToolPacking(int packing)
{
  toolPacking = packing;
}

void wxToolBar::SetToolSeparation(int separation)
{
  toolSeparation = separation;
}

void wxToolBar::Layout(void)
{
  currentRowsOrColumns = 0;
  lastX = xMargin;
  lastY = yMargin;
  int maxToolWidth = 0;
  int maxToolHeight = 0;
  maxWidth = 0.0;
  maxHeight = 0.0;

  // Find the maximum tool width and height
  wxNode *node = tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->GetWidth() > maxToolWidth)
      maxToolWidth = (int)tool->GetWidth();
    if (tool->GetHeight() > maxToolHeight)
      maxToolHeight = (int)tool->GetHeight();
    node = node->Next();
  }

  int separatorSize = toolSeparation;

  node = tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->toolStyle == wxTOOL_STYLE_SEPARATOR)
    {
      if (tilingDirection == wxHORIZONTAL)
      {
        if (currentRowsOrColumns >= rowsOrColumns)
          lastY += separatorSize;
        else
          lastX += separatorSize;
      }
      else
      {
        if (currentRowsOrColumns >= rowsOrColumns)
          lastX += separatorSize;
        else
          lastY += separatorSize;
      }
    }
    else if (tool->toolStyle == wxTOOL_STYLE_BUTTON)
    {
      if (tilingDirection == wxHORIZONTAL)
      {
        if (currentRowsOrColumns >= rowsOrColumns)
        {
          currentRowsOrColumns = 0;
          lastX = xMargin;
          lastY += maxToolHeight + toolPacking;
        }
        tool->x = (float)(lastX + (maxToolWidth - tool->GetWidth())/2.0);
        tool->y = (float)(lastY + (maxToolHeight - tool->GetHeight())/2.0);
  
        lastX += maxToolWidth + toolPacking;
      }
      else
      {
        if (currentRowsOrColumns >= rowsOrColumns)
        {
          currentRowsOrColumns = 0;
          lastX += (float)(maxToolWidth + toolPacking);
          lastY = (float)yMargin;
        }
        tool->x = (float)(lastX + (maxToolWidth - tool->GetWidth())/2.0);
        tool->y = (float)(lastY + (maxToolHeight - tool->GetHeight())/2.0);
  
        lastY += maxToolHeight + toolPacking;
      }
      currentRowsOrColumns ++;
    }
    
    if (lastX > maxWidth)
      maxWidth = lastX;
    if (lastY > maxHeight)
      maxHeight = lastY;

    node = node->Next();
  }
  if (tilingDirection == wxVERTICAL)
    maxWidth += (float)maxToolWidth;
  else
    maxHeight += (float)maxToolHeight;

  maxWidth += xMargin;
  maxHeight += yMargin;
}


#endif
