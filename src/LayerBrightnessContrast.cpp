#include "LayerBrightnessContrast.h"


const float LayerBrightnessContrast::DEFAULT_BRIGHTNESS = 1.0f;
const float LayerBrightnessContrast::DEFAULT_CONTRAST = 1.0f;


LayerBrightnessContrast::LayerBrightnessContrast() : brightness(DEFAULT_BRIGHTNESS), contrast(DEFAULT_CONTRAST) {
}


void LayerBrightnessContrast::Process() {
    // Input image is RGBA - Output image is RGBA
    if (brightness == DEFAULT_BRIGHTNESS && contrast == DEFAULT_CONTRAST) {
        return;
    }

    // Combine brightness and contrast in a single convertTo call
    float alpha = contrast * brightness; // combined scale factor
    float beta = 128 * (1 - contrast);   // combined offset for contrast adjustment

    image_adjusted->convertTo(*image_adjusted, -1, alpha, beta);
}

