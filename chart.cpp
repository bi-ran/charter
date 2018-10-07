#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "include/blockmap.h"
#include "include/utility.h"

static const std::vector<std::string> colours = {
   "\033[0m", "\033[41m", "\033[43m", "\033[42m",
   "\033[46m", "\033[44m", "\033[45m", "\033[101m" };

int chart(const char* input) {
   /* open file */
   std::ifstream fstream(input);
   if (!fstream) {
      printf("invalid file\n");
      return 1;
   }

   /* read input */
   std::string line;
   std::deque<std::string> lines;
   while (std::getline(fstream, line))
      lines.emplace_back(std::move(line));

   /* initialise blockmap */
   auto chart = new blockmap(lines);
   auto map = chart->map();
   auto size = chart->size();

   /* TODO: infill from edges to outer border */
   /* assume input maximally subtends array   */

   /* check for well-formedness of input
    *    ! border blocks with only one neighbouring block
    *    ! diagonal blocks
    */
   for (uint32_t i=0; i<size; ++i)
      if (!chart->check_well_formed(i))
         return 1;

   /* find patches */
   std::deque<std::set<uint32_t>> patches;
   for (uint32_t i=0; i<size; ++i) {
      if (map[i] & 0x3) { continue; }
      patches.emplace_back(chart->explore(i));
   }

   /* expand all patches to pixel left/above */
   for (auto& p : patches)
      chart->expand_block(p);

   /* reformat blockmap */
   chart->reset_map();
   for (uint32_t i=0; i<patches.size(); ++i)
      chart->rewrite(patches[i], i+1);

   /* output coloured diagram */
   if (patches.size() < colours.size())
      chart->display_2d([&](uint8_t b) {
         return colours[b] + " " + colours[0]; });

   /* merge patches for overall canvas size */
   std::vector<uint32_t> merged;
   for (const auto& p : patches)
      std::copy(std::begin(p), std::end(p),
         std::back_inserter(merged));

   /* find bounding rectangles for all patches */
   std::vector<uint32_t> bounds;
   bounds.emplace_back(chart->bounding_box(merged));
   for (const auto& p : patches)
      bounds.emplace_back(chart->bounding_box(p));

   /* input dimensions
    *    highlight frame border
    *    automatically identify equivalent borders
    *       and assign dimensions
    */

   return 0;
}

int main(int argc, char* argv[]) {
   if (argc == 2)
      return chart(argv[1]);

   printf("usage: %s [input]\n", argv[0]);
   return 1;
}
