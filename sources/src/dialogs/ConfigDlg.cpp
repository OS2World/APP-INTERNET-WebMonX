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

#include "ConfigDlg.h"

#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"
#include "ConfigUpdateAlerts.h"
#include "ConfigErrorAlerts.h"
#include "ConfigVisual.h"
#include "ConfigProxy.h"
#include "ConfigConnections.h"
#include "ConfigWebBrowser.h"

ConfigDlg::ConfigDlg(CoreGUI* core, QWidget* parent) : QDialog(parent), c(core) {
  setupUi(this);
  setGeometry(c->settingsGUI()->loadWindow(this));
  
  connect(listWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
  
  connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
  
  updateAlerts = new ConfigUpdateAlerts(c, stackedWidget);
  stackedWidget->addWidget(updateAlerts);
  
  errorAlerts = new ConfigErrorAlerts(c, stackedWidget);
  stackedWidget->addWidget(errorAlerts);
  
  visual = new ConfigVisual(c, stackedWidget);
  stackedWidget->addWidget(visual);
  
  proxy = new ConfigProxy(c, stackedWidget);
  stackedWidget->addWidget(proxy);
  
  connections = new ConfigConnections(c, stackedWidget);
  stackedWidget->addWidget(connections);
  
  webBrowser = new ConfigWebBrowser(c, stackedWidget);
  stackedWidget->addWidget(webBrowser);
  
  c->settingsGUI()->load("config/page", listWidget);
}

void ConfigDlg::accept() {
  c->settingsGUI()->save("config/page", listWidget);
  
  updateAlerts->save();
  errorAlerts->save();
  visual->save();
  proxy->save();
  connections->save();
  webBrowser->save();
  
  QDialog::accept();
}

// workaround for accept() and reject() only hiding the dialog
void ConfigDlg::done(int r) {
  QDialog::done(r);
  close();
}

void ConfigDlg::closeEvent(QCloseEvent* event) {
  c->settingsGUI()->saveWindow(this);
  QDialog::closeEvent(event);
}
