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

#include "PopUp.h"
#include "core/Page.h"
#include "gui/CoreGUI.h"
#include "gui/Icons.h"
#include "gui/LinkLabel.h"
#include "gui/SettingsGUI.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>

PopUp::PopUp(CoreGUI* core, QWidget* parent, Page* p) : QFrame(parent), c(core), page(p) {
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  setFrameShape(QFrame::Box);
  setWindowTitle("WebMonX Update");
  
  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor(255,255,231));
  setPalette(pal);
  setAutoFillBackground(true);
  
  resize( QSize(181,65) );
  
  QRect r = QApplication::desktop()->availableGeometry(this);
  switch (c->settings()->get("update/popup/position").toInt()) {
    case 0:
      move( r.topLeft() ); // top left
      break;
    case 1:
      move( r.topRight() - QPoint(width(), 0) ); // top right
      break;
    case 2:
      move( r.center() - rect().center() ); // center
      break;
    case 3:
      move( r.bottomLeft() - QPoint(0, width()) ); // bottom left
      break;
    case 4:
      move( r.bottomRight() - QPoint(width(), height()) ); // bottom right
      break;
  }
  
  QGridLayout* layout = new QGridLayout;
  
  la = new QLabel(this);
  la->setPixmap(c->icons()->get(Icons::WebMonX32).pixmap(32,32));
  layout->addWidget(la, 0, 0, 2, 1);
  
  la = new QLabel(tr("Updated web page:"), this);
  layout->addWidget(la, 0, 1);
  
  ll = new LinkLabel(c, page->data()->description, this);
  if (page->data()->hasAltAddr)
    ll->setUrl(page->data()->altAddr);
  else
    ll->setUrl(page->data()->address);
  layout->addWidget(ll, 1, 1);
  connect(ll, SIGNAL(opened()), page, SLOT(resetUpdated()));
  connect(ll, SIGNAL(opened()), SLOT(close()));
  
  setLayout(layout);
}

void PopUp::show() {
  if (c->settings()->get("update/popup/autoclose").toBool())
    QTimer::singleShot(5000, this, SLOT(close()));
  
  QFrame::show();
}

void PopUp::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button()==Qt::LeftButton)
    ll->open();
  
  QFrame::mouseReleaseEvent(event);
}
