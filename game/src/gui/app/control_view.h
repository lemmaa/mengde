#ifndef CONTROL_VIEW_H_
#define CONTROL_VIEW_H_

#include "gui/uifw/composite_view.h"

namespace mengde {
namespace gui {
namespace app {

namespace mengde {
namespace core {
  class Game;
}
}

class RootView;
class TextView;

class ControlView : public CompositeView {
 public:
  ControlView(const Rect*, mengde::core::Game*, RootView*);
  void SetTurnText(int, int);

 private:
  mengde::core::Game* game_;
  RootView* rv_;
  TextView* tv_turn_;
};

} // namespace app
} // namespace gui
} // namespace mengde

#endif // CONTROL_VIEW_H_
