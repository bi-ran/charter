#ifndef UTILITY_H
#define UTILITY_H

template<template<typename...> class T, typename U>
void print_comma_separated(T<U> c) {
   std::adjacent_difference(std::begin(c), std::end(c),
      std::ostream_iterator<U>(std::cout),
      [](const U& x, const U&) -> U {
         return std::cout << ", ", x; });
   printf("\n");
}

#endif /* UTILITY_H */
