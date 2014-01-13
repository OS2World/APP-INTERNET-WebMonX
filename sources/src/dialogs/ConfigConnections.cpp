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

#include "ConfigConnections.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"

ConfigConnections::ConfigConnections(CoreGUI* core, QWidget* parent) : QWidget(parent), c(core) {
  setupUi(this);
  
  connect(sliderCon, SIGNAL(valueChanged(int)), SLOT(updateConLabel(int)));
  connect(sliderTransfer, SIGNAL(valueChanged(int)), SLOT(updateTransferLabel(int)));
  
  c->settingsGUI()->load("connections/connection", sliderCon);
  c->settingsGUI()->load("connections/transfer", sliderTransfer);
  c->settingsGUI()->load("connections/retry", checkRetry);
  
  updateConLabel(sliderCon->value());
  updateTransferLabel(sliderTransfer->value());
}

void ConfigConnections::save() {
  c->settingsGUI()->save("connections/connection", sliderCon);
  c->settingsGUI()->save("connections/transfer", sliderTransfer);
  c->settingsGUI()->save("connections/retry", checkRetry);
}

void ConfigConnections::updateConLabel(int value) {
  labelCon->setText(tr("Connection timeout: %1 secs").arg(value));
}

void ConfigConnections::updateTransferLabel(int value) {
  labelTransfer->setText(tr("Transfer timeout: %1 secs").arg(value));
}
