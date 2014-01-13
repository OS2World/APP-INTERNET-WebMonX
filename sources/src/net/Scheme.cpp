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

#include "Scheme.h"

#include "File.h"
#include "Ftp.h"
#include "Http.h"

#include <QTimer>

Scheme::Scheme(QObject* parent) : QObject(parent) {
  timerCon = new QTimer(this);
  timerCon->setSingleShot(true);
  connect(timerCon, SIGNAL(timeout()), SLOT(triggerTimeout()));
  timerTrans = new QTimer(this);
  timerTrans->setSingleShot(true);
  connect(timerTrans, SIGNAL(timeout()), SLOT(triggerTimeout()));
}

QString Scheme::errorString() {
  switch (error()) {
    case NoError:
      return tr("OK");
    case AbortedError:
      return tr("Aborted by user");
    case TimeoutError:
      return tr("Request timed out");
    case ProtocolError:
      return protocolErrorString();
    default:
      return QString();
  }
}

Scheme* Scheme::getScheme(const QUrl& url, QObject* parent) {
  QString s(url.scheme().toLower());
  
  Scheme* scheme;
  if (s=="file")
    scheme = new File(parent);
  else if (s=="ftp")
    scheme = new Ftp(parent);
  else if (s=="http" || s=="https")
    scheme = new Http(parent);
  else {
    return NULL;
  }
  
  scheme->setUrl(url);
  return scheme;
}

void Scheme::setUrl(const QUrl& url) {
  this->url = url;
}

void Scheme::setTimeout(int connection, int transfer) {
  pConnection = connection;
  pTransfer = transfer;
}

void Scheme::request(const QUrl& url) {
  setUrl(url);
  request();
}

void Scheme::emitProgress(int done, int total) {
  if (total==0)
    emit progress(0);
  else
    emit progress(done/total*100);
}

void Scheme::startConnectionTimer() {
  if (pConnection!=0)
    timerCon->start(pConnection);
}

void Scheme::startTransferTimer() {
  if (pTransfer!=0)
    timerTrans->start(pTransfer);
}

void Scheme::stopConnectionTimer() {
  timerCon->stop();
}

void Scheme::stopTransferTimer() {
  timerTrans->stop();
}

void Scheme::triggerTimeout() {
  pError = TimeoutError;
  cancel();
}
