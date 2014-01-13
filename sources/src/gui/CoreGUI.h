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

#ifndef WEBMONX_COREGUI_H
#define WEBMONX_COREGUI_H

#include "include.h"
#include "core/Core.h"

#include <QUrl>

class CoreGUI : public Core
{
  Q_OBJECT
  
  public:
    CoreGUI();
    inline MainWindow* mainWindow() { return pMainWindow; };
    inline Icons* icons() const { return pIcons; }
    inline SettingsGUI* settingsGUI() const { return pSettingsGUI; };
    inline Sounds* sounds() const { return pSounds; }
    inline TrayIcon* trayIcon() const { return pTrayIcon; };
    virtual void showWarning(const QString& msg, const QString& title = QString()) const;
    virtual void showError(const QString& msg, const QString& title = QString()) const;
    void openUrl(const QUrl& url) const;
    void openAllUpdatedPages();
    
  private slots:
    void actionRequestFinished(Page* page);
    void openNextUpdatedPage();
    
  private:
    Icons* pIcons;
    MainWindow* pMainWindow;
    SettingsGUI* pSettingsGUI;
    Sounds* pSounds;
    TrayIcon* pTrayIcon;
};

#endif // WEBMONX_COREGUI_H
