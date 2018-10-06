#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <vector>

static std::vector<const char*> colours = {
   "\033[0m", "\033[41m", "\033[43m", "\033[42m",
   "\033[46m", "\033[44m", "\033[45m", "\033[101m" };

std::set<uint32_t> explore(uint8_t* blockmap,
   uint32_t i, std::size_t width, std::size_t height);

inline void dump_patches(const std::vector<
      std::set<uint32_t>>& patches) {
   for (const auto& p : patches) {
      std::adjacent_difference(std::begin(p), std::end(p),
         std::ostream_iterator<uint32_t>(std::cout),
         [](uint32_t x, uint32_t) -> uint32_t {
            return std::cout << ", ", x; });
      printf("\n");
   }
}

int chart(const char* input) {
   /* open file */
   std::ifstream fstream(input);
   if (!fstream) {
      printf("invalid file\n");
      return 1;
   }

   /* read input */
   std::string line;
   std::vector<std::string> lines;
   while (std::getline(fstream, line))
      lines.emplace_back(std::move(line));

   auto width = std::max_element(std::begin(lines), std::end(lines),
      [](std::string a, std::string b) {
         return a.size() < b.size(); })->size();
   auto height = lines.size();
   auto size = width * height;

   /* build array of blocks */
   uint8_t* blockmap = new uint8_t[size];
   for (uint32_t i = 0; i < lines.size(); ++i)
      for (uint32_t c = 0; c < lines[i].size(); ++c)
         blockmap[i * width + c] = lines[i][c] != ' ';

   /* TODO: infill from edges to outer border */
   /* assume input maximally subtends array   */

   /* TODO: check for stranded islands                 */
   /* check for border blocks with only one connection */

   /* find patches */
   std::vector<std::set<uint32_t>> patches;
   for (uint32_t i = 0; i < size; ++i) {
      if (blockmap[i] & 0x3) { continue; }
      patches.emplace_back(explore(blockmap, i,
         width, height));
   }

   /* TODO: check for diagonals   */
   /* assume input is well-formed */

   /* expand all patches to pixel left/above */
   for (auto& p : patches) {
      for (const auto& b : p)
         p.insert(b-1);
      for (const auto& b : p)
         p.insert(b-width);
   }

   /* reformat blockmap */
   memset(blockmap, 0, size * sizeof(uint8_t));
   for (uint32_t i = 0; i < patches.size(); ++i) {
      const auto& p = patches[i];
      for (const auto& b : p)
         blockmap[b] = i + 1;
   }

   /* output coloured diagram */
   if (patches.size() < colours.size()) {
      for (uint32_t i = 0; i < size; ++i) {
         printf("%s %s", colours[blockmap[i]], colours[0]);
         if (i % width == width - 1)
            printf("\n");
      }
   } else {
      printf("too many patches for coloured display\n");
   }

   /* find bounding rectangles for all patches */

   /* input dimensions
    *    highlight frame border
    *    automatically identify equivalent borders
    *       and assign dimensions
    */

   return 0;
}

std::set<uint32_t> explore(uint8_t* blockmap, uint32_t i,
      std::size_t width, std::size_t height) {
   std::set<uint32_t> blocks;
   std::queue<uint32_t> queue;

   blocks.insert(i); queue.push(i);
   while (!queue.empty()) {
      i = queue.front(); queue.pop();
      if (blockmap[i] & 0x3) { continue; }

      blockmap[i] |= 0x2; blocks.insert(i);
      if (i % width) queue.push(i-1);
      if (i % width + 1 < width) queue.push(i+1);
      if (i / width) queue.push(i-width);
      if (i / width + 1 < height) queue.push(i+width);
   }

   return blocks;
}

int main(int argc, char* argv[]) {
   if (argc == 2)
      return chart(argv[1]);

   printf("usage: %s [input]\n", argv[0]);
   return 1;
}
