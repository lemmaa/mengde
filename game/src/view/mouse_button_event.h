#ifndef MOUSE_BUTTON_EVENT_H_
#define MOUSE_BUTTON_EVENT_H_

#include "common.h"

class MouseButtonEvent {
 public:
  enum Button : uint16_t {
    kButtonNone,
    kButtonLeft,
    kButtonMiddle,
    kButtonRight
  };

  enum State : uint16_t {
    kStateIdle,
    kStateDown,
    kStateUp
  };

 public:
  MouseButtonEvent(const Button, const State, const Vec2D);
  bool IsLeftButtonDown() const;
  bool IsLeftButtonUp() const;
  bool IsMiddleButtonDown() const;
  bool IsMiddleButtonUp() const;
  bool IsRightButtonDown() const;
  bool IsRightButtonUp() const;
  Button GetButton() const { return mouse_button_; }
  State GetState() const { return mouse_button_state_; }
  Vec2D GetCoords() const { return coords_; }
  void SetCoords(Vec2D coords) { coords_ = coords; }

 private:
  Button mouse_button_;
  State  mouse_button_state_;
  Vec2D  coords_;
};

#endif // MOUSE_BUTTON_EVENT_H_