#ifndef DRAWER_H_
#define DRAWER_H_

#include <stack>
#include "util/common.h"
#include "color.h"
#include "rect.h"
#include "sprite_type.h"
#include "layout_helper.h"

struct Viewport {
  Rect  rect;
  Vec2D neg_coords;
  Viewport(const Rect& r, Vec2D neg) : rect(r), neg_coords(neg) { /* ASSERT(neg.x < 0 && neg.y < 0); */ }
};

class Window;
class Renderer;
class Texture;
class TextureManager;

class Drawer {
 public:
  Drawer(Window* window);
  ~Drawer();
  Vec2D GetWindowSize() { return window_size_; }
  TextureManager* GetTextureManager() { return texture_manager_; }
  Renderer* GetRenderer() { return renderer_; }
  void Start();
  void End();
  void SetViewport(const Rect*);
  void ResetViewport();
  void SetOffset(Vec2D offset) { offset_ = offset; }
  Vec2D GetOffset() { return offset_; } // FIXME Remove this method

  // Draw Functions
  void SetDrawColor(Color c);
  void CopyTexture(Texture*, Rect*, Rect*, bool = false);
  void CopyTextureToCell(Texture*, Rect*, Vec2D);
  void CopyTextureBackground(Texture*, Rect* = NULL, Rect* = NULL);
  void CopySprite(const std::string&,
                  SpriteType,
                  Direction,
                  int,
                  SpriteEffect,
                  Vec2D,
                  Vec2D = {0, 0});
  void DrawRect(const Rect*, const int = 1);
  void FillRect(const Rect*);
  void DrawText(const std::string&, int, Color, Vec2D);
  void DrawText(const std::string&, int, Color, const Rect*, LayoutHelper::Align);
  void BorderCell(Vec2D, const int = 1);
  void FillCell(Vec2D);

 private:
  TextureManager* texture_manager_;
  Renderer* renderer_;
  Vec2D window_size_;
  Vec2D offset_;
  std::stack<Viewport> viewports_;
};

#endif // DRAWER_H_
