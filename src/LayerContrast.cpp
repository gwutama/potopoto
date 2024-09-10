#include "LayerContrast.h"

void LayerContrast::Process() {
    // Input image is RGBA - Output image is RGBA
    if (contrast == 1.0f) {
        return;
    }

    image.convertTo(adjusted_image, -1, contrast, 128 * (1 - contrast));
}