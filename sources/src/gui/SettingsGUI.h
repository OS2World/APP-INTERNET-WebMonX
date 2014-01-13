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

#ifndef WEBMONX_SETTINGSGUI_H
#define WEBMONX_SETTINGSGUI_H

#include "include.h"
#include "core/Settings.h"

#include <QObject>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QListWidget;
class QRadioButton;
class QSettings;
class QSlider;
class QSpinBox;
class QWidget;

class SettingsGUI : public Settings
{
  Q_OBJECT
  
  public:
    explicit SettingsGUI(CoreGUI* core);
    SettingsGUI(CoreGUI* core, const QString& file);
    void load(const QString& key, QCheckBox* widget);
    void load(const QString& key, QComboBox* widget);
    void load(const QString& key, QLineEdit* widget);
    void load(const QString& key, QListWidget* widget);
    void load(const QString& key, QRadioButton* widget);
    void load(const QString& key, QSlider* widget);
    void load(const QString& key, QSpinBox* widget);
    void save(const QString& key, const QCheckBox* widget);
    void save(const QString& key, const QComboBox* widget);
    void save(const QString& key, const QLineEdit* widget);
    void save(const QString& key, const QListWidget* widget);
    void save(const QString& key, const QRadioButton* widget);
    void save(const QString& key, const QSlider* widget);
    void save(const QString& key, const QSpinBox* widget);
    QRect loadWindow(const QWidget* widget);
    void saveWindow(const QWidget* widget);
    
  private:
    void init();
    CoreGUI* c;
};

#endif // WEBMONX_SETTINGSGUI_H
