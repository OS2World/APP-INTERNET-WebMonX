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

#include "Page.h"
#include "Core.h"
#include "Settings.h"
#include "net/Scheme.h"

#include <QCryptographicHash>
#include <QProcess>
#include <QTimer>

PageData::PageData() : interval(1440), hasAltAddr(false), referer(0), usesAuth(false), fromBeginning(true),
  atEnding(true), usesRegExp(false), enabled(true), error(false), updated(false), unchecked(true) {
}

class InvalidRange {};

Page::Page(Core* core, const PageData& data) : QObject(core), pData(data), c(core), pChecking(false), pNumber(-1), pRetry(false) {
  pLastCheckedT = pData.lastChecked.toTime_t();
  if (pData.unchecked)
    check();
}

Page::~Page() {
  emit deleted();
}

void Page::setData(const PageData& data) {
  Q_ASSERT(!pChecking);
  
  bool emitStateChanged = data.enabled!=pData.enabled || data.updated!=pData.updated || data.error!=pData.error;
  bool emitDescriptionChanged = data.description!=pData.description;
  bool emitCheckTimeChanged = data.interval!=pData.interval;
  
  bool unchecked = false;
  if (data.fromBeginning!=pData.fromBeginning || data.fromText!=pData.fromText || data.atEnding!=pData.atEnding ||
      data.atText!=pData.atText || data.usesRegExp!=pData.usesRegExp || data.regExp!=pData.regExp
  ) {
    unchecked = true;
  }
  
  pData = data;
  
  emit modified();
  
  if (emitStateChanged)
    emit stateChanged();
  
  if (emitDescriptionChanged)
    emit descriptionChanged();
  
  if (emitCheckTimeChanged)
    emit checkTimeChanged();
  
  if (unchecked) {
    pData.unchecked = true;
    check();
  }
}

void Page::check() {
  if (pChecking)
    return;
  
  pChecking = true;
  scheme = getNetScheme(this);
  if (scheme==NULL) {
    PageData newData = pData;
    newData.error = true;
    newData.statusMsg = tr("Protocol is not supported");
    setData(newData);
    return;
  }
  connect(scheme, SIGNAL(requestFinished(bool)), SLOT(requestFinished(bool)));
  connect(scheme, SIGNAL(progress(int)), SIGNAL(progressChanged(int)));
  scheme->request();
  
  emit checkStarted();
}

void Page::cancelCheck() {
  Q_ASSERT(pChecking);
  
  scheme->cancel();
}

Scheme* Page::getNetScheme(QObject* parent) const {
  return c->settings()->getNetScheme(data(), parent);
}

void Page::resetUpdated() {
  PageData pageData(*data());
  pageData.updated = false;
  setData(pageData);
}

void Page::requestFinished(bool error) {
  QString hash;
  bool updated;
  QString statusMsg;
  Scheme::Error errorType = scheme->error();
  bool setRetry = false;
  
  if (error) {
    statusMsg = scheme->errorString();
  }
  else {
    try {
      hash = checkBody(scheme->data());
      statusMsg = tr("OK");
    }
    catch (InvalidRange) {
      error = true;
      statusMsg = tr("Invalid range"); // TODO better text
    }
  }
  
  delete scheme;
  
  if (error) {
    updated = false;
    if (errorType!=Scheme::AbortedError && !pRetry && c->settings()->get("connections/retry").toBool()) {
      pRetry = true;
      setRetry = true;
      QTimer::singleShot(5000, this, SLOT(check()));
    }
  }
  else if (!pData.unchecked) {
    if (hash != pData.hash) {
      updated = true;
      pData.lastModified = QDateTime::currentDateTime();
    }
    else {
      updated = false;
    }
  }
  else {
    updated = false;
    pData.unchecked = false;
  }
  
  bool hasStateChanged = (pData.error!=error || pData.updated!=updated);
  
  pData.error = error;
  pData.updated = updated;
  pData.statusMsg = statusMsg;
  
  if ( !( pData.error = error) )
    pData.hash = hash;
  
  
  pData.lastChecked = QDateTime::currentDateTime();
  pLastCheckedT = pData.lastChecked.toTime_t();
  
  pChecking = false;
  if (!setRetry)
    pRetry = false;
  
  if (updated && c->settings()->get("update/run/enabled").toBool()) {
    QString cmd = c->settings()->get("update/run/cmd").toString();
    cmd.replace("%%", "\1");
    cmd.replace("%u", pData.address, Qt::CaseInsensitive).replace("%d", pData.description, Qt::CaseInsensitive);
    cmd.replace("%t", pData.lastModified.toString(), Qt::CaseInsensitive).replace("%h", pData.hash, Qt::CaseInsensitive);
    cmd.replace("\1", "%");
    QProcess::startDetached(cmd);
  }
  
  emit checkFinished();
  emit modified();
  emit checkTimeChanged();
  
  if (hasStateChanged)
    emit stateChanged();
}

QString Page::checkBody(const QString& body) const {
  QString cmpStr;
  if (pData.usesRegExp) {
    QRegExp regExp(pData.regExp, Qt::CaseSensitive, QRegExp::RegExp2);
    regExp.indexIn(body);
    int no;
    if (regExp.pos(1)!=-1) {
      no = 1;
    }
    else {
      if (regExp.pos(0)==-1) throw InvalidRange();
      else no = 0;
    }
    cmpStr = regExp.cap(no);
  }
  else {
    int start;
    if (pData.fromBeginning) {
      start = 0;
    }
    else {
      start = body.indexOf(pData.fromText);
      if (start==-1) throw InvalidRange();
    }
    
    int end;
    if (pData.atEnding) {
      end = -1;
    }
    else {
      end = body.indexOf( pData.atText, start+pData.fromText.count() );
      if (end==-1) throw InvalidRange();
      end += pData.atText.count();
    }
    
    cmpStr = body.mid( start, (end==-1)?(-1):(end-start) );
  }
  
  return QCryptographicHash::hash(cmpStr.toUtf8(), QCryptographicHash::Md5).toHex();
}
