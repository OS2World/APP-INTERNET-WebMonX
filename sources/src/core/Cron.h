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

#ifndef WEBMONX_CRON_H
#define WEBMONX_CRON_H

#include "include.h"

#include <QObject>
#include <QQueue>
#include <QSet>
#include <QTimer>

#define CRON_PARALLEL 3

class Cron : public QObject
{
  Q_OBJECT
  
  public:
    explicit Cron(Core* core);
    inline bool isOffline() { return offline; };
    void cancelCheckAll();
    inline bool isCheckAll() { return checkAll; };
    
  public slots:
    void checkAllPages();
    void toggleOffline();
    
  signals:
    void checkAllStarted();
    void checkAllFinished();
    
  private:
    Core* c;
    bool offline;
    QTimer* timer;
    QList<Page*> list;
    bool checkAll;
    QSet<Page*> checkAllList;
    QSet<Page*> checking;
    static bool cmpPage(const Page* p1, const Page* p2);
  
  private slots:
    void addPage(Page* page);
    void removePage(Page* page);
    void sort();
    void checkFinished();
    void checkPages();
};

#endif // WEBMONX_CRON_H
