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

#include "Core.h"
#include "Cron.h"
#include "Settings.h"
#include "Log.h"
#include "Page.h"

#include <QCoreApplication>

QString Core::VERSION_STR("0.3.3");

Core::Core() : pCheckStatus(false), pIsUpdated(false) {
  QStringList args = QCoreApplication::arguments();
  if (args.size()>=2)
    pSettings = new Settings(this, args[1]);
  else
    pSettings = new Settings(this);
  pLog = new Log(this);
  pCron = new Cron(this);
}

Core::Core(void* null) : pCheckStatus(false), pIsUpdated(false) {
  Q_UNUSED(null);
}

void Core::init(Settings* s) {
  pSettings = s;
  pLog = new Log(this);
  pCron = new Cron(this);
}

Page* Core::createPage(const PageData* data, int num) {
  Page* page = new Page(this, *data);
  page->setNumber(num);
  pPages.append(page);
  if (data->updated) {
    pUpdated.append(page);
    emitUpdatedStatus();
  }
  
  connect(page, SIGNAL(modified()), SLOT(emitPageModified()));
  connect(page, SIGNAL(stateChanged()), SLOT(emitPageStateChanged()));
  connect(page, SIGNAL(descriptionChanged()), SLOT(emitPageDescriptionChanged()));
  connect(page, SIGNAL(checkTimeChanged()), SLOT(emitPageCheckTimeChanged()));
  connect(page, SIGNAL(checkStarted()), SLOT(emitPageCheckStarted()));
  connect(page, SIGNAL(checkFinished()), SLOT(emitPageCheckFinished()));
  connect(page, SIGNAL(progressChanged(int)), SLOT(emitPageProgressChanged(int)));
  connect(page, SIGNAL(deleted()), SLOT(emitPageDeleted()));
  emit pageCreated(page);
  emit pageCountChanged(pPages.size());
  
  return page;
}

void Core::showWarning(const QString& msg, const QString& title) const {
  QString t = title.isEmpty() ? QCoreApplication::applicationName().append(tr(" - Warning")) : title;
  qWarning("%s:\n%s", t.toUtf8().constData(), msg.toUtf8().constData());
}

void Core::showError(const QString& msg, const QString& title) const {
  QString t = title.isEmpty() ? QCoreApplication::applicationName().append(tr(" - Error")) : title;
  qCritical("%s:\n%s", t.toUtf8().constData(), msg.toUtf8().constData());
}

void Core::cancelChecks() {
  cron()->cancelCheckAll();
  
  for (QList<Page*>::const_iterator i = pCheckingPages.constBegin(); i != pCheckingPages.constEnd(); ++i) {
    (*i)->cancelCheck();
  }
}

void Core::emitPageModified() {
  emit pageModified( static_cast<Page*>(sender()) );
}

void Core::emitPageStateChanged() {
  Page* page = static_cast<Page*>(sender());
  int index;
  if (page->data()->updated && !pUpdated.contains(page)) {
    pUpdated.append(page);
    emitUpdatedStatus();
  }
  else if (!page->data()->updated && (index = pUpdated.indexOf(page))!=-1) {
    pUpdated.removeAt(index);
    emitUpdatedStatus();
  }
  emit pageStateChanged(page);
}

void Core::emitPageDescriptionChanged() {
  emit pageDescriptionChanged( static_cast<Page*>(sender()) );
}

void Core::emitPageCheckTimeChanged() {
  emit pageCheckTimeChanged( static_cast<Page*>(sender()) );
}

void Core::emitPageCheckStarted() {
  Page* page = static_cast<Page*>(sender());
  pCheckingPages.append(page);
  updateCheckStatus();
  emit pageCheckStarted(page);
}

void Core::emitPageCheckFinished() {
  Page* page = static_cast<Page*>(sender());
  pCheckingPages.removeAt(pCheckingPages.indexOf(page));
  updateCheckStatus();
  emit pageCheckFinished(page);
}

void Core::emitPageProgressChanged(int percent) {
  emit pageProgressChanged( static_cast<Page*>(sender()), percent );
}

void Core::emitPageDeleted() {
  Page* page = static_cast<Page*>(sender());
  
  pPages.removeAt(pPages.indexOf(page));
  int index;
  if ( (index = pCheckingPages.indexOf(page)) != -1 )
    pCheckingPages.removeAt(index);
  if ( (index = pUpdated.indexOf(page)) != -1 ) {
    pUpdated.removeAt(index);
    emitUpdatedStatus();
  }
  
  updateCheckStatus();
  
  emit pageDeleted(page);
  emit pageCountChanged(pPages.size());
}

void Core::updateCheckStatus() {
  bool newCheckStatus = ( cron()->isCheckAll() || !pCheckingPages.isEmpty() );
  
  if (pCheckStatus != newCheckStatus) {
    pCheckStatus = newCheckStatus;
    emit checkStatusChanged(newCheckStatus);
  }
}

void Core::emitUpdatedStatus() {
  bool updated = pUpdated.size()!=0;
  if (updated != pIsUpdated) {
    pIsUpdated = updated;
    emit updatedStatusChanged(updated);
  }
}
