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

#include "ConfigErrorAlerts.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"
#include "gui/Sounds.h"

#include <QFileDialog>

ConfigErrorAlerts::ConfigErrorAlerts(CoreGUI* core, QWidget* parent) : QWidget(parent), c(core) {
  setupUi(this);
  
  connect(checkSoundPlay, SIGNAL(stateChanged(int)), SLOT(updateEnabled()));
  connect(radioSoundCust, SIGNAL(toggled(bool)), SLOT(updateEnabled()));
  connect(buttonSoundTest, SIGNAL(clicked()), SLOT(playTestSound()));
  connect(buttonSoundBrowse, SIGNAL(clicked()), SLOT(browseSoundFile()));
  
  c->settingsGUI()->load("error/sound/play", checkSoundPlay);
  c->settingsGUI()->load("error/sound/default", radioSoundDef);
  c->settingsGUI()->load("error/sound/custom", radioSoundCust);
  c->settingsGUI()->load("error/sound/file", editSoundCust);
}

void ConfigErrorAlerts::save() {
  c->settingsGUI()->save("error/sound/play", checkSoundPlay);
  c->settingsGUI()->save("error/sound/default", radioSoundDef);
  c->settingsGUI()->save("error/sound/custom", radioSoundCust);
  c->settingsGUI()->save("error/sound/file", editSoundCust);
}

void ConfigErrorAlerts::updateEnabled() {
  bool enabled = checkSoundPlay->isChecked();
  
  radioSoundDef->setEnabled(enabled);
  buttonSoundTest->setEnabled(enabled);
  radioSoundCust->setEnabled(enabled);
  editSoundCust->setEnabled(enabled && radioSoundCust->isChecked());
  buttonSoundBrowse->setEnabled(enabled && radioSoundCust->isChecked());
}

void ConfigErrorAlerts::playTestSound() {
  if (radioSoundDef->isChecked())
    c->sounds()->play(Sounds::Error, true);
  else
    c->sounds()->play(editSoundCust->text());
}

void ConfigErrorAlerts::browseSoundFile() {
  QString sound(
    QFileDialog::getOpenFileName(
      this,
      tr("Select a sound file"),
      QString(),
      tr("Sounds (*.wav)\nAll files (*)")
    )
  );
  if (!sound.isEmpty())
    editSoundCust->setText(sound);
}
