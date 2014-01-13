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

#include "PropertiesDlg.h"
#include "core/Page.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"

PropertiesDlg::PropertiesDlg(CoreGUI* core, QWidget* parent, const PageData* data) : QDialog(parent), c(core) {
  setupUi(this);
  setGeometry(c->settingsGUI()->loadWindow(this));
  
  connect(buttonOk, SIGNAL(clicked()), SLOT(close()));
  
  labelValDescr->setText(data->description);
  labelValAddr->setText(data->address);
  labelValStatus->setText(data->statusMsg);
  labelValLast->setText(data->lastChecked.toString(Qt::SystemLocaleDate));
  labelValNext->setText( data->lastChecked.addSecs(data->interval*60).toString(Qt::SystemLocaleDate) );
  labelValMod->setText(data->lastModified.toString(Qt::SystemLocaleDate));
  labelValCheck->setText(data->hash);
}

void PropertiesDlg::closeEvent(QCloseEvent* event) {
  c->settingsGUI()->saveWindow(this);
  
  QDialog::closeEvent(event);
}
