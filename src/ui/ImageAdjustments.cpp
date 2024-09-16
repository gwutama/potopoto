#include "ImageAdjustments.h"
#include <imgui.h>


void ImageAdjustments::Render() {
    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");

    // Local variables to hold the current values
    float brightness = adjustments_parameters.GetBrightness();
    float contrast = adjustments_parameters.GetContrast();

    float hue = adjustments_parameters.GetHue();
    float saturation = adjustments_parameters.GetSaturation();
    float value = adjustments_parameters.GetValue();

    float cyan = adjustments_parameters.GetCyan();
    float magenta = adjustments_parameters.GetMagenta();
    float yellow = adjustments_parameters.GetYellow();
    float black = adjustments_parameters.GetBlack();

    bool changed = false;

    // ImGui sliders to update the values
    if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f, "%.2f")) {
        if (brightness != adjustments_parameters.GetBrightness()) {
            adjustments_parameters.SetBrightness(brightness);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f, "%.2f")) {
        if (contrast != adjustments_parameters.GetContrast()) {
            adjustments_parameters.SetContrast(contrast);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Hue", &hue, 0.0f, 180.0f, "%.0f")) {
        if (hue != adjustments_parameters.GetHue()) {
            adjustments_parameters.SetHue(hue);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Saturation", &saturation, -128.0f, 128.0f, "%.0f")) {
        if (saturation != adjustments_parameters.GetSaturation()) {
            adjustments_parameters.SetSaturation(saturation);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Value", &value, -128.0f, 128.0f, "%.0f")) {
        if (value != adjustments_parameters.GetValue()) {
            adjustments_parameters.SetValue(value);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Cyan", &cyan, -1.0f, 1.0f, "%.2f")) {
        if (cyan != adjustments_parameters.GetCyan()) {
            adjustments_parameters.SetCyan(cyan);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Magenta", &magenta, -1.0f, 1.0f, "%.2f")) {
        if (magenta != adjustments_parameters.GetMagenta()) {
            adjustments_parameters.SetMagenta(magenta);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Yellow", &yellow, -1.0f, 1.0f, "%.2f")) {
        if (yellow != adjustments_parameters.GetYellow()) {
            adjustments_parameters.SetYellow(yellow);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    if (ImGui::SliderFloat("Black", &black, -1.0f, 1.0f, "%.2f")) {
        if (black != adjustments_parameters.GetBlack()) {
            adjustments_parameters.SetBlack(black);
            changed = true;
        }
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        mouse_released();
    }

    // Emit the signal if any parameter has changed
    if (changed) {
        parameters_changed(adjustments_parameters);
    }

    ImGui::EndChild();
}


void ImageAdjustments::Reset() {
    adjustments_parameters.Reset();
}
