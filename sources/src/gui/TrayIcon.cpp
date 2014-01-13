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

#include "TrayIcon.h"
#include "CoreGUI.h"
#include "Icons.h"
#include "SettingsGUI.h"
#include "core/Cron.h"
#include "dialogs/MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QMenu>

#if defined Q_WS_WIN
  #include <windows.h>
  #include <winuser.h>
#endif

TrayIcon::TrayIcon(CoreGUI* core, MainWindow* parent) : QObject(core), c(core), w(parent), pUpdated(false)
#if defined Q_WS_WIN
  , minimized(false)
#endif
{
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(c->icons()->get(Icons::WebMonX));
  trayIcon->setToolTip(QApplication::applicationName());
  
  QMenu* contextMenu = new QMenu(c->mainWindow());
  QAction* action;
  
  action = new QAction(this);
  action->setText("Show Window");
  action->setIcon(c->icons()->get(Icons::WebMonX));
  connect(action, SIGNAL(triggered()), SLOT(restoreWindow()));
  contextMenu->addAction(action);
  contextMenu->addSeparator();
  
  action = new QAction(this);
  checkAllAction = action;
  action->setText(tr("Check all Pages"));
  action->setIcon(c->icons()->get(Icons::CheckAll));
  connect(c->cron(), SIGNAL(checkAllStarted()), SLOT(disableCheckAll()));
  connect(c->cron(), SIGNAL(checkAllFinished()), SLOT(enableCheckAll()));
  connect(action, SIGNAL(triggered()), c->cron(), SLOT(checkAllPages()));
  contextMenu->addAction(action);
  contextMenu->addSeparator();
  
  action = new QAction(this);
  action->setText(tr("Quit"));
  action->setIcon(c->icons()->get(Icons::Quit));
  connect(action, SIGNAL(triggered()), c->mainWindow(), SLOT(close()));
  contextMenu->addAction(action);
  
  trayIcon->setContextMenu(contextMenu);
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(clicked(QSystemTrayIcon::ActivationReason)));
  connect(c, SIGNAL(updatedStatusChanged(bool)), SLOT(updateTrayIcon(bool)));
  trayIcon->show();
}

bool TrayIcon::minimize() {
  if (c->settings()->get("visual/minimizetotray").toBool()) {
#if defined Q_WS_WIN
    minimized = true;
    QTimer::singleShot(100, this, SLOT(hideWindow()));
#else
    w->setVisible(false);
#endif
    return false;
  }
  else {
    return true;
  }
}

bool TrayIcon::close() {
  if (c->settings()->get("visual/closetotray").toBool()) {
    w->setVisible(false);
    return false;
  }
  else {
    return true;
  }
}

void TrayIcon::clicked(QSystemTrayIcon::ActivationReason reason) {
  if (reason==QSystemTrayIcon::MiddleClick) {
    c->openAllUpdatedPages();
  }
  else if (reason!=QSystemTrayIcon::Context) {
#if defined Q_WS_WIN
    if (minimized || !w->isVisible()) {
      minimized = false;
      QTimer::singleShot(100, this, SLOT(restoreWindow()));
    }
#else
    if (!w->isVisible()) {
      restoreWindow();
    }
#endif
    else {
      w->setVisible(false);
    }
  }
}

void TrayIcon::updateTrayIcon(bool updated) {
  if (updated && !pUpdated) {
    trayIcon->setIcon(c->icons()->get(Icons::WebMonXUpdated));
    pUpdated = true;
  }
  else if (pUpdated) {
    trayIcon->setIcon(c->icons()->get(Icons::WebMonX));
    pUpdated = false;
  }
}

void TrayIcon::restoreWindow() {
  w->showNormal();
  w->activateWindow();
}

#if defined Q_WS_WIN
void TrayIcon::hideWindow() {
  ShowWindow(w->winId(), SW_HIDE);
}
#endif

void TrayIcon::enableCheckAll() {
  checkAllAction->setEnabled(true);
}

void TrayIcon::disableCheckAll() {
  checkAllAction->setEnabled(false);
}
