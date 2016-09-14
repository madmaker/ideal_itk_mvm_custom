all: pre x86 x64
cppcheck: cppcheck_

!if ( $(TEAMCENTER_MAJOR_VERSION) >= 9 )
x86: pre x86_ clean_x86_
x64: pre x64_ clean_x64_
!else
x86: pre x86_ clean_x86_
x64: pre
!endif

target_x86 = m9_idealplm_custom
target_x64 = m9_idealplm_custom
target_x86_ext = dll
target_x64_ext = dll
!if "$(target_x86_ext)" == "exe"
mt_target_x86_ext = 1
target_x86_is_dll =
target_x86_is_exe = itk_main.obj
!else
mt_target_x86_ext = 2
target_x86_is_dll = /DLL
target_x86_is_exe =
!endif
!if "$(target_x64_ext)" == "exe"
mt_target_x64_ext = 1
target_x64_is_dll =
target_x64_is_exe = itk_main.obj
!else
mt_target_x64_ext = 2
target_x64_is_dll = /DLL
target_x64_is_exe =
!endif

clean_x86_:
	@del /Q /S /F .\bin\x86\*.lib
	@del /Q /S /F .\bin\x86\*.exp
	@del /Q /S /F .\bin\x86\*.ilk

clean_x64_:
	@del /Q /S /F .\bin\x64\*.lib
	@del /Q /S /F .\bin\x64\*.exp
	@del /Q /S /F .\bin\x64\*.ilk

pre:
	@if '$(TEAMCENTER_MAJOR_VERSION)' EQU '' ( @echo ***TEAMCENTER_MAJOR_VERSION must be defined*** & exit 1 )
	@if '$(TEAMCENTER_MINOR_VERSION)' EQU '' ( @echo ***TEAMCENTER_MINOR_VERSION must be defined*** & exit 1 )
	@if '$(TEAMCENTER_SUBMINOR_VERSION)' EQU '' ( @echo ***TEAMCENTER_SUBMINOR_VERSION must be defined*** & exit 1 )
	@if '$(TEAMCENTER_SUBSUBMINOR_VERSION)' EQU '' ( @echo ***TEAMCENTER_SUBSUBMINOR_VERSION must be defined*** & exit 1 )

