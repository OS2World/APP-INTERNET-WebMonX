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

#include "MainWindow.h"
#include "core/Cron.h"
#include "core/Page.h"
#include "gui/CoreGUI.h"
#include "gui/Icons.h"
#include "gui/PagesWidget.h"
#include "gui/SettingsGUI.h"
#include "gui/TrayIcon.h"
#include "dialogs/AboutDlg.h"
#include "dialogs/ConfigDlg.h"
#include "dialogs/PageDlg.h"
#include "dialogs/PropertiesDlg.h"

#include <QClipboard>
#include <QCloseEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QUrl>

MainWindow::MainWindow(CoreGUI* core) : c(core), s(core->settingsGUI()) {
  setupUi(this);
  pagesWidget = new PagesWidget(c, centralwidget);
  pagesWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  vboxLayout->addWidget(pagesWidget);
  setWindowIcon(c->icons()->get(Icons::WebMonX));
  QApplication::setWindowIcon(c->icons()->get(Icons::WebMonX));
  contextMenu = new QMenu(this);
  setGeometry(s->loadWindow(this));
  if (s->get("visual/stayontop").toBool())
    setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
  
  addActions();
  toolbar->toggleViewAction()->setVisible(false); // TODO don't hide and make a entry in view menu
  connect(pagesWidget, SIGNAL(contextMenu(QPoint)), SLOT(displayContextMenu(QPoint)));
  connect(pagesWidget, SIGNAL(selectionChanged(QList<Page*>)), SLOT(selectionChanged(QList<Page*>)));
  connect(c, SIGNAL(pageCountChanged(int)), SLOT(itemCountChanged(int)));
  connect(pagesWidget, SIGNAL(doubleClicked(Page*)), SLOT(openPage(Page*)));
  
  QMetaObject::invokeMethod(s, "loadPages", Qt::QueuedConnection);
}

void MainWindow::displayContextMenu(QPoint pos) {
  contextMenu->popup(pos);
}

void MainWindow::selectionChanged(QList<Page*> selected) {
  int count = selected.count();
  
  bool hasSelected = (count!=0);
  bool hasOneSelected = (count==1);
  
  if (hasOneSelected) {
    const PageData* data = selected[0]->data();
    if (data->error)
      statusbar->showMessage(data->statusMsg);
    else if (data->hasAltAddr)
      statusbar->showMessage(data->altAddr);
    else
      statusbar->showMessage(data->address);
  }
  else if (hasSelected)
    statusbar->showMessage(tr("%1 selected items").arg(count));
  else
    statusbar->showMessage("");
  
  actions[ActionModify]->setEnabled(hasOneSelected);
  actions[ActionDelete]->setEnabled(hasSelected);
  actions[ActionToggle]->setEnabled(hasSelected);
  actions[ActionCopy]->setEnabled(hasOneSelected);
  actions[ActionCheckSelected]->setEnabled(hasSelected);
  actions[ActionProperties]->setEnabled(hasOneSelected);
  actions[ActionOpen]->setEnabled(hasSelected);
}

void MainWindow::itemCountChanged(int count) {
  actions[ActionCheckAll]->setEnabled( count!=0 );
}

void MainWindow::addItem() {
  PageData data;
  PageDlg* pageDlg = new PageDlg(c, this, tr("Add Page"), &data);
  if (pageDlg->exec()==QDialog::Accepted) {
    c->createPage(&data);
  }
  delete pageDlg;
}

void MainWindow::modifyItem() {
  QList<Page*> selected = pagesWidget->selectedPages();
  if (selected.count()!=1) return;
  
  PageData data( *selected[0]->data() );
  PageDlg* pageDlg = new PageDlg(c, this, tr("Modify Page"), &data);
  if (pageDlg->exec() == QDialog::Accepted) {
    selected[0]->setData(data);
    selectionChanged(selected);
  }
  delete pageDlg;
}

void MainWindow::deleteItem() {
  QList<Page*> selected = pagesWidget->selectedPages();
  int count = selected.count();
  const QString itemStr = (count==1)?tr("item"):tr("items");
  
  QString msg = tr("Really delete %1 ").arg(itemStr);
  for (int i=0; i<count; ++i) {
    if ( i!=0 && (i%3)==0 )
      msg.append("\n");
    msg.append( "'"+selected[i]->data()->description+"'" );
    if (i==(count-2))
      msg.append(tr(" and "));
    else if (i!=(count-1))
      msg.append(", ");
  }
  
  int answer = QMessageBox::question(
    this,
    tr("Delete %1").arg(itemStr),
    msg.append(tr("?")),
    QMessageBox::Yes,
    QMessageBox::No|QMessageBox::Default
  );
  
  if (answer==QMessageBox::Yes) {
    for (int i=0; i<count; ++i) {
      delete selected[i];
    }
    s->savePages();
  }
}

void MainWindow::toggleItem() {
  QList<Page*> selected = pagesWidget->selectedPages();
  int count = selected.count();
  for (int i=0; i<count; ++i) {
    PageData data(*selected[i]->data());
    data.enabled = !data.enabled;
    selected[i]->setData(data);
  }
}

