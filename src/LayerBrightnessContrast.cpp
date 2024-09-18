#include "LayerBrightnessContrast.h"


const float LayerBrightnessContrast::DEFAULT_BRIGHTNESS = 1.0f;
const float LayerBrightnessContrast::DEFAULT_CONTRAST = 1.0f;


LayerBrightnessContrast::LayerBrightnessContrast() : brightness(DEFAULT_BRIGHTNESS), contrast(DEFAULT_CONTRAST) {
}


void LayerBrightnessContrast::SetBrightness(float in_brightness) {
    if (brightness == in_brightness) {
        return;
    }

    brightness = in_brightness;
    values_have_changed = true;
}


void LayerBrightnessContrast::SetContrast(float in_contrast) {
    if (contrast == in_contrast) {
        return;
    }

    contrast = in_contrast;
    values_have_changed = true;
}


bool LayerBrightnessContrast::Process(const cv::Rect& region) {
    // Input image is RGBA - Output image is RGBA
    if (brightness == DEFAULT_BRIGHTNESS && contrast == DEFAULT_CONTRAST) {
        return false;
    }

    // Combine brightness and contrast in a single convertTo call
    float alpha = contrast * brightness; // combined scale factor
    float beta = 128 * (1 - contrast);   // combined offset for contrast adjustment

    cv::UMat region_of_interest = (*image_adjusted)(region);
    region_of_interest.convertTo(region_of_interest, -1, alpha, beta);
    values_have_changed = false;

    return true;
}