x86_:
	@if not exist .\obj\x86 mkdir .\obj\x86
	@if not exist .\obj\x86\lib mkdir .\obj\x86\lib
	@if not exist .\bin\x86 mkdir .\bin\x86

	cl\
		./src/*.c*\
		/c\
		/nologo\
		/EHsc\
		/W4\
		/Ox\
		/Oy-\
		/MD\
		/D "TEAMCENTER_MAJOR_VERSION=$(TEAMCENTER_MAJOR_VERSION)"\
		/D "TEAMCENTER_MINOR_VERSION=$(TEAMCENTER_MINOR_VERSION)"\
		/D "TEAMCENTER_SUBMINOR_VERSION=$(TEAMCENTER_SUBMINOR_VERSION)"\
		/D "TEAMCENTER_SUBSUBMINOR_VERSION=$(TEAMCENTER_SUBSUBMINOR_VERSION)"\
		/D "NT40" /D "POMDLL" /D "CRTAPI1=_cdecl" /D "CRTAPI2=cdecl"\
		/D "_WIN32" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "WNT" /D "BYPASS_FLEX" /D "_INTEL=1"\
		/D "_SECURE_SCL=0"\
		/D "XERCES_STATIC_LIBRARY"\
		/D "IPLIB=none"\
		/Fo"./obj/x86/"

	rc\
		/DBUILD_NUMBER=\"$(BUILD_NUMBER)\"\
		/DBUILD_ID=\"$(BUILD_ID)\"\
		/DMERCURIAL_REVISION=\"$(MERCURIAL_REVISION)\"\
		/DMERCURIAL_REVISION_NUMBER=\"$(MERCURIAL_REVISION_NUMBER)\"\
		/DTEAMCENTER_MAJOR_VERSION=$(TEAMCENTER_MAJOR_VERSION)\
		/DTEAMCENTER_MINOR_VERSION=$(TEAMCENTER_MINOR_VERSION)\
		/DTEAMCENTER_SUBMINOR_VERSION=$(TEAMCENTER_SUBMINOR_VERSION)\
		/DTEAMCENTER_SUBSUBMINOR_VERSION=$(TEAMCENTER_SUBSUBMINOR_VERSION)\
		/DSISW_VERSION=\"BuildNumber='$(BUILD_NUMBER)'BuildDate='$(BUILD_ID)'SourceRevision='$(MERCURIAL_REVISION)'SourceRevisionNumber='$(MERCURIAL_REVISION_NUMBER)'\"\
		/fo"./obj/x86/version.res"\
		./version.rc

	link\
		"./obj/x86/*.obj"\
		/OUT:"./bin/x86/$(target_x86).$(target_x86_ext)"\
		"./obj/x86/version.res"\
		$(target_x86_is_exe)\
		$(target_x86_is_dll)\
		/DEBUG\
		/NOLOGO\
		/SUBSYSTEM:CONSOLE\
		/NODEFAULTLIB:libcmt\
		"wsock32.lib" "advapi32.lib" "msvcrt.lib" "oldnames.lib" "kernel32.lib" "winmm.lib"\
		$(LIBS_X86)\
		/MANIFEST\
		/ManifestFile:"./obj/x86/$(target_x86).manifest"

	mt.exe -manifest "./obj/x86/$(target_x86).manifest" -outputresource:"./bin/x86/$(target_x86).$(target_x86_ext)";$(mt_target_x86_ext)

x64_:
	@if not exist .\obj\x64 mkdir .\obj\x64
	@if not exist .\obj\x64\lib mkdir .\obj\x64\lib
	@if not exist .\bin\x64 mkdir .\bin\x64

	cl\
		./src/*.c*\
		./src/actions/*.c*\
		./src/properties/*.c*\
		/c\
		/nologo\
		/EHsc\
		/W4\
		/Ox\
		/Oy-\
		/MD\
		/D "TEAMCENTER_MAJOR_VERSION=$(TEAMCENTER_MAJOR_VERSION)"\
		/D "TEAMCENTER_MINOR_VERSION=$(TEAMCENTER_MINOR_VERSION)"\
		/D "TEAMCENTER_SUBMINOR_VERSION=$(TEAMCENTER_SUBMINOR_VERSION)"\
		/D "TEAMCENTER_SUBSUBMINOR_VERSION=$(TEAMCENTER_SUBSUBMINOR_VERSION)"\
		/D "STEP" /D "AP203" /D "AP214" /D "WIN32_LEAN_AND_MEAN" /D "WNT" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE"\
		/D "_SECURE_SCL=0" /D "_HAS_TR1=0" /D "WNT" /D "_WIN32" /D "WIN32" /D "_WIN64" /D "_INTEL=1" /D "CRTAPI1=_cdecl"\
		/D "CRTAPI2=cdecl" /D "IMAN_TAO" /D "BYPASS_FLEX" /D "POMDLL"\
		/D "IPLIB=none"\
		/Fo"./obj/x64/"

	rc\
		/DBUILD_NUMBER=\"$(BUILD_NUMBER)\"\
		/DBUILD_ID=\"$(BUILD_ID)\"\
		/DMERCURIAL_REVISION=\"$(MERCURIAL_REVISION)\"\
		/DMERCURIAL_REVISION_NUMBER=\"$(MERCURIAL_REVISION_NUMBER)\"\
		/DTEAMCENTER_MAJOR_VERSION=$(TEAMCENTER_MAJOR_VERSION)\
		/DTEAMCENTER_MINOR_VERSION=$(TEAMCENTER_MINOR_VERSION)\
		/DTEAMCENTER_SUBMINOR_VERSION=$(TEAMCENTER_SUBMINOR_VERSION)\
		/DTEAMCENTER_SUBSUBMINOR_VERSION=$(TEAMCENTER_SUBSUBMINOR_VERSION)\
		/DSISW_VERSION=\"BuildNumber='$(BUILD_NUMBER)'BuildDate='$(BUILD_ID)'SourceRevision='$(MERCURIAL_REVISION)'SourceRevisionNumber='$(MERCURIAL_REVISION_NUMBER)'\"\
		/fo"./obj/x64/version.res"\
		./version.rc

	link\
		"./obj/x64/*.obj"\
		/OUT:"./bin/x64/$(target_x64).$(target_x64_ext)"\
		"./obj/x64/version.res"\
		$(target_x86_is_exe)\
		$(target_x64_is_dll)\
		/DEBUG\
		/NOLOGO\
		/SUBSYSTEM:CONSOLE\
		/NODEFAULTLIB:libcmt\
		"wsock32.lib" "advapi32.lib" "msvcrt.lib" "oldnames.lib" "kernel32.lib" "winmm.lib"\
		$(LIBS_X64)\
		/MANIFEST\
		/ManifestFile:"./obj/x64/$(target_x64).manifest"

	mt.exe -manifest "./obj/x64/$(target_x64).manifest" -outputresource:"./bin/x64/$(target_x64).$(target_x64_ext)";$(mt_target_x64_ext)
	
	
cppcheck_:
	cppcheck.exe\
		./src\
		$(CPPCHECK_I_VALUE)\
		--report-progress\
		--force\
		--inline-suppr\
		--enable=all\
		--verbose\
		--xml-version=2
