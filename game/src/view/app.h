#ifndef WINDOW_VIEW_H_
#define WINDOW_VIEW_H_

#include <functional>
#include <vector>
#include "window.h"
#include "common.h"
#include "timer.h"

class View;
class Game;
class Drawer;

class FrameConfig {
 public:
  FrameConfig(uint16_t max_frames_sec);
  uint16_t GetMaxFps() const { return max_frames_sec_; }
  uint32_t MsecToFrame(uint32_t) const;
  uint32_t SecToFrame(uint32_t) const;

 private:
  uint16_t max_frames_sec_;
};

class FpsTimer {
 public:
  FpsTimer() : timer_(), frames_cur_sec_(0), fps_(0.f) {}
  void Start();
  void Update();
  float GetLastFps() { return fps_; }

 private:
  Timer timer_;
  uint32_t frames_cur_sec_;
  float fps_;
};

class App {
 public:
  App(int, int, uint32_t /* max_frame_sec */ = 60);
  ~App();

 public:
  void Run();

  // Getters and Setters
  Drawer* GetDrawer();
  Vec2D GetWindowSize() { return window_size_; }
  uint16_t GetMaxFps() { return frame_config_.GetMaxFps(); }
  uint32_t MsecToFrame(uint32_t ms) { return frame_config_.MsecToFrame(ms); }
  void SetMagicListViewVisible(bool);
  void SetQuit(bool b) { quit_ = b; }
  void EndGame();

  void StartNewGame();

 public:
  static const int kBlockSize = 48;

 private:
  bool HandleEvents();
  void Update();
  void Render();

 private:
  Vec2D   window_size_;
  Window* window_;
  Drawer* drawer_;
  View*   main_view_;
  View*   root_view_;
  View*   target_view_;
  Game*   game_;

  // fps
  const FrameConfig frame_config_;
  FpsTimer fps_timer_;
  int   frames_sec_;
  int   frames_total_;
  float fps_;

  bool quit_;
};

#endif
