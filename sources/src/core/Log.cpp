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

#include "Log.h"
#include "Core.h"
#include "Page.h"
#include "Settings.h"
#include "Cron.h"

#include <QDateTime>

Log::Log(Core* core) : QObject(core) {
  f.setFileName( core->settings()->getPath().filePath("webmonx.log") );
  f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
  s.setDevice(&f);
  
  connect(core, SIGNAL(pageCheckFinished(Page*)), SLOT(log(Page*)));
}

void Log::log(Page* page) {
  if ( page->data()->updated ) {
    s << "Description: " << page->data()->description << endl;
    s << "Address:     " << page->data()->address << endl;
    s << "Time:        " << page->data()->lastModified.toString() << endl;
    s << "----------------------------------------------------------" << endl;
  }
}