void MainWindow::copyAddress() {
  QList<Page*> selected = pagesWidget->selectedPages();
  if (selected.count()!=1) return;
  
  QApplication::clipboard()->setText( selected[0]->data()->address );
}

void MainWindow::showAboutDialog() {
  AboutDlg* aboutDlg = new AboutDlg(c, this);
  aboutDlg->exec();
  delete aboutDlg;
}

void MainWindow::checkSelected() {
  QList<Page*> selected = pagesWidget->selectedPages();
  
  for (QList<Page*>::const_iterator i = selected.begin(); i != selected.end(); ++i) {
    if (!(*i)->isChecking())
      (*i)->check();
  }
}

void MainWindow::openPage(Page* page) {
  if (page->data()->updated) {
    page->resetUpdated();
  }
  
  if (page->data()->hasAltAddr)
    c->openUrl( QUrl(page->data()->altAddr) );
  else
    c->openUrl( QUrl(page->data()->address) );
}

void MainWindow::openPage() {
  QList<Page*> selected = pagesWidget->selectedPages();
  
  for (QList<Page*>::const_iterator i = selected.begin(); i != selected.end(); ++i) {
    openPage(*i);
  }
}

void MainWindow::checkAll() {
  actions[ActionCheckAll]->setEnabled(false);
  c->cron()->checkAllPages();
}

void MainWindow::showPreferences() {
  bool toolbarVisible = s->get("visual/toolbar").toBool();
  bool sortAlpha = s->get("visual/sorting/alpha").toBool();
  bool stayOnTop = s->get("visual/stayontop").toBool();
  
  ConfigDlg* configDlg = new ConfigDlg(c, this);
  configDlg->exec();
  delete configDlg;
  
  if (toolbarVisible != s->get("visual/toolbar").toBool())
    toolbar->setVisible(s->get("visual/toolbar").toBool());
  
  if (sortAlpha != s->get("visual/sorting/alpha").toBool())
    pagesWidget->sortItems();
  
  if (!stayOnTop && s->get("visual/stayontop").toBool())
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  else if (stayOnTop && !s->get("visual/stayontop").toBool())
    setWindowFlags(windowFlags() & !Qt::WindowStaysOnTopHint);
}

void MainWindow::showProperties() {
  QList<Page*> selected = pagesWidget->selectedPages();
  if (selected.count()!=1) return;
  
  PropertiesDlg* propertiesDlg = new PropertiesDlg(c, this, selected[0]->data());
  propertiesDlg->exec();
  delete propertiesDlg;
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (event->spontaneous() && !c->trayIcon()->close()) {
    event->ignore();
    return;
  }
  
  c->settingsGUI()->saveWindow(this);
  s->savePages();
  
  QMainWindow::closeEvent(event);
  
  QCoreApplication::quit();
}

void MainWindow::hideEvent(QHideEvent* event) {
  if (event->spontaneous() && isMinimized() && !c->trayIcon()->minimize()) {
    event->ignore();
    return;
  }
  
  QMainWindow::hideEvent(event);
}

