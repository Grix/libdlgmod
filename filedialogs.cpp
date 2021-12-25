/*

 MIT License

 Copyright © 2021 Samuel Venable

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*/

#include <clocale>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <vector>
#include <map>

#include "filedialogs.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <lib/ImGuiFileDialog/ImGuiFileDialog.h>
#include <unistd.h>
#if defined(_WIN32) 
#include <windows.h>
#include <share.h>
#include <io.h>
#define STR_SLASH "\\"
#define CHR_SLASH '\\'
#else
#if defined(__APPLE__) && defined(__MACH__)
#include <libproc.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/sysctl.h>
#include <sys/user.h>
#endif
#define STR_SLASH "/"
#define CHR_SLASH '/'
#endif

using std::string;
using std::wstring;
using std::vector;

namespace {

  void message_pump() {
    #if defined(_WIN32) 
    MSG msg; while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    #endif
  }
  
  #if defined(_WIN32) 
  wstring widen(string str) {
    size_t wchar_count = str.size() + 1; vector<wchar_t> buf(wchar_count);
    return wstring { buf.data(), (size_t)MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buf.data(), (int)wchar_count) };
  }

  string narrow(wstring wstr) {
    int nbytes = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), nullptr, 0, nullptr, nullptr); vector<char> buf(nbytes);
    return string { buf.data(), (size_t)WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), buf.data(), nbytes, nullptr, nullptr) };
  }
  #endif

  string string_replace_all(string str, string substr, string nstr) {
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != string::npos) {
      message_pump();
      str.replace(pos, substr.length(), nstr);
      pos += nstr.length();
    }
    return str;
  }

  vector<string> string_split(string str, char delimiter) {
    vector<string> vec;
    std::stringstream sstr(str); string tmp;
    while (std::getline(sstr, tmp, delimiter)) {
       message_pump(); vec.push_back(tmp);
    }
    return vec;
  }

  string imgui_filter(string input) {
    input = string_replace_all(input, "\r", "");
    input = string_replace_all(input, "\n", "");
    input = string_replace_all(input, "{", "");
    input = string_replace_all(input, "}", "");
    input = string_replace_all(input, ",", "");
    vector<string> stringVec = string_split(input, '|');
    string string_output;
    unsigned index = 0;
    for (string str : stringVec) {
      message_pump();
      if (index % 2 == 0)
        string_output += str + string("{");
      else {
        std::replace(str.begin(), str.end(), ';', ',');
        string_output += string_replace_all(str, "*.", ".") + string("},");
      }
      index += 1;
    }
    if (!string_output.empty() && string_output.back() == ',') {
      string_output.pop_back();
    } else if (string_output.empty()) {
      string_output = ".*";
    }
    return string_output;
  }

  string filename_path(string fname) {
    #if defined(_WIN32)
    size_t fp = fname.find_last_of("\\/");
    #else
    size_t fp = fname.find_last_of("/");
    #endif
    if (fp == string::npos) return fname;
    return fname.substr(0, fp + 1);
  }

  string filename_name(string fname) {
    #if defined(_WIN32)
    size_t fp = fname.find_last_of("\\/");
    #else
    size_t fp = fname.find_last_of("/");
    #endif
    if (fp == string::npos) return fname;
    return fname.substr(fp + 1);
  }

  string filename_ext(string fname) {
    fname = filename_name(fname);
    size_t fp = fname.find_last_of(".");
    if (fp == string::npos) return "";
    return fname.substr(fp);
  }

  void write_temporary_file(string fname, unsigned char *str, size_t len) {
    FILE *fp = nullptr;
    #if defined(_WIN32)
    wstring wfname = widen(fname);
    if (_wfopen_s(&fp, wfname.c_str(), L"wb, ccs=UTF-8")) 
    return;
    #else
    fp = fopen(fname.c_str(), "wb" );
    #endif
    if (!fp) return;
    fwrite((char *)str, sizeof(char), len, fp);
    fclose(fp);
  }

  string environment_get_variable(string name) {
    #if defined(_WIN32)
    string value;
    wchar_t buffer[32767];
    wstring u8name = widen(name);
    if (GetEnvironmentVariableW(u8name.c_str(), buffer, 32767) != 0) {
      value = narrow(buffer);
    }
    return value;
    #else
    char *value = getenv(name.c_str());
    return value ? value : "";
    #endif
  }

  bool environment_set_variable(string name, string value) {
    #if defined(_WIN32)
    wstring u8name = widen(name);
    wstring u8value = widen(value);
    return (SetEnvironmentVariableW(u8name.c_str(), u8value.c_str()) != 0);
    #else
    return (setenv(name.c_str(), value.c_str(), 1) == 0);
    #endif
  }

  bool environment_unset_variable(string name) {
    #if defined(_WIN32)
    wstring u8name = widen(name);
    return (SetEnvironmentVariableW(u8name.c_str(), nullptr) != 0);
    #else
    return (unsetenv(name.c_str()) == 0);
    #endif
  }

  string filename_canonical(string fname);
  string filename_remove_slash(string dname, bool canonical = false) {
    if (canonical) dname = filename_canonical(dname);
    #if defined(_WIN32)
    while (dname.back() == '\\' || dname.back() == '/') {
      message_pump(); dname.pop_back();
    }
    #else
    while (dname.back() == '/') {
      message_pump(); dname.pop_back();
    }
    #endif
    return dname;
  }

  string filename_add_slash(string dname, bool canonical = false) {
    dname = filename_remove_slash(dname, canonical);
    #if defined(_WIN32)
    if (dname.back() != '\\') dname += "\\";
    #else
    if (dname.back() != '/') dname += "/";
    #endif
    return dname;
  }

  string directory_get_current_working() {
    std::error_code ec;
    string result = filename_add_slash(std::filesystem::current_path(ec).u8string());
    return (ec.value() == 0) ? result : "";
  }

  bool directory_set_current_working(string dname) {
    std::error_code ec;
    const std::filesystem::path path = std::filesystem::u8path(dname);
    std::filesystem::current_path(path, ec);
    return (ec.value() == 0);
  }

  string directory_get_temporary_path() {
    std::error_code ec;
    string result = filename_add_slash(std::filesystem::temp_directory_path(ec).u8string());
    return (ec.value() == 0) ? result : directory_get_current_working();
  }

  string directory_get_executable_path() {
    string path;
    #if defined(_WIN32) 
    wchar_t buffer[MAX_PATH];
    if (GetModuleFileNameW(nullptr, buffer, MAX_PATH) != 0) {
      path = narrow(buffer);
    }
    #elif defined(__APPLE__) && defined(__MACH__)
    char buffer[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(getpid(), buffer, sizeof(buffer)) > 0) {
      path = string(buffer) + "\0";
    }
    #elif defined(__linux__)
    char *buffer = realpath("/proc/self/exe", nullptr);
    path = buffer ? buffer : "";
    free(buffer);
    #elif defined(__FreeBSD__) || defined(__DragonFly__)
    size_t length;
    // CTL_KERN::KERN_PROC::KERN_PROC_PATHNAME(-1)
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
    if (sysctl(mib, 4, nullptr, &length, nullptr, 0) == 0) {
      path.resize(length, '\0');
      char *buffer = path.data();
      if (sysctl(mib, 4, buffer, &length, nullptr, 0) == 0) {
        path = string(buffer) + "\0";
      }
    }
    #endif
    return filename_path(path);
  }

  string environment_expand_variables(string str) {
    if (str.find("${") == string::npos) return str;
    string pre = str.substr(0, str.find("${"));
    string post = str.substr(str.find("${") + 2);
    if (post.find('}') == string::npos) return str;
    string variable = post.substr(0, post.find('}'));
    post = post.substr(post.find('}') + 1);
    string value = environment_get_variable(variable);
    return environment_expand_variables(pre + value + post);
  }

  bool file_exists(string fname) {
    std::error_code ec;
    fname = environment_expand_variables(fname);
    const std::filesystem::path path = std::filesystem::u8path(fname);
    return (std::filesystem::exists(path, ec) && ec.value() == 0 && 
      (!std::filesystem::is_directory(path, ec)) && ec.value() == 0);
  }

  bool directory_exists(string dname) {
    std::error_code ec;
    dname = filename_remove_slash(dname, false);
    dname = environment_expand_variables(dname);
    const std::filesystem::path path = std::filesystem::u8path(dname);
    return (std::filesystem::exists(path, ec) && ec.value() == 0 && 
      std::filesystem::is_directory(path, ec) && ec.value() == 0);
  }

  string filename_canonical(string fname) {
    std::error_code ec;
    fname = environment_expand_variables(fname);
    const std::filesystem::path path = std::filesystem::u8path(fname);
    string result = std::filesystem::weakly_canonical(path, ec).u8string();
    if (ec.value() == 0 && directory_exists(result)) {
      return filename_add_slash(result);
    }
    return (ec.value() == 0) ? result : "";
  }

  string filename_absolute(string fname) {
    string result = "";
    if (directory_exists(fname)) {
      result = filename_add_slash(fname, true);
    } else if (file_exists(fname)) {
      result = filename_canonical(fname);
    }
    return result;
  }

  vector<string> directory_contents(string dname, string pattern, bool includedirs) {
    std::error_code ec; vector<string> result_unfiltered;
    if (!directory_exists(dname)) return result_unfiltered;
    dname = filename_remove_slash(dname, true);
    const std::filesystem::path path = std::filesystem::u8path(dname);
    if (directory_exists(dname)) {
      std::filesystem::directory_iterator end_itr;
      for (std::filesystem::directory_iterator dir_ite(path, ec); dir_ite != end_itr; dir_ite++) {
        message_pump();
        if (ec.value() != 0) { break; }
        std::filesystem::path file_path = std::filesystem::u8path(filename_absolute(dir_ite->path().u8string()));
        if (!std::filesystem::is_directory(dir_ite->status(ec)) && ec.value() == 0) {
          result_unfiltered.push_back(file_path.u8string());
        } else if (ec.value() == 0 && includedirs) {
          result_unfiltered.push_back(filename_add_slash(file_path.u8string()));
        }
      }
    }
    if (pattern.empty()) pattern = "*.*";
    pattern = string_replace_all(pattern, " ", "");
    pattern = string_replace_all(pattern, "*", "");
    vector<string> extVec = string_split(pattern, ';');
    std::set<string> filteredItems;
    for (const string &item : result_unfiltered) {
      message_pump();
      for (const string &ext : extVec) {
        message_pump();
        if (ext == "." || ext == filename_ext(item) || directory_exists(item)) {
          filteredItems.insert(item);
          break;
        }
      }
    }
    vector<string> result_filtered;
    if (filteredItems.empty()) return result_filtered;
    for (const string &filteredName : filteredItems) {
      message_pump();
      result_filtered.push_back(filteredName);
    }
    return result_filtered;
  }

  string filename_drive(string fname) {
    #if defined(_WIN32)
    size_t fp = fname.find_first_of("\\/");
    #else
    size_t fp = fname.find_first_of("/");
    #endif
    if (!fp || fp == string::npos || fname[fp - 1] != ':')
      return "";
    return fname.substr(0, fp);
  }

  bool directory_create(string dname) {
    std::error_code ec;
    dname = filename_remove_slash(dname, true);
    const std::filesystem::path path = std::filesystem::u8path(dname);
    return (std::filesystem::create_directories(path, ec) && ec.value() == 0);
  }

  enum {
    openFile,
    openFiles,
    saveFile,
    selectFolder
  };

  string file_dialog_helper(string filter, string fname, string dir, string title, int type) {
    std::setlocale(LC_ALL, ".UTF8");
    SDL_Window *window; SDL_Renderer *renderer;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
      return "";
    }
    #if defined(IMGUI_IMPL_OPENGL_ES2)
    const char *glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(__APPLE__)
    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
    SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR); window = SDL_CreateWindow(title.c_str(), 
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 348, windowFlags);
    if (window == nullptr) return "";
    #if defined(_WIN32)
    SDL_SysWMinfo system_info;
    SDL_VERSION(&system_info.version);
    if (!SDL_GetWindowWMInfo(window, &system_info)) return "";
    HWND hWnd = system_info.info.win.window;
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    #endif
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); if (environment_get_variable("IMGUI_FONT_PATH").empty())
    environment_set_variable("IMGUI_FONT_PATH", directory_get_executable_path() + "fonts");
    if (environment_get_variable("IMGUI_FONT_SIZE").empty())
    environment_set_variable("IMGUI_FONT_SIZE", std::to_string(30));
    ImGuiIO& io = ImGui::GetIO(); (void)io; ImFontConfig config; 
    config.MergeMode = true; ImFont *font = nullptr; ImWchar ranges[] = { 0x0020, 0xFFFF, 0 }; 
    vector<string> fonts = directory_contents(filename_absolute(environment_get_variable("IMGUI_FONT_PATH")), "*.ttf;*.otf", false);
    unsigned long long fontSize = strtoull(environment_get_variable("IMGUI_FONT_SIZE").c_str(), nullptr, 10);
    for (unsigned i = 0; i < fonts.size(); i++) if (file_exists(fonts[i])) io.Fonts->AddFontFromFileTTF(fonts[i].c_str(), fontSize, (!i) ? nullptr : &config, ranges);
    io.Fonts->Build(); ImGui::StyleColorsDark(); ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version); ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    string filterNew = imgui_filter(filter); bool quit = false; SDL_Event e;
    string result; while (!quit) {
      while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame(); ImGui::SetNextWindowPos(ImVec2(0, 0));
      if (!dir.empty() && dir.back() != CHR_SLASH) dir.push_back(CHR_SLASH);
      if (type == openFile) ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "", filterNew.c_str(), dir.c_str(), fname.c_str(), 1, nullptr, 0);
      if (type == openFiles) ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "", filterNew.c_str(), dir.c_str(), fname.c_str(), 0, nullptr, 0);
      if (type == saveFile) ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "", filterNew.c_str(), dir.c_str(), fname.c_str(), 1, nullptr, 
      ImGuiFileDialogFlags_ConfirmOverwrite); if (type == selectFolder) ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "", nullptr, dir.c_str());
      int display_w, display_h; SDL_GetWindowSize(window, &display_w, &display_h);
      ImVec2 maxSize = ImVec2((float)display_w, (float)display_h); ImVec2 minSize = maxSize;
      if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey",
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove, minSize, maxSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
          if (type == openFile || type == openFiles) {
            auto selection = ImGuiFileDialog::Instance()->GetSelection();
            for (auto const& [key, val] : selection) {
              result += filename_absolute(val) + string("\n");
            }
            if (!result.empty() && result.back() == '\n') {
              result.pop_back();
            }
          } else if (type == saveFile || type == selectFolder) {
            result = ImGuiFileDialog::Instance()->GetFilePathName();
            if (type == saveFile) {
              result = string_replace_all(filename_canonical(result), ".*", "");
            } else {
              while (!result.empty() && result.back() == CHR_SLASH) {
                result.pop_back();
              }
              #if defined(_WIN32)
              if (!result.empty()) {
                result.push_back(CHR_SLASH);
              }
              #else
              result.push_back(CHR_SLASH);
              #endif
              result = filename_canonical(result);
              bool ret = (directory_exists(result) || strcmp(result.c_str(), 
              (filename_drive(result + STR_SLASH) + STR_SLASH).c_str()) == 0);
              if (!ret) ret = directory_create(result);
              if (!ret) result = "";
            }
          }
        }
        ImGuiFileDialog::Instance()->Close();
        quit = true;
      }
      ImGui::Render(); glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
  }

} // anonymous namespace

namespace ngs::imgui {

  string get_open_filename(string filter, string fname) {
    return file_dialog_helper(filter, fname, directory_get_executable_path(), "Open", openFile);
  }

  string get_open_filename_ext(string filter, string fname, string dir, string title) {
    return file_dialog_helper(filter, fname, dir, title, openFile);
  }

  string get_open_filenames(string filter, string fname) {
    return file_dialog_helper(filter, fname, directory_get_executable_path(), "Open", openFiles);
  }
 
  string get_open_filenames_ext(string filter, string fname, string dir, string title) {
    return file_dialog_helper(filter, fname, dir, title, openFiles);
  }

  string get_save_filename(string filter, string fname) {
    return file_dialog_helper(filter, fname, directory_get_executable_path(), "Save As", saveFile);
  }

  string get_save_filename_ext(string filter, string fname, string dir, string title) {
    return file_dialog_helper(filter, fname, dir, title, saveFile);
  }

  string get_directory(string dname) {
    return file_dialog_helper("", "", dname, "Select Directory", selectFolder);
  }

  string get_directory_alt(string capt, string root) {
    return file_dialog_helper("", "", root, capt, selectFolder);
  }

} // namespace ngs::imgui

