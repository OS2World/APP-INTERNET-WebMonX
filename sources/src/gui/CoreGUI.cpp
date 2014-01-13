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

#include "CoreGUI.h"
#include "Icons.h"
#include "SettingsGUI.h"
#include "Sounds.h"
#include "TrayIcon.h"
#include "core/Page.h"
#include "dialogs/MainWindow.h"
#include "dialogs/PopUp.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

#if defined Q_WS_WIN
  #include <shlobj.h>
#endif

CoreGUI::CoreGUI() : Core(NULL), pMainWindow(NULL) {
  QApplication::setQuitOnLastWindowClosed(false);
  QStringList args = QCoreApplication::arguments();
  if (args.size()>=2)
    pSettingsGUI = new SettingsGUI(this, args[1]);
  else
    pSettingsGUI = new SettingsGUI(this);
  init(pSettingsGUI);
  
  pIcons = new Icons(this);
  pSounds = new Sounds(this);
  pMainWindow = new MainWindow(this);
  pTrayIcon = new TrayIcon(this, pMainWindow);
  
  connect(this, SIGNAL(pageCheckFinished(Page*)), SLOT(actionRequestFinished(Page*)));
  
  if (!settings()->get("visual/minimized").toBool())
    pMainWindow->show();
}

void CoreGUI::showWarning(const QString& msg, const QString& title) const {
  QString t = title.isEmpty() ? QApplication::applicationName().append(tr(" - Warning")) : title;
  QMessageBox::warning(pMainWindow, t, msg);
}

void CoreGUI::showError(const QString& msg, const QString& title) const {
  QString t = title.isEmpty() ? QApplication::applicationName().append(tr(" - Error")) : title;
  QMessageBox::critical(pMainWindow, t, msg);
}

void CoreGUI::actionRequestFinished(Page* page) {
  if (page->data()->updated) {
    if (settings()->get("update/run/enabled").toBool()) {
      QString cmd = settings()->get("update/run/cmd").toString();
      cmd.replace("%%", "\1");
      cmd.replace("%u", page->data()->address).replace("%d", page->data()->description);
      cmd.replace("%t", page->data()->lastModified.toString()).replace("%h", page->data()->hash);
      cmd.replace("\1", "%");
      QProcess::startDetached(cmd);
    }
    if (settings()->get("update/popup/display").toBool()) {
      PopUp* popUp = new PopUp(this, NULL, page);
      popUp->show();
    }
    sounds()->play(Sounds::Update);
  }
  else if (page->data()->error) {
    sounds()->play(Sounds::Error);
  }
}

void CoreGUI::openUrl(const QUrl& url) const {
  QByteArray encodedUrl( url.toEncoded() );
  
  if (settings()->get("browser/autodetect").toBool()) {
#if defined Q_WS_X11
    if ( QProcess::startDetached( QString("xdg-open ").append(encodedUrl) ) ) return;
    if ( !qgetenv("DEFAULT_BROWSER").isEmpty() ) {
      if ( QProcess::startDetached(
           QString(qgetenv("DEFAULT_BROWSER")).append(" ").append(encodedUrl)
                                  ) ) return;
    }
	if ( !qgetenv("BROWSER").isEmpty() ) {
      QString browser( qgetenv("BROWSER") );
      if (browser.contains("%u", Qt::CaseInsensitive))
        browser.replace("%u", encodedUrl, Qt::CaseInsensitive);
      else
        browser.append(" ").append(encodedUrl);
      
      if ( QProcess::startDetached(browser)) return;
    }
	if ( !qgetenv("KDE_FULL_SESSION").isEmpty() ) {
      if ( QProcess::startDetached( QString("kfmclient exec ").append(encodedUrl) ) ) return;
    }
	else if ( !qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty() ) {
      if ( QProcess::startDetached( QString("gnome-open ").append(encodedUrl) ) ) return;
    }
#elif defined Q_WS_WIN
    QT_WA({
      ShellExecute(0, 0, (TCHAR*)QString(encodedUrl).utf16(), 0, 0, SW_SHOWNORMAL);
    } , {
      ShellExecuteA(0, 0, QString(encodedUrl).toLocal8Bit(), 0, 0, SW_SHOWNORMAL);
    });
#endif
  }
  else {
    QString browser = settings()->get("browser/file").toString();
    if (browser.contains("%u", Qt::CaseInsensitive))
      browser.replace("%u", encodedUrl, Qt::CaseInsensitive);
    else
      browser.append(" ").append(encodedUrl);
    QProcess::startDetached(browser);
  }
}

void CoreGUI::openAllUpdatedPages() {
  openNextUpdatedPage();
}

void CoreGUI::openNextUpdatedPage() {
  int size = pagesUpdated().size();
  
  if (size>0) {
    mainWindow()->openPage(pagesUpdated().first());
    if (size>1)
      QTimer::singleShot(3000, this, SLOT(openNextUpdatedPage()));
  }
}
