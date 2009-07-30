; Script generated by the HM NIS Edit Script Wizard.

!define JF_TODOLIST_VERSION "5.8.8"
!define JF_CALANDER_VERSION "1.0"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "ToDoList"
!define PRODUCT_VERSION "${JF_TODOLIST_VERSION}"
!define PRODUCT_PUBLISHER "Dan.G & James Fancy"
!define PRODUCT_WEB_SITE "http://hi.baidu.com/jamesfancy"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\ToDoList.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "ToDoList"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\ToDoList.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "TdlSetup.exe"
InstallDir "$PROGRAMFILES\ToDoList"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
BrandingText "  http://hi.baidu.com/jamesfancy"

SectionGroup "ToDoList" SEC_TODOLIST
  Section "核心组件" SEC_CORE
    SetOutPath "$INSTDIR"
    SetOverwrite ifnewer
    File "ToDoList\*"

    SetOutPath "$INSTDIR\Resources"
    File "ToDoList\Resources\*"

    SetOverwrite on
    File "ToDoList\Resources\ToDoListDocumentation.tdl"
    SetFileAttributes "$INSTDIR\Resources\ToDoListDocumentation.tdl" READONLY

  ; Shortcuts
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\ToDoList.lnk" "$INSTDIR\ToDoList.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
  SectionEnd
SectionGroupEnd

SectionGroup "简体中文资源" SEC_ZH_CN
  Section "简体中文界面" SEC_ZH_CN_UI
    SetOutPath "$INSTDIR"
    SetOverwrite ifnewer
    File "ToDoList\zh-CN\*.dll"
  SectionEnd

  Section "简体中文文档" SEC_ZH_CN_DOC
    SetOutPath "$INSTDIR\Resources"
    SetOverwrite on
    File "ToDoList\zh-CN\*.tdl"
    File "ToDoList\zh-CN\*.xsl"
    SetFileAttributes "$INSTDIR\Resources\Introduction.tdl" READONLY
    SetFileAttributes "$INSTDIR\Resources\ToDoListDocumentation.tdl" READONLY
  SectionEnd
SectionGroupEnd

SectionGroup /e "快捷方式" SEC_SHORTCUT
  Section "桌面快捷方式" SEC_DESKTOP
    CreateShortCut "$DESKTOP\ToDoList.lnk" "$INSTDIR\ToDoList.exe"
  SectionEnd

  Section "相关网站" SEC_SITES
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$ICONS_GROUP\相关网站"
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\相关网站\ToDoList官方网站.lnk" "http://www.codeproject.com/KB/applications/todolist2.aspx"
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\相关网站\ToDoList中文资讯.lnk" "http://hi.baidu.com/jamesfancy"
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\相关网站\ToDoList中文讨论区.lnk" "http://jamesfancy.5d6d.com/forum-4-1.html"
    !insertmacro MUI_STARTMENU_WRITE_END
  SectionEnd
SectionGroupEnd

SectionGroup /e "插件" SEC_PLUGINS
  Section "日历插件 ${JF_CALANDER_VERSION}" SEC_PLUGIN_CALC
    SetOutPath "$INSTDIR"
    File "ToDoList\plugins\*"
  SectionEnd
SectionGroupEnd

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\卸载ToDoList.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\ToDoList.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\ToDoList.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_TODOLIST} "ToDoList英文原版"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE} "ToDoList主程序和核心插件"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_ZH_CN} "简体中文资源，包括界面和文档等"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_ZH_CN_UI} "使ToDoList界面文本显示为简体中文"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_ZH_CN_DOC} "安装简体中文的文档，它会替换掉原来的英文文档。"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_SHORTCUT} "创建附加的快捷方式"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_PLUGINS} "ToDoList的一些插件"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_DESKTOP} "在桌面创建ToDoList的快捷方式"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_SITES} "在开始菜单中创建ToDoList相关的一些网站链接"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_PLUGIN_CALC} "Dan.G推荐的一个不错的日历插件"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
  SectionGetFlags "${SEC_CORE}" $R0
  IntOp $R0 $R0 | ${SF_RO}
  SectionSetFlags "${SEC_CORE}" $R0
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，其及所有的组件？" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"

  Delete "$INSTDIR\Resources\*.xsl"
  Delete "$INSTDIR\Resources\*.tdl"

  RMDir "$INSTDIR\Resources"
  RMDir "$INSTDIR"

  Delete "$SMPROGRAMS\$ICONS_GROUP\卸载ToDoList.lnk"
  Delete "$DESKTOP\ToDoList.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\ToDoList.lnk"
  RMDir /r "$SMPROGRAMS\$ICONS_GROUP\相关网站"
  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose false
SectionEnd
