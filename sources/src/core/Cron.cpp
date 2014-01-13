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

#include "Cron.h"
#include "Core.h"
#include "Page.h"
#include "Settings.h"

Cron::Cron(Core* core) : c(core), checkAll(false) {
  offline = c->settings()->get("cron/offline").toBool();
  
  connect(c, SIGNAL(pageCreated(Page*)), SLOT(addPage(Page*)));
  connect(c, SIGNAL(pageDeleted(Page*)), SLOT(removePage(Page*)));
  connect(c, SIGNAL(pageCheckTimeChanged(Page*)), SLOT(sort()));
  
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), SLOT(checkPages()));
  timer->setInterval(30000); // 30 secs
  timer->start();
  
  QTimer::singleShot(5000, this, SLOT(checkPages()));
}

void Cron::checkAllPages() {
  Q_ASSERT(!checkAll);
  
  for (QList<Page*>::const_iterator i = list.begin(); i != list.end(); ++i) {
    if ((*i)->data()->enabled && !(*i)->data()->updated)
      checkAllList << *i;
  }
  
  checkAll = true;
  emit checkAllStarted();
  
  checkPages();
}

void Cron::cancelCheckAll() {
  checkAll = false;
  checkAllList.clear();
  emit checkAllFinished();
}

void Cron::toggleOffline() {
  offline = !offline;
  c->settings()->set("cron/offline", offline);
}

void Cron::addPage(Page* page) {
  list.append(page);
  sort();
}

void Cron::removePage(Page* page) {
  list.removeAt(list.indexOf(page));
  
  if (checkAll && checkAllList.contains(page))
    checkAllList.remove(page);
}

void Cron::checkPages() {
  if (offline || checking.size()==CRON_PARALLEL)
    return;
  
  if (checkAll) {
    QMutableSetIterator<Page*> i(checkAllList);
    while (i.hasNext()) {
      Page* p = i.next();
      if (!p->isChecking()) {
        checking << p;
        connect(p, SIGNAL(checkFinished()), SLOT(checkFinished()));
        p->check();
        i.remove();
        if (checking.size()==CRON_PARALLEL)
          break;
      }
    }
  }
  else {
    uint curTime = QDateTime::currentDateTime().toTime_t();
    for (QList<Page*>::const_iterator i = list.begin(); i != list.end(); ++i) {
      if ((*i)->isChecking() || (*i)->data()->updated || !(*i)->data()->enabled) continue;
      
      if ( ((*i)->lastCheckedT()+(*i)->data()->interval*60) <= curTime ) {
        checking << *i;
        connect(*i, SIGNAL(checkFinished()), SLOT(checkFinished()));
        (*i)->check();
        
        if (checking.size()==CRON_PARALLEL)
          break;
      }
      else {
        break;
      }
    }
  }
}

void Cron::checkFinished() {
  Page* page = static_cast<Page*>(sender());
  page->disconnect(this);
  
  checking.remove(page);
  
  if (checkAll) {
    checkAllList.remove(page);
    if (checkAllList.isEmpty()) {
      checkAll = false;
      emit checkAllFinished();
    }
  }
  
  checkPages();
}

void Cron::sort() {
  qSort(list.begin(), list.end(), cmpPage);
}

bool Cron::cmpPage(const Page* p1, const Page* p2) {
  return (
    (p1->lastCheckedT()+p1->data()->interval*60) <
    (p2->lastCheckedT()+p2->data()->interval*60)
  );
}
