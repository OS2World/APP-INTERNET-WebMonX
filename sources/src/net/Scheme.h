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

#ifndef WEBMONX_SCHEME_H
#define WEBMONX_SCHEME_H

#include "core/include.h"

#include <QUrl>

class QTimer;

class Scheme : public QObject
{
  Q_OBJECT
  
  public:
    enum Error { NoError, AbortedError, TimeoutError, ProtocolError };
    explicit Scheme(QObject* parent=NULL);
    void setUrl(const QUrl& url);
    void setTimeout(int connection, int transfer);
    virtual void request() = 0;
    void request(const QUrl& url);
    virtual QString data() = 0;
    inline Error error() const { return pError; };
    QString errorString();
    virtual void cancel() = 0;
    virtual bool isRunning() = 0;
    static Scheme* getScheme(const QUrl& url, QObject* parent=NULL);
    
  signals:
    void progress(int percent);
    void requestFinished(bool error);
    
  protected slots:
    void emitProgress(int done, int total);
    
  protected:
    virtual QString protocolErrorString() = 0;
    void startConnectionTimer();
    void startTransferTimer();
    void stopConnectionTimer();
    void stopTransferTimer();
    QUrl url;
    Error pError;
    int pConnection;
    int pTransfer;
    
  private:
    QTimer* timerCon;
    QTimer* timerTrans;
    
  private slots:
    void triggerTimeout();
};

#endif // WEBMONX_SCHEME_H
