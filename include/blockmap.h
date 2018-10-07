#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include <algorithm>
#include <iostream>
#include <set>
#include <stack>
#include <string>

class blockmap {
   private:
      uint8_t* map_;

      uint32_t width_;
      uint32_t height_;
      uint32_t size_;

   public:
      template<template<typename...> class T>
      blockmap(const T<std::string>& lines) { load(lines); }

      uint8_t* map() { return map_; }
      uint32_t size() { return size_; }

      void reset_map() { memset(map_, 0, size_ * sizeof(uint8_t)); }

      template<template<typename...> class T>
      void load(const T<std::string>& lines);

      template<template<typename...> class T>
      void rewrite(const T<uint32_t>& set, uint8_t i);

      std::set<uint32_t> explore(uint32_t i);
      void expand_block(std::set<uint32_t>& patch);

      template<template<typename...> class T>
      uint32_t bounding_box(const T<uint32_t>& patch);

      template<typename L>
      void display_2d(L lambda);

      bool check_well_formed(uint32_t i);
      bool check_neighbours(uint32_t i);
      bool check_diagonal(uint32_t i, uint32_t j);
};

template<template<typename...> class T>
void blockmap::load(const T<std::string>& lines) {
   width_ = std::max_element(std::begin(lines), std::end(lines),
      [](std::string a, std::string b) {
         return a.size() < b.size(); })->size();
   height_ = lines.size();
   size_ = width_ * height_;

   /* build array of blocks */
   map_ = new uint8_t[size_];
   for (uint32_t i=0; i<lines.size(); ++i)
      for (uint32_t c=0; c<lines[i].size(); ++c)
         map_[i*width_+c] = lines[i][c] != ' ';
}

template<template<typename...> class T>
void blockmap::rewrite(const T<uint32_t>& set, uint8_t i) {
   for (const auto& m : set)
      map_[m] = i;
}

std::set<uint32_t> blockmap::explore(uint32_t i) {
   std::set<uint32_t> patch;
   std::stack<uint32_t> blocks;

   patch.insert(i); blocks.push(i);
   while (!blocks.empty()) {
      i = blocks.top(); blocks.pop();
      if (map_[i] & 0x3) { continue; }

      map_[i] |= 0x2; patch.insert(i);
      if (i%width_) blocks.push(i-1);
      if (i%width_+1 < width_) blocks.push(i+1);
      if (i/width_) blocks.push(i-width_);
      if (i/width_+1 < height_) blocks.push(i+width_);
   }

   return patch;
}

void blockmap::expand_block(std::set<uint32_t>& patch) {
   for (const auto& b : patch)
      patch.insert(b-1);
   for (const auto& b : patch)
      patch.insert(b-width_);
}

template<template<typename...> class T>
uint32_t blockmap::bounding_box(const T<uint32_t>& patch) {
   auto xb = std::minmax_element(std::begin(patch), std::end(patch),
      [&](auto a, auto b) { return (a%width_) < (b%width_); });
   auto yb = std::minmax_element(std::begin(patch), std::end(patch),
      [&](auto a, auto b) { return (a/width_) < (b/width_); });

   auto xspan = (*xb.second - *xb.first) % width_ + 1;
   auto yspan = *yb.second/width_ - *yb.first/width_ + 1;

   return xspan * yspan;
}

template<typename L>
void blockmap::display_2d(L lambda) {
   for (uint32_t i=0; i<height_; ++i) {
      for (uint32_t j=0; j<width_; ++j)
         std::cout << lambda(map_[i*width_+j]);
      std::cout << std::endl;
   }
}

bool blockmap::check_well_formed(uint32_t i) {
   if (map_[i]) {
      if (!check_neighbours(i)) {
         printf("error: incomplete border\n");
         return false;
      }
   } else {
      /* assume patch blocks cannot exist on edges */
      if (!check_diagonal(i, i-width_-1)
            || !check_diagonal(i, i-width_+1)) {
         printf("error: diagonal border\n");
         return false;
      }
   }

   return true;
}

bool blockmap::check_neighbours(uint32_t i) {
   uint32_t nb = 0;
   if (i%width_ && map_[i-1]) ++nb;
   if (i%width_+1 < width_ && map_[i+1]) ++nb;
   if (i/width_ && map_[i-width_]) ++nb;
   if (i/width_+1 < height_ && map_[i+width_]) ++nb;

   if (nb < 2)
      return false;
   return true;
}

bool blockmap::check_diagonal(uint32_t i, uint32_t j) {
   uint32_t ij = i + j%width_ - i%width_;
   uint32_t ji = j + i%width_ - j%width_;
   if (map_[ij] && map_[ji] && !map_[j])
      return false;
   return true;
}

#endif /* BLOCKMAP_H */
