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

#include "Ftp.h"

#include <QFtp>

Ftp::Ftp(QObject* parent) : Scheme(parent) {
  ftp = new QFtp(this);
  connect(ftp, SIGNAL(stateChanged(int)), SLOT(stopConTimer(int)));
}

void Ftp::setAuth(const QString& username, const QString& password) {
  this->username = username;
  this->password = password;
}

void Ftp::request() {
  Q_ASSERT(!isRunning() && !url.isEmpty());
  pError = NoError;
  
  ftp->connectToHost(url.host(), (url.port()==-1) ? 21 : url.port());
  if (!username.isEmpty())
    ftp->login(username, password);
  else
    ftp->login();
  if (!url.userName().isEmpty())
    ftp->login(url.userName(), url.password());
  
  connect(ftp, SIGNAL(dataReadProgress(int,int)), this, SLOT(emitProgress(int,int)));
  connect(ftp, SIGNAL(done(bool)), SIGNAL(emitRequestFinished(bool)), Qt::QueuedConnection);
  
  startConnectionTimer();
  ftp->get(url.path());
}

QString Ftp::data() {
  return QString::fromAscii(ftp->readAll());
}

QString Ftp::protocolErrorString() {
  return ftp->errorString();
}

void Ftp::cancel() {
  if (pError==NoError)
    pError = AbortedError;
  ftp->abort();
}

bool Ftp::isRunning() {
  return (ftp->state() != QFtp::Unconnected);
}

void Ftp::emitRequestFinished(bool error) {
  url.clear();
  pConnection = 0;
  pTransfer = 0;
  
  stopConnectionTimer();
  stopTransferTimer();
  if (pError==NoError && error)
    pError = ProtocolError;
  
  emit requestFinished(error);
}

void Ftp::stopConTimer(int state) {
  if (state==QFtp::Connected) {
    stopConnectionTimer();
    startTransferTimer();
  }
}
