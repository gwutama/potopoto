#ifndef POTOPOTO_BLURFILTERPARAMETERS_H
#define POTOPOTO_BLURFILTERPARAMETERS_H

#include "IImageProcessingParameters.h"

class BlurFilterParameters : public IImageProcessingParameters {
public:
    BlurFilterParameters() { Reset(); }
    ~BlurFilterParameters() = default;

    void Reset() override {
        strength = 0;
    }

    bool operator==(const IImageProcessingParameters& other) const override {
        const auto* otherParams = dynamic_cast<const BlurFilterParameters*>(&other);
        return otherParams && strength == otherParams->strength;
    }

    bool operator!=(const IImageProcessingParameters& other) const override {
        return !(*this == other);
    }

    int GetStrength() const { return strength; }
    void SetStrength(int value) { strength = value; }

private:
    int strength;
};

#endif //POTOPOTO_BLURFILTERPARAMETERS_H
