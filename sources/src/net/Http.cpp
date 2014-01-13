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

#include "Http.h"

#include <QHttp>
#include <QTextCodec>

Http::Http(QObject* parent) : Scheme(parent) {
  http = new QHttp(this);
  connect(http, SIGNAL(stateChanged(int)), SLOT(stopConTimer(int)));
  connect(http, SIGNAL(dataReadProgress(int,int)), SLOT(emitProgress(int,int)));
  connect(http, SIGNAL(done(bool)), SLOT(emitRequestFinished(bool)), Qt::QueuedConnection);
}

void Http::setReferer(const QString& referer) {
  this->referer = referer;
}

void Http::setUserAgent(const QString& userAgent) {
  this->userAgent = userAgent;
}

void Http::setAuth(const QString& username, const QString& password) {
  this->username = username;
  this->password = password;
}

void Http::setProxy(const QString& host, int port) {
  this->proxyHost = host;
  this->proxyPort = port;
}

void Http::request() {
  Q_ASSERT(!url.isEmpty());
  
  if (isRunning())
    http->abort();
  
  pError = NoError;
  
  http->setHost(
    url.host(),
    (url.scheme().toLower()=="https") ? (QHttp::ConnectionModeHttps) : (QHttp::ConnectionModeHttp),
    (url.port() == -1) ? 0 : url.port()
  );
  http->setProxy(proxyHost, proxyPort);
  
  if (url.path().isEmpty())
    url.setPath("/");
  
  QHttpRequestHeader header;
  if (proxyHost.isEmpty() || proxyPort==0) {
    header.setRequest("GET", url.toEncoded(QUrl::RemoveScheme|QUrl::RemoveAuthority|QUrl::RemoveFragment));
    header.setValue("Host", url.host());
  }
  else
    header.setRequest("GET", url.toString());
  if (!referer.isEmpty())
    header.setValue("Referer", referer);
  if (!userAgent.isEmpty())
    header.setValue("User-Agent", userAgent);
  if (!username.isEmpty()) {
    QByteArray auth = QString("%1:%2").arg(username).arg(password).toAscii().toBase64();
    header.setValue("Authorization", QString("Basic ").append( QString::fromAscii(auth) ));
  }
  header.setValue("Connection", "close");
  
  startConnectionTimer();
  http->request(header);
}

QString Http::data() {
  QHttpResponseHeader header = http->lastResponse();
  QTextCodec* codec = NULL;
  if (header.hasKey("content-type")) {
    QString type = header.value("content-type");
    int pos = type.indexOf("charset=");
    if ( pos!=-1 && pos<(type.size()-8) ) {
      codec = QTextCodec::codecForName( type.mid(pos+8).toAscii() );
    }
  }
  
  if (codec == NULL)
    return QString::fromAscii(http->readAll());
  else
    return codec->toUnicode(http->readAll());
}

QString Http::protocolErrorString() {
  return http->errorString();
}

void Http::cancel() {
  if (pError==NoError)
    pError = AbortedError;
  http->abort();
}

bool Http::isRunning() {
  return (http->state() != QHttp::Unconnected);
}

void Http::emitRequestFinished(bool error) {
  QHttpResponseHeader response = http->lastResponse();
  int status = response.statusCode();
  if (status==301 || status==302 || status==303 || status==307) {
    QUrl tmpUrl = QUrl(response.value("Location"));
    if (tmpUrl.isRelative())
      url = url.resolved(tmpUrl);
    else
      url = tmpUrl;
    request();
  }
  else {
    url.clear();
    pConnection = 0;
    pTransfer = 0;
    referer.clear();
    userAgent.clear();
    username.clear();
    password.clear();
    proxyHost.clear();
    proxyPort = 0;
    
    stopConnectionTimer();
    stopTransferTimer();
    if (pError==NoError && error)
      pError = ProtocolError;
    
    emit requestFinished(error);
  }
}

void Http::stopConTimer(int state) {
  if (state==QHttp::Sending) {
    stopConnectionTimer();
    startTransferTimer();
  }
}
