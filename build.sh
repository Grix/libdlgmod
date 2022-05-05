cd "${0%/*}"

if [ `uname` = "Darwin" ]; then
  clang++ "/opt/local/lib/libSDL2.a" "DlgModule/Universal/dlgmodule.cpp" "DlgModule/MacOSX/dlgmodule.mm" "DlgModule/MacOSX/config.cpp" "DlgModule/MacOSX/filedialogs.cpp" "DlgModule/MacOSX/filesystem.cpp" "DlgModule/MacOSX/ImFileDialog.cpp" "DlgModule/MacOSX/imgui_draw.cpp" "DlgModule/MacOSX/imgui_impl_sdl.cpp" "DlgModule/MacOSX/imgui_impl_sdlrenderer.cpp" "DlgModule/MacOSX/imgui_tables.cpp" "DlgModule/MacOSX/imgui_widgets.cpp" "DlgModule/MacOSX/imgui.cpp" -o "libdlgmod.dylib" -shared -std=c++17 -liconv -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -I/opt/local/include -I/opt/local/include/SDL2 -std=c++17 -liconv -Wno-deprecated-enum-enum-conversion -ObjC++ -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal -fPIC -arch arm64 -arch x86_64 -fPIC
elif [ $(uname) = "Linux" ]; then
  g++ "DlgModule/Universal/dlgmodule.cpp" "DlgModule/xlib/dlgmodule.cpp" "DlgModule/xlib/lodepng.cpp" -o "libdlgmod.so" -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -lprocps -lpthread -fPIC
elif [ $(uname) = "FreeBSD" ]; then
  clang++ "DlgModule/Universal/dlgmodule.cpp" "DlgModule/xlib/dlgmodule.cpp" "DlgModule/xlib/lodepng.cpp" -o "libdlgmod.so" -std=c++17 -I/usr/local/include -L/usr/local/lib -shared -lX11 -lprocstat -lutil -lc -lpthread -fPIC
elif [ $(uname) = "DragonFly" ]; then
  g++ "DlgModule/Universal/dlgmodule.cpp" "DlgModule/xlib/dlgmodule.cpp" "DlgModule/xlib/lodepng.cpp" -o "libdlgmod.so" -std=c++17 -I/usr/local/include -L/usr/local/lib -shared -static-libgcc -static-libstdc++ -lX11 -lkvm -lc -lpthread -fPIC
elif [ $(uname) = "OpenBSD" ]; then
  clang++ "DlgModule/Universal/dlgmodule.cpp" "DlgModule/xlib/dlgmodule.cpp" "DlgModule/xlib/lodepng.cpp" -o "libdlgmod.so" -std=c++17 -I/usr/local/include -L/usr/local/lib -shared -lX11 -lkvm -lc -lpthread -fPIC
fi
