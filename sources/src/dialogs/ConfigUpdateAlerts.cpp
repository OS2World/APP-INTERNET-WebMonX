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

#include "ConfigUpdateAlerts.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"
#include "gui/Sounds.h"

#include <QFileDialog>

ConfigUpdateAlerts::ConfigUpdateAlerts(CoreGUI* core, QWidget* parent) : QWidget(parent), c(core) {
  setupUi(this);
  
  connect(checkPopupDisp, SIGNAL(stateChanged(int)), SLOT(updatePopupEnabled()));
  connect(checkSoundPlay, SIGNAL(stateChanged(int)), SLOT(updateSoundEnabled()));
  connect(radioSoundCust, SIGNAL(toggled(bool)), SLOT(updateSoundEnabled()));
  connect(buttonSoundTest, SIGNAL(clicked()), SLOT(playTestSound()));
  connect(buttonSoundBrowse, SIGNAL(clicked()), SLOT(browseSoundFile()));
  connect(checkRun, SIGNAL(toggled(bool)), editRun, SLOT(setEnabled(bool)));
  
  c->settingsGUI()->load("update/popup/display", checkPopupDisp);
  c->settingsGUI()->load("update/popup/autoclose", checkPopupClose);
  c->settingsGUI()->load("update/popup/position", comboPopupPos);
  
  c->settingsGUI()->load("update/sound/play", checkSoundPlay);
  c->settingsGUI()->load("update/sound/default", radioSoundDef);
  c->settingsGUI()->load("update/sound/custom", radioSoundCust);
  c->settingsGUI()->load("update/sound/file", editSoundCust);
  
  c->settingsGUI()->load("update/run/enabled", checkRun);
  c->settingsGUI()->load("update/run/cmd", editRun);
}

void ConfigUpdateAlerts::save() {
  c->settingsGUI()->save("update/popup/display", checkPopupDisp);
  c->settingsGUI()->save("update/popup/autoclose", checkPopupClose);
  c->settingsGUI()->save("update/popup/position", comboPopupPos);
  
  c->settingsGUI()->save("update/sound/play", checkSoundPlay);
  c->settingsGUI()->save("update/sound/default", radioSoundDef);
  c->settingsGUI()->save("update/sound/custom", radioSoundCust);
  c->settingsGUI()->save("update/sound/file", editSoundCust);
  
  c->settingsGUI()->save("update/run/enabled", checkRun);
  c->settingsGUI()->save("update/run/cmd", editRun);
}

void ConfigUpdateAlerts::updatePopupEnabled() {
  bool enabled = checkPopupDisp->isChecked();
  
  checkPopupClose->setEnabled(enabled);
  labelPopupPos->setEnabled(enabled);
  comboPopupPos->setEnabled(enabled);
}

void ConfigUpdateAlerts::updateSoundEnabled() {
  bool enabled = checkSoundPlay->isChecked();
  
  radioSoundDef->setEnabled(enabled);
  buttonSoundTest->setEnabled(enabled);
  radioSoundCust->setEnabled(enabled);
  editSoundCust->setEnabled(enabled && radioSoundCust->isChecked());
  buttonSoundBrowse->setEnabled(enabled && radioSoundCust->isChecked());
}

void ConfigUpdateAlerts::playTestSound() {
  if (radioSoundDef->isChecked())
    c->sounds()->play(Sounds::Update, true);
  else
    c->sounds()->play(editSoundCust->text());
}

void ConfigUpdateAlerts::browseSoundFile() {
  QString sound(QFileDialog::getOpenFileName(
    this, tr("Select a sound file"),
    QString(), tr("Sounds (*.wav)\nAll files (*)")
  ));
  if (!sound.isEmpty())
    editSoundCust->setText(sound);
}
