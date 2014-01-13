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

#ifndef WEBMONX_PAGE_H
#define WEBMONX_PAGE_H

#include "include.h"

#include <QDateTime>
#include <QObject>

class Scheme;

struct PageData
{
  PageData();
  
  QString description;
  QString address;
  int interval; // in min
  bool hasAltAddr;
  QString altAddr; // alternative address
  int referer;
  QString customReferer;
  bool usesAuth;
  QString username;
  QString password;
  bool fromBeginning;
  QString fromText;
  bool atEnding;
  QString atText;
  bool usesRegExp;
  QString regExp;
  bool enabled;
  bool error;
  QString statusMsg;
  bool updated;
  QDateTime lastChecked;
  QDateTime lastModified;
  bool unchecked;
  QString hash;
};

class Page : public QObject
{
  Q_OBJECT
  
  public:
    Page(Core* core, const PageData& data);
    ~Page();
    inline const PageData* data() const { return &pData; };
    void setData(const PageData& data);
    inline int number() const { return pNumber; };
    inline void setNumber(int number) { pNumber = number; };
    inline bool isChecking() const { return pChecking; };
    inline uint lastCheckedT() const { return pLastCheckedT; };
    void cancelCheck();
    Scheme* getNetScheme(QObject* parent=NULL) const;
    
  public slots:
    void check();
    void resetUpdated();
    
  signals:
    void modified();
    void stateChanged();
    void descriptionChanged();
    void checkTimeChanged();
    void checkStarted();
    void checkFinished();
    void progressChanged(int percent);
    void deleted();
    
  private slots:
    void requestFinished(bool error);
    
  private:
    QString checkBody(const QString& body) const;
    PageData pData;
    Core* c;
    bool pChecking;
    int pNumber;
    bool pRetry;
    uint pLastCheckedT;
    Scheme* scheme;
};

#endif // WEBMONX_PAGE_H
