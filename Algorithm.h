#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#include "Node.h"

#define COLUMNS 50
#define ROWS    50

#define WIDTH   800.0f
#define HEIGHT  800.0f

bool isBlocked(node_t* n, std::vector<node_t>& nodes);
void findNeighbors(std::vector<node_t>& nodes, std::vector<node_t*>& open, node_t* currentNode, std::vector<node_t*>& closed, node_t* start, node_t* end);
template<typename T>
void swap(T& a, T& b);
void selectionSort(std::vector<node_t*>& open);
