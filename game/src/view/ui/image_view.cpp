#include "image_view.h"
#include "../texture.h"
#include "../drawer.h"
#include "../texture_manager.h"

ImageView::ImageView(const string& path)
    : path_(path), texture_(nullptr) {
  SetBgColor(COLOR_TRANSPARENT);
  SetPadding(0);
}

ImageView::ImageView(const Rect* frame, const string& path)
    : View(frame), path_(path), texture_(nullptr) {
  SetBgColor(COLOR_TRANSPARENT);
  SetPadding(0);
}

ImageView::~ImageView() {
}

void ImageView::SetPath(const string& path) {
  path_ = path;
  texture_ = nullptr;
}

void ImageView::RenderView(Drawer* drawer) {
  Rect frame = GetActualFrame();
  if (texture_ == nullptr) { // TODO better design?
    TextureManager* tm = drawer->GetTextureManager();
    texture_ = tm->FetchTexture(path_);
  }
  ASSERT(texture_ != nullptr);
  drawer->CopyTextureAbs(texture_, nullptr, nullptr);
}

bool ImageView::OnMouseButtonEvent(const MouseButtonEvent e) {
  if (e.IsLeftButtonDown()) {
    LOG_DEBUG("LeftButtonDown on the a ImageView");
    return true;
  }
  return true;
}

bool ImageView::OnMouseMotionEvent(const MouseMotionEvent e) {
  return false;
}
