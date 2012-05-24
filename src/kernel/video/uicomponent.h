/*
** File:	uicomponent.h
**
** Author:	Sean Congden
**
** Description:	
*/

#ifndef _UI_COMPONENT_H
#define _UI_COMPONENT_H

extern "C" {
	#include "linkedlist.h"
	#include "input/mouse.h"
}

#include "painter.h"

class UIComponent {
public:

	UIComponent(Painter *painter, Rect bounds);
	virtual ~UIComponent(void);

	void AddComponent(UIComponent *component);

	virtual void Move(Int32 x, Int32 y);

	void Invalidate(void);

	Rect& GetBounds(void) { return bounds; }
	bool IsDirty(void) { return dirty; }

	/*
	** HandleMouseEvent(event)
	**
	** Handles a mouse event generated when the window has
	** focus at the given x and y coordinates.
	*/
	virtual void HandleMouseEvent(MouseEvent *event);

protected:

	virtual void Draw(void) = 0;

	Painter *painter;
	Rect bounds;

private:
	void Repaint(void);

	LinkedList *children;
	bool dirty;

	// Desktop should be the only class allowed to call Repaint().  Other
	// classes should call Invalidate() to mark the compontent as dirty.
	friend class Desktop;
};

#endif