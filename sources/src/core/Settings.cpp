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

#include "Settings.h"
#include "Core.h"
#include "Page.h"
#include "net/Http.h"
#include "net/Ftp.h"
#include "net/File.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QUrl>

#if defined Q_WS_X11
  #include <QFile>
  #include <QProcess>
#elif defined Q_WS_WIN
  #include <windows.h>
#endif

Settings::Settings(Core* core) : QObject(core), c(core), s(QSettings::IniFormat, QSettings::UserScope, "webmonx", "webmonx", this) {
  QString str = s.fileName();
  init();
}

Settings::Settings(Core* core, const QString& file) : QObject(core), c(core), s(file, QSettings::IniFormat, this) {
  QString str = s.fileName();
  init();
}

Settings::~Settings() {
  s.setValue("config/pid", 0);
}

void Settings::init() {
  if ( !s.contains("config/version") ) {
    s.setValue("config/version", 1);
  }
  else if ( s.value("config/version")!=VERSION ) {
    c->showError(
      tr("Config file version is newer than expected.\n"
      "To upgrade WebMonX visit http://debfx.fobos.de/webmonx/")
    );
    throw SettingsException();
  }
  
  if ( s.contains("config/pid") && isProcessRunning(s.value("config/pid").toInt()) ) {
    c->showError(tr("WebMonX is already running!"));
    throw SettingsException();
  }
  else {
    s.setValue("config/pid", getProcessId());
  }
  
  defaults.insert("config/page", 0);
  defaults.insert("update/run/enabled", false);
  defaults.insert("update/run/cmd", "");
  
  defaults.insert("proxy/mode", 0);
  defaults.insert("proxy/address", "");
  defaults.insert("proxy/port", 80);
  
  defaults.insert("connections/connection", 10);
  defaults.insert("connections/transfer", 30);
  defaults.insert("connections/retry", true);
  
  defaults.insert("cron/offline", false);
  
  connect(c, SIGNAL(pageCreated(Page*)), SLOT(savePage(Page*)));
  connect(c, SIGNAL(pageModified(Page*)), SLOT(savePage(Page*)));
  connect(c, SIGNAL(pageDeleted(Page*)), SLOT(savePages()));
}

QDir Settings::getPath() const {
  return QDir( QFileInfo(s.fileName()).absolutePath() );
}

QVariant Settings::get(const QString& key) {
  if (cache.contains(key)) {
    return cache.value(key);
  }
  else {
    QVariant v = s.value(key, defaults[key]);
    cache[key] = v;
    return v;
  }
}

void Settings::set(const QString& key, const QVariant& value) {
  cache[key] = value;
  s.setValue(key, value);
}

void Settings::loadPages() {
  int size = s.value("pages/size", 0).toInt();
  QString head("pages/%1/%2");
  for (int i=1; i<=size; ++i) {
    QString base = head.arg(i);
    if (!s.contains(base.arg("description")))
      continue;
    PageData p;
    p.description = s.value(base.arg("description")).toString();
    p.address = s.value(base.arg("address")).toString();
    p.interval = s.value(base.arg("interval")).toInt();
    p.hasAltAddr = s.value(base.arg("hasAltAddr")).toBool();
    p.altAddr = s.value(base.arg("altAddr")).toString();
    p.referer = s.value(base.arg("referer")).toInt();
    p.customReferer = s.value(base.arg("customReferer")).toString();
    p.usesAuth = s.value(base.arg("usesAuth")).toBool();
    p.username = s.value(base.arg("username")).toString();
    p.password = QString::fromAscii(QByteArray::fromBase64( s.value(base.arg("password")).toByteArray() ));
    p.fromBeginning = s.value(base.arg("fromBeginning")).toBool();
    p.fromText = s.value(base.arg("fromText")).toString();
    p.atEnding = s.value(base.arg("atEnding")).toBool();
    p.atText = s.value(base.arg("atText")).toString();
    p.usesRegExp = s.value(base.arg("usesRegExp")).toBool();
    p.regExp = s.value(base.arg("regExp")).toString();
    p.enabled = s.value(base.arg("enabled")).toBool();
    p.error = s.value(base.arg("error")).toBool();
    p.statusMsg = s.value(base.arg("statusMsg")).toString();
    p.updated = s.value(base.arg("updated")).toBool();
    p.lastChecked = s.value(base.arg("lastChecked")).toDateTime();
    p.lastModified = s.value(base.arg("lastModified")).toDateTime();
    p.unchecked = s.value(base.arg("unchecked")).toBool();
    p.hash = s.value(base.arg("hash")).toString();
    c->createPage(&p, i);
  }
}

