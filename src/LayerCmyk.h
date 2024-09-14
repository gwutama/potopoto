#ifndef POTOPOTO_LAYERCMYK_H
#define POTOPOTO_LAYERCMYK_H

#include <imgui.h>
#include "LayerBase.h"

class LayerCmyk : public LayerBase {
public:
    LayerCmyk();
    ~LayerCmyk() = default;

    void SetCyan(float in_cyan);
    void SetMagenta(float in_magenta);
    void SetYellow(float in_yellow);
    void SetBlack(float in_black);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_CYAN;
    static const float DEFAULT_MAGENTA;
    static const float DEFAULT_YELLOW;
    static const float DEFAULT_BLACK;

private:
    std::string GetName() override { return "Cmyk"; }
    bool Process(const ImVec2& top_left, const ImVec2& bottom_right) override;

private:
    float cyan;
    float magenta;
    float yellow;
    float black;
    bool values_have_changed;
};


#endif //POTOPOTO_LAYERCMYK_H
