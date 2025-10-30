#pragma once

#include "player/player.hpp"

namespace bnscup2025::ui {

class TutorialText {
public:
  struct Entry {
    RectF area;
    String text;
  };

public:
  TutorialText(const player::Player& player);

  void AddEntry(const Entry& entry);

  void Update();
  void Render() const;

private:
  const player::Player& player_;
  Array<Entry> entries_;

  double alpha_ { 0.0 };
  String current_text_;
};

}
