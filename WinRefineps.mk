
WinRefineps.dll: dlldata.obj WinRefine_p.obj WinRefine_i.obj
	link /dll /out:WinRefineps.dll /def:WinRefineps.def /entry:DllMain dlldata.obj WinRefine_p.obj WinRefine_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del WinRefineps.dll
	@del WinRefineps.lib
	@del WinRefineps.exp
	@del dlldata.obj
	@del WinRefine_p.obj
	@del WinRefine_i.obj
