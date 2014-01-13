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

#include "SettingsGUI.h"
#include "CoreGUI.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QRadioButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QUrl>
#include <QWidget>

SettingsGUI::SettingsGUI(CoreGUI* core) : Settings(core), c(core) {
  init();
}

SettingsGUI::SettingsGUI(CoreGUI* core, const QString& file) : Settings(core, file), c(core) {
  init();
}

void SettingsGUI::init() {
  defaults.insert("update/popup/display", true);
  defaults.insert("update/popup/autoclose", true);
  defaults.insert("update/popup/position", 4);
  defaults.insert("update/sound/play", true);
  defaults.insert("update/sound/default", true);
  defaults.insert("update/sound/custom", false);
  defaults.insert("update/sound/file", "");
  defaults.insert("update/run/enabled", false);
  defaults.insert("update/run/cmd", "");
  
  defaults.insert("error/sound/play", true);
  defaults.insert("error/sound/default", true);
  defaults.insert("error/sound/custom", false);
  defaults.insert("error/sound/file", "");
  
  defaults.insert("browser/autodetect", true);
  defaults.insert("browser/custom", false);
  defaults.insert("browser/file", "");
  
  defaults.insert("visual/toolbar", true);
  defaults.insert("visual/minimized", false);
  defaults.insert("visual/minimizetotray", false);
  defaults.insert("visual/closetotray", false);
  defaults.insert("visual/stayontop", false);
  defaults.insert("visual/sorting/alpha", true);
  defaults.insert("visual/sorting/status", false);
}

void SettingsGUI::load(const QString& key, QCheckBox* widget) {
  widget->setChecked( get(key).toBool() );
}

void SettingsGUI::load(const QString& key, QComboBox* widget) {
  int value = get(key).toInt();
  if ( value<0 || value>(widget->count()-1) ) value = defaults[key].toInt();
  
  widget->setCurrentIndex(value);
}

void SettingsGUI::load(const QString& key, QLineEdit* widget) {
  widget->setText( get(key).toString() );
}

void SettingsGUI::load(const QString& key, QListWidget* widget) {
  int value = get(key).toInt();
  if ( value<0 || value>(widget->count()-1) ) value = defaults[key].toInt();
  
  widget->setCurrentRow(value);
}

void SettingsGUI::load(const QString& key, QRadioButton* widget) {
  widget->setChecked( get(key).toBool() );
}

void SettingsGUI::load(const QString& key, QSlider* widget) {
  int value = get(key).toInt();
  if ( (value < widget->minimum()) || value>widget->maximum() )
    value = defaults[key].toInt();
  
  widget->setValue(value);
}

void SettingsGUI::load(const QString& key, QSpinBox* widget) {
  int value = get(key).toInt();
  if ( (value < widget->minimum()) || value>widget->maximum() )
    value = defaults[key].toInt();
  
  widget->setValue(value);
}

void SettingsGUI::save(const QString& key, const QCheckBox* widget) {
  set(key, widget->isChecked());
}

void SettingsGUI::save(const QString& key, const QComboBox* widget) {
  set(key, widget->currentIndex());
}

void SettingsGUI::save(const QString& key, const QLineEdit* widget) {
  set(key, widget->text());
}

void SettingsGUI::save(const QString& key, const QListWidget* widget) {
  set(key, widget->currentRow());
}

void SettingsGUI::save(const QString& key, const QRadioButton* widget) {
  set(key, widget->isChecked());
}

void SettingsGUI::save(const QString& key, const QSlider* widget) {
  set(key, widget->value());
}

void SettingsGUI::save(const QString& key, const QSpinBox* widget) {
  set(key, widget->value());
}

QRect SettingsGUI::loadWindow(const QWidget* widget) {
  QString key = QString("windows/").append( widget->objectName() );
  
  if (contains(key)) {
    return get(key).toRect();
  }
  else {
    QRect r = QApplication::desktop()->geometry();
    return QRect(r.center() - widget->rect().center(), widget->size());
  }
}

void SettingsGUI::saveWindow(const QWidget* widget) {
  QString key = QString("windows/").append( widget->objectName() );
  
  set( key, widget->geometry() );
}
