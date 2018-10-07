#ifndef BLOCKMAP_H
#define BLOCKMAP_H

std::set<uint32_t> explore(uint8_t* blockmap, uint32_t i,
      std::size_t width, std::size_t height) {
   std::set<uint32_t> patch;
   std::stack<uint32_t> blocks;

   patch.insert(i); blocks.push(i);
   while (!blocks.empty()) {
      i = blocks.top(); blocks.pop();
      if (blockmap[i] & 0x3) { continue; }

      blockmap[i] |= 0x2; patch.insert(i);
      if (i%width) blocks.push(i-1);
      if (i%width+1 < width) blocks.push(i+1);
      if (i/width) blocks.push(i-width);
      if (i/width+1 < height) blocks.push(i+width);
   }

   return patch;
}

template<template<typename...> class T>
uint32_t bounding_box(const T<uint32_t>& patch, uint32_t width) {
   auto xb = std::minmax_element(std::begin(patch), std::end(patch),
      [&](auto a, auto b) { return (a%width) < (b%width); });
   auto yb = std::minmax_element(std::begin(patch), std::end(patch),
      [&](auto a, auto b) { return (a/width) < (b/width); });

   auto xspan = (*xb.second - *xb.first) % width + 1;
   auto yspan = *yb.second/width - *yb.first/width + 1;

   return xspan * yspan;
}

bool check_diagonal(uint32_t i, uint32_t j,
      uint8_t* blockmap, uint32_t width) {
   uint32_t ij = i + j%width - i%width;
   uint32_t ji = j + i%width - j%width;
   if (blockmap[ij] && blockmap[ji] && !blockmap[j])
      return false;
   return true;
}

#endif /* BLOCKMAP_H */
