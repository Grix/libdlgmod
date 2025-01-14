﻿/*
 
 MIT License
 
 Copyright © 2020 Samuel Venable
 
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

#include <thread>
#include <string>

#include "dlgmodule.h"

#ifdef _WIN32
#define EXPORTED_FUNCTION extern "C" __declspec(dllexport)
#else /* macOS, Linux, and BSD */
#define EXPORTED_FUNCTION extern "C" __attribute__((visibility("default")))
#endif

EXPORTED_FUNCTION double show_message(char *str);
EXPORTED_FUNCTION double show_message_async(char *str);
EXPORTED_FUNCTION double show_message_cancelable(char *str);
EXPORTED_FUNCTION double show_message_cancelable_async(char *str);
EXPORTED_FUNCTION double show_question(char *str);
EXPORTED_FUNCTION double show_question_async(char *str);
EXPORTED_FUNCTION double show_question_cancelable(char *str);
EXPORTED_FUNCTION double show_question_cancelable_async(char *str);
EXPORTED_FUNCTION double show_attempt(char *str);
EXPORTED_FUNCTION double show_attempt_async(char *str);
EXPORTED_FUNCTION double show_error(char *str, double abort);
EXPORTED_FUNCTION double show_error_async(char *str, double abort);
EXPORTED_FUNCTION char *get_string(char *str, char *def);
EXPORTED_FUNCTION double get_string_async(char *str, char *def);
EXPORTED_FUNCTION char *get_password(char *str, char *def);
EXPORTED_FUNCTION double get_password_async(char *str, char *def);
EXPORTED_FUNCTION double get_integer(char *str, double def);
EXPORTED_FUNCTION double get_integer_async(char *str, double def);
EXPORTED_FUNCTION double get_passcode(char *str, double def);
EXPORTED_FUNCTION double get_passcode_async(char *str, double def);
EXPORTED_FUNCTION char *get_open_filename(char *filter, char *fname);
EXPORTED_FUNCTION double get_open_filename_async(char *filter, char *fname);
EXPORTED_FUNCTION char *get_open_filename_ext(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION double get_open_filename_ext_async(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION char *get_open_filenames(char *filter, char *fname);
EXPORTED_FUNCTION double get_open_filenames_async(char *filter, char *fname);
EXPORTED_FUNCTION char *get_open_filenames_ext(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION double get_open_filenames_ext_async(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION char *get_save_filename(char *filter, char *fname);
EXPORTED_FUNCTION double get_save_filename_async(char *filter, char *fname);
EXPORTED_FUNCTION char *get_save_filename_ext(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION double get_save_filename_ext_async(char *filter, char *fname, char *dir, char *title);
EXPORTED_FUNCTION char *get_directory(char *dname);
EXPORTED_FUNCTION double get_directory_async(char *dname);
EXPORTED_FUNCTION char *get_directory_alt(char *capt, char *root);
EXPORTED_FUNCTION double get_directory_alt_async(char *capt, char *root);
EXPORTED_FUNCTION double get_color(double defcol);
EXPORTED_FUNCTION double get_color_async(double defcol);
EXPORTED_FUNCTION double get_color_ext(double defcol, char *title);
EXPORTED_FUNCTION double get_color_ext_async(double defcol, char *title);
EXPORTED_FUNCTION char *widget_get_caption();
EXPORTED_FUNCTION double widget_set_caption(char *str);
EXPORTED_FUNCTION char *widget_get_owner();
EXPORTED_FUNCTION double widget_set_owner(char *hwnd);
EXPORTED_FUNCTION char *widget_get_icon();
EXPORTED_FUNCTION double widget_set_icon(char *icon);
EXPORTED_FUNCTION char *widget_get_system();
EXPORTED_FUNCTION double widget_set_system(char *sys);
EXPORTED_FUNCTION char *widget_get_button_name(double type);
EXPORTED_FUNCTION double widget_set_button_name(double type, char *name);
EXPORTED_FUNCTION void RegisterCallbacks(char *arg1, char *arg2, char *arg3, char *arg4);

namespace {

unsigned dialog_identifier = 100;
bool enable_dialog_creation = true;
void(*CreateAsynEventWithDSMap)(int, int);
int(*CreateDsMap)(int _num, ...);
bool(*DsMapAddDouble)(int _index, char *_pKey, double value);
bool(*DsMapAddString)(int _index, char *_pKey, char *pVal);

std::string arg1;
std::string arg2;
std::string arg3;
std::string arg4;

void show_message_threaded(char *str, unsigned id) {
  double result = show_message(str);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void show_message_cancelable_threaded(char *str, unsigned id) {
  double result = show_message_cancelable(str);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void show_question_threaded(char *str, unsigned id) {
  double result = show_question(str);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void show_question_cancelable_threaded(char *str, unsigned id) {
  double result = show_question_cancelable(str);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void show_attempt_threaded(char *str, unsigned id) {
  double result = show_attempt(str);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void show_error_threaded(char *str, double abort, unsigned id) {
  double result = show_error(str, abort);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_string_threaded(char *str, char *def, unsigned id) {
  char *result = get_string(str, def);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_password_threaded(char *str, char *def, unsigned id) {
  char *result = get_password(str, def);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_integer_threaded(char *str, double def, unsigned id) {
  double result = get_integer(str, def);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", isnan(result) ? 0 : 1);
  DsMapAddDouble(resultMap, (char *)"value", isnan(result) ? 0 : result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_passcode_threaded(char *str, double def, unsigned id) {
  double result = get_passcode(str, def);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddDouble(resultMap, (char *)"value", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_open_filename_threaded(char *filter, char *fname, unsigned id) {
  char *result = get_open_filename(filter, fname);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_open_filename_ext_threaded(char *filter, char *fname, char *dir, char *title, unsigned id) {
  char *result = get_open_filename_ext(filter, fname, dir, title);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_open_filenames_threaded(char *filter, char *fname, unsigned id) {
  char *result = get_open_filenames(filter, fname);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_open_filenames_ext_threaded(char *filter, char *fname, char *dir, char *title, unsigned id) {
  char *result = get_open_filenames_ext(filter, fname, dir, title);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_save_filename_threaded(char *filter, char *fname, unsigned id) {
  char *result = get_save_filename(filter, fname);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_save_filename_ext_threaded(char *filter, char *fname, char *dir, char *title, unsigned id) {
  char *result = get_save_filename_ext(filter, fname, dir, title);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_directory_threaded(char *dname, unsigned id) {
  char *result = get_directory(dname);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_directory_alt_threaded(char *capt, char *root, unsigned id) {
  char *result = get_directory_alt(capt, root);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddString(resultMap, (char *)"result", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_color_threaded(double defcol, unsigned id) {
  double result = get_color(defcol);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddDouble(resultMap, (char *)"value", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

void get_color_ext_threaded(double defcol, char *title, unsigned id) {
  double result = get_color_ext(defcol, title);
  int resultMap = CreateDsMap(0);
  DsMapAddDouble(resultMap, (char *)"id", id);
  DsMapAddDouble(resultMap, (char *)"status", 1);
  DsMapAddDouble(resultMap, (char *)"value", result);
  CreateAsynEventWithDSMap(resultMap, 63);
  enable_dialog_creation = true;
}

} // anonymous namespace

double show_message(char *str) {
  return dialog_module::show_message(str);
}

double show_message_async(char *str) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_message_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double show_message_cancelable(char *str) {
  return dialog_module::show_message_cancelable(str);
}

double show_message_cancelable_async(char *str) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_message_cancelable_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double show_question(char *str) {
  return dialog_module::show_question(str);
}

double show_question_async(char *str) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_question_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double show_question_cancelable(char *str) {
  return dialog_module::show_question_cancelable(str);
}

double show_question_cancelable_async(char *str) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_question_cancelable_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double show_attempt(char *str) {
  return dialog_module::show_attempt(str);
}

double show_attempt_async(char *str) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_attempt_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double show_error(char *str, double abort) {
  return dialog_module::show_error(str, abort);
}

double show_error_async(char *str, double abort) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(show_error_threaded, (char *)arg1.c_str(), abort, id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_string(char *str, char *def) {
  return dialog_module::get_string(str, def);
}

double get_string_async(char *str, char *def) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    arg2 = def;
    std::thread dialog_thread(get_string_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_password(char *str, char *def) {
  return dialog_module::get_password(str, def);
}

double get_password_async(char *str, char *def) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    arg2 = def;
    std::thread dialog_thread(get_password_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double get_integer(char *str, double def) {
  return dialog_module::get_integer(str, def);
}

double get_integer_async(char *str, double def) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(get_integer_threaded, (char *)arg1.c_str(), def, id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double get_passcode(char *str, double def) {
  return dialog_module::get_passcode(str, def);
}

double get_passcode_async(char *str, double def) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = str;
    std::thread dialog_thread(get_passcode_threaded, (char *)arg1.c_str(), def, id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_open_filename(char *filter, char *fname) {
  return dialog_module::get_open_filename(filter, fname);
}

double get_open_filename_async(char *filter, char *fname) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    std::thread dialog_thread(get_open_filename_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_open_filename_ext(char *filter, char *fname, char *dir, char *title) {
  return dialog_module::get_open_filename_ext(filter, fname, dir, title);
}

double get_open_filename_ext_async(char *filter, char *fname, char *dir, char *title) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    arg3 = dir;
    arg4 = title;
    std::thread dialog_thread(get_open_filename_ext_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), (char *)arg3.c_str(), (char *)arg4.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_open_filenames(char *filter, char *fname) {
  return dialog_module::get_open_filenames(filter, fname);
}

double get_open_filenames_async(char *filter, char *fname) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    std::thread dialog_thread(get_open_filenames_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_open_filenames_ext(char *filter, char *fname, char *dir, char *title) {
  return dialog_module::get_open_filenames_ext(filter, fname, dir, title);
}

double get_open_filenames_ext_async(char *filter, char *fname, char *dir, char *title) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    arg3 = dir;
    arg4 = title;
    std::thread dialog_thread(get_open_filenames_ext_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), (char *)arg3.c_str(), (char *)arg4.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_save_filename(char *filter, char *fname) {
  return dialog_module::get_save_filename(filter, fname);
}

double get_save_filename_async(char *filter, char *fname) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    std::thread dialog_thread(get_save_filename_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_save_filename_ext(char *filter, char *fname, char *dir, char *title) {
  return dialog_module::get_save_filename_ext(filter, fname, dir, title);
}

double get_save_filename_ext_async(char *filter, char *fname, char *dir, char *title) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = filter;
    arg2 = fname;
    arg3 = dir;
    arg4 = title;
    std::thread dialog_thread(get_save_filename_ext_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), (char *)arg3.c_str(), (char *)arg4.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_directory(char *dname) {
  return dialog_module::get_directory(dname);
}

double get_directory_async(char *dname) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = dname;
    std::thread dialog_thread(get_directory_threaded, (char *)arg1.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *get_directory_alt(char *capt, char *root) {
  return dialog_module::get_directory_alt(capt, root);
}

double get_directory_alt_async(char *capt, char *root) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg1 = capt;
    arg2 = root;
    std::thread dialog_thread(get_directory_alt_threaded, (char *)arg1.c_str(), (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double get_color(double defcol) {
  return dialog_module::get_color((int)defcol);
}

double get_color_async(double defcol) {
  if (enable_dialog_creation) {
	enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    std::thread dialog_thread(get_color_threaded, (int)defcol, id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

double get_color_ext(double defcol, char *title) {
  return dialog_module::get_color_ext((int)defcol, title);
}

double get_color_ext_async(double defcol, char *title) {
  if (enable_dialog_creation) {
    enable_dialog_creation = false;
    unsigned id = dialog_identifier++;
    arg2 = title;
    std::thread dialog_thread(get_color_ext_threaded, (int)defcol, (char *)arg2.c_str(), id);
    dialog_thread.detach();
    return (double)id;
  }
  return dialog_identifier - 1;
}

char *widget_get_caption() {
  return dialog_module::widget_get_caption();
}

double widget_set_caption(char *str) {
  dialog_module::widget_set_caption(str);
  return 0;
}

char *widget_get_icon() {
  return dialog_module::widget_get_icon();
}

double widget_set_icon(char *icon) {
  dialog_module::widget_set_icon(icon);
  return 0;
}

char *widget_get_owner() {
  return dialog_module::widget_get_owner();
}

double widget_set_owner(char *hwnd) {
  dialog_module::widget_set_owner(hwnd);
  return 0;
}

char *widget_get_system() {
  return dialog_module::widget_get_system();
}

double widget_set_system(char *sys) {
  dialog_module::widget_set_system(sys);
  return 0;
}

char *widget_get_button_name(double type) {
  return dialog_module::widget_get_button_name((int)type);
}

double widget_set_button_name(double type, char *name) {
  dialog_module::widget_set_button_name((int)type, name);
  return 0;
}

void RegisterCallbacks(char *arg1, char *arg2, char *arg3, char *arg4) {
  void(*CreateAsynEventWithDSMapPtr)(int, int) = (void(*)(int, int))(arg1);
  int(*CreateDsMapPtr)(int _num, ...) = (int(*)(int _num, ...))(arg2);
  CreateAsynEventWithDSMap = CreateAsynEventWithDSMapPtr;
  CreateDsMap = CreateDsMapPtr;

  bool(*DsMapAddDoublePtr)(int _index, char *_pKey, double value) = (bool(*)(int, char *, double))(arg3);
  bool(*DsMapAddStringPtr)(int _index, char *_pKey, char *pVal) = (bool(*)(int, char *, char *))(arg4);

  DsMapAddDouble = DsMapAddDoublePtr;
  DsMapAddString = DsMapAddStringPtr;
}
