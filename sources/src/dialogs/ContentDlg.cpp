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

#include "ContentDlg.h"
#include "core/Page.h"
#include "gui/CoreGUI.h"
#include "gui/SettingsGUI.h"

#include <QCloseEvent>
#include <QTextCursor>

class InvalidRange {};

ContentDlg::ContentDlg(CoreGUI* core, QWidget* parent, PageData* editPage)
  : QDialog(parent), c(core), closeWindow(false), page(editPage), start(-1), end(-1)
{
  setupUi(this);
  int max = (radioFrom->width()>radioAt->width()) ? radioFrom->width() : radioAt->width();
  radioFrom->setMinimumSize(max, 0);
  radioAt->setMinimumSize(max, 0);
  setGeometry(c->settingsGUI()->loadWindow(this));
  
  connect(radioBeginning, SIGNAL(toggled(bool)), SLOT(updateHighlighting()));
  connect(radioFrom, SIGNAL(toggled(bool)), SLOT(updateHighlighting()));
  connect(editFrom, SIGNAL(textChanged(const QString&)), SLOT(updateHighlighting()));
  connect(radioEnding, SIGNAL(toggled(bool)), SLOT(updateHighlighting()));
  connect(radioAt, SIGNAL(toggled(bool)), SLOT(updateHighlighting()));
  connect(editAt, SIGNAL(textChanged(const QString&)), SLOT(updateHighlighting()));
  connect(groupRegExp, SIGNAL(toggled(bool)), SLOT(updateHighlighting()));
  connect(editRegExp, SIGNAL(textChanged(const QString&)), SLOT(updateHighlighting()));
  
  connect(radioFrom, SIGNAL(toggled(bool)), SLOT(radioFromToggled(bool)));
  connect(radioAt, SIGNAL(toggled(bool)), SLOT(radioAtToggled(bool)));
  connect(groupRegExp, SIGNAL(toggled(bool)), SLOT(groupRegExpToggled(bool)));
  connect(buttonDownload, SIGNAL(clicked()), SLOT(download()));
  connect(buttonBox, SIGNAL(accepted()), SLOT(buttonOkClicked()));
  connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
  
  connect(buttonJumpFrom, SIGNAL(clicked()), SLOT(jumpToBeginning()));
  connect(buttonJumpAt, SIGNAL(clicked()), SLOT(jumpToEnding()));
  
  radioBeginning->setChecked(page->fromBeginning);
  radioFrom->setChecked(!page->fromBeginning);
  editFrom->setText(page->fromText);
  
  radioEnding->setChecked(page->atEnding);
  radioAt->setChecked(!page->atEnding);
  editAt->setText(page->atText);
  
  groupRegExp->setChecked(page->usesRegExp);
  editRegExp->setText(page->regExp);
  
  download();
}

// workaround for accept() and reject() only hiding the dialog
void ContentDlg::done(int r) {
  QDialog::done(r);
  close();
}

void ContentDlg::radioFromToggled(bool checked) {
  editFrom->setEnabled(checked);
}

void ContentDlg::radioAtToggled(bool checked) {
  editAt->setEnabled(checked);
}

void ContentDlg::groupRegExpToggled(bool on) {
  groupStart->setEnabled(!on);
  groupStop->setEnabled(!on);
}

void ContentDlg::download() {
  buttonDownload->setEnabled(false);
  textPage->clear();
  
  scheme = c->settings()->getNetScheme(page, this);
  if (scheme==NULL)
    return;
  connect(
    scheme,
    SIGNAL(requestFinished(bool)),
    SLOT(checkFinished(bool))
  );
  scheme->request();
}

void ContentDlg::buttonOkClicked() {
  page->fromBeginning = radioBeginning->isChecked();
  page->fromText = editFrom->text();
  
  page->atEnding = radioEnding->isChecked();
  page->atText = editAt->text();
  
  page->usesRegExp = groupRegExp->isChecked();
  page->regExp = editRegExp->text();
  
  accept();
}

void ContentDlg::checkFinished(bool error) {
  if (closeWindow) {
    delete scheme;
    scheme = NULL;
    close();
    return;
  }
  
  if (!error) {
    QString response = scheme->data();
    pageBody = response.replace('\r',"");
    textPage->setPlainText(pageBody);
    labelStatus->clear();
    updateHighlighting();
  }
  else {
    labelStatus->setText("<span style=\"color:red;\">"+scheme->errorString()+"</span>");
  }
  
  delete scheme;
  scheme = NULL;
  buttonDownload->setEnabled(true);
}

void ContentDlg::updateHighlighting() {
  QTextCharFormat format;
  QTextCursor cursor(textPage->document());
  
  format.setForeground(Qt::black);
  cursor.setPosition(0, QTextCursor::MoveAnchor);
  cursor.setPosition(pageBody.count(), QTextCursor::KeepAnchor);
  cursor.setCharFormat(format);
  
  try {
    if (groupRegExp->isChecked()) {
      QRegExp regExp(editRegExp->text(), Qt::CaseSensitive, QRegExp::RegExp2);
      if (!regExp.isValid())
        throw InvalidRange();
      regExp.indexIn(pageBody);
      start = regExp.pos(1);
      int no;
      if (start!=-1) {
        no = 1;
      }
      else {
        start = regExp.pos(0);
        if (start==-1) throw InvalidRange();
        no = 0;
      }
      
      end = start + regExp.cap(no).count();
    }
    else {
      if (radioBeginning->isChecked()) {
        start = 0;
      }
      else {
        start = pageBody.indexOf(editFrom->text());
        if (start==-1) throw InvalidRange();
      }
      
      if (radioEnding->isChecked()) {
        end = pageBody.count();
      }
      else {
        end = pageBody.indexOf(editAt->text(), start+editFrom->text().count());
        if (end==-1) throw InvalidRange();
        end += editAt->text().count();
      }
    }
    
    format.setForeground(Qt::blue);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
    buttonJumpFrom->setEnabled(true);
    buttonJumpAt->setEnabled(true);
  }
  catch (InvalidRange) {
    format.setForeground(Qt::red);
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    cursor.setPosition(pageBody.count(), QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
    buttonJumpFrom->setEnabled(false);
    buttonJumpAt->setEnabled(false);
  }
}

void ContentDlg::jumpToBeginning() {
  if (start != -1) {
    QTextCursor cursor = textPage->textCursor();
    cursor.setPosition(start);
    textPage->setTextCursor(cursor);
  }
}

void ContentDlg::jumpToEnding() {
  if (end != -1) {
    QTextCursor cursor = textPage->textCursor();
    cursor.setPosition(end);
    textPage->setTextCursor(cursor);
  }
}

void ContentDlg::closeEvent(QCloseEvent* event) {
  if (scheme!=NULL && scheme->isRunning()) {
    closeWindow = true;
    scheme->cancel();
    event->ignore();
  }
  else {
    c->settingsGUI()->saveWindow(this);
    QDialog::closeEvent(event);
  }
}
