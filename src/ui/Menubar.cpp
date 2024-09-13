#include "Menubar.h"

#include <imgui.h>
#include <tinyfiledialogs.h>
#include <SDL.h>


void Menubar::Render() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open")) {
            const char *filename = tinyfd_openFileDialog("Open Image", "", 0,
                                                         nullptr, nullptr, 0);
            if (filename) {
                std::string filename_str(filename);
                open_signal(filename_str);
            }
        }

        if (ImGui::MenuItem("Save")) {
            save_signal();
        }

        if (ImGui::MenuItem("Close")) {
            close_signal();
        }

        if (ImGui::MenuItem("Exit")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }

        ImGui::EndMainMenuBar();
    }
}
