/*
** File:	painter.h
**
** Author:	Sean Congden
**
** Description:	Basic drawing functions
*/

#ifndef _PAINTER24_H
#define _PAINTER24_H

#include "painter.h"

class Painter24 : public Painter {
public:

	Painter24(Screen *screen, Rect bounds);
	virtual ~Painter24(void);

	void SetPixel(Int32 x, Int32 y, Uint32 color);
	void FillRect(Rect bounds, Uint32 color);
	void Fill(Uint32 color);

	void DrawCursor(Int32 x, Int32 y);
};

#endif