#include "unit_info_view.h"

#include "core/unit.h"
#include "core/cell.h"
#include "view/uifw/gauge_view.h"
#include "view/uifw/text_view.h"
#include "view/foundation/drawer.h"
#include "view/foundation/rect.h"

UnitInfoView::UnitInfoView(const Rect* frame, mengde::core::Unit* unit)
    : CompositeView(frame),
      unit_(unit),
      gv_hp_(nullptr),
      gv_mp_(nullptr),
      tv_name_(nullptr),
      tv_lv_(nullptr),
      tv_lftbot_(nullptr),
      tv_rgtbot_(nullptr) {
  const int kAlpha = 160;
  SetBgColor({64, 64, 64, kAlpha});
  SetPadding(8);

  Rect gv_frame = {0, 22, 184, 16};
  gv_hp_ = new GaugeView(&gv_frame,
                         0,
                         1,
                         0,
                         COLOR("gauge_hp", kAlpha),
                         COLOR("gauge_bg", kAlpha),
                         COLOR("gauge_hp_damage", kAlpha));
  gv_frame.Move(0, 22);
  gv_mp_ = new GaugeView(&gv_frame,
                         0,
                         1,
                         COLOR("gauge_mp", kAlpha),
                         COLOR("gauge_bg", kAlpha));
  gv_hp_->SetHelpTextType(GaugeView::kHelpTextCurMax);
  gv_mp_->SetHelpTextType(GaugeView::kHelpTextCurMax);

  Rect tv_rect = GetActualFrame();
  tv_name_   = new TextView(&tv_rect, "", COLOR("white"), 14, LayoutHelper::kAlignLftTop);
  tv_lv_     = new TextView(&tv_rect, "", COLOR("white"), 14, LayoutHelper::kAlignRgtTop);
  tv_lftbot_ = new TextView(&tv_rect, "", COLOR("white"), 14, LayoutHelper::kAlignLftBot);
  tv_rgtbot_ = new TextView(&tv_rect, "", COLOR("white"), 14, LayoutHelper::kAlignRgtBot);

  AddChild(gv_hp_);
  AddChild(gv_mp_);
  AddChild(tv_name_);
  AddChild(tv_lv_);
  AddChild(tv_lftbot_);
  AddChild(tv_rgtbot_);
}

UnitInfoView::~UnitInfoView() {
}

void UnitInfoView::SetUnitTerrainInfo(mengde::core::Cell* cell) {
  ASSERT(cell->IsUnitPlaced());

  mengde::core::Unit* unit = cell->GetUnit();
  if (unit_ == unit) return;
  SetUnit(unit);

  string name = cell->GetTerrainName();
  int effect = cell->GetTerrainEffectThisCell();
  string terrain_effect = name + " " + std::to_string(effect) + "%";
  tv_rgtbot_->SetText(terrain_effect);
}

void UnitInfoView::SetUnitAttackInfo(mengde::core::Unit* unit, int accuracy, int expected_damage) {
  SetUnit(unit);
  gv_hp_->SetExtVal(expected_damage);
  tv_rgtbot_->SetText("Accuracy : " + std::to_string(accuracy) + "%");
}

void UnitInfoView::SetUnit(mengde::core::Unit* unit) {
  unit_ = unit;
  const mengde::core::HpMp& cur_xtat = unit_->GetCurrentHpMp();
  const mengde::core::HpMp& ori_xtat = unit_->GetOriginalHpMp();
  gv_hp_->SetCurVal(cur_xtat.hp);
  gv_hp_->SetMaxVal(ori_xtat.hp);
  gv_hp_->SetExtVal(0);
  gv_mp_->SetCurVal(cur_xtat.mp);
  gv_mp_->SetMaxVal(ori_xtat.mp);
  gv_mp_->SetExtVal(0);
  tv_name_->SetText(unit_->GetId());
  tv_lv_->SetText("Lv " + std::to_string(unit_->GetLevel()));
}

void UnitInfoView::SetContents(const std::string& id,
                               int lv,
                               const mengde::core::HpMp& xcur,
                               const mengde::core::HpMp& xmax,
                               int damage,
                               int mp_cost) {
  unit_ = nullptr; // Reset cache
  gv_hp_->SetCurVal(xcur.hp);
  gv_hp_->SetMaxVal(xmax.hp);
  gv_hp_->SetExtVal(damage);
  gv_mp_->SetCurVal(xcur.mp);
  gv_mp_->SetMaxVal(xmax.mp);
  gv_mp_->SetExtVal(mp_cost);
  tv_name_->SetText(id);
  tv_lv_->SetText("Lv " + std::to_string(lv));
  tv_lftbot_->SetText("");
  tv_rgtbot_->SetText("");
}

void UnitInfoView::SetCoordsByUnitCoords(Vec2D unit_cell, Vec2D camera_coords, Vec2D game_frame_size) {
  const int kCellSize = 48;
  const Vec2D frame_size = GetFrameSize();
  Vec2D cands[] = {(unit_cell + Vec2D(1, 0)) * kCellSize,
                   (unit_cell + Vec2D(0, 1)) * kCellSize,
                   (unit_cell * kCellSize - Vec2D(frame_size.x, frame_size.y - kCellSize)),
                   (unit_cell * kCellSize - Vec2D(frame_size.x - kCellSize, frame_size.y))};
  Rect game_frame({0, 0}, game_frame_size);
  Vec2D res(0, 0);
  for (auto e : cands) {
    Vec2D calc_lt = e - camera_coords;
    Vec2D calc_rb = calc_lt + GetFrameSize();;
    if (game_frame.Contains(calc_lt) && game_frame.Contains(calc_rb)) {
      res = calc_lt;
      break;
    }
  }
  SetCoords(res);
}

bool UnitInfoView::OnMouseMotionEvent(const MouseMotionEvent) {
  return false;
}