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

#ifndef WEBMONX_PAGESWIDGET_H
#define WEBMONX_PAGESWIDGET_H

#include "include.h"

#include <QListWidget>

class PagesWidget : public QListWidget
{
  Q_OBJECT
  
  public:
    PagesWidget(CoreGUI* core, QWidget* parent);
    inline const QList<Page*> selectedPages() const { return pSelectedPages; };
    
  signals:
    void selectionChanged(QList<Page*> selection);
    void doubleClicked(Page* page);
    void contextMenu(QPoint pos);
    
  private slots:
    void addPage(Page* page);
    void removePage(Page* page);
    void updatePage(Page* page);
    void updateIcon(Page* page);
    void checkStarted(Page* page);
    void checkProgress(Page* page, int percent);
    void resetLabel(Page* page);
    void emitSelectionChanged();
    void emitDoubleClicked(QListWidgetItem* item);
    
  protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    
  private:
    CoreGUI* c;
    QHash<QListWidgetItem*,Page*> itemPage;
    QHash<Page*,QListWidgetItem*> pageItem;
    QList<Page*> pSelectedPages;
};


#endif // WEBMONX_PAGESWIDGET_H
