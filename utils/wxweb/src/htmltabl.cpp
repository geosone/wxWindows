///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
//
// Edit History
//
//	  Started November 1995 by Andrew Davison.
//
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#include <wx_prec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "wxhtml.h"
#include "htmlpars.h"
#include "htmltabl.h"

const float DEFAULT_BORDER_WIDTH = 1.0F;
const float DEFAULT_CELL_SPACING = 2.0F;
const float DEFAULT_CELL_PADDING = 1.0F;
const float EXTRA_X_SPACING = 2.0F;
const float EXTRA_Y_SPACING = 4.0F;

ListOf table_list;

///////////////////////////////////////////////////////////////////////////////

void TableCell::Adjust(float x, float y)
{
	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->SetLeft(x + obj->GetLeft());
			obj->SetTop(y + obj->GetTop());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void TableCell::GetDimensions(Table* t)
{
	float min_width = 0.0F;
	width = height = 0.0F;

	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		float line_width = 0.0F, line_height = 0.0F;
		Bool first = TRUE;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;

			// Accumulate the line width...

			//line_width += obj->GetWidth();
			line_width = obj->GetLeft() + obj->GetWidth();
			line_height = wxMax(line_height, obj->GetHeight());

			// Find the MINIMUM cell width...

			if (first)
			{
				float first_width;

				if (obj->IsText())
				{
					HtmlText* tt = (HtmlText*)obj;
					string text = tt->GetText();

					// Find the width of the first WORD in the line...

					const char* ptr = strchr(text.data(), ' ');

					if (ptr && (ptr != text.data()))
					{
						string s(text.data(), ptr - text.data());
						float w, h;

						obj->HtmlObject::SetFont(&t->mem_dc);
						t->mem_dc.GetTextExtent((char*)s.data(), &w, &h);
						first_width = w;
					}
					else
						first_width = obj->GetWidth();

					min_width = wxMax(min_width, first_width);
					first = FALSE;
				}
				else if (obj->IsImage())
				{
					first_width = obj->GetWidth();
					min_width = wxMax(min_width, first_width);
					first = FALSE;
				}
			}
		}

		// Find the MAXIMUM cell width...

		width = wxMax(width, line_width);
		height += line_height;

		//tags->Append((wxObject*)item);
	}

	if (col_span == 1)
	{
		t->max_width[col_nbr] =
			wxMax(t->max_width[col_nbr], width + (2 * EXTRA_X_SPACING));

		t->min_width[col_nbr] =
			wxMax(t->min_width[col_nbr], min_width + (2 * EXTRA_X_SPACING));
	}
#if 1
	else
	{
		float spanned_width = 0.0F;

		for (int i = 0; i < col_span; i++)
			spanned_width += t->max_width[col_nbr + i];

		// If a cell overflows it's spannees, then spread the
		// difference over the spanned cells...

		if (width > spanned_width)
		{                                           
			float delta = (width - spanned_width) / col_span;
			for (int i = 0; i < col_span; i++)
				t->max_width[col_nbr + i] += delta;
		}
	}
#endif

	if (row_span == 1)
		t->current_row->max_height =
			wxMax(t->current_row->max_height, height + (2 * EXTRA_Y_SPACING));
}

///////////////////////////////////////////////////////////////////////////////

//#include <iostream.h>
//static ofstream debug("x.out");

