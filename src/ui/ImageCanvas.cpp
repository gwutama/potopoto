#include "ImageCanvas.h"

wxBEGIN_EVENT_TABLE(ImageCanvas, wxGLCanvas)
                EVT_PAINT(ImageCanvas::OnPaint)
                EVT_SIZE(ImageCanvas::OnResize)
                EVT_LEFT_DOWN(ImageCanvas::OnMouseDown)
                EVT_LEFT_UP(ImageCanvas::OnMouseUp)
                EVT_MOTION(ImageCanvas::OnMouseMove)
wxEND_EVENT_TABLE()

ImageCanvas::ImageCanvas(wxWindow *parent, std::shared_ptr<ImagePreview2> imagePreview)
        : wxGLCanvas(parent, wxID_ANY, nullptr), imagePreview(imagePreview), zoomFactor(1.0f),
          offsetX(0.0f), offsetY(0.0f), imageLoaded(false), textureId(0), isDragging(false),
          currentLodLevel(ImagePreview2::LodLevel::LOW) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    glContext = new wxGLContext(this);
}


ImageCanvas::~ImageCanvas() {
    if (glContext) {
        delete glContext;
    }

    if (textureId) {
        glDeleteTextures(1, &textureId);
    }
}


void ImageCanvas::Reset() {
    imagePreview->Reset();
    imageLoaded = false;
    zoomFactor = 1.0f;
    offsetX = 0.0f;
    offsetY = 0.0f;
    isDragging = false;
    dragStartPos = wxPoint(0, 0);
    lastOffsetX = 0.0f;
    lastOffsetY = 0.0f;
    currentLodLevel = ImagePreview2::LodLevel::LOW;

    if (textureId) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }

    Refresh();  // This will trigger OnPaint to clear the canvas to dark grey
}


void ImageCanvas::LoadImage(const std::shared_ptr<Image> &image) {
    imagePreview->LoadImage(image);  // Load the image into ImagePreview

    // Set the initial LOD level to LOW and get the image for display
    imagePreview->SetLodLevel(ImagePreview2::LodLevel::LOW);

    imageLoaded = true;
    UpdateTexture();  // Update OpenGL texture based on the new image
    FitImageToCanvas();
    Refresh();
}


void ImageCanvas::SetZoomLevel(float zoom) {
    zoomFactor = std::clamp(zoom, MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);
    UpdateLodLevel();  // Update LOD level based on the zoom factor
    Refresh();
}


void ImageCanvas::CenterImageOnCanvas() {
    wxSize clientSize = GetClientSize();
    auto high_size = imagePreview->GetSize(ImagePreview2::LodLevel::HIGH);
    float scaledWidth = high_size.width * zoomFactor;
    float scaledHeight = high_size.height * zoomFactor;

    offsetX = (clientSize.GetWidth() - scaledWidth) / 2.0f;
    offsetY = (clientSize.GetHeight() - scaledHeight) / 2.0f;

    Refresh();
}


void ImageCanvas::FitImageToCanvas() {
    if (!imageLoaded)
        return;

    wxSize clientSize = GetClientSize();
    int paddingPx = 100;
    auto high_size = imagePreview->GetSize(ImagePreview2::LodLevel::HIGH);
    float scaleX = static_cast<float>(clientSize.GetWidth()) / (high_size.width + paddingPx);
    float scaleY = static_cast<float>(clientSize.GetHeight()) / (high_size.height + paddingPx);
    zoomFactor = std::min(scaleX, scaleY);

    UpdateLodLevel();  // Update the LOD level after fitting the image
    CenterImageOnCanvas();
}


void ImageCanvas::UpdateLodLevel() {
    ImagePreview2::LodLevel newLodLevel;

    if (zoomFactor <= 0.5f)
        newLodLevel = ImagePreview2::LodLevel::LOW;
    else if (zoomFactor <= 1.5f)
        newLodLevel = ImagePreview2::LodLevel::MEDIUM;
    else
        newLodLevel = ImagePreview2::LodLevel::HIGH;

    if (newLodLevel != currentLodLevel) {
        currentLodLevel = newLodLevel;
        imagePreview->SetLodLevel(currentLodLevel);  // Inform ImagePreview of the new LOD level
        UpdateTexture();  // Update the OpenGL texture with the new LOD image
    }
}