void MainWindow::addActions() {
  QAction* action;
  
  action = new QAction(this);
  action->setText(tr("Work &Offline"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
  action->setCheckable(true);
  action->setChecked(c->cron()->isOffline());
  actions.insert(ActionOffline, action);
  connect(action, SIGNAL(triggered()), c->cron(), SLOT(toggleOffline()));
  
  action = new QAction(this);
  action->setText(tr("&Quit"));
  action->setIconText(tr("Quit"));
  action->setIcon(c->icons()->get(Icons::Quit));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  actions.insert(ActionQuit, action);
  connect(action, SIGNAL(triggered()), SLOT(close()));
  
  action = new QAction(this);
  action->setText(tr("&Add"));
  action->setIconText(tr("Add"));
  action->setToolTip(tr("Add page"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setIcon(c->icons()->get(Icons::EntryAdd));
  actions.insert(ActionAdd, action);
  connect(action, SIGNAL(triggered()), SLOT(addItem()));
  
  action = new QAction(this);
  action->setText(tr("&Modify"));
  action->setIconText(tr("Modify"));
  action->setToolTip(tr("Modify page"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_M));
  action->setIcon(c->icons()->get(Icons::EntryModify));
  action->setEnabled(false);
  actions.insert(ActionModify, action);
  connect(action, SIGNAL(triggered()), SLOT(modifyItem()));
  
  action = new QAction(this);
  action->setText(tr("&Delete"));
  action->setIconText(tr("Delete"));
  action->setToolTip(tr("Delete page"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setIcon(c->icons()->get(Icons::EntryDelete));
  action->setEnabled(false);
  actions.insert(ActionDelete, action);
  connect(action, SIGNAL(triggered()), SLOT(deleteItem()));
  
  action = new QAction(this);
  action->setText(tr("&Toggle Enable/Disable"));
  action->setIconText(tr("Toggle"));
  action->setToolTip(tr("Toggle Enable/Disable"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
  action->setIcon(c->icons()->get(Icons::EntryToggle));
  action->setEnabled(false);
  actions.insert(ActionToggle, action);
  connect(action, SIGNAL(triggered()), SLOT(toggleItem()));
  
  action = new QAction(this);
  action->setText(tr("&Copy Address"));
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setEnabled(false);
  actions.insert(ActionCopy, action);
  connect(action, SIGNAL(triggered()), SLOT(copyAddress()));
  
  action = new QAction(this);
  action->setText(tr("Check &selected Page(s)"));
  action->setIconText(tr("Check"));
  action->setToolTip(tr("Check selected Page(s)"));
  action->setShortcut(QKeySequence(Qt::Key_F5));
  action->setIcon(c->icons()->get(Icons::CheckSelected));
  action->setEnabled(false);
  actions.insert(ActionCheckSelected, action);
  connect(action, SIGNAL(triggered()), SLOT(checkSelected()));
  
  action = new QAction(this);
  action->setText(tr("&Properties"));
  action->setEnabled(false);
  actions.insert(ActionProperties, action);
  connect(action, SIGNAL(triggered()), SLOT(showProperties()));
  
  action = new QAction(this);
  action->setText(tr("Check &all Pages"));
  action->setIconText(tr("Check All"));
  action->setToolTip(tr("Check all Pages"));
  action->setShortcut(QKeySequence(Qt::Key_F6));
  action->setIcon(c->icons()->get(Icons::CheckAll));
  action->setEnabled(false);
  actions.insert(ActionCheckAll, action);
  connect(c->cron(), SIGNAL(checkAllFinished()), SLOT(enableCheckAll()));
  connect(action, SIGNAL(triggered()), SLOT(checkAll()));
  
  action = new QAction(this);
  action->setText(tr("&Cancel current check(s)"));
  action->setIconText(tr("Cancel"));
  action->setToolTip(tr("Cancel current check(s)"));
  action->setIcon(c->icons()->get(Icons::CheckCancel));
  action->setEnabled(false);
  actions.insert(ActionCancelCheck, action);
  connect(c, SIGNAL(checkStatusChanged(bool)), action, SLOT(setEnabled(bool)));
  connect(action, SIGNAL(triggered()), c, SLOT(cancelChecks()));
  
  action = new QAction(this);
  action->setText(tr("&Preferences"));
  action->setIcon(c->icons()->get(Icons::Configure));
  actions.insert(ActionPreferences, action);
  connect(action, SIGNAL(triggered()), SLOT(showPreferences()));
  
  action = new QAction(this);
  action->setText(tr("&About"));
  action->setIcon(c->icons()->get(Icons::WebMonX));
  actions.insert(ActionAbout, action);
  connect(action, SIGNAL(triggered()), SLOT(showAboutDialog()));
  
  action = new QAction(this);
  action->setText(tr("&Open"));
  action->setEnabled(false);
  actions.insert(ActionOpen, action);
  connect(action, SIGNAL(triggered()), SLOT(openPage()));
  
  menuFile->addAction(actions[ActionOffline]);
  menuFile->addSeparator();
  menuFile->addAction(actions[ActionQuit]);
  
  menuPage->addAction(actions[ActionAdd]);
  menuPage->addAction(actions[ActionModify]);
  menuPage->addAction(actions[ActionDelete]);
  menuPage->addAction(actions[ActionToggle]);
  menuPage->addAction(actions[ActionCopy]);
  menuPage->addSeparator();
  menuPage->addAction(actions[ActionCheckSelected]);
  menuPage->addSeparator();
  menuPage->addAction(actions[ActionProperties]);
  
  menuTools->addAction(actions[ActionCheckAll]);
  menuTools->addAction(actions[ActionCancelCheck]);
  menuTools->addSeparator();
  menuTools->addAction(actions[ActionPreferences]);
  
  menuHelp->addAction(actions[ActionAbout]);
  
  toolbar->addAction(actions[ActionAdd]);
  toolbar->addAction(actions[ActionModify]);
  toolbar->addAction(actions[ActionDelete]);
  toolbar->addAction(actions[ActionToggle]);
  toolbar->addSeparator();
  toolbar->addAction(actions[ActionCheckSelected]);
  toolbar->addAction(actions[ActionCheckAll]);
  toolbar->addAction(actions[ActionCancelCheck]);
  
  contextMenu->addAction(actions[ActionOpen]);
  contextMenu->addAction(actions[ActionCheckSelected]);
  contextMenu->addSeparator();
  contextMenu->addAction(actions[ActionModify]);
  contextMenu->addAction(actions[ActionDelete]);
  contextMenu->addAction(actions[ActionToggle]);
  contextMenu->addAction(actions[ActionCopy]);
  contextMenu->addSeparator();
  contextMenu->addAction(actions[ActionProperties]);
  contextMenu->setDefaultAction(actions[ActionOpen]);
}

void MainWindow::enableCheckAll() {
  actions[ActionCheckAll]->setEnabled(true);
}
