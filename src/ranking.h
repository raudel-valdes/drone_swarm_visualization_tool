#pragma once

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "nodetree.h"
#include "droneinfo.h"
#include "drone.h"

#define PI 3.14159265358979323846

// Ranking specific defines
#define SEPARATION 200
#define BASE_ANGLE 80
#define BATTERY_THRESHOLD 10
#define COST_THRESHOLD 300
#define BATTERY_WEIGHT 0.75
#define COST_WEIGHT 0.25

extern std::vector<NodeTree*> swaps;

class Ranking
{
public:
	static std::vector<NodeTree*> getCandidates(NodeTree*, NodeTree*);
	static NodeTree* suggest(std::vector<NodeTree*>, NodeTree*);
	static float cost(int, int);
	static float fitness(float, float);
	static int determineCase(int, int, int, int);
	static bool fitsCriteria(NodeTree*, NodeTree*);
	static bool inVector(NodeTree*, std::vector<NodeTree*>);
	static void levelorderRank(NodeTree*);
	static void rankNode(NodeTree*, NodeTree*);
	static bool invalidSwaps(std::vector<NodeTree*>);
};
