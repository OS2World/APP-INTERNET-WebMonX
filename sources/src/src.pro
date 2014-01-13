#
# WebMonX
#  http://debfx.fobos.de/webmonx/
#
# Copyright (C) 2006-2010 Felix Geyer <debfx-webmonx@fobos.de>
#
# This file is part of WebMonX.
#
# WebMonX is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 or (at your option) version 3 as published by
# the Free Software Foundation.
#
# WebMonX is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>
# or write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

TEMPLATE = app

QMAKE_DEBUG_TMP = $$(QMAKE_DEBUG)
isEqual(QMAKE_DEBUG_TMP,1):DEBUG = 1

isEqual(DEBUG,1) {
  CONFIG = debug
  DEFINES += QT_DEBUG
}
else {
  CONFIG = release
}

CONFIG += warn_on \
          qt \
          uic \
          thread \
          stl
QT =      core gui network

HEADERS = core/Core.h \
          core/Settings.h \
          core/Cron.h \
          core/Log.h \
          core/Page.h \
          core/include.h \
          dialogs/MainWindow.h \
          dialogs/AboutDlg.h \
          dialogs/ConfigDlg.h \
          dialogs/ConfigUpdateAlerts.h \
          dialogs/ConfigErrorAlerts.h \
          dialogs/ConfigVisual.h \
          dialogs/ConfigProxy.h \
          dialogs/ConfigConnections.h \
          dialogs/ConfigWebBrowser.h \
          dialogs/PageDlg.h \
          dialogs/ContentDlg.h \
          dialogs/PropertiesDlg.h \
          dialogs/PopUp.h \
          net/Scheme.h \
          net/File.h \
          net/Ftp.h \
          net/Http.h \
          gui/CoreGUI.h \
          gui/Icons.h \
          gui/LinkLabel.h \
          gui/PagesWidget.h \
          gui/SettingsGUI.h \
          gui/Sounds.h \
          gui/TrayIcon.h \
          gui/include.h
SOURCES = main.cpp \
          core/Core.cpp \
          core/Settings.cpp \
          core/Cron.cpp \
          core/Log.cpp \
          core/Page.cpp \
          dialogs/MainWindow.cpp \
          dialogs/AboutDlg.cpp \
          dialogs/ConfigDlg.cpp \
          dialogs/ConfigUpdateAlerts.cpp \
          dialogs/ConfigErrorAlerts.cpp \
          dialogs/ConfigVisual.cpp \
          dialogs/ConfigProxy.cpp \
          dialogs/ConfigConnections.cpp \
          dialogs/ConfigWebBrowser.cpp \
          dialogs/PageDlg.cpp \
          dialogs/ContentDlg.cpp \
          dialogs/PropertiesDlg.cpp \
          dialogs/PopUp.cpp \
          net/Scheme.cpp \
          net/File.cpp \
          net/Ftp.cpp \
          net/Http.cpp \
          gui/CoreGUI.cpp \
          gui/Icons.cpp \
          gui/LinkLabel.cpp \
          gui/PagesWidget.cpp \
          gui/SettingsGUI.cpp \
          gui/Sounds.cpp \
          gui/TrayIcon.cpp
FORMS =   forms/MainWindow.ui \
          forms/AboutDlg.ui \
          forms/ConfigDlg.ui \
          forms/ConfigUpdateAlerts.ui \
          forms/ConfigErrorAlerts.ui \
          forms/ConfigVisual.ui \
          forms/ConfigProxy.ui \
          forms/ConfigConnections.ui \
          forms/ConfigWebBrowser.ui \
          forms/PageDlg.ui \
          forms/PropertiesDlg.ui \
          forms/ContentDlg.ui

INCLUDEPATH *= .

OBJECTS_DIR = ../build
MOC_DIR =     ../build/moc
UI_DIR =      ../build/ui
TARGET =      ../bin/webmonx

icons.files =  ../share/icons/16x16 ../share/icons/24x24 ../share/icons/32x32
sounds.files = ../share/sounds/*
INSTALLS =     target icons sounds

win32:QMAKE_WIN32 = 1

isEqual(QMAKE_WIN32,1) {
  CONFIG += windows
  
  LIBS += -lwininet
  RC_FILE = ../ico/webmonx.rc
  QMAKE_LINK_OBJECT_SCRIPT = $${OBJECTS_DIR}$${DIR_SEPARATOR}$${QMAKE_LINK_OBJECT_SCRIPT}
  
  isEmpty(PREFIX):PREFIX = "C:/Program files/WebMonX"
  
  target.path = $${PREFIX}
  icons.path =  $${PREFIX}/icons
  sounds.path = $${PREFIX}/sounds
}
else:unix {
  isEmpty(PREFIX):PREFIX = /usr/local
  
  target.path = $${PREFIX}/bin
  icons.path =  $${PREFIX}/share/apps/webmonx/icons
  sounds.path = $${PREFIX}/share/apps/webmonx/sounds
  
  desktop.files = ../share/desktop/*
  desktop.path =  $${PREFIX}/share/applications/
  icon16.files =  ../share/icons/16x16/webmonx.png
  icon16.path =   $${PREFIX}/share/icons/hicolor/16x16/apps
  icon32.files =  ../share/icons/32x32/webmonx.png
  icon32.path =   $${PREFIX}/share/icons/hicolor/32x32/apps
  pixmaps.files = ../share/icons/pixmaps/webmonx.xpm
  pixmaps.path =  $${PREFIX}/share/pixmaps/
  INSTALLS +=     desktop icon16 icon32 pixmaps
}

os2: RC_FILE = ../ico/webmonx_os2.rc