#include "GuiManager.h"
#include "ScriptingLibrary.h"
#include "imgui.h"
#include "nurender.h"
#include "pch.h"

#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <string>
#include <vector>

#include "CoreMod.h"
#include "MinHook.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

static bool showConsole = false;

struct GameConsole {
  char InputBuf[256];
  std::vector<std::string> Items;
  ImVector<const char *> Commands;
  ImVector<char *> History;
  int HistoryPos;
  ImGuiTextFilter Filter;
  bool AutoScroll;
  bool ScrollToBottom;

  GameConsole() {
    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");
    AutoScroll = true;
    ScrollToBottom = false;
  }
  ~GameConsole() {
    ClearLog();
    for (int i = 0; i < History.Size; i++)
      free(History[i]);
  }

  static int Stricmp(const char *s1, const char *s2) {
    int d;
    while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
      s1++;
      s2++;
    }
    return d;
  }
  static int Strnicmp(const char *s1, const char *s2, int n) {
    int d = 0;
    while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
      s1++;
      s2++;
      n--;
    }
    return d;
  }
  static char *Strdup(const char *s) {
    IM_ASSERT(s);
    size_t len = strlen(s) + 1;
    void *buf = malloc(len);
    IM_ASSERT(buf);
    return (char *)memcpy(buf, (const void *)s, len);
  }
  static void Strtrim(char *s) {
    char *str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ')
      str_end--;
    *str_end = 0;
  }

  void ClearLog() { Items.clear(); }

  void AddLog(const std::string &log) // IM_FMTARGS(2)
  {
    Items.push_back(log);
  }

  void Draw(const char *title, bool *p_open) {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title, p_open)) {
      ImGui::End();
      return;
    }

    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::MenuItem("Close Console"))
        *p_open = false;
      ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Clear")) {
      ClearLog();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");

    ImGui::Separator();

    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
                      false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::Selectable("Clear"))
        ClearLog();
      ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
      ImGui::LogToClipboard();
    for (auto &str : Items) {
      if (!Filter.PassFilter(str.c_str()))
        continue;

      ImVec4 color;
      bool has_color = false;
      if (strstr(str.c_str(), "[error]")) {
        color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        has_color = true;
      } else if (strncmp(str.c_str(), "# ", 2) == 0) {
        color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
        has_color = true;
      }
      if (has_color)
        ImGui::PushStyleColor(ImGuiCol_Text, color);
      ImGui::TextUnformatted(str.c_str());
      if (has_color)
        ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
      ImGui::LogFinish();

    if (ScrollToBottom ||
        (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
      ImGui::SetScrollHereY(1.0f);
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf),
                         input_text_flags, &TextEditCallbackStub, this)) {
      char *s = InputBuf;
      Strtrim(s);
      if (s[0])
        ExecCommand(s);
      strcpy_s(s, IM_ARRAYSIZE(s), "");
      reclaim_focus = true;
    }

    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
      ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
  }

  void ExecCommand(const char *command_line) {
    auto str = std::string(command_line);

    AddLog("# " + str + "\n");

    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
      if (Stricmp(History[i], command_line) == 0) {
        free(History[i]);
        History.erase(History.begin() + i);
        break;
      }
    History.push_back(Strdup(command_line));

    if (str == "clear" || str == "cls") {
      ClearLog();
    } else if (str == "fennel") {
      CoreMod::useFennelInterpreter = true;
    } else if (str == "lua") {
      CoreMod::useFennelInterpreter = false;
    } else {
      CoreMod::execScript(str);
    }

    ScrollToBottom = true;
  }

  static int TextEditCallbackStub(ImGuiInputTextCallbackData *data) {
    GameConsole *console = (GameConsole *)data->UserData;
    return console->TextEditCallback(data);
  }

  int TextEditCallback(ImGuiInputTextCallbackData *data) {
    switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
      const char *word_end = data->Buf + data->CursorPos;
      const char *word_start = word_end;
      while (word_start > data->Buf) {
        const char c = word_start[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';')
          break;
        word_start--;
      }

      ImVector<const char *> candidates;
      for (int i = 0; i < Commands.Size; i++)
        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
            0)
          candidates.push_back(Commands[i]);

      if (candidates.Size == 0) {
        // AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start),
        // word_start);
      } else if (candidates.Size == 1) {
        data->DeleteChars((int)(word_start - data->Buf),
                          (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
      } else {
        int match_len = (int)(word_end - word_start);
        for (;;) {
          int c = 0;
          bool all_candidates_matches = true;
          for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
            if (i == 0)
              c = toupper(candidates[i][match_len]);
            else if (c == 0 || c != toupper(candidates[i][match_len]))
              all_candidates_matches = false;
          if (!all_candidates_matches)
            break;
          match_len++;
        }

        if (match_len > 0) {
          data->DeleteChars((int)(word_start - data->Buf),
                            (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0],
                            candidates[0] + match_len);
        }

        AddLog("Possible matches:\n");
        // for (int i = 0; i < candidates.Size; i++)
        // AddLog("- %s\n", candidates[i]);
      }

      break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
      const int prev_history_pos = HistoryPos;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (HistoryPos == -1)
          HistoryPos = History.Size - 1;
        else if (HistoryPos > 0)
          HistoryPos--;
      } else if (data->EventKey == ImGuiKey_DownArrow) {
        if (HistoryPos != -1)
          if (++HistoryPos >= History.Size)
            HistoryPos = -1;
      }

      if (prev_history_pos != HistoryPos) {
        const char *history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history_str);
      }
    }
    }
    return 0;
  }
};

GameConsole console;
static bool started = false;
static bool enableMouse = false;

WNDPROC lastProc;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static char emptyKeyState[0x100];
TRAVELLER_REGISTER_RAW_FUNCTION(0x6d5990, char *, ReadKey, void);
char *_fastcall stubReadKey(void *thisValue) { return emptyKeyState; }

void Gui::initializeImGui() {
  if (started)
    return;

  MH_CreateHook((LPVOID)ReadKey, (LPVOID)&stubReadKey, nullptr);

  ScriptingLibrary::log("Initializing ImGui");
  auto d3d9Device = reinterpret_cast<IDirect3DDevice9 **>(0x029765ec);

  ImGui::CreateContext();
  ImGui_ImplWin32_Init(_HWND);
  ImGui_ImplDX9_Init(*d3d9Device);

  ImGui::StyleColorsDark();
  ImGui::GetStyle().AntiAliasedFill = false;

  auto io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  lastProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(
      *_HWND, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc)));

  ScriptingLibrary::log("Initialized ImGui");

  started = true;
}

void Gui::writeToConsole(const std::string &value) { console.AddLog(value); }

void Gui::startRender(int width, int height) {

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();

  auto io = ImGui::GetIO();

  POINT pos;
  if (::GetCursorPos(&pos) && ::ScreenToClient(*_HWND, &pos)) {
    io.MousePos = ImVec2(pos.x, pos.y);
  }

  ImGui::NewFrame();

  ImGui::GetMainViewport()->Size = ImVec2(width, height);
  ImGui::GetMainViewport()->Pos = ImVec2(0, 0);

  if (showConsole) {
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    console.Draw("Console", &showConsole);
  }
}

void Gui::endRender() {
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  auto io = ImGui::GetIO();

  ScriptingLibrary::log(std::to_string(msg));

  if (msg == WM_KEYDOWN && wParam == VK_F9) {
    enableMouse = !enableMouse;

    if (enableMouse) {
      ShowCursor(TRUE);
      SetCapture(*_HWND);
    } else {
      ShowCursor(FALSE);
      ReleaseCapture();
    }
  }

  MH_DisableHook((LPVOID)ReadKey);
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
    MH_EnableHook((LPVOID)ReadKey);
    return true;
  }


  if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST && io.WantCaptureKeyboard) {
    MH_EnableHook((LPVOID)ReadKey);
    return true;
  }

  if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
  
    ScriptingLibrary::log("here?");
  }

  if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST && io.WantCaptureMouse) {
    ScriptingLibrary::log("Here!");
    return true;
  }

  if (msg == WM_KEYDOWN || msg == WM_KEYUP) {
    ScriptingLibrary::onKeyboardInput(msg, wParam);

    if (msg == WM_KEYDOWN && wParam == VK_F8) {
      showConsole = !showConsole;
    }
  }

  return CallWindowProc(lastProc, hWnd, msg, wParam, lParam);
}
