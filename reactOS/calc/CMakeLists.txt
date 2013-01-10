
add_definitions(-DDISABLE_HTMLHELP_SUPPORT=1)

add_executable(calc2
    about2.c
    winmain2.c
    
buffer/buffer.c
iniparser/iniparser.c
ircprotocol/ircprotocol.c
list/list.c
network/network.c
util/util.c
IRC/irc.c
IRC/resource.rc
    )

set_module_type(calc2 win32gui UNICODE)
add_importlibs(calc2 advapi32 user32 gdi32 msvcrt kernel32 ws2_32 winmm comctl32 shell32)
if(MSVC)
    add_importlibs(calc2 ntdll)
endif()
add_pch(calc2 calc2.h)
add_cd_file(TARGET calc2 DESTINATION reactos/system32 FOR all)
