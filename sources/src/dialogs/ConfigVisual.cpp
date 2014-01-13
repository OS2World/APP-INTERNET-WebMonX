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

#include "ConfigVisual.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"

ConfigVisual::ConfigVisual(CoreGUI* core, QWidget* parent) : QWidget(parent), c(core) {
  setupUi(this);
  
  c->settingsGUI()->load("visual/toolbar", checkToolbar);
  c->settingsGUI()->load("visual/minimized", checkMinimized);
  c->settingsGUI()->load("visual/minimizetotray", checkMinTray);
  c->settingsGUI()->load("visual/closetotray", checkCloseTray);
  c->settingsGUI()->load("visual/stayontop", checkOnTop);
  c->settingsGUI()->load("visual/sorting/alpha", radioAlpha);
  c->settingsGUI()->load("visual/sorting/status", radioStatus);
}

void ConfigVisual::save() {
  c->settingsGUI()->save("visual/toolbar", checkToolbar);
  c->settingsGUI()->save("visual/minimized", checkMinimized);
  c->settingsGUI()->save("visual/minimizetotray", checkMinTray);
  c->settingsGUI()->save("visual/closetotray", checkCloseTray);
  c->settingsGUI()->save("visual/stayontop", checkOnTop);
  c->settingsGUI()->save("visual/sorting/alpha", radioAlpha);
  c->settingsGUI()->save("visual/sorting/status", radioStatus);
}
