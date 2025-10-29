#pragma once

namespace bnscup2025::npc {

class SpeakerLines {
  SpeakerLines() = delete;
  SpeakerLines(const SpeakerLines&) = delete;
  SpeakerLines& operator=(const SpeakerLines&) = delete;

public:
  static Array<String> Get(int level);
};

}
