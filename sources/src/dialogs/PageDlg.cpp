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

#include "PageDlg.h"
#include "ContentDlg.h"
#include "core/Page.h"
#include "gui/CoreGUI.h"
#include "gui/Icons.h"
#include "gui/SettingsGUI.h"

#include <QMessageBox>
#include <QUrl>

PageDlg::PageDlg(CoreGUI* core, QWidget* parent, const QString& title, PageData* editPage)
  : QDialog(parent), c(core), page(editPage), rxAddrProtocol("^[a-zA-Z]+://"), rxAddrValid("^(https?|ftp|file)://.")
{
  setupUi(this);
  setWindowTitle(title);
  setWindowIcon(c->icons()->get(Icons::WebMonX));
  setGeometry(c->settingsGUI()->loadWindow(this));
  
  connect(buttonSpecify, SIGNAL(clicked()), SLOT(openContentDlg()));
  connect(comboReferer, SIGNAL(currentIndexChanged(int)), SLOT(enableEditReferer(int)));
  connect(buttonBox, SIGNAL(accepted()), SLOT(buttonOkClicked()));
  connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
  connect(buttonAddr, SIGNAL(clicked()), SLOT(buttonAddressClicked()));
  connect(buttonAlt, SIGNAL(clicked()), SLOT(buttonAltAddrClicked()));
  connect(editAddr, SIGNAL(textChanged(const QString&)), SLOT(validateAddr(const QString&)));
  
  editDescr->setText(page->description);
  editAddr->setText(page->address);
  spinCheckD->setValue( page->interval/1440 ); // 1440 = 60*24
  spinCheckH->setValue( (page->interval-spinCheckD->value()*1440) / 60 );
  spinCheckM->setValue( page->interval - spinCheckD->value()*1440 - spinCheckH->value()*60 );
  groupAlt->setChecked(page->hasAltAddr);
  editAlt->setText(page->altAddr);
  comboReferer->setCurrentIndex(page->referer);
  editReferer->setText(page->customReferer);
  groupAuth->setChecked(page->usesAuth);
  editUsername->setText(page->username);
  editPassword->setText(page->password);
}

// workaround for accept() and reject() only hiding the dialog
void PageDlg::done(int r) {
  QDialog::done(r);
  close();
}

void PageDlg::openContentDlg() {
  if (rxAddrValid.indexIn(editAddr->text()) == -1)
    return;
  
  page->address = editAddr->text();
  page->referer = comboReferer->currentIndex();
  page->customReferer = editReferer->text();
  page->usesAuth = groupAuth->isChecked();
  page->username = editUsername->text();
  page->password = editPassword->text();
  
  ContentDlg* contentDlg = new ContentDlg(c, this, page);
  contentDlg->exec();
  delete contentDlg;
}

void PageDlg::enableEditReferer(int index) {
  editReferer->setEnabled( index==2 );
}

void PageDlg::buttonOkClicked() {
  if (spinCheckD->value()==0 && spinCheckH->value()==0 && spinCheckM->value()==0) {
    QMessageBox::warning(
      this,
      tr("Warning"),
      tr("The check interval must be > 0!")
    );
    return;
  }
  if (editDescr->text().isEmpty() || editAddr->text().isEmpty()) {
    QMessageBox::warning(
      this,
      tr("Warning"),
      tr("Please enter a page description and address.")
    );
    return;
  }
  
  page->description = editDescr->text();
  page->address = editAddr->text();
  if (rxAddrProtocol.indexIn(page->address) == -1)
    page->address = QString("http://").append(page->address);
  page->interval = spinCheckD->value()*1440 + spinCheckH->value()*60 + spinCheckM->value();
  page->hasAltAddr = groupAlt->isChecked();
  page->altAddr = editAlt->text();
  page->referer = comboReferer->currentIndex();
  page->customReferer = editReferer->text();
  page->usesAuth = groupAuth->isChecked();
  page->username = editUsername->text();
  page->password = editPassword->text();
  
  accept();
}

void PageDlg::buttonAddressClicked() {
  QString text = editAddr->text().trimmed();
  if (!text.isEmpty())
    c->openUrl(QUrl(text));
}

void PageDlg::buttonAltAddrClicked() {
  QString text = editAlt->text().trimmed();
  if (!text.isEmpty())
    c->openUrl(QUrl(text));
}

void PageDlg::validateAddr(const QString& text) {
  QPalette palette;
  
  if (rxAddrValid.indexIn(text) == -1)
    palette.setColor(editAddr->backgroundRole(), QColor(255, 150, 150));
  else
    palette.setColor(editAddr->backgroundRole(), Qt::white);
  
  editAddr->setPalette(palette);
}

void PageDlg::closeEvent(QCloseEvent* event) {
  c->settingsGUI()->saveWindow(this);
  
  QDialog::closeEvent(event);
}