void Settings::savePages() {
  const QList<Page*>& p = c->pages();
  
  s.remove("pages");
  
  QString head("pages/%1/%2");
  for (int i=1; i<=p.size(); ++i) {
    QString base = head.arg(i);
    const PageData* data = p[i-1]->data();
    s.setValue(base.arg("description"), data->description);
    s.setValue(base.arg("address"), data->address);
    s.setValue(base.arg("interval"), data->interval);
    s.setValue(base.arg("hasAltAddr"), data->hasAltAddr);
    s.setValue(base.arg("altAddr"), data->altAddr);
    s.setValue(base.arg("referer"), data->referer);
    s.setValue(base.arg("customReferer"), data->customReferer);
    s.setValue(base.arg("usesAuth"), data->usesAuth);
    s.setValue(base.arg("username"), data->username);
    s.setValue(base.arg("password"), data->password.toAscii().toBase64());
    s.setValue(base.arg("fromBeginning"), data->fromBeginning);
    s.setValue(base.arg("fromText"), data->fromText);
    s.setValue(base.arg("atEnding"), data->atEnding);
    s.setValue(base.arg("atText"), data->atText);
    s.setValue(base.arg("usesRegExp"), data->usesRegExp);
    s.setValue(base.arg("regExp"), data->regExp);
    s.setValue(base.arg("enabled"), data->enabled);
    s.setValue(base.arg("error"), data->error);
    s.setValue(base.arg("statusMsg"), data->statusMsg);
    s.setValue(base.arg("updated"), data->updated);
    s.setValue(base.arg("lastChecked"), data->lastChecked);
    s.setValue(base.arg("lastModified"), data->lastModified);
    s.setValue(base.arg("unchecked"), data->unchecked);
    s.setValue(base.arg("hash"), data->hash);
    p[i-1]->setNumber(i);
  }
  
  s.setValue("pages/size", p.size());
}

void Settings::savePage(Page* p) {
  if (p->number() == -1) {
    int index = s.value("pages/size", 0).toInt();
    p->setNumber(index+1);
    s.setValue("pages/size", index+1);
  }
  
  QString head = QString("pages/%1/%2").arg(p->number());
  const PageData* data = p->data();
  s.setValue(head.arg("description"), data->description);
  s.setValue(head.arg("address"), data->address);
  s.setValue(head.arg("interval"), data->interval);
  s.setValue(head.arg("hasAltAddr"), data->hasAltAddr);
  s.setValue(head.arg("altAddr"), data->altAddr);
  s.setValue(head.arg("referer"), data->referer);
  s.setValue(head.arg("customReferer"), data->customReferer);
  s.setValue(head.arg("usesAuth"), data->usesAuth);
  s.setValue(head.arg("username"), data->username);
  s.setValue(head.arg("password"), data->password.toAscii().toBase64());
  s.setValue(head.arg("fromBeginning"), data->fromBeginning);
  s.setValue(head.arg("fromText"), data->fromText);
  s.setValue(head.arg("atEnding"), data->atEnding);
  s.setValue(head.arg("atText"), data->atText);
  s.setValue(head.arg("usesRegExp"), data->usesRegExp);
  s.setValue(head.arg("regExp"), data->regExp);
  s.setValue(head.arg("enabled"), data->enabled);
  s.setValue(head.arg("error"), data->error);
  s.setValue(head.arg("statusMsg"), data->statusMsg);
  s.setValue(head.arg("updated"), data->updated);
  s.setValue(head.arg("lastChecked"), data->lastChecked);
  s.setValue(head.arg("lastModified"), data->lastModified);
  s.setValue(head.arg("unchecked"), data->unchecked);
  s.setValue(head.arg("hash"), data->hash);
}

int Settings::getProcessId() {
#if defined Q_WS_X11
  return getpid();
#elif defined Q_WS_WIN
  return GetCurrentProcessId();
#endif
}

bool Settings::isProcessRunning(int pid) {
  if (pid==0) return false;
  
#if defined Q_WS_X11
  QFile f(QString("/proc/%1/exe").arg(pid));
  QString appPath = QCoreApplication::arguments()[0];
  int lastSlash = appPath.lastIndexOf("/");
  if (f.exists() && lastSlash!=-1) {
    QString appName = appPath.right(appPath.size() - appPath.lastIndexOf("/"));
    return f.symLinkTarget().endsWith(appName);
  }
  else
    return false;
#elif defined Q_WS_WIN
  HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
  if (h!=0) CloseHandle(h);
  return (h!=0);
#endif
}

