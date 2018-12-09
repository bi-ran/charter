#include <deque>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "include/box.h"
#include "include/blockmap.h"
#include "include/utility.h"

static const std::vector<std::string> colours = {
   "\033[0m", "\033[41m", "\033[43m", "\033[42m",
   "\033[46m", "\033[44m", "\033[45m", "\033[101m" };

uint32_t resolve(uint32_t b, std::multimap<uint32_t, uint32_t>& stacks,
                 std::vector<uint32_t>& layers) {
   uint32_t max = 0;
   auto range = stacks.equal_range(b);
   std::for_each(range.first, range.second, [&](const auto& r) {
      if (layers[r.second] == 0)
         layers[r.second] = resolve(r.second, stacks, layers);
      max = std::max(layers[r.second], max);
   });

   return ++max;
}

int chart(const char* input) {
   /* open file */
   std::ifstream fstream(input);
   if (!fstream) { printf("invalid file\n"); return 1; }

   /* read input */
   std::string line;
   std::deque<std::string> lines;
   while (std::getline(fstream, line))
      lines.emplace_back(std::move(line));

   /* initialise blockmap */
   auto chart = new blockmap(lines);

   /* infill from edges to outer border */
   chart->for_all_blocks([&](uint32_t i) {
      if (chart->on_edge(i)) chart->explore(i); });

   /* check for well-formedness of input */
   chart->for_all_blocks([&](uint32_t i) {
      if (!chart->check_well_formed(i)) exit(1); });

   /* find patches */
   std::deque<std::set<uint32_t>> patches;
   chart->for_all_blocks([&](uint32_t i) {
      std::set<uint32_t>&& p = chart->explore(i);
      if (!p.empty()) patches.emplace_back(p); });

   /* expand all patches to pixel left/above */
   for (auto& p : patches) {
      for (const auto& b : p)
         p.insert(chart->west(b));
      for (const auto& b : p)
         p.insert(chart->north(b));
   }

   /* reformat blockmap */
   chart->reset_map();
   for (uint32_t i=0; i<patches.size(); ++i)
      chart->rewrite(patches[i], i+1);

   auto colour = [&](uint8_t b) {
      return colours[b] + " " + colours[0]; };

   /* output coloured diagram */
   if (patches.size() < colours.size())
      chart->display_2d(colour);

   /* merge patches for overall canvas size */
   std::vector<uint32_t> merged;
   for (const auto& p : patches)
      std::copy(std::begin(p), std::end(p), std::back_inserter(merged));

   /* find bounding rectangles for all patches */
   std::vector<box*> bounds;
   for (const auto& p : patches)
      bounds.emplace_back(chart->bounding_box(p));

   /* highlight frames (bounding rectange) */
   for (auto b : bounds) {
      chart->reset_map();
      chart->rewrite(merged, 7);
      chart->rewrite(chart->blockset_from_box(*b), 1);
      chart->display_2d(colour);

      /* prompt for dimensions */
   }

   /* determine layering of bounds */
   std::multimap<uint32_t, uint32_t> stacks;
   for (uint32_t i=0; i<bounds.size(); ++i) {
      if (bounds[i]->area() == patches[i].size())
         continue;

      /* record patches that lie above a bounding rectangle */
      const auto& blockset = chart->blockset_from_box(*bounds[i]);
      for (uint32_t j=0; j<patches.size(); ++j) {
         if (j == i) { continue; }

         std::set<uint32_t> intersection;
         std::set_intersection(patches[j].begin(), patches[j].end(),
            blockset.begin(), blockset.end(),
            std::inserter(intersection, intersection.begin()));
         if (!intersection.empty()) { stacks.emplace(j, i); }
      }
   }

   /* check for circular dependencies */
   for (const auto& s : stacks) {
      auto range = stacks.equal_range(s.second);
      std::for_each(range.first, range.second, [&](const auto& r) {
         if (r.second == s.first) {
            printf("circular dependency: chart not well-formed!\n");
            exit(1);
         }
      });
   }

   /* loop over patches and find longest chain */
   std::vector<uint32_t> layers(bounds.size(), 0);
   for (uint32_t i=0; i<bounds.size(); ++i)
      layers[i] = resolve(i, stacks, layers);

   /* draw pads from bounds with shortest chain */

   return 0;
}

int main(int argc, char* argv[]) {
   if (argc == 2)
      return chart(argv[1]);

   printf("usage: %s [input]\n", argv[0]);
   return 1;
}
