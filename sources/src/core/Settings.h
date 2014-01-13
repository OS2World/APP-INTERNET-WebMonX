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

#ifndef WEBMONX_SETTINGS_H
#define WEBMONX_SETTINGS_H

#include "include.h"

#include <QDir>
#include <QHash>
#include <QSettings>
#include <QVariant>

class Scheme;

class SettingsException {};

class Settings : public QObject
{
  Q_OBJECT
  
  public:
    static const int VERSION = 1;
    explicit Settings(Core* core);
    Settings(Core* core, const QString& file);
    ~Settings();
    QDir getPath() const;
    QVariant get(const QString& key);
    void set(const QString& key, const QVariant& value);
    inline bool contains(const QString& key) const { return s.contains(key); };
    Scheme* getNetScheme(const PageData* data, QObject* parent=NULL);
    bool getProxy(const QUrl& url, QString& host, int& port);
    
  public slots:
    void loadPages();
    
  public slots:
    void savePage(Page* page);
    void savePages();
    
  protected:
    Core* c;
    QHash<QString,QVariant> defaults;
    QHash<QString,QVariant> cache;
    
  private:
    void init();
    static int getProcessId();
    static bool isProcessRunning(int pid);
#if defined Q_WS_X11
    QString getKDEConfig(const QString& key);
    QString getGnomeConfig(const QString& key);
#endif
    QSettings s;
};

#endif // WEBMONX_SETTINGS_H
