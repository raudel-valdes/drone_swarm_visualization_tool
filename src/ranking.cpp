#include "ranking.h"

// Determine which cost case we have based on positions of src and dest
int Ranking::determineCase(int src, int srcLevel, int dest, int destLevel)
{
	// Cases detailed below in cost()
	if (src == dest)
	{
		return 0;
	}
	else if (src == 1 || dest == 1)
	{
		return 1;
	}
	else if (abs(src - dest) % 2 == 0)
	{
		return 2;
	}
	else if (abs(src - dest) % 2 == 1 && srcLevel == destLevel)
	{
		return 3;
	}
	else
	{
		return 4;
	}
	return 0;
}

// Calculates and returns cost of swap
float Ranking::cost(int src, int dest)
{
	// 180 - BASE_ANGLE / 2, 180 - BASE_ANGLE / 2, BASE_ANGLE triangle
	// level floor(position / 2) + 1
	//
	// 0) src == dest
	// 1) root case, i.e. special case of case 2
	// 2) Cost to i.e. 2->3 = (levelDifference * SEPARATION)
	// 3) Cost across i.e. 4->5 = (2 * level * SEPARATION) * cos(BASE_ANGLE)
	// 4) Cost to across and up / down
	//     i.e 2->5 = sqrt(verticalCost^2 + acrossCost^2)

	float cost = 0.0;
	int srcLevel = floor(src / 2) + 1;
	int destLevel = floor(dest / 2) + 1;
	float verComp = 0.0;
	float horComp = 0.0;

	switch (determineCase(src, srcLevel, dest, destLevel))
	{
	case 0:
		// Cost is 0, no change
		break;

	case 1:
		cost = abs(srcLevel - destLevel) * SEPARATION;
		break;

	case 2:
		cost = abs(srcLevel - destLevel) * SEPARATION;
		break;

	case 3:
		// Base of isosceles triangle with side length and angle
		cost = 2 * (srcLevel * SEPARATION) * sin((BASE_ANGLE / 2) * (PI / 180.0));
		break;

	case 4:
		// Pythagorean theorem on triangle created by two sub costs,
		// solve for 3rd leg (CHECK THIS)
		verComp = 2 * (srcLevel * SEPARATION) * sin((BASE_ANGLE / 2) * (PI / 180.0));
		horComp = abs(srcLevel - destLevel) * SEPARATION;
		cost = sqrt(pow(verComp, 2) + pow(horComp, 2));
		break;

	default:
		break;
	}

	return cost;
}

// Returns list of nodes that meet battery threshold criteria
std::vector<NodeTree*> Ranking::getCandidates(NodeTree* root, NodeTree* node)
{
	// Calculate battery deficits / gains
	std::vector<NodeTree*> candidates;

	// Left side
	NodeTree* current = root;
	while (current != NULL)
	{
		if (fitsCriteria(current, node) && !inVector(current, candidates))
		{
			candidates.push_back(current);
		}

		current = current->GetChild();
	}

	// Right side
	current = root->GetChild()->GetSibling();
	while (current != NULL)
	{
		if (fitsCriteria(current, node) && !inVector(current, candidates))
		{
			candidates.push_back(current);
		}

		current = current->GetChild();
	}

	return candidates;
}

// Suggest a swap based on candidates and cost threshold
NodeTree* Ranking::suggest(std::vector<NodeTree*> candidates, NodeTree* node)
{
	if (candidates.empty())
	{
		std::cout << "No swap suggested!" << std::endl;
		return NULL;
	}

	NodeTree* cheapest = candidates.front();
	float currCost = cost(node->GetDrone()->GetInfo().GetID(), cheapest->GetDrone()->GetInfo().GetID());
	float currDelta = cheapest->GetDrone()->GetInfo().GetBattery() - node->GetDrone()->GetInfo().GetBattery();

	for (NodeTree* candidate : candidates)
	{
		if (candidate->GetDrone()->GetInfo().GetID() == node->GetDrone()->GetInfo().GetID()) continue;

		float tempCost = cost(node->GetDrone()->GetInfo().GetID(), candidate->GetDrone()->GetInfo().GetID());
		float tempDelta = candidate->GetDrone()->GetInfo().GetBattery() - node->GetDrone()->GetInfo().GetBattery();

		if (tempCost < COST_THRESHOLD)
		{
			if (fitness(tempDelta, tempCost) > fitness(currDelta, currCost))
			{
				cheapest = candidate;
				currCost = tempCost;
				currDelta = tempDelta;
			}
		}
	}

	std::cout << "suggest(" << node->GetDrone()->GetInfo().GetID() << "," << cheapest->GetDrone()->GetInfo().GetID() << "): " << currCost << std::endl;

	return cheapest;
}

// Returns fitness score of swap
float Ranking::fitness(float batteryDelta, float cost)
{
	return (BATTERY_WEIGHT * batteryDelta - COST_WEIGHT * cost / SEPARATION);
}

// Suggests one swap for every node pre-order
void Ranking::levelorderRank(NodeTree* root)
{
	int maxLevel = root->GetMaxLevel();

	for (int i = 0; i < maxLevel; i++)
	{
		if (i == 0)
		{
			rankNode(root, root);
		}
		else
		{
			rankNode(root, root->GetLeftNodeAtLevel(i));
			rankNode(root, root->GetRightNodeAtLevel(i));
		}
	}

	std::cout << std::endl << std::endl;
}

void Ranking::rankNode(NodeTree* root, NodeTree* node)
{
	if (node != NULL)
	{
		// Suggest swap for current node
		std::vector<NodeTree*> candidates = Ranking::getCandidates(root, node);
		swaps.push_back(Ranking::suggest(candidates, node));
	}
}

bool Ranking::fitsCriteria(NodeTree* current, NodeTree* node)
{
	int curLevel = floor(current->GetDrone()->GetInfo().GetID() / 2) + 1;
	int nodeLevel = floor(node->GetDrone()->GetInfo().GetID() / 2) + 1;

	return current->GetDrone()->GetInfo().GetID() != node->GetDrone()->GetInfo().GetID()
		&& current->GetDrone()->GetInfo().GetBattery() - node->GetDrone()->GetInfo().GetBattery() > BATTERY_THRESHOLD
		&& cost(current->GetDrone()->GetInfo().GetID(), node->GetDrone()->GetInfo().GetID()) < COST_THRESHOLD
		&& curLevel > nodeLevel;
}

bool Ranking::inVector(NodeTree* node, std::vector<NodeTree*> candidates)
{
	return std::find(candidates.begin(), candidates.end(), node) != candidates.end();
}

bool Ranking::invalidSwaps(std::vector<NodeTree*> swaps)
{
	for (NodeTree* swap : swaps)
	{
		if (swap->GetDrone()->GetInfo().GetID() != -1)
		{
			return false;
		}
	}

	return true;
}
