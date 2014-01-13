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

#include "ConfigWebBrowser.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"

#include <QFileDialog>

ConfigWebBrowser::ConfigWebBrowser(CoreGUI* core, QWidget* parent) : QWidget(parent), c(core) {
  setupUi(this);
  
  connect(radioSpecify, SIGNAL(toggled(bool)), editSpecify, SLOT(setEnabled(bool)));
  connect(radioSpecify, SIGNAL(toggled(bool)), buttonBrowse, SLOT(setEnabled(bool)));
  connect(buttonBrowse, SIGNAL(clicked()), SLOT(browseFile()));
  
  c->settingsGUI()->load("browser/autodetect", radioAutoDetect);
  c->settingsGUI()->load("browser/custom", radioSpecify);
  c->settingsGUI()->load("browser/file", editSpecify);
}

void ConfigWebBrowser::save() {
  c->settingsGUI()->save("browser/autodetect", radioAutoDetect);
  c->settingsGUI()->save("browser/custom", radioSpecify);
  c->settingsGUI()->save("browser/file", editSpecify);
}

void ConfigWebBrowser::browseFile() {
  QString file(
    QFileDialog::getOpenFileName(this, tr("Select an executable"))
  );
  if (!file.isEmpty())
    editSpecify->setText(file);
}
