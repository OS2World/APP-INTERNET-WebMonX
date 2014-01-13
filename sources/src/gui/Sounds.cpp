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

#include "Sounds.h"
#include "CoreGUI.h"
#include "SettingsGUI.h"

#include <QApplication>
#include <QFile>
#include <QProcess>
#include <QSound>

Sounds::Sounds(CoreGUI* core) : QObject(core), c(core) {
  QString dirPath = QApplication::applicationDirPath();
  
  if (QFile::exists(dirPath+"/../share/apps/webmonx/sounds")) // linux
    soundPath = dirPath+"/../share/apps/webmonx/sounds/";
  else if (QFile::exists(dirPath+"/sounds/")) // windows
    soundPath = dirPath+"/sounds/";
  else if (QFile::exists(dirPath+"/../share/sounds")) // dev
    soundPath = dirPath+"/../share/sounds/";
}

void Sounds::play(Sound sound, bool forceDefault) {
  if (sound==Update && (c->settings()->get("update/sound/play").toBool() || forceDefault)) {
    if (forceDefault || c->settings()->get("update/sound/default").toBool())
      play(soundPath+"update.wav");
    else
      play(c->settings()->get("update/sound/file").toString());
  }
  else if (sound==Error && (c->settings()->get("error/sound/play").toBool() || forceDefault)) {
    if (c->settings()->get("error/sound/default").toBool() || forceDefault)
      play(soundPath+"error.wav");
    else
      play(c->settings()->get("error/sound/file").toString());
  }
}

void Sounds::play(const QString& sound) {
#ifdef Q_WS_X11
  if (!QProcess::startDetached("aplay", QStringList() << sound)) {
    QSound::play(sound);
  }
#else
  QSound::play(sound);
#endif
}
