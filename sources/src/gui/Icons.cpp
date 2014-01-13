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

#include "Icons.h"
#include "CoreGUI.h"

#include <QApplication>
#include <QFile>

Icons::Icons(CoreGUI* core) : QObject(core), pAvailable(true) {
  QString dirPath = QApplication::applicationDirPath();
  QString iconPath;
  
#if defined Q_WS_X11
  if (QFile::exists(dirPath+"/../share/apps/webmonx/icons")) // linux
    iconPath = dirPath+"/../share/apps/webmonx/icons/";
#elif defined Q_WS_WIN
  if (QFile::exists(dirPath+"/icons/")) // windows
    iconPath = dirPath+"/icons/";
#else
  if (false) ;
#endif
  else if (QFile::exists(dirPath+"/../share/icons")) // dev
    iconPath = dirPath+"/../share/icons/";
  else {
    pAvailable = false;
    core->showWarning(tr("Icons not found!"));
  }
  
  if (pAvailable) {
    icons.insert(WebMonX, QIcon(iconPath+"16x16/webmonx.png"));
    icons.insert(WebMonXUpdated, QIcon(iconPath+"16x16/webmonx_update.png"));
    icons.insert(WebMonX32, QIcon(iconPath+"32x32/webmonx.png"));
    icons.insert(CheckAll, QIcon(iconPath+"24x24/check_all.png"));
    icons.insert(CheckCancel, QIcon(iconPath+"24x24/check_cancel.png"));
    icons.insert(CheckSelected, QIcon(iconPath+"24x24/check_selected.png"));
    icons.insert(Configure, QIcon(iconPath+"16x16/configure.png"));
    icons.insert(Quit, QIcon(iconPath+"16x16/quit.png"));
    icons.insert(EntryAdd, QIcon(iconPath+"24x24/entry_add.png"));
    icons.insert(EntryDelete, QIcon(iconPath+"24x24/entry_delete.png"));
    icons.insert(EntryModify, QIcon(iconPath+"24x24/entry_modify.png"));
    icons.insert(EntryToggle, QIcon(iconPath+"24x24/entry_toggle.png"));
    icons.insert(Entry, QIcon(iconPath+"16x16/entry.png"));
    icons.insert(EntryDisabled, QIcon(iconPath+"16x16/entry_disabled.png"));
    icons.insert(EntryError, QIcon(iconPath+"16x16/entry_error.png"));
    icons.insert(EntryErrorDisabled, QIcon(iconPath+"16x16/entry_error_disabled.png"));
    icons.insert(EntryUpdated, QIcon(iconPath+"16x16/entry_updated.png"));
    icons.insert(EntryUpdatedDisabled, QIcon(iconPath+"16x16/entry_updated_disabled.png"));
    icons.insert(EntryChecking, QIcon(iconPath+"16x16/entry_checking.png"));
  }
}
