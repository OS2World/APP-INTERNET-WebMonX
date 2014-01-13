;
; WebMonX 0.2.0
;  http://sniperbeamer.de/webmonx/
;
; Copyright (C) 2006-2007 Felix 'SniperBeamer' Geyer
;
; This file is part of WebMonX.
;
; WebMonX is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; version 2 as published by the Free Software Foundation.
;
; WebMonX is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;

;--------------------------------
;Compression

  ;!define DevCompile

  !ifdef DevCompile
    SetCompress off
    SetDatablockOptimize off
  !else
    SetCompressor /SOLID lzma
  !endif

;--------------------------------
;Include

  !include "MUI.nsh"

;--------------------------------
;Product Info

  !define APP_PRODUCT "WebMonX"
  !define APP_PRODUCT_L "webmonx"
  !define APP_VERSION "0.2.0"
  !define UNINSTALLER "${APP_PRODUCT_L}-uninstall.exe"
  !define ShellVarContext all

;--------------------------------
;Configuration

  Name "${APP_PRODUCT}"
  Caption "${APP_PRODUCT} ${APP_VERSION} Setup"
  BrandingText "   ${APP_PRODUCT} "
  OutFile "${APP_PRODUCT_L}-${APP_VERSION}-setup.exe"
  CRCCheck on
  XPStyle on
  ShowInstDetails hide
  ShowUninstDetails hide

  InstallDir "$PROGRAMFILES\${APP_PRODUCT}"
  InstallDirRegKey HKLM "Software\${APP_PRODUCT}" ""

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_COMPONENTSPAGE_NODESC

  !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
  !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

  !define MUI_ABORTWARNING

  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${APP_PRODUCT}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;--------------------------------
;Pages

  ;Install Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "Installer-License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  ;Uninstall Pages
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Language

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Version Information

  VIProductVersion "${APP_VERSION}.0"
  VIAddVersionKey "ProductName" "${APP_PRODUCT}"
  VIAddVersionKey "LegalCopyright" "Copyright (C) 2006-2007 Felix 'SniperBeamer' Geyer"
  VIAddVersionKey "FileDescription" "${APP_PRODUCT}"
  VIAddVersionKey "FileVersion" "${APP_VERSION}"
  VIAddVersionKey "ProductVersion" "${APP_VERSION}"
  VIAddVersionKey "InternalName" "${APP_PRODUCT}"
  VIAddVersionKey "OriginalFilename" "${APP_PRODUCT}.exe"

;--------------------------------
;Installer Sections

Section "!${APP_PRODUCT}"

  SectionIn RO

  ;Copy Files
  SetOutPath "$INSTDIR"
  File "${APP_PRODUCT}.exe"
  File "*.dll"
  SetOutPath "$INSTDIR\icons\16x16"
  File "icons\16x16\*.png"
  SetOutPath "$INSTDIR\icons\24x24"
  File "icons\24x24\*.png"
  SetOutPath "$INSTDIR\icons\32x32"
  File "icons\32x32\*.png"
  SetOutPath "$INSTDIR\sounds"
  File "sounds\*.wav"

  SetShellVarContext ${ShellVarContext}

  ;Create Startmenu Items
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${APP_PRODUCT}.lnk" "$INSTDIR\${APP_PRODUCT}.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\${UNINSTALLER}"
  !insertmacro MUI_STARTMENU_WRITE_END

  ;Store in Registry
  WriteRegStr HKLM "Software\${APP_PRODUCT}" "" "$INSTDIR"
  WriteRegStr HKLM "Software\${APP_PRODUCT}" "Version" "${APP_VERSION}"

  ;Software
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "DisplayName" "${APP_PRODUCT}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "DisplayVersion" "${APP_VERSION}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "DisplayIcon" "$INSTDIR\${APP_PRODUCT}.exe"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "Publisher" "Felix 'SniperBeamer' Geyer"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "HelpLink" "http://sniperbeamer.de/webmonx/"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "URLInfoAbout" "http://sniperbeamer.de/webmonx/"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "URLUpdateInfo" "http://sniperbeamer.de/webmonx/"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "UninstallString" "$INSTDIR\${UNINSTALLER}"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "NoModify" 1
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}" "NoRepair" 1

  ;Create Uninstaller
  WriteUninstaller "$INSTDIR\${UNINSTALLER}"

SectionEnd


Section "Desktop Icon"

  SetShellVarContext ${ShellVarContext}
  CreateShortCut "$DESKTOP\${APP_PRODUCT}.lnk" "$INSTDIR\${APP_PRODUCT}.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;Delete Files
  Delete "$INSTDIR\${APP_PRODUCT}.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\icons\16x16\*.png"
  Delete "$INSTDIR\icons\24x24\*.png"
  Delete "$INSTDIR\icons\32x32\*.png"
  Delete "$INSTDIR\sounds\*.wav"

  SetShellVarContext ${ShellVarContext}

  ;Delete Startmenu Items
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
  Delete "$MUI_TEMP\${APP_PRODUCT}.lnk"
  Delete "$MUI_TEMP\Uninstall.lnk"
  RMDir "$MUI_TEMP"

  ;Delete Icons
  Delete "$SMSTARTUP\${APP_PRODUCT}.lnk"
  Delete "$DESKTOP\${APP_PRODUCT}.lnk"

  ;Delete Uninstaller
  Delete "$INSTDIR\${UNINSTALLER}"

  RMDir "$INSTDIR\icons\16x16"
  RMDir "$INSTDIR\icons\24x24"
  RMDir "$INSTDIR\icons\32x32"
  RMDir "$INSTDIR\icons"
  RMDir "$INSTDIR\sounds"
  RMDir "$INSTDIR"

  ;Delete Registry Keys
  DeleteRegKey HKLM "Software\${APP_PRODUCT}"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_PRODUCT}"

SectionEnd