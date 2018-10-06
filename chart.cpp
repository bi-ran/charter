#include <fstream>
#include <queue>
#include <set>
#include <string>
#include <vector>

std::set<uint32_t> explore(uint8_t* blockmap,
   uint32_t i, std::size_t width, std::size_t height);

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
   for (auto i = 0; i < lines.size(); ++i)
      for (auto c = 0; c < lines[i].size(); ++c)
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

   /* check for diagonals */
   /* expand all patches to pixel to the left */
   /* expand all patches to pixel above */
   /* output coloured diagram */

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
