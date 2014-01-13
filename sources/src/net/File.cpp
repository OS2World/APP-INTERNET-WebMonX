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

#include "File.h"

#include <QFile>

File::File(QObject* parent) : Scheme(parent), notExist(false) {
}

void File::request() {
  Q_ASSERT(!url.isEmpty());
  
  QFile* file = new QFile(url.path(), this);
  url.clear();
  if (!file->exists()) {
    pError = ProtocolError;
    notExist = true;
    QMetaObject::invokeMethod(this, "emitFinished", Qt::QueuedConnection, Q_ARG(bool, true));
  }
  else {
    pError = NoError;
    file->open(QIODevice::ReadOnly|QIODevice::Text);
    dataStr = QString::fromAscii(file->readAll());
    file->close();
    QMetaObject::invokeMethod(this, "emitFinished", Qt::QueuedConnection, Q_ARG(bool, false));
  }
}

QString File::data() {
  return dataStr;
}

QString File::protocolErrorString() {
  if (notExist)
    return tr("File not found");
  else
    return QString();
}

void File::cancel() {
}

bool File::isRunning() {
  return false;
}

void File::emitFinished(bool error) {
  emit requestFinished(error);
}
