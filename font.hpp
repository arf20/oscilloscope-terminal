#include "main.hpp"

#include <vector>

std::vector<std::vector<line>> font = {
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},

    {},
    {},
    {},
    {},
    {},
    {
        {{0, 7}, {0, 5}}, {{0, 5}, {2, 1}}, {{2, 1}, {4, 5}}, {{4, 5}, {4, 7}}, {{4, 5}, {0, 5}}    // A
    },
    {
        {{0, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {4, 5}}, {{4, 5}, {3, 4}}, {{3, 4}, {4, 3}}, {{4, 3}, {4, 2}}, {{4, 2}, {3, 1}}, {{3, 1}, {0, 1}}, {{1, 1}, {1, 7}}, {{1, 4}, {3, 4}}  // B
    },
    {
        {{4, 6}, {3, 7}}, {{3, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{0, 6}, {0, 2}}, {{0, 2}, {1, 1}}, {{1, 1}, {3, 1}}, {{3, 1}, {4, 2}}    // C
    },
    {
        {{0, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {4, 2}}, {{4, 2}, {3, 1}}, {{3, 1}, {0, 1}}, {{1, 1}, {1, 7}}  // D
    },
    {
        {{4, 7}, {0, 7}}, {{0, 7}, {0, 4}}, {{0, 4}, {2, 4}}, {{0, 4}, {0, 1}}, {{0, 1}, {4, 1}}    // E
    },

    {
        {{0, 7}, {0, 4}}, {{0, 4}, {2, 4}}, {{0, 4}, {0, 1}}, {{0, 1}, {4, 1}}  // F
    },
    {
        {{3, 4}, {4, 4}}, {{4, 4}, {4, 7}}, {{4, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{0, 6}, {0, 2}}, {{0, 2}, {1, 1}}, {{1, 1}, {4, 1}}    // G
    },
    {
        {{0, 7}, {0, 1}}, {{0, 4}, {4, 4}}, {{4, 7}, {4, 1}}    // H
    },
    {
        {{1, 7}, {3, 7}}, {{2, 7}, {2, 1}}, {{1, 1}, {3, 1}}    // I
    },
    {
        {{0, 6}, {1, 7}}, {{1, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {4, 1}}  // J
    },
    {
        {{0, 7}, {0, 1}}, {{0, 4}, {1, 4}}, {{1, 4}, {4, 1}}, {{1, 4}, {4, 7}}  // K
    },
    {
        {{4, 7}, {0, 7}}, {{0, 7}, {0, 1}}  // L
    },
    {
        {{0, 7}, {0, 1}}, {{0, 1}, {2, 4}}, {{2, 4}, {4, 1}}, {{4, 1}, {4, 7}}  // M
    },
    {
        {{0, 7}, {0, 1}}, {{0, 1}, {4, 7}}, {{4, 7}, {4, 1}}    // N
    },
    {
        {{0, 6}, {0, 2}}, {{0, 2}, {1, 1}}, {{1, 1}, {3, 1}}, {{3, 1}, {4, 2}}, {{4, 2}, {4, 6}}, {{4, 6}, {3, 7}}, {{3, 7}, {1, 7}}, {{1, 7}, {0, 6}}  // O
    },

    {
        {{0, 7}, {0, 1}}, {{0, 1}, {3, 1}}, {{3, 1}, {4, 2}}, {{4, 2}, {4, 3}}, {{4, 3}, {3, 4}}, {{3, 4}, {0, 4}}  // P
    },
    {
        {{0, 6}, {0, 2}}, {{0, 2}, {1, 1}}, {{1, 1}, {3, 1}}, {{3, 1}, {4, 2}}, {{4, 2}, {4, 5}}, {{4, 5}, {2, 7}}, {{2, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{4, 7}, {2, 5}}  // Q
    },
    {
        {{0, 7}, {0, 1}}, {{0, 1}, {3, 1}}, {{3, 1}, {4, 2}}, {{4, 2}, {4, 3}}, {{4, 3}, {3, 4}}, {{3, 4}, {0, 4}}, {{1, 4}, {4, 7}}    // R
    },
    {
        {{0, 6}, {1, 7}}, {{1, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {0, 2}}, {{0, 2}, {1, 1}}, {{1, 1}, {3, 1}}, {{3, 1}, {4, 2}}    // S
    },
    {
        {{2, 7}, {2, 1}}, {{0, 1}, {4, 1}}  // T
    },
    {
        {{0, 1}, {0, 6}}, {{0, 6}, {1, 7}}, {{1, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {4, 1}}    // U
    },
    {
        {{0, 1}, {2, 7}}, {{2, 7}, {4, 1}}  // V
    },
    {
        {{0, 1}, {1, 7}}, {{1, 7}, {2, 5}}, {{2, 5}, {3, 7}}, {{3, 7}, {4, 1}}  // W
    },
    {
        {{0, 1}, {4, 7}}, {{0, 7}, {4, 1}}  // X
    },
    {
        {{2, 7}, {2, 4}}, {{2, 4}, {0, 1}}, {{2, 4}, {4, 1}}    // Y
    },

    {
        {{0, 1}, {4, 1}}, {{4, 1}, {0, 7}}, {{0, 7}, {4, 7}}    // Z
    },
    {},
    {},
    {},
    {},
    {},
    {},
    {
        {{0, 4}, {1, 3}}, {{1, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 6}}, {{4, 6}, {3, 7}}, {{3, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{0, 6}, {1, 5}}, {{1, 5}, {4, 5}} // a
    },
    {
        {{0, 1}, {0, 7}}, {{0, 5}, {2, 3}}, {{2, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 6}}, {{4, 6}, {3, 7}}, {{3, 7}, {2, 7}}, {{2, 7}, {0, 5}}  // b
    },
    {
        {{4, 3}, {1, 3}}, {{1, 3}, {0, 4}}, {{0, 4}, {0, 6}}, {{0, 6}, {1, 7}}, {{1, 7}, {4, 7}}    // c
    },

    {
        {{4, 7}, {4, 1}}, {{4, 5}, {2, 3}}, {{2, 3}, {1, 3}}, {{1, 3}, {0, 4}}, {{0, 4}, {0, 6}}, {{0, 6}, {1, 7}}, {{1, 7}, {2, 7}}, {{2, 7}, {4, 5}}  // d
    },
    {
        {{3, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{0, 6}, {0, 4}}, {{0, 4}, {1, 3}}, {{1, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {3, 5}}, {{3, 5}, {0, 5}}  // e
    },
    {
        {{1, 7}, {1, 2}}, {{0, 4}, {3, 4}}, {{1, 2}, {2, 1}}, {{2, 1}, {3, 1}}, {{3, 1}, {4, 2}}    // f
    },
    {
        {{4, 7}, {1, 7}}, {{1, 7}, {0, 6}}, {{0, 6}, {0, 4}}, {{0, 4}, {1, 3}}, {{1, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 8}}, {{4, 8}, {3, 9}}, {{3, 9}, {1, 9}}, {{1, 9}, {0, 8}}  // g
    },
    {
        {{0, 7}, {0, 1}}, {{0, 5}, {2, 3}}, {{2, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 7}}    // h
    },
    {
        {{2, 7}, {2, 3}}, {{2, 2}, {2, 1}}  // i
    },
    {
        {{0, 8}, {1, 9}}, {{1, 9}, {2, 9}}, {{2, 9}, {3, 8}}, {{3, 8}, {3, 3}}, {{3, 2}, {3, 1}}    // j
    },
    {
        {{0, 7}, {0, 1}}, {{0, 5}, {2, 5}}, {{2, 5}, {4, 3}}, {{2, 5}, {4, 7}}  // k
    },
    {
        {{2, 7}, {1, 6}}, {{1, 6}, {1, 1}}  // l
    },
    {
        {{0, 7}, {0, 3}}, {{0, 4}, {1, 3}}, {{1, 3}, {2, 4}}, {{2, 4}, {2, 5}}, {{2, 4}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 7}}    // m
    },

    {
        {{0, 7}, {0, 3}}, {{0, 5}, {2, 3}}, {{2, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 7}}    // n
    },
    {
        {{0, 6}, {0, 4}}, {{0, 4}, {1, 3}}, {{1, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 6}}, {{4, 6}, {3, 7}}, {{3, 7}, {1, 7}}, {{1, 7}, {0, 6}}  // o
    },
    {
        {{0, 9}, {0, 3}}, {{0, 4}, {1, 3}}, {{1, 3}, {3, 3}}, {{3, 3}, {4, 4}}, {{4, 4}, {4, 6}}, {{4, 6}, {3, 7}}, {{3, 7}, {0, 7}}    // p
    },
    {
        {{4, 9}, {4, 3}}, {{4, 4}, {3, 3}}, {{3, 3}, {1, 3}}, {{1, 3}, {0, 4}}, {{0, 4}, {0, 6}}, {{0, 6}, {1, 7}}, {{1, 7}, {4, 7}}    // q
    },
    {
        {{0, 7}, {0, 3}}, {{0, 5}, {2, 3}}, {{2, 3}, {3, 3}}, {{3, 3}, {4, 4}}  // r
    },
    {
        {{0, 7}, {3, 7}}, {{3, 7}, {4, 6}}, {{4, 6}, {3, 5}}, {{3, 5}, {1, 5}}, {{1, 5}, {0, 4}}, {{0, 4}, {1, 3}}, {{1, 3}, {4, 3}}    // s
    },
    {
        {{0, 3}, {4, 3}}, {{2, 1}, {2, 6}}, {{2, 6}, {3, 7}}, {{3, 7}, {4, 6}}  // t
    },
    {
        {{0, 3}, {0, 6}}, {{0, 6}, {1, 7}}, {{1, 7}, {2, 7}}, {{2, 7}, {4, 5}}, {{4, 3}, {4, 7}}    // u
    },
    {
        {{0, 3}, {2, 7}}, {{2, 7}, {4, 3}}  // v
    },
    {
        {{0, 3}, {1, 7}}, {{1, 7}, {2, 3}}, {{2, 3}, {3, 7}}, {{3, 7}, {4, 3}}  // w
    },

    {
        {{0, 3}, {4, 7}}, {{0, 7}, {4, 3}}  // x
    },
    {
        {{0, 3}, {2, 7}}, {{2, 7}, {4, 3}}, {{2, 7}, {1, 9}}, {{1, 9}, {0, 9}}  // y
    },
    {
        {{0, 3}, {4, 3}}, {{4, 3}, {0, 7}}, {{0, 7}, {4, 7}}    // z
    },
    {},
    {},
    {},
    {},
    {},
};