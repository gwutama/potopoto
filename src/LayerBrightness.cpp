#include "LayerBrightness.h"

void LayerBrightness::Process() {
    // Input image is RGBA - Output image is RGBA
    if (brightness == 1.0f) {
        return;
    }

    image.convertTo(adjusted_image, -1, brightness, 0);
}
