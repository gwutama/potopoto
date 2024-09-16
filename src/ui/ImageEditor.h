#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once

#include <imgui.h>

#include "../ImagePreview.h"
#include "../ImageHistogram.h"
#include "../MetadataReader.h"
#include "../AdjustmentsParameters.h"

#include "Menubar.h"
#include "Toolbar.h"
#include "Histogram.h"
#include "ImageInfo.h"
#include "ExifMetadata.h"
#include "FileInfo.h"
#include "ImageAdjustments.h"
#include "Canvas.h"

class ImageEditor {
public:
    ImageEditor();
    ~ImageEditor();
    void Render();

private:
    void OnOpenClicked(const std::string& filename);
    void OnSaveClicked();
    void OnCloseClicked();

    void OnImageAdjustmentsParametersChanged(const AdjustmentsParameters& parameters);
    void OnImageAdjustmentsMouseReleased();

    void RenderImageAnalysisTabs();

private:
    Menubar menubar;
    Toolbar toolbar;
    Histogram histogram;
    ImageInfo image_info;
    ExifMetadata exif_metadata;
    FileInfo file_info;
    ImageAdjustments image_adjustments;
    Canvas canvas;

    std::shared_ptr<ImagePreview> image_preview;
    std::shared_ptr<ImageHistogram> image_histogram;
    MetadataReader metadata_reader;
};

#endif //POTOPOTO_IMAGEEDITOR_H
