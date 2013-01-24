
add_definitions(-DDISABLE_HTMLHELP_SUPPORT=1)

add_executable(irc
    buffer/buffer.c
    iniparser/iniparser.c
    ircprotocol/ircprotocol.c
    list/list.c
    network/network.c
    util/util.c
    irc.c
    resource.rc
)

set_module_type(irc win32gui UNICODE)
add_importlibs(irc advapi32 user32 gdi32 msvcrt kernel32 ws2_32 winmm comctl32 shell32)
if(MSVC)
    add_importlibs(irc ntdll)
endif()
add_pch(irc irc.h)
add_cd_file(TARGET irc DESTINATION reactos/system32 FOR all)
