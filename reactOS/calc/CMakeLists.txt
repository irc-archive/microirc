
add_definitions(-DDISABLE_HTMLHELP_SUPPORT=1)

add_executable(calc 
    about.c
    convert.c
    function.c
    rpn.c
    utl.c
    winmain.c
buffer.c
iniparser.c
ircprotocol.c
list.c
network.c
util.c
    resource.rc)

set_module_type(calc win32gui UNICODE)
add_importlibs(calc advapi32 user32 gdi32 msvcrt kernel32 ws2_32 winmm)
if(MSVC)
    add_importlibs(calc ntdll)
endif()
add_pch(calc calc.h)
add_cd_file(TARGET calc DESTINATION reactos/system32 FOR all)
