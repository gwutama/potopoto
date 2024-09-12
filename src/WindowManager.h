#ifndef POTOPOTO_WINDOWMANAGER_H
#define POTOPOTO_WINDOWMANAGER_H

#pragma once
#include <SDL.h>

bool InitializeWindow();
void ApplyCustomStyle();
bool ProcessEvents();
void StartImGuiFrame();
void RenderFrame();
void Cleanup();


#endif //POTOPOTO_WINDOWMANAGER_H
