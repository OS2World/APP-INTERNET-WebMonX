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

#ifndef WEBMONX_MAINWINDOW_H
#define WEBMONX_MAINWINDOW_H

#include "gui/include.h"
#include "ui_MainWindow.h"

#include <QHash>

class QCloseEvent;
class QHideEvent;
class QLabel;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
  
  public:
    MainWindow(CoreGUI* core);
  
  public slots:
    void openPage(Page* page);
  
  private slots:
    void displayContextMenu(QPoint pos);
    void selectionChanged(QList<Page*> selected);
    void itemCountChanged(int count);
    void addItem();
    void modifyItem();
    void deleteItem();
    void toggleItem();
    void copyAddress();
    void showAboutDialog();
    void checkSelected();
    void openPage();
    void checkAll();
    void showPreferences();
    void showProperties();
    void enableCheckAll();
  
  protected:
    void closeEvent(QCloseEvent* event);
    void hideEvent(QHideEvent* event);
  
  private:
    enum Action {
      ActionOffline, ActionQuit,
      ActionAdd, ActionModify, ActionDelete, ActionToggle, ActionCopy, ActionCheckSelected, ActionProperties,
      ActionCheckAll, ActionCancelCheck, ActionPreferences,
      ActionAbout,
      ActionOpen
    };
    CoreGUI* c;
    SettingsGUI* s;
    PagesWidget* pagesWidget;
    QLabel* statusBarLabel;
    QHash<Action,QAction*> actions;
    QMenu* contextMenu;
    #if defined Q_WS_WIN
    bool minimized;
    #endif
    void addActions();
};

#endif // WEBMONX_MAINWINDOW_H