void ImageCanvas::EnableGestures(bool enable) {
    if (enable) {
        if (!EnableTouchEvents(wxTOUCH_ZOOM_GESTURE)) {
            wxLogError("Failed to enable touch events");
        }

        Bind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    } else {
        Unbind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    }
}


void ImageCanvas::SetZoomCallback(std::function<void(float)> callback) {
    zoomCallback = callback;
}


void ImageCanvas::OnPaint(wxPaintEvent &evt) {
    wxPaintDC dc(this);
    SetCurrent(*glContext);

    // Set the background color to dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!imageLoaded || textureId == 0) {
        SwapBuffers();  // Apply the clear operation
        return;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    wxSize clientSize = GetClientSize();
    glOrtho(0, clientSize.GetWidth(), clientSize.GetHeight(), 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(offsetX, offsetY, 0);
    glScalef(zoomFactor, zoomFactor, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);

    auto high_size = imagePreview->GetSize(ImagePreview2::LodLevel::HIGH);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(high_size.width, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(high_size.width, high_size.height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, high_size.height);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    SwapBuffers();
}


void ImageCanvas::OnResize(wxSizeEvent &evt) {
    FitImageToCanvas();
}


void ImageCanvas::OnMouseDown(wxMouseEvent &event) {
    if (event.LeftDown()) {
        SetCursor(wxCursor(wxCURSOR_HAND));
        isDragging = true;
        dragStartPos = event.GetPosition();
        lastOffsetX = offsetX;
        lastOffsetY = offsetY;
    }
}


void ImageCanvas::OnMouseUp(wxMouseEvent &event) {
    if (event.LeftUp()) {
        SetCursor(wxCursor(wxCURSOR_DEFAULT));
        isDragging = false;
    }
}


void ImageCanvas::OnMouseMove(wxMouseEvent &event) {
    if (isDragging) {
        wxPoint currentPos = event.GetPosition();
        int deltaX = currentPos.x - dragStartPos.x;
        int deltaY = currentPos.y - dragStartPos.y;

        offsetX = lastOffsetX + deltaX;
        offsetY = lastOffsetY + deltaY;

        Refresh();  // Redraw the canvas after panning
    }
}


void ImageCanvas::OnGestureZoom(wxZoomGestureEvent &evt) {
    wxPoint mousePos = ScreenToClient(wxGetMousePosition());

    float mouseImageX = (mousePos.x - offsetX) / zoomFactor;
    float mouseImageY = (mousePos.y - offsetY) / zoomFactor;

    float zoomDelta = evt.GetZoomFactor();
    zoomFactor = std::clamp(zoomFactor * (1 + (zoomDelta - 1) * 0.2f), MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);

    UpdateLodLevel();  // Update the LOD level after zooming

    offsetX = mousePos.x - mouseImageX * zoomFactor;
    offsetY = mousePos.y - mouseImageY * zoomFactor;

    if (zoomCallback) {
        zoomCallback(zoomFactor);
    }

    Refresh();
}


void ImageCanvas::UpdateTexture() {
    if (!imageLoaded)
        return;

    SetCurrent(*glContext);

    if (textureId) {
        glDeleteTextures(1, &textureId);
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Get the current LOD image from ImagePreview
    auto img = imagePreview->GetImage();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


cv::Rect ImageCanvas::GetVisibleImageRegion() {
    if (!imageLoaded) {
        return cv::Rect();  // Return an empty rectangle if no image is loaded
    }

    // Get the size of the visible canvas area
    wxSize clientSize = GetClientSize();

    // Get the size of the current LOD image
    auto lodSize = imagePreview->GetSize(currentLodLevel);

    // Compute the top-left corner of the visible region in the image coordinate system
    float imageXStart = (0 - offsetX) / zoomFactor;
    float imageYStart = (0 - offsetY) / zoomFactor;

    // Compute the size of the visible region in image coordinates
    float visibleWidth = clientSize.GetWidth() / zoomFactor;
    float visibleHeight = clientSize.GetHeight() / zoomFactor;

    // Clamp the values to ensure they are within the image bounds
    int x = std::clamp(static_cast<int>(imageXStart), 0, lodSize.width);
    int y = std::clamp(static_cast<int>(imageYStart), 0, lodSize.height);
    int width = std::clamp(static_cast<int>(visibleWidth), 0, lodSize.width - x);
    int height = std::clamp(static_cast<int>(visibleHeight), 0, lodSize.height - y);

    // Return the visible region as a rectangle
    return cv::Rect(x, y, width, height);
}
