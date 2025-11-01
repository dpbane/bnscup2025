#pragma once

#include <vector>

namespace bnscup2025::terrain {

template<typename T>
class GridPoints {
public:
  GridPoints(size_t width, size_t height) :
    width_(width),
    height_(height),
    values_(width* height) {
  }

  GridPoints(Size size) :
    width_(static_cast<size_t>(size.x)),
    height_(static_cast<size_t>(size.y)),
    values_(width_* height_) {
  }

  Size GetSize() const { return Size(static_cast<int32>(width_), static_cast<int32>(height_)); }

  bool IsBound(Point pos) const {
    if (pos.x < 0) return false;
    if (pos.y < 0) return false;
    if (static_cast<size_t>(pos.x) >= width_) return false;
    if (static_cast<size_t>(pos.y) >= height_) return false;
    return true;
  }

  T Get(Point pos) const {
    const auto index = GetIndex(pos);
    if (not index) return T {};
    return values_[*index];
  }

  void Set(Point pos, T value) {
    const auto index = GetIndex(pos);
    if (not index) return;
    values_[*index] = value;
  }

  void Fill(T value) {
    std::fill(values_.begin(), values_.end(), value);
  }

  GridPoints<T>& operator=(const GridPoints<T>& other);

private:
  Optional<size_t> GetIndex(Point pos) const {
    if (not IsBound(pos)) return none;
    return Optional<size_t>(static_cast<size_t>(pos.y) * width_ + static_cast<size_t>(pos.x));
  }

private:
  const size_t width_;
  const size_t height_;
  std::vector<T> values_;
};

template<typename T>
inline GridPoints<T>& GridPoints<T>::operator=(const GridPoints<T>& other) {
  if (this != &other) {
    if (width_ != other.width_ || height_ != other.height_) {
      throw std::runtime_error("GridPoints assignment: size mismatch");
    }
    values_ = other.values_;
  }
  return *this;
}

}
