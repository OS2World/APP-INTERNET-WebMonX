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

#ifndef WEBMONX_CORE_H
#define WEBMONX_CORE_H

#include "include.h"

#include <QList>
#include <QObject>

class Core : public QObject
{
  Q_OBJECT
  
  public:
    static QString VERSION_STR;
    Core();
    inline Cron* cron() const { return pCron; };
    inline Settings* settings() const { return pSettings; };
    inline Log* log() const { return pLog; };
    Page* createPage(const PageData* data, int num=-1);
    virtual void showWarning(const QString& msg, const QString& title = QString()) const;
    virtual void showError(const QString& msg, const QString& title = QString()) const;
    inline const QList<Page*>& pages() const { return pPages; };
    inline const QList<Page*>& pagesUpdated() const { return pUpdated; };
    inline bool isUpdated() { return pIsUpdated; };
    inline bool isChecking() { return pCheckStatus; };
    
  public slots:
    void cancelChecks();
    
  signals:
    void pageModified(Page* page);
    void pageCreated(Page* page);
    void pageDeleted(Page* page);
    void pageStateChanged(Page* page);
    void pageDescriptionChanged(Page* page);
    void pageCheckTimeChanged(Page* page);
    void pageCheckStarted(Page* page);
    void pageCheckFinished(Page* page);
    void pageProgressChanged(Page* page, int percent);
    void pageCountChanged(int count);
    void checkStatusChanged(bool checking);
    void updatedStatusChanged(bool updated);
    
  private slots:
    void emitPageModified();
    void emitPageStateChanged();
    void emitPageDescriptionChanged();
    void emitPageCheckTimeChanged();
    void emitPageCheckStarted();
    void emitPageCheckFinished();
    void emitPageProgressChanged(int percent);
    void emitPageDeleted();
    
  protected:
    Core(void* null);
    void init(Settings* s);
    
  private:
    void updateCheckStatus();
    void emitUpdatedStatus();
    Settings* pSettings;
    Cron* pCron;
    Log* pLog;
    QList<Page*> pPages;
    QList<Page*> pCheckingPages;
    bool pCheckStatus;
    QList<Page*> pUpdated;
    bool pIsUpdated;
};

#endif // WEBMONX_CORE_H