void Table::AutoLayout(wxHtml* canvas, float drawing_width)
{
	if (spec_width)
	{
		if (pc)
		{
			drawing_width *= spec_width;
			drawing_width /= 100;
		}
		else
			drawing_width = spec_width;
	}

	int i, cnt = 0;

	for (i = 0; i < max_cols; i++)
	{
		if (col_spec_width[i] > 0)
		{
			if (col_pc[i])
			{
				max_width[i] *= col_spec_width[i];
				max_width[i] /= 100;
			}
			else
				max_width[i] = col_spec_width[i];

			cnt++;
		}
	}

	// Calculate minimum and maximum table widths...

	float w1 = 0.0F, w2 = 0.0F;

	for (i = 0; i < max_cols; i++)
	{
		w1 += min_width[i];
		w2 += max_width[i];
	}

	// Does it fit?

	if (w2 < drawing_width)
	{
		if (spec_width)
		{                                 
			float ratio = drawing_width / w2;

			for (int ii = 0; ii < max_cols; ii++)
				max_width[ii] *= ratio;
		}
	}
	else
	{
		// No, so adjust everything...

		//drawing_width -= (2*Border()) + Spacing() + (max_cols*2*Padding()) + (max_cols*Spacing());

		float W = drawing_width - w1;
		float D = w2 - w1;

		for (i = 0; i < max_cols; i++)
		{
			//if (col_spec_width[i] == 0)
			{
				float m = min_width[i];
				float M = max_width[i];
				float d = M - m;
                               
				m += (d * W) / (D?D:1);

				max_width[i] = m - (2*Padding() + Spacing());
			}
		}
	}

	// Go thru each row...

	ListOfIterator ilist = rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;
			float cell_width = 0.0F, cell_height = 0.0F;

			// Calculate cell width...

			for (int ii = 0; ii < cell->col_span; ii++)
				cell_width += max_width[cell->col_nbr+ii];

			// Go thru each line...

			ListOfIterator ilist3 = cell->tags;
			void* a3;

			for (int ok3 = ilist3.First(a3); ok3; ok3 = ilist3.Next(a3))
			{
				wxHtml::Tag* item = (wxHtml::Tag *)a3;
				float x = 0.0F, line_height = 0.0F, actual_height = 0.0F;
				int cnt = 0;
				Bool first = TRUE;

				if (item->objects.Count() > 1)
				{
					//cell->left = TRUE;
					//cell->center = cell->right = FALSE;

					//cell->top = TRUE;
					//cell->middle = cell->bottom = FALSE;
				}

				// Go thru each object...

				ListOfIterator ilist4 = item->objects;
				void* a4;

				for (int ok4 = ilist4.First(a4); ok4; ok4 = ilist4.Next(a4))
				{
					HtmlObject* obj = (HtmlObject*)a4;
					obj->SetTop(item->y);

					if (first)
					{
						x = obj->GetLeft();
						first = FALSE;
					}

					// Recursively apply to tables...

					if (obj->IsTable())
					{
						obj->SetLeft(x);
						Table* table = ((HtmlTable*)obj)->GetTable();
						table->AutoLayout(canvas, cell_width);
						table->Process();
						max_width[cell->col_nbr] = wxMax(max_width[cell->col_nbr], table->width);
						actual_height = wxMax(actual_height, obj->GetHeight());
					}
					else if (obj->IsImage())
					{
						HtmlImage* image = (HtmlImage*)obj;

						if (!image->IsLeftAligned() && !image->IsRightAligned())
							actual_height = wxMax(actual_height, obj->GetHeight());
					}
					else
						actual_height = wxMax(actual_height, obj->GetHeight());

					line_height = wxMax(line_height, obj->GetHeight());

					float w = obj->GetWidth();

					// Check for cell wrap...

					if ((x + w) > cell_width)
					{
						cell->left = TRUE;
						cell->center = cell->right = FALSE;

						cell->top = TRUE;
						cell->middle = cell->bottom = FALSE;

						// Create a new line...

						wxHtml::Tag* titem = new wxHtml::Tag;
						titem->center = item->center;
						titem->p_center = item->p_center;
						titem->y = item->y + actual_height;
						titem->left_margin = item->left_margin;
						titem->right_margin = item->right_margin;
						titem->left_floating = item->left_floating;
						titem->right_floating = item->right_floating;
						item = titem;

						if (ilist3.PeekNext())
							ilist3.InsertAfter(titem);
						else
							cell->tags.Append(titem);

						// If text then try to split it...

						line_height = 0.0F;

						if (obj->IsText())
						{
							HtmlText* tt = (HtmlText*)obj;
							string text = tt->GetText();

							// Find the end of the first WORD in the line, a word
							// is deemed to be delimited by a space or hyphen...

							const char* ptr = text.data();
							Bool not_wrapped = TRUE;

							obj->HtmlObject::SetFont(&mem_dc);

							do
							{
								const char* ptr2 = strchr(ptr, ' ');

								if (!ptr2)
									ptr2 = strchr(ptr, 0);
								else
									ptr2++;

								string s(ptr, ptr2 - ptr);
								float _w = 0.0F, _h = 0.0F;

								mem_dc.GetTextExtent((char*)s.data(), &_w, &_h);

								// Found a wrap point?

								if (x && ((x + _w) > cell_width))
								{
									not_wrapped = FALSE;
									cell->width = cell_width;

									// Fix old object...

									string s2(text.data(), ptr - text.data());
									mem_dc.GetTextExtent((char*)s2.data(), &_w, &_h);
									tt->SetText(s2, _w);

									// Fix new object...

									mem_dc.GetTextExtent(ptr, &_w, &_h);
									x = item->left_margin;
									line_height = _h;
									obj = new HtmlText(canvas, x, titem->y, _w, _h, ptr);
									x += _w;
									titem->objects.Append(obj);

									break;
								}

								// Try the next word...

								x += _w;
								ptr = ptr2;
							}
							  while (*ptr);

							if (!cnt && not_wrapped) continue;
						}
						else
						{
							if (!cnt) continue;
						}

						// Transfer remaining objects to the new line...

						while (ilist4.Next(a4))
						{
							obj = (HtmlObject*)a4;
							obj->SetLeft(x);
							obj->SetTop(titem->y);
							x += obj->GetWidth();
							line_height = wxMax(line_height, obj->GetHeight());
							titem->objects.Append(obj);
							ilist4.Delete();
							//ilist4.Next(a4);
						}

						break;
					}
					else
					{
						x += w;
					}

					//cnt++;
				}

				cell_height += line_height;
			}

			row->max_height = wxMax(row->max_height, cell_height+5);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Table::Process()
{
	float x = Border() + Spacing();
	float y = Border() + Spacing();

	width = 2*Border() + Spacing();
	height = 2*Border() + Spacing();

	ListOfIterator ilist = rows;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		int col_idx = 0;

		// Now adjust the position of all the objects that make up
		// a cell based upon calculated cell co-ordinates...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Skip any spanned cols...

			while (col_idx < cell->col_nbr)
				x += max_width[col_idx++] + 2*Padding() + Spacing();

			// Check a spanning cell doesn't overflow it's spanees. If it does
			// then spread the difference over the spanned columns...

			if (cell->col_span > 1)
			{
				float spanned_width = 0.0F;

				for (int i = 0; i < cell->col_span; i++)
					spanned_width += max_width[cell->col_nbr + i];

				if (cell->width > spanned_width)
				{                                                 
					float delta = (cell->width - spanned_width) / cell->col_span;

					for (int i = 0; i < cell->col_span; i++)
						max_width[cell->col_nbr + i] += delta;
				}
			}

			// Allow for spanned rows...

			float cell_width = 0.0F, cell_height = 0.0F;
			ListOfIterator ilist3 = ilist;
			void* a3 = a;

			for (int i = 0; i < cell->col_span; i++)
				cell_width += max_width[cell->col_nbr + i];

			for (int j = 0; j < cell->row_span; j++)
			{
				TableRow* tmp_row = (TableRow*)a3;
				cell_height += tmp_row->max_height + 2*Padding() + Spacing();
				ilist3.Next(a3);
			}

			float _x = x;
			float _y = y;

			if (cell->center)
				_x += (cell_width - cell->width - Padding()) / 2;
			else if (cell->right)
				_x += (cell_width - cell->width - Padding() + EXTRA_X_SPACING);
			else
				_x += Padding() + EXTRA_X_SPACING;

			if (cell->bottom)
				_y += (cell_height - cell->height - Padding() - EXTRA_Y_SPACING);
			else if (cell->top)
				_y += Padding() + EXTRA_Y_SPACING;
			else
				_y += (cell_height - cell->height - Padding()) / 2;

			// Adjust the location of cells as needed...

			cell->Adjust(_x, _y);

			x += max_width[col_idx++] + 2*Padding() + Spacing();
		}

		height += row->max_height + 2*Padding() + Spacing();
		y += row->max_height + 2*Padding() + Spacing();
		x = Border() + Spacing();
	}

	for (int i = 0; i < max_cols; i++)
		width += max_width[i] + 2*Padding() + Spacing();

	y += Border();

	// Now the caption...

	ilist = caption;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		float max_height = 0.0F;

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->SetLeft(x + obj->GetLeft());
			obj->SetTop(y + obj->GetTop());
			max_height = wxMax(max_height, obj->GetHeight());
		}

		height += max_height;
		y += max_height;
	}

	html_object->SetWidth(width);
	html_object->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////

