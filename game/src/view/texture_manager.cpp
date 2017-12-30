#include "texture_manager.h"
#include "util/common.h"

TextureManager::TextureManager(Renderer* renderer) : renderer_(renderer) {
  // Create dummy texture : a big square fully filled white
  SDL_Surface* s = SDL_CreateRGBSurface(0, 1024, 1024, 32, 0, 0, 0, 0);
  SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 255, 255, 255));
  dummy_texture_ = new Texture(renderer_, s);
}

TextureManager::~TextureManager() {
  for (const auto& u : container_) {
    Texture* texture = u.second;
    delete texture;
  }
  delete dummy_texture_;
}

Texture* TextureManager::FetchTexture(const string& key) {
  auto iter = container_.find(key);
  if (iter == container_.end()) {
    Texture* texture = new Texture(renderer_, key, Color(247, 0, 255)); // XXX colorkey hardcoded
    if (texture->Loaded()) {
      container_[key] = texture;
      return texture;
    } else {
      // When failed loading the bitmap file failed we just return the dummy texture
      delete texture;
      return dummy_texture_;
    }
  } else {
    return iter->second;
  }
}

Texture* TextureManager::FetchWhitenedTexture(const string& key) {
  const string whitened_key = key + "?white";
  auto iter = container_.find(whitened_key);
  if (iter == container_.end()) {
    Texture* orig_texture = FetchTexture(key);
    if (!orig_texture->Loaded()) {
      return dummy_texture_;
    }
    Texture* new_texture = orig_texture->NewWhitenedTexture(renderer_);
    container_[whitened_key] = new_texture;
    return new_texture;
  } else {
    return iter->second;
  }
}

Texture* TextureManager::FetchTextTexture(const string& text,
                                          int size,
                                          Color color,
                                          uint32_t wrap) {
  string key = std::to_string(size) +
                    string(":") + text +
                    string(":") + color.ToHexString();
  auto iter = container_.find(key);
  if (iter == container_.end()) {
    Texture* texture = new Texture(renderer_, text, size, color, wrap);
    container_[key] = texture;
    return texture;
  } else {
    return iter->second;
  }
}