Scheme* Settings::getNetScheme(const PageData* data, QObject* parent) {
  QUrl url(data->address);
  QString s(url.scheme().toLower());
  
  if (s=="http" || s=="https") {
    Http* http = new Http(parent);
    http->setUrl(url);
    if (data->referer==1)
      http->setReferer(QString("%1://%2/").arg(s).arg(url.host()));
    else if (data->referer==2)
      http->setReferer(data->customReferer);
    http->setUserAgent(QString("WebMonX/").append(Core::VERSION_STR));
    if (data->usesAuth)
      http->setAuth(data->username, data->password);
    QString proxyHost;
    int proxyPort;
    if (getProxy(url, proxyHost, proxyPort))
      http->setProxy(proxyHost, proxyPort);
    http->setTimeout(get("connections/connection").toInt()*1000, get("connections/transfer").toInt()*1000);
    return http;
  }
  else if (s=="ftp") {
    Ftp* ftp = new Ftp(parent);
    ftp->setUrl(url);
    if (data->usesAuth)
      ftp->setAuth(data->username, data->password);
    return ftp;
  }
  else if (s=="file") {
    File* file = new File(parent);
    file->setUrl(url);
    return file;
  }
  else {
    Q_ASSERT(false);
    return NULL;
  }
}

bool Settings::getProxy(const QUrl& url, QString& host, int& port) {
  int proxyMode = get("proxy/mode").toInt();
  
  if (proxyMode==0)
    return false;
  
  if (proxyMode==2) { // Manual
    if (url.scheme().toLower()=="http") {
      QString proxyHost = get("proxy/address").toString();
      int proxyPort = get("proxy/port").toInt();
      if (!proxyHost.isEmpty() && proxyPort!=0) {
        host = proxyHost;
        port = proxyPort;
        return true;
      }
      else
        return false;
    }
    else
      return false;
  }
  
#if defined Q_WS_X11
  if ( !qgetenv("KDE_FULL_SESSION").isEmpty() ) {
    QString httpProxy, httpsProxy;
    switch ( getKDEConfig("ProxyType").toInt() ) {
      case 0:
        return false;
      case 1:
        httpProxy = getKDEConfig("httpProxy");
        httpsProxy = getKDEConfig("httpsProxy");
        break;
      case 4:
        httpProxy = qgetenv(getKDEConfig("httpProxy").toLocal8Bit());
		httpsProxy = qgetenv(getKDEConfig("httpsProxy").toLocal8Bit());
        break;
        
      default:
        return false;
    }
    
    if (url.scheme().toLower()=="http" && !httpProxy.isEmpty()) {
      QRegExp regExp("http\\://([^:]+)\\:(\\d+)");
      if (regExp.indexIn(httpProxy)==-1)
        return false;
      
      host = regExp.cap(1);
      port = regExp.cap(2).toInt();
      if (port==-1)
        port = 80;
      
      return true;
    }
    else if (url.scheme().toLower()=="https" && !httpsProxy.isEmpty()) {
      QRegExp regExp("https\\://([^:]+)\\:(\\d+)");
      if (regExp.indexIn(httpsProxy)==-1)
        return false;
      
      host = regExp.cap(1);
      port = regExp.cap(2).toInt();
      if (port==-1)
        port = 443;
      
      return true;
    }
    else
      return false;
  }
  else if ( !qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty() ) {
    QString mode = getGnomeConfig("/system/proxy/mode");
    if (mode=="manual") {
      if (url.scheme().toLower()=="http") {
        QString httpProxy = getGnomeConfig("/system/http_proxy/host");
        int httpProxyPort = getGnomeConfig("/system/http_proxy/port").toInt();
        if (!httpProxy.isEmpty() && httpProxyPort!=0) {
          host = httpProxy;
          port = httpProxyPort;
          return true;
        }
      }
      else if (url.scheme().toLower()=="https") {
        QString httpsProxy = getGnomeConfig("/system/proxy/secure_host");
        int httpsProxyPort = getGnomeConfig("/system/proxy/secure_port").toInt();
        if (!httpsProxy.isEmpty() && httpsProxyPort!=0) {
          host = httpsProxy;
          port = httpsProxyPort;
          return true;
        }
      }
    }
    return false;
  }
#endif // Q_WS_X11
  
  return false;
}

#if defined Q_WS_X11
QString Settings::getKDEConfig(const QString& key) {
  QProcess process;
  process.start(
    QString("kreadconfig --file kioslaverc --group \"Proxy Settings\" --key \"").append(key).append("\""),
    QIODevice::ReadOnly
  );
  if (process.waitForFinished(1000))
    return QString( process.readAllStandardOutput() ).trimmed();
  else
    return QString();
}

QString Settings::getGnomeConfig(const QString& key) {
  QProcess process;
  process.start(
    QString("gconftool-2 --get  \"").append(key).append("\""),
    QIODevice::ReadOnly
  );
  if (process.waitForFinished(1000)) {
    return QString( process.readAllStandardOutput() ).trimmed();
  }
  else
    return QString();
}
#endif
