/*
 * WebMonX
 *  http://debfx.fobos.de/webmonx/
 *
 * Copyright (C) 2006-2010 Felix Geyer <debfx-webmonx@fobos.de>
 *
 * This file is part of WebMonX.
 *
 * WebMonX is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 or (at your option) version 3 as published by
 * the Free Software Foundation.
 *
 * WebMonX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>
 * or write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef WEBMONX_POPUP_H
#define WEBMONX_POPUP_H

#include "gui/include.h"

#include <QFrame>

class QLabel;
class QMouseEvent;

class PopUp : public QFrame
{
  Q_OBJECT
  
  public:
    PopUp(CoreGUI* core, QWidget* parent, Page* p);
    
  public slots:
    void show();
  
  protected:
    void mouseReleaseEvent(QMouseEvent* event);
  
  private:
    CoreGUI* c;
    Page* page;
    QLabel* la;
    LinkLabel* ll;
};

#endif // WEBMONX_POPUP_H
