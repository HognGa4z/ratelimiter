cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall -std=c++17")
add_library(libratelimiter STATIC utils.hpp utils.cpp ratelimiter.hpp)