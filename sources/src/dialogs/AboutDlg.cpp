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

#include "AboutDlg.h"
#include "gui/CoreGUI.h"
#include "gui/Icons.h"
#include "gui/LinkLabel.h"

#include <QScrollArea>

AboutDlg::AboutDlg(CoreGUI* c, QWidget* parent) : QDialog(parent) {
  setupUi(this);
  labelIcon->setPixmap( c->icons()->get(Icons::WebMonX32).pixmap(32,32) );
  
  labelLibs->setText(tr(
    "Qt/%1 (GPL / LGPL)\n"
    "Icons adapted from Crystal SVG (LGPLv2)\n"
    "Sounds from KDE4: KNotify (GPLv2)\n"
  ).arg(qVersion()));
  
  labelVersion->setText(
    QString::fromUtf8("<span style=\"font-size:10pt;font-weight:bold;\">WebMonX %1</span>").arg(Core::VERSION_STR)
  );
  layoutLicense = new QVBoxLayout(tabLicense);
  scrollArea = new QScrollArea(tabLicense);
  layoutLicense->addWidget(scrollArea);
  labelLicense = new QLabel(scrollArea);
  labelLicense->setText(
    "WebMonX is free software; you can redistribute it and/or\n"
    "modify it under the terms of the GNU General Public License\n"
    "version 2 or (at your option) version 3 as published by\n"
    "the Free Software Foundation."
    "\n"
    "WebMonX is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
    "See the GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program; if not, see <http://www.gnu.org/licenses/>\n"
    "or write to the Free Software Foundation, Inc.,\n"
    "59 Temple Place - Suite 330, Boston, MA  02111-1307, USA."
  );
  scrollArea->setWidget(labelLicense);
  
  QString url = labelA5->text();
  QRect geo = labelA5->geometry();
  delete labelA5;
  labelA5 = new LinkLabel(c, url, tabAbout);
  labelA5->setGeometry(geo);
  
  setFixedSize( size() );
  
  connect(buttonOk, SIGNAL(clicked()), SLOT(close()));
}
