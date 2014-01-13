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

#include "LinkLabel.h"
#include "gui/CoreGUI.h"

#include <QMouseEvent>

LinkLabel::LinkLabel(CoreGUI* core, QWidget* parent, Qt::WFlags f) : QLabel(parent, f), c(core) {
  init();
}

LinkLabel::LinkLabel(CoreGUI* core, const QString& text, QWidget* parent, Qt::WFlags f)
  : QLabel(text, parent, f), c(core)
{
  init();
}

void LinkLabel::open() {
  c->openUrl( pUrl.isEmpty() ? QUrl(text()) : pUrl );
  
  emit opened();
}

void LinkLabel::init() {
  QPalette p = palette();
  p.setBrush(QPalette::WindowText, p.link());
  setPalette(p);
  
  QFont f = font();
  f.setUnderline(true);
  setFont(f);
  
  setCursor(Qt::PointingHandCursor);
}

void LinkLabel::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button()==Qt::LeftButton)
    open();
}