void Table::PreProcess()
{
	float min_table_width = 2.0F*Border() + Spacing();

	width = 2*Border() + Spacing();
	height = 2*Border() + Spacing();

	ListOfIterator ilist = rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		height += row->max_height + 2*Padding() + Spacing();
	}

	for (int i = 0; i < max_cols; i++)
	{
		min_table_width += min_width[i] * 2*Padding() * Spacing();
		width += max_width[i] + 2*Padding() + Spacing();
	}

	if (caption.Count())
	{
		height += 30;
	}

	html_object->SetWidth(min_table_width);
	html_object->SetHeight(height);
}

///////////////////////////////////////////////////////////////////////////////

void Table::DrawTags(wxDC* dc, ListOf& tags)
{
	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		if (item->center || item->p_center)
		{
			float w = 0.0F;

			for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
			{
				HtmlObject* obj = (HtmlObject*)a2;
				w += obj->GetWidth();
			}
		}

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			float save_left = obj->GetLeft();
			float save_top = obj->GetTop();

			obj->SetLeft(html_object->GetLeft()+obj->GetLeft());
			obj->SetTop(html_object->GetTop()+obj->GetTop());
			obj->Draw(dc);

			obj->SetLeft(save_left);
			obj->SetTop(save_top);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Table::DrawCells(wxDC* dc, ListOf& list)
{
	// Go thru each row...

	ListOfIterator ilist = list;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;
			DrawTags(dc, cell->tags);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

static void DrawBorder(wxDC* dc, float x, float y, float w, float h, float bw, Bool raised)
{
	dc->SetPen(wxThePenList->FindOrCreatePen(raised?"WHITE":"GREY", (int)bw, wxSOLID));
	dc->DrawLine(x, y, x, y+h);
	dc->DrawLine(x, y, x+w, y);

	dc->SetPen(wxThePenList->FindOrCreatePen(raised?"GREY":"WHITE", (int)bw, wxSOLID));
	dc->DrawLine(x, y+h, x+w, y+h);
	dc->DrawLine(x+w, y, x+w, y+h);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableDraw(wxDC* dc, Table* t, float at_x, float at_y)
{
	if (!t->border_width)
	{
		t->DrawCells(dc, t->rows);
		t->DrawTags(dc, t->caption);
		return;
	}

	// Draw table border...

	DrawBorder(GetDC(), at_x, at_y, t->width, t->height, t->border_width, TRUE);

	float xx = at_x + t->Border() + t->Spacing();
	float yy = at_y + t->Border() + t->Spacing();

	// Go thru each row...

	ListOfIterator ilist = t->rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;
		int col_idx = 0;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Skip any spanned cols...

			while (col_idx < cell->col_nbr)
				xx += t->max_width[col_idx++] + 2*t->Padding() + t->Spacing();

			// Calculate column span...

			float width = -t->Spacing();

			for (int i = 0; i < cell->col_span; i++)
				width += t->max_width[col_idx++] + 2*t->Padding() + t->Spacing();

			// Calculate row span...

			float height = -t->Spacing();
			ListOfIterator ilist3 = ilist;
			void* a3 = a;

			for (int j = 0; j < cell->row_span; j++)
			{
				TableRow* tmp_row = (TableRow*)a3;
				height += tmp_row->max_height + 2*t->Padding() + t->Spacing();
				ilist3.Next(a3);
			}

			// Clear the cell background...

			if (cell->bg != background)
			{
				dc->SetBrush(wxTheBrushList->FindOrCreateBrush(cell->bg, wxSOLID));
				dc->SetPen(wxThePenList->FindOrCreatePen(background, 1, wxTRANSPARENT));
				dc->DrawRectangle(xx, yy, width, height);
			}

			// Draw the cell border...

			DrawBorder(GetDC(), xx, yy, width, height, 1.0F, FALSE);

			if (cell->bg != background)
			{
				dc->SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
			}

			xx += width + t->Spacing();
		}

		xx = at_x + t->Border() + t->Spacing();
		yy += row->max_height + 2*t->Padding() + t->Spacing();
	}

	// Now draw the cell contents...

	t->DrawCells(dc, t->rows);
	t->DrawTags(dc, t->caption);
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableDelete(Table* t)
{
	// Go thru the caption...

	ListOfIterator ilist = t->caption;
	void* a;
	int ok;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;
		DropLine(item->objects);
	}

	// Go thru each row...

	ilist = t->rows;

	for (ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			// Go thru each line...

			ListOfIterator ilist3 = cell->tags;
			void* a3;

			for (int ok3 = ilist3.First(a3); ok3; ok3 = ilist3.Next(a3))
			{
				wxHtml::Tag* item = (wxHtml::Tag *)a3;
				DropLine(item->objects);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Table::Output(ListOf& tags, string& text)
{
	// Go thru each line...

	ListOfIterator ilist = tags;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		wxHtml::Tag* item = (wxHtml::Tag *)a;

		// Go thru each object...

		ListOfIterator ilist2 = item->objects;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			HtmlObject* obj = (HtmlObject*)a2;
			obj->Output(text);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void wxHtml::TableOutput(Table* t, string& text)
{
	text += "\n<TABLE";
	
	if (t->spec_width)
	{
		text += " width=";
		text += Number(t->spec_width);
		text += (t->pc ? "%" : "");
	}

	if (t->set_border)
	{
		text += " border=";
		text += Number(t->Border());
	}

	if (t->set_padding)
	{
		text += " cellpadding=";
		text += Number(t->Padding());
	}

	if (t->set_spacing)
	{
		text += " cellspacing=";
		text += Number(t->Spacing());
	}

#if 0
	if (!IsCompiling())
	{
		text += " cols=";
		text+= Number(t->col_idx + 1);
	}
#endif

	text += ">";

	if (t->caption.Count())
	{
		text += "\n<CAPTION";

		if (t->align_top)
			text += " align=top";
		else if (t->align_bottom)
			text += " align=bottom";

		text += ">";

		t->Output(t->caption, text);

		text += "</CAPTION>";
	}

	// Go thru each row...

	ListOfIterator ilist = t->rows;
	void* a;

	for (int ok = ilist.First(a); ok; ok = ilist.Next(a))
	{
		TableRow* row = (TableRow*)a;

		text += "\n<TR";
		
		if (row->left || row->center || row->right)
		{
			text += " align=";

			if (row->left)
				text += "left";
			else if (row->center)
				text += "center";
			else if (row->right)
				text += "right";
		}

		if (row->top || row->middle || row->bottom)
		{
			text += " valign=";

			if (row->top)
				text += "top";
			else if (row->middle)
				text += "middle";
			else if (row->bottom)
				text += "bottom";
		}

		text += ">";

		// Go thru each cell...

		ListOfIterator ilist2 = row->cells;
		void* a2;

		for (int ok2 = ilist2.First(a2); ok2; ok2 = ilist2.Next(a2))
		{
			TableCell* cell = (TableCell*)a2;

			if (cell->is_header)
			{
				text += "\n<TH";
				cell->html_object->Output(text);
			}
			else
				text += "\n<TD";

			if (cell->left || cell->center || cell->right)
			{
				text += " align=";

				if (cell->left)
					text += "left";
				else if (cell->center)
					text += "center";
				else if (cell->right)
					text += "right";
			}

			if (cell->top || cell->middle || cell->bottom)
			{
				text += " valign=";
	
				if (cell->top)
					text += "top";
				else if (cell->middle)
					text += "middle";
				else if (cell->bottom)
					text += "bottom";
			}

			if (cell->col_span > 1)
				text += " colspan=" + Number(cell->col_span);

			if (cell->row_span > 1)
				text += " rowspan=" + Number(cell->row_span);

			if (cell->bgcolor.length())
				text += " bgcolor=\"" + cell->bgcolor + "\"";

			if (t->col_spec_width[cell->col_nbr])
			{
				text += " width=";
				text += Number(t->col_spec_width[cell->col_nbr]);

				if (t->col_pc[cell->col_nbr])
					text += '%';
			}

			if (cell->nowrap)
				text += " nowrap";

			text += ">";

			t->Output(cell->tags, text);

			if (cell->is_header)
			{
				cell->html_end_object->Output(text);
				text += "</TH>";
			}
			else
				text += "</TD>";
		}

		text += "\n</TR>";
	}

	text += "\n</TABLE>\n";
}

///////////////////////////////////////////////////////////////////////////////

int wxHtml::ParseTable(wxDC* dc, char*&, char* /*buf*/, char* tag)
{
	static Table* current_table = 0;

	if (!tables)
		return 0;

	int status = 0;

	if (!IsCompiling() && !strncmpi(tag, "TABLE", 5))
	{
		Release(dc);
		tabling++;

		if (current_table)
			table_list.Append(current_table);

		if (!left_floating && left_limit_y) { y = left_limit_y; left_limit_y = 0.0F; }
		if (x != left_margin) y += max_h;
		if (!left_floating) left_margin = default_left_margin;
		x = left_margin;
		y += default_left_margin;
		max_h = 0.0F;

		current_table = new Table;
		current_table->current_row = 0;
		current_table->current_cell= 0;
		current_table->row_idx = -1;
		current_table->col_idx = -1;
		current_table->border_width = 0.0F;
		current_table->cell_padding = DEFAULT_CELL_PADDING;
		current_table->cell_spacing = DEFAULT_CELL_SPACING;
		current_table->max_cols = 0;
		current_table->in_row = 0;
		current_table->in_cell = 0;
		current_table->spec_width = 0.0F;
		current_table->pc = FALSE;
		current_table->set_padding = FALSE;
		current_table->set_spacing = FALSE;
		current_table->saved_x = x;
		current_table->saved_y = y;
		current_table->saved_nowrap = nowrap;
		current_table->saved_left_margin = left_margin;
		current_table->saved_default_left_margin = default_left_margin;
		current_table->align_top = FALSE;
		current_table->align_bottom = FALSE;

		nowrap = 1;

		string key;
		string name;
		const char* ptr = tag+5;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "BORDER"))
			{
				current_table->set_border = TRUE;

				if (name.length())
					current_table->border_width = (float)atoi(name.data());
				else
					current_table->border_width = DEFAULT_BORDER_WIDTH;
			}
			else if (!strcmpi(key.data(), "CELLPADDING"))
			{
				current_table->set_padding = TRUE;

				if (name.length())
					current_table->cell_padding = (float)atoi(name.data());
				else
					current_table->cell_padding = DEFAULT_CELL_PADDING;
			}
			else if (!strcmpi(key.data(), "CELLSPACING"))
			{
				current_table->set_spacing = TRUE;

				if (name.length())
					current_table->cell_spacing = (float)atoi(name.data());
				else
					current_table->cell_spacing = DEFAULT_CELL_SPACING;
			}
			else if (!strcmpi(key.data(), "WIDTH"))
			{
				current_table->spec_width = (float)atoi(name.data());

				if (name[name.length()-1] == '%')
					current_table->pc = TRUE;
			}
		}

		for (int i = 0; i < HTML_MAX_COLS; i++)
		{
			current_table->row_span[i] = 0;
			current_table->min_width[i] = 0.0F;
			current_table->max_width[i] = 0.0F;
			current_table->col_spec_width[i] = 0.0F;
			current_table->col_pc[i] = FALSE;
		}

		Hold(current_table->html_object = new HtmlTable(this, x, y, current_table));
		Release(dc);
		void* a;
		tags->Tail(a);
		current_table->saved_tag = (Tag*)a;

		SaveState();
		center = p_center = p_right = 0;
		status = 1;
	}

	if (tabling && !strncmpi(tag, "CAPTION", 7))
	{
		string key;
		string name;
		const char* ptr = tag+7;
		Bool align_top = FALSE;
		Bool align_bottom = FALSE;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
					align_top = TRUE;
				else if (!strcmpi(name.data(), "BOTTOM"))
					align_bottom = TRUE;
			}
		}

		current_table->align_top = align_top;
		current_table->align_bottom = align_bottom;
		current_table->saved_tags = tags;
		tags = &current_table->caption;
		x = y = 0.0F;
		status = 1;
	}

	if (tabling && !strcmpi(tag, "/CAPTION"))
	{
		Release(dc);
		tags = current_table->saved_tags;
		status = 1;
	}

	if (tabling && current_table->in_cell &&
				(!strcmpi(tag, "/TH") ||
					!strcmpi(tag, "/TD") ||
					!strcmpi(tag, "/TR") ||
					!strcmpi(tag, "/TABLE") ||
					!strncmpi(tag, "TR", 2) ||
					!strncmpi(tag, "TD", 2) ||
					!strncmpi(tag, "TH", 2)
				))
	{
		Release(dc);

		current_table->in_cell = FALSE;
		RestoreState();

		TableCell* cell = current_table->current_cell;
		tags = cell->saved_tags;
		cell->GetDimensions(current_table);
			
		Hold(cell->html_end_object = new HtmlEndTableCell(this, x, y));
		Release(dc);
		current_table->current_cell = 0;
		max_h = 0.0F;
		status = 1;
	}

	if (tabling && current_table->in_row &&
				(!strcmpi(tag, "/TR") ||
					!strncmpi(tag, "TR", 2) ||
					!strcmpi(tag, "/TABLE")
				))
	{
		current_table->in_row = FALSE;
		current_table->current_row = 0;
		DropLine(holding_line);
		Hold(new HtmlEndTableRow(this, x, y));
		Release(dc);
		RestoreState();
		status = 1;
	}

	if (tabling && !strncmpi(tag, "TR", 2))
	{
		DropLine(holding_line);

		current_table->col_idx = -1;
		current_table->row_idx++;
		current_table->in_row = TRUE;

		TableRow* row = current_table->current_row = new TableRow;
		row->left = row->center = row->right = FALSE;
		row->top = row->middle = row->bottom = FALSE;
		row->cell_count = 0;
        row->max_height = 0.0F;
        
		string key;
		string name;
		const char* ptr = tag+2;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "ALIGN") ||
						!strcmpi(key.data(), "HALIGN"))
			{
				if (!strcmpi(name.data(), "LEFT"))
				{
					row->left = TRUE;
					row->center = row->right = FALSE;
				}
				else if (!strcmpi(name.data(), "CENTER"))
				{
					row->center = TRUE;
					row->left = row->right = FALSE;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					row->right = TRUE;
					row->center = row->left = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "VALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
				{
					row->top = TRUE;
					row->middle = row->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "MIDDLE"))
				{
					row->middle = TRUE;
					row->top = row->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "BOTTOM"))
				{
					row->bottom = TRUE;
					row->top = row->middle = FALSE;
				}
			}
		}

		current_table->rows.Append(row);
		Hold(new HtmlTableRow(this, x, y, row));
		Release(dc);
		SaveState();
		status = 1;
	}

	if (tabling && current_table->current_row &&
		(!strncmpi(tag, "TH", 2) || !strncmpi(tag, "TD", 2)))
	{
		DropLine(holding_line);

		default_left_margin = 0.0F;
		left_limit_y = right_limit_y = left_margin = 0.0F;
		left_floating = right_floating = 0;
		max_h = 0.0F;

		// Skip over spanned rows...

		while (--current_table->row_span[++current_table->col_idx] > 0)
		{
			current_table->current_row->cell_count++;
		}

		string key;
		string name;
		const char* ptr = tag+2;

		TableCell* cell = current_table->current_cell = new TableCell;
		cell->col_span = cell->row_span = 1;
		cell->width = cell->height = 0.0F;
		cell->left = current_table->current_row->left;
		cell->center = current_table->current_row->center;
		cell->right = current_table->current_row->right;
		cell->top = current_table->current_row->top;
		cell->middle = current_table->current_row->middle;
		cell->bottom = current_table->current_row->bottom;
		cell->bgcolor = "";
		cell->nowrap = FALSE;

		current_table->in_cell = TRUE;

		if (!strncmpi(tag, "TH", 2))
		{
			cell->center = TRUE;
			cell->is_header = TRUE;
		}
		else
			cell->is_header = FALSE;

		while (GetAttributeValuePair(ptr, key, name))
		{
			if (!strcmpi(key.data(), "COLSPAN"))
			{
				cell->col_span = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "ROWSPAN"))
			{
				cell->row_span = atoi(name.data());
			}
			else if (!strcmpi(key.data(), "BGCOLOR"))
			{
				cell->bgcolor = name;
			}
			else if (!strcmpi(key.data(), "NOWRAP"))
			{
				cell->nowrap = TRUE;
			}
			else if (!strcmpi(key.data(), "ALIGN") ||
						!strcmpi(key.data(), "HALIGN"))
			{
				if (!strcmpi(name.data(), "LEFT"))
				{
					cell->left = TRUE;
					cell->center = cell->right = FALSE;
				}
				else if (!strcmpi(name.data(), "CENTER"))
				{
					cell->center = TRUE;
					cell->left = cell->right = FALSE;
				}
				else if (!strcmpi(name.data(), "RIGHT"))
				{
					cell->right = TRUE;
					cell->center = cell->left = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "VALIGN"))
			{
				if (!strcmpi(name.data(), "TOP"))
				{
					cell->top = TRUE;
					cell->middle = cell->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "MIDDLE"))
				{
					cell->middle = TRUE;
					cell->top = cell->bottom = FALSE;
				}
				else if (!strcmpi(name.data(), "BOTTOM"))
				{
					cell->bottom = TRUE;
					cell->top = cell->middle = FALSE;
				}
			}
			else if (!strcmpi(key.data(), "WIDTH"))
			{
				current_table->col_spec_width[current_table->col_idx] = (float)atoi(name.data());

				if (name[name.length()-1] == '%')
					current_table->col_pc[current_table->col_idx] = TRUE;
			}
		}

		if (cell->bgcolor.length())
		{
			string localname;

			if (cell->bgcolor[0] == '#')
			{
				unsigned long rgb = strtoul((cell->bgcolor.data())+1, 0, 16);
				rgb = FindMatchingColour(rgb);
				cell->bg = new wxColour(char((rgb>>16)&0xFF),char((rgb>>8)&0xFF),char(rgb&0xFF));
			}
			else
			{
				cell->bg = HtmlColour(cell->bgcolor.data());
			}

			SetBackground(wxTheBrushList->FindOrCreateBrush(cell->bg, wxSOLID));
		}
		else
			cell->bg = background;

		SaveState();

		if (!strncmpi(tag, "TH", 2))
		{
			mode |= BOLD;
			dc->SetFont(wxTheFontList->FindOrCreateFont(fsz,
				(mode & FIXED ? wxMODERN : family),
				(mode & ITALIC ? wxITALIC : style),
				(mode & BOLD ? wxBOLD : weight),
				(mode & UNDERLINE ? TRUE : FALSE)));
		}

		current_table->current_row->cells.Append(cell);
		current_table->row_span[current_table->col_idx] = cell->row_span;

		Hold(cell->html_object = new HtmlTableCell(this, x, y, cell));
		Release(dc);
		cell->col_nbr = current_table->col_idx;
		cell->saved_tags = tags;
		tags = &cell->tags;
		current_table->col_idx += cell->col_span - 1;
		current_table->max_cols = wxMax(current_table->max_cols, current_table->col_idx+1);
		x = y = 0.0F;
		status = 1;
	}

	if (tabling && !strcmpi(tag, "/TABLE"))
	{
		RestoreState();
		tabling--;

		if (tabling) current_table->PreProcess();
		float width = current_table->width;

		// Wait till any nested tables are done, then recursively
		// do layout...

		if (!tabling)
		{
			current_table->AutoLayout(this, (right_margin - left_margin) - 2*DEFAULT_LEFT_MARGIN);
			current_table->Process();
			current_table->html_object->SetLeft(current_table->saved_x);
			current_table->html_object->SetTop(current_table->saved_y);
			x = current_table->saved_x + current_table->width;
			y = current_table->saved_y + current_table->height;

			nowrap = current_table->saved_nowrap;
			left_margin = current_table->saved_left_margin;
			default_left_margin = current_table->saved_default_left_margin;
		}

		Hold(new HtmlEndTable(this, x, y));
		Release(dc);

		// Now draw it...

		if (!tabling) DrawTags(dc, current_table->saved_tag->objects);

		void* a;
		if (table_list.Remove(a))
		{
			current_table = (Table*)a;

			current_table->max_width[current_table->col_idx] =
				wxMax(current_table->max_width[current_table->col_idx], width);
		}
		else
			current_table = 0;

		SetBackground(wxTheBrushList->FindOrCreateBrush(background, wxSOLID));
		SetBrush(wxTheBrushList->FindOrCreateBrush(background, wxTRANSPARENT));
		overflow = 1;
		status = 1;
	}

	return status;
}


