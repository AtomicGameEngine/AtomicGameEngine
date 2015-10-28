;--------------------------------
;Atomic Editor NSIS Installation Script
;--------------------------------

!ifndef ATOMIC_ROOT
!error "ATOMIC_ROOT NOT DEFINED"
!endif

!ifndef EDITOR_VERSION
!error "EDITOR_VERSION NOT DEFINED"
!endif

!define prodname "Atomic Editor"
!define coname "THUNDERBEAST GAMES LLC"
!define outfile "${ATOMIC_ROOT}\Artifacts\Windows_Installer\AtomicEditorSetup.exe"
!define appexe "AtomicEditor.exe"
!define produrl "http://www.atomicgameengine.com/"

;--------------------------------
;Include Modern UI
;--------------------------------

!include "MUI.nsh"

;--------------------------------
;General
;--------------------------------

;Name and file
Name "${prodname}"
OutFile "${outfile}"

;Default installation folder
InstallDir "$PROGRAMFILES64\${prodname}"

;Get installation folder from registry if available
InstallDirRegKey HKLM "Software\${coname}\InstallDir" "${prodname}"

;--------------------------------
;Interface Settings
;--------------------------------

!define MUI_ABORTWARNING

;--------------------------------
;Pages
;--------------------------------

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
;--------------------------------

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections
;--------------------------------

Section "${prodname}" SecMain

	SetShellVarContext all

	SetOutPath "$INSTDIR"

  File /r "${ATOMIC_ROOT}\Artifacts\Dist\AtomicEditorInstallerSourceFiles\Windows_Package\*.*"

	;Store installation folder
	WriteRegStr HKLM "Software\${coname}\InstallDir" "${prodname}" $INSTDIR

	; Create shortcut
	CreateShortCut "$SMPROGRAMS\${prodname}.lnk" "$INSTDIR\${appexe}"

	; Update Add/Remove Programs
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "DisplayName" "${prodname}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "DisplayIcon" "$INSTDIR\${appexe},0"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "Publisher" "${coname}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "URLInfoAbout" "${produrl}"
	WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "NoRepair" 1
	WriteRegDWord HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}" "NoModify" 1

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section
;--------------------------------

Section "Uninstall"

	SetShellVarContext all

	; Remove short cut
	Delete "$SMPROGRAMS\${prodname}.lnk"

	; Remove installation folder
  RMDir /r $INSTDIR

	; Remove from Add/Remove Programs
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"

	; Remove installation folder
	DeleteRegValue HKLM "Software\${coname}\InstallDir" "${prodname}"
	DeleteRegKey /ifempty HKLM "Software\${coname}\InstallDir"
	DeleteRegKey /ifempty HKLM "Software\${coname}"

SectionEnd
