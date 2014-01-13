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

#include "PagesWidget.h"
#include "CoreGUI.h"
#include "Icons.h"
#include "core/Page.h"
#include "core/Settings.h"

#include <QContextMenuEvent>

class QPagesWidgetItem : public QListWidgetItem
{
  public:
    QPagesWidgetItem(Core* core, const Page* page, const QString& text, QListWidget* parent = 0, int type = Type);
    inline const Page* getPage() const { return p; };
    bool operator<(const QListWidgetItem& other) const;
    
  private:
    Core* c;
    const Page* p;
};

PagesWidget::PagesWidget(CoreGUI* core, QWidget* parent) : QListWidget(parent), c(core) {
  connect(c, SIGNAL(pageCreated(Page*)), SLOT(addPage(Page*)));
  connect(c, SIGNAL(pageDeleted(Page*)), SLOT(removePage(Page*)));
  connect(c, SIGNAL(pageDescriptionChanged(Page*)), SLOT(updatePage(Page*)));
  connect(c, SIGNAL(pageStateChanged(Page*)), SLOT(updateIcon(Page*)));
  connect(c, SIGNAL(pageCheckFinished(Page*)), SLOT(updateIcon(Page*)));
  connect(c, SIGNAL(pageCheckStarted(Page*)), SLOT(checkStarted(Page*)));
  connect(c, SIGNAL(pageProgressChanged(Page*,int)), SLOT(checkProgress(Page*,int)));
  connect(c, SIGNAL(pageCheckFinished(Page*)), SLOT(resetLabel(Page*)));
  
  connect(this, SIGNAL(itemSelectionChanged()), SLOT(emitSelectionChanged()));
  connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(emitDoubleClicked(QListWidgetItem*)));
}

void PagesWidget::addPage(Page* page) {
  QListWidgetItem* item = new QPagesWidgetItem(c, page, page->data()->description, this);
  itemPage.insert(item, page);
  pageItem.insert(page, item);
  updateIcon(page);
  sortItems();
}

void PagesWidget::removePage(Page* page) {
  QListWidgetItem* item = pageItem.take(page);
  itemPage.remove(item);
  delete item;
}

void PagesWidget::updatePage(Page* page) {
  pageItem[page]->setText( page->data()->description );
  sortItems();
}

void PagesWidget::updateIcon(Page* page) {
  if (page->isChecking())
    pageItem[page]->setIcon(c->icons()->get(Icons::EntryChecking));
  else if (page->data()->updated)
    pageItem[page]->setIcon(c->icons()->get( (page->data()->enabled)?(Icons::EntryUpdated):(Icons::EntryUpdatedDisabled) ));
  else if (page->data()->error)
    pageItem[page]->setIcon(c->icons()->get( (page->data()->enabled)?(Icons::EntryError):(Icons::EntryErrorDisabled) ));
  else
    pageItem[page]->setIcon(c->icons()->get( (page->data()->enabled)?(Icons::Entry):(Icons::EntryDisabled) ));
  sortItems();
}

void PagesWidget::checkStarted(Page* page) {
  pageItem[page]->setText( QString::fromUtf8("%1 [%2]").arg(page->data()->description).arg(tr("connecting")) );
}

void PagesWidget::checkProgress(Page* page, int percent) {
  pageItem[page]->setText( QString::fromUtf8("%1 [%2 %]").arg(page->data()->description).arg(percent) );
}

void PagesWidget::resetLabel(Page* page) {
  pageItem[page]->setText(page->data()->description);
}

void PagesWidget::emitSelectionChanged() {
  QList<Page*> selPages;
  QList<QListWidgetItem*> items = selectedItems();
  
  for (int i=0; i<items.count(); ++i) {
    Page* page = itemPage[items[i]];
    if (!page->isChecking()) // TODO really !page->isChecking() ? Open not possible when checking?
      selPages.append(page);
  }
  
  if (selPages!=pSelectedPages) {
    pSelectedPages = selPages;
    emit selectionChanged(pSelectedPages);
  }
}

void PagesWidget::emitDoubleClicked(QListWidgetItem* item) {
  emit doubleClicked(itemPage[item]);
}

void PagesWidget::contextMenuEvent(QContextMenuEvent* event) {
  if (itemAt(event->pos())!=NULL)
    emit contextMenu(event->globalPos());
}

void PagesWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button()==Qt::RightButton) {
    QListWidgetItem* item = itemAt(event->pos());
    
    if (item!=NULL && !isItemSelected(item)) {
      QList<QListWidgetItem*> items = selectedItems();
      for (QList<QListWidgetItem*>::const_iterator i = items.begin(); i != items.end(); ++i) {
        setItemSelected(*i, false);
      }
      
      setItemSelected(item, true);
    }
  }
  
  QListWidget::mousePressEvent(event);
}

QPagesWidgetItem::QPagesWidgetItem(Core* core, const Page* page, const QString& text, QListWidget* parent, int type) :
  QListWidgetItem(text, parent,type), c(core), p(page) {
}

bool QPagesWidgetItem::operator<(const QListWidgetItem& other) const {
  const Page* oP = static_cast<const QPagesWidgetItem*>(&other)->getPage();
  
  if (!c->settings()->get("visual/sorting/alpha").toBool()) {
    if (oP->data()->unchecked && !p->data()->unchecked) return false;
    if (!oP->data()->unchecked && p->data()->unchecked) return true;
    if (oP->data()->updated && !p->data()->updated) return false;
    if (!oP->data()->updated && p->data()->updated) return true;
    if (oP->data()->enabled && !p->data()->enabled) return false;
    if (!oP->data()->enabled && p->data()->enabled) return true;
    if (oP->data()->error && !p->data()->error) return false;
    if (!oP->data()->error && p->data()->error) return true;
  }
  
  return (QString::compare(p->data()->description, oP->data()->description, Qt::CaseInsensitive) < 0);
}
