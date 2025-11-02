#include "stdafx.h"
#include "ending.hpp"

#include "input/input.hpp"
#include "screen/fade.hpp"
#include "ui/text_window.hpp"

namespace bnscup2025::scene {

Ending::Ending(const InitData& init_data) :
  IScene(init_data) {

  if (getData().next_level == 12) {
    str_lines_ = {
      U"すると突然、シンハライトが輝き始めた。",
      U"驚きも束の間、放たれた光が\n目の前の人物を包み込んでゆく。",
      U"その者は言う。\n「これ…が…　シンパン…ということ…です…か……。」",
      U"そして輝いたまま床に伏し、動かなくなった。",
      U"シンハライトの量が少なかったのだろうか？\nどの道私には関係が無い。",
      U"報酬とやらも元々当てにはしていなかったが、\n私はシンになることはできなかった。",
      U"今はとりあえずここから出る方法を探さなければ…。"
    };
  }
  if (getData().next_level == 13) {
    str_lines_ = {
      U"すると突然、シンハライトが輝き始めた。",
      U"驚きも束の間、放たれた光が\n目の前の人物を包み込んでゆく。",
      U"その者は言う。\n「ふふふっ！　何というチカラでしょうか！」",
      U"「ワタシはシンになった！何でもできそうです！\n試しにこの洞窟を崩壊させてみましょうか！」",
      U"「……おっと。アナタには\n感謝しないといけませんね。報酬として…」",
      U"「このチカラを味合わう\n最初の人類にして差し上げます。」",
      U"強大な力により洞窟が崩れてゆく。\nもう助かるすべはないだろう。",
      U"私はシンになることができなかった。\n到底満足できる結果ではない。",
      U"あの幽霊の言う通り、\nこの者に石を渡してはならなかったのかもしれない…。"
    };
  }
  if (getData().next_level == 14) {
    str_lines_ = {
      U"私はシンになった。",
      U"目の前の者は形容しがたいほど滑稽な表情を浮かべている。",
      U"そもそもなぜこの者は洞窟の地下深くにいるのだ？\n何かあるに違いない。",
      U"私は訊く。\n「お前は一体何者なのだ？」",
      U"「おお、シンよ！\nワタシにもそのチカラをお与えください！」",
      U"会話が通じない相手だ。何者かは知らないが\n変わり者であることは間違いない。",
      U"変わり者にもこの洞窟にも嫌気がさしてきた。",
      U"シンの力でこの者も鬼もまとめて滅ぼしてやろうか。",
      U"その後のことは…　今は考えなくてもよいだろう。",
      U"神出でて、鬼没す。"
    };
  }

  ui::TextWindow::GetInstance().Reset();
  auto& fade = screen::Fade::GetInstance();
  fade.BeginFadeIn(0.5);
}

void Ending::update() {
  const auto& input_data = input::Input::GetInstance().GetData();
  auto& fade = screen::Fade::GetInstance();
  auto& text_window = ui::TextWindow::GetInstance();
  text_window.SetActive();
  text_window.Update();
  fade.Update();

  if (input_data.confirm_trigger) {
    if (current_line_index_ < str_lines_.size() - 1) {
      ++current_line_index_;
    }
    else {
      finished_ = true;
    }
  }

  if (finished_) {
    timer_ += Scene::DeltaTime();
    if (timer_ >= 1.0) {
      changeScene(SceneEnum::Title, 0);
    }
  }

  ui::TextWindow::GetInstance().SetText(str_lines_[current_line_index_]);

}

void Ending::draw() const {
  Scene::Rect().draw(Color { 0 });
  if (not finished_) {
    ui::TextWindow::GetInstance().Render();
  }
}


}
