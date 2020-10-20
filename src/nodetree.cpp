#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include "fileio.h"
#include "socket.h"
#include "nodetree.h"

// Queue struct for datastrings and managing votes of the group decision making process.
// TODO: Should probably convert this into a class, since a bunch of queue-related functions are floating around inside nodetree.cpp.
struct Queue
{
	struct Queue* front;
	int votes;
	std::string data;
};

// The AllDone() function from mavlink_control.cpp.
// TODO: Find a better place to put the definition of AllDone().
bool AllDone(std::vector<StatusList> listcheck);
// Checks differences in battery levels across the node and performs swaps appropriately.
// TODO: Rename this.
int CheckNodeTreeDown(NodeTree*, NodeTree*);
//sets the globalNodeTree to the Lead node of a NodeTree (used after SwapNodes)
// TODO: Rename this.
void NodeTreeSetter(NodeTree*);
// Makes a string from a NodeTree with the drone IDs.
// TODO: Rename this.
std::string TreeToString(NodeTree*, std::string);
// Gets the string with the highest votes in the queue.
// TODO: Rename this.
std::string CheckVotes(std::string, int, int, struct Queue*);
//adjusts the node if the data string of the node doesnt match the winning voted string
// TODO: Name is too vague. Rename it.
void AdjustTree(std::string, std::string);

// Allocates and creates a Queue.
struct Queue* CreateQueue(struct Queue*, std::string);
// Adds a string to the given Queue, or increments the votes if the string already exists.
// TODO: Should we have a separate function for incrementing votes?
void Enqueue(struct Queue*, std::string);
// TODO: Do we need a Dequeue() function?
// Frees the memory allocated by the queue.
void DestroyQueue(struct Queue*);

// Enqueues the given string to the globalDroneQueue.
// TODO: Get rid of this after converting Queue into a class.
void AppendStringData(std::string);
int GetSwapsFromStrings(std::string, std::string, int*);

// Used for node initialization.
int levelComparison = 1;
// Global Queue for this drone.
Queue* globalDroneQueue;

NodeTree::NodeTree(Drone* drone, NodeTree* parent, NodeTree* sibling, NodeTree* child)
{
	this->drone = drone;
	this->parent = parent;
	this->sibling = sibling;
	this->child = child;
}

// Copy constructor.
NodeTree::NodeTree(const NodeTree& copy)
{
	if (copy.drone == NULL) return;

	this->drone = copy.drone;
	this->parent = copy.parent;
	this->sibling = copy.sibling;

	if (copy.child != NULL)
	{
		this->child = new NodeTree(*copy.child);
	}
	else
	{
		this->child = NULL;
	}
}

struct Queue* CreateQueue(struct Queue* front, std::string data)
{
	struct Queue* queue = new Queue;
	queue->front = front;
	queue->data = data;
	queue->votes = 0;
	return queue;
}

bool NodeTree::IsDronePositionAcceptable(NodeTree* node, float* x, float* y, float* z)
{
	if (x != NULL)
	{
		if (abs(node->GetDrone()->GetInfo().GetLocation().getX() - *x) > ACCEPTABLE_X_OFFSET) return false;
	}

	if (y != NULL)
	{
		if (abs(node->GetDrone()->GetInfo().GetLocation().getY() - *y) > ACCEPTABLE_Y_OFFSET) return false;
	}

	if (z != NULL)
	{
		if (abs(node->GetDrone()->GetInfo().GetLocation().getZ() - *z) > ACCEPTABLE_Z_OFFSET) return false;
	}

	return true;
}

//Recursively go down the node with a node to compare to
void NodeTree::GroupPoll(NodeTree* node)
{
	if (node->GetChild() && node->GetChild()->GetDrone())
	{
		// Recursively compare drone's battery life with the drone at node.
		while (CheckNodeTreeDown(node->GetChild(), node) != 1)
		{
			continue;
		}
	}

	// Recursively go down with node's child as the comparison point.
	if (node->GetChild() && node->GetChild()->GetDrone())
	{
		GroupPoll(node->GetChild());
	}

	// Check with the sibling, too.
	if (node->GetSibling() && node->GetSibling()->GetDrone())
	{
		GroupPoll(node->GetSibling());
	}
}

void NodeTree::AddDrone(NodeTree* tree, Drone* drone)
{
	if (drone->GetLevel() == levelComparison)
	{
		if (tree->GetDrone() == NULL && (drone->GetLevel() == 1 || (drone->GetInfo().GetID() % 2 == 0)))
		{
			tree->SetDrone(drone);
			drone->GetInfoPointer()->SetIsRightSide(true);
			std::cout << "Set drone " << drone->GetInfo().GetID() << " as 1." << std::endl;
		}
		else
		{
			if (tree->GetSibling())
			{
				AddDrone(tree->GetSibling(), drone);
			}
			else
			{
				NodeTree* sibling = new NodeTree(drone, tree->GetParent(), NULL, tree->GetChild());
				tree->SetSibling(sibling);
				drone->GetInfoPointer()->SetIsRightSide(false);
				std::cout << "Set drone " << drone->GetInfo().GetID() << " as 0." << std::endl;
			}
		}
	}
	else if (drone->GetLevel() > levelComparison)
	{
		// Increment comparison level corresponding to the level of the node checked.
		levelComparison++;

		if (tree->GetChild())
		{
			AddDrone(tree->GetChild(), drone);
		}
		else
		{
			NodeTree* child = new NodeTree(NULL, tree, NULL, NULL);
			globalNodeTree->SetChild(child);
			AddDrone(tree->GetChild(), drone);
		}
	}

	// Reset level comparison now that drone is added.
	levelComparison = 1;
}

void NodeTree::UpdateDrone(NodeTree* tree, Drone* drone)
{
	if (tree && tree->GetDrone() && tree->GetDrone()->GetInfo().GetID() == drone->GetInfo().GetID())
	{
		drone->SetLevel(tree->GetDrone()->GetLevel());
		drone->GetInfoPointer()->SetIsRightSide(tree->GetDrone()->GetInfo().IsRightSide());
		tree->SetDrone(drone);
	}
	else if (tree && tree->GetSibling() && tree->GetSibling()->GetDrone())
	{
		if (tree->GetSibling()->GetDrone()->GetInfo().GetID() == drone->GetInfo().GetID())
		{
			drone->SetLevel(tree->GetSibling()->GetDrone()->GetLevel());
			drone->GetInfoPointer()->SetIsRightSide(tree->GetSibling()->GetDrone()->GetInfo().IsRightSide());
			tree->GetSibling()->SetDrone(drone);
		}
		else
		{
			UpdateDrone(tree->GetChild(), drone);
		}
	}
	else
	{
		UpdateDrone(tree->GetChild(), drone);
	}
}

NodeTree::~NodeTree()
{
	/*	from old c nodetreedestroy function
	if (node)
		{
			if (node->GetSibling)
			{
				if (node->GetSibling->GetDrone != NULL)
				{
					free(node->sibling->drone);
					node->sibling->drone = NULL;
					if (node->sibling->sibling != NULL)
					{
						free(node->sibling->sibling);
						node->sibling->sibling = NULL;
					}
					free(node->sibling);
					node->sibling = NULL;
				}
				else
				{
					free(node->sibling);
					node->sibling = NULL;
				}
			}
			if (node->child)
			{
				NodeTree_destroy(node->child);
				node->child = NULL;
			}
			if (node->drone)
			{
				free(node->drone);
				node->drone = NULL;
			}
			free(node);
			node = NULL;
		}
		*/
}

void NodeTree::UpdateLeadBits(NodeTree* head, bool lead)
{
	// Update top of node.
	if (head && head->GetDrone())
	{
		// Update lead.
		head->GetDrone()->GetInfoPointer()->SetIsLeadDrone(lead);

		// Update sibling.
		if (head->GetSibling() && head->GetSibling()->GetDrone())
		{
			head->GetSibling()->GetDrone()->GetInfoPointer()->SetIsLeadDrone(false);
		}

		// Update next level.
		UpdateLeadBits(head->GetChild(), false);
	}
}

// TODO: There is a ton of copied code here. Split this up into multiple functions.
void NodeTree::SwapNodes(NodeTree* node1, NodeTree* node2, bool test)
{
	// Changes the values of the level in the node, and the location of the drone, the id of the drone and the isMe bit is left untouched.
	int tempLevel = node1->GetDrone()->GetLevel();
	bool tempLead = node1->GetDrone()->GetInfo().IsLead();
	bool tempSide = node1->GetDrone()->GetInfoPointer()->IsRightSide();

	if (test)
	{
		Vector3D tempLocation = node1->GetDrone()->GetInfo().GetLocation();
		node1->GetDrone()->SetLevel(node2->GetDrone()->GetLevel());
		node1->GetDrone()->GetInfo().SetLocation(node2->GetDrone()->GetInfo().GetLocation());
		node1->GetDrone()->GetInfo().SetIsRightSide(node2->GetDrone()->GetInfo().IsRightSide());
		node2->GetDrone()->SetLevel(tempLevel);
		node2->GetDrone()->GetInfo().SetLocation(tempLocation);
		node2->GetDrone()->GetInfo().SetIsRightSide(tempSide);
	}
	else
	{
		std::cout << "actual swap " << tempSide << " " << node1->GetDrone()->GetInfoPointer()->IsRightSide() << " " << node2->GetDrone()->GetInfoPointer()->IsRightSide() << std::endl;

		Vector3D tempLocation = node1->GetDrone()->GetInfo().GetLocation();
		node1->GetDrone()->SetLevel(node2->GetDrone()->GetLevel());
		node1->GetDrone()->GetInfo().SetLocation(node2->GetDrone()->GetInfo().GetLocation());
		node1->GetDrone()->GetInfoPointer()->SetIsLeadDrone(node2->GetDrone()->GetInfo().IsLead());
		node1->GetDrone()->GetInfoPointer()->SetIsRightSide(node2->GetDrone()->GetInfo().IsRightSide());
		node2->GetDrone()->SetLevel(tempLevel);
		node2->GetDrone()->GetInfo().SetLocation(tempLocation);
		node2->GetDrone()->GetInfoPointer()->SetIsLeadDrone(tempLead);
		node2->GetDrone()->GetInfoPointer()->SetIsRightSide(tempSide);
	}

	NodeTree* tempParent;
	if (node1->GetParent() != node2)
	{
		tempParent = node1->GetParent();
	}
	else
	{
		tempParent = node1;
	}

	NodeTree* tempChild;
	if (node1->GetChild() != node2)
	{
		tempChild = node1->GetChild();
	}
	else
	{
		tempChild = node1;
	}

	NodeTree* tempSibling = node1->GetSibling();
	// Don't do these swaps if they are siblings of each other.
	if (tempSibling != node2 && node2->GetSibling() != node1)
	{
		if (node1->GetParent())
		{
			if (node1->GetParent() != node2)
			{
				if (node1->GetParent()->GetSibling() && node1->GetParent()->GetSibling() != node2)
				{
					if (node1->GetParent()->GetChild() == node1)
					{
						node1->GetParent()->SetChild(node2);
						node1->GetParent()->GetSibling()->SetChild(node2);

						if (node1->GetChild() == node2 || node1->GetChild()->GetSibling() == node2)
						{
							node1->SetParent(node2);
						}
						else
						{
							node1->SetParent(node2->GetParent());
						}
					}
					else
					{
						node1->SetParent(node2->GetParent());
					}
				}
				else if (node1->GetParent()->GetSibling())
				{
					if (node1->GetParent()->GetChild() == node1)
					{
						node1->GetParent()->SetChild(node2);
						node1->SetParent(node2->GetParent());
					}
					else
					{
						node1->SetParent(node2->GetParent());
					}
				}
				else
				{
					if (node1->GetParent()->GetChild() == node1)
					{
						node1->GetParent()->SetChild(node2);
						node1->SetParent(node2->GetParent());
					}
					else
					{
						node1->SetParent(node2->GetParent());
					}
				}
			}
			else
			{
				if (node1->GetParent()->GetSibling())
				{
					if (node1->GetParent()->GetChild() == node1)
					{
						node1->GetParent()->GetSibling()->SetChild(node2);
						node1->SetParent(node2->GetParent());
					}
					else
					{
						node1->SetParent(node2->GetParent());
					}
				}
				else
				{
					node1->SetParent(node2->GetParent());
				}
			}
		}
		else
		{
			if (node2->GetParent() && node2->GetParent() == node1)
			{
				node1->SetParent(node2);
			}
			else
			{
				node1->SetParent(node2->GetParent());
			}
		}

		if (node2->GetParent())
		{
			if (node2->GetParent() != node1)
			{
				if (node2->GetParent()->GetSibling() && node2->GetParent()->GetSibling() != node1)
				{
					if (node2->GetParent()->GetChild() == node2)
					{
						node2->GetParent()->SetChild(node1);
						node2->GetParent()->GetSibling()->SetChild(node1);
						node2->SetParent(tempParent);
					}
					else
					{
						node2->SetParent(tempParent);
					}
				}
				else if (node2->GetParent()->GetSibling())
				{
					if (node2->GetParent()->GetChild() == node2)
					{
						node2->GetParent()->SetChild(node1);
						node2->SetParent(tempParent);
					}
					else
					{
						node2->SetParent(tempParent);
					}
				}
				else
				{
					if (node2->GetParent()->GetChild() == node2)
					{
						node2->GetParent()->SetChild(node1);
						node2->SetParent(tempParent);
					}
					else
					{
						node2->SetParent(tempParent);
					}
				}
			}
			else
			{
				if (node2->GetParent()->GetSibling())
				{
					if (node2->GetParent()->GetChild() == node2)
					{
						node2->GetParent()->GetSibling()->SetChild(node1);
						node2->SetParent(tempParent);
					}
					else
					{
						node2->SetParent(tempParent);
					}
				}
				else
				{
					node2->SetParent(tempParent);
				}
			}
		}

		//******************CHILD SECTION************************/

		if (node1->GetChild())
		{
			if (node1->GetChild() != node2)
			{
				if (node1->GetChild()->GetSibling() && node1->GetChild()->GetSibling() != node2)
				{
					if (node1->GetChild()->GetParent() == node1)
					{
						node1->GetChild()->SetParent(node2);
						node1->GetChild()->GetSibling()->SetParent(node2);
						node1->SetChild(node2->GetChild());
					}
					else
					{
						node1->SetChild(node2->GetChild());
					}
				}
				else if (node1->GetChild()->GetSibling())
				{
					if (node1->GetChild()->GetParent() == node1)
					{
						node1->GetChild()->SetParent(node2);
						node1->SetParent(node2);
						node1->SetChild(node2->GetChild());
					}
					else
					{
						node1->SetChild(node2->GetChild());
					}
				}
				else
				{
					if (node1->GetChild()->GetParent() == node1)
					{
						node1->GetChild()->SetParent(node2);
						node1->SetChild(node2->GetChild());
					}
					else
					{
						node1->SetChild(node2->GetChild());
					}
				}
			}
			else
			{
				if (node1->GetChild()->GetSibling())
				{
					if (node1->GetChild()->GetParent() == node1)
					{
						//this section might need changing but more tests need to be done with 3+ drones to confirm
						//because node2->child section if I recall accounts for this
						node1->GetChild()->SetParent(node2); //this line in particular bothers me, because node1's child was node2
						//I suggest test a situation where node1 is the lead drone and node2 is the child (not child sibling) of node1, 
						//then after the swaps check to be sure node2's parent is null
						node1->GetChild()->GetSibling()->SetParent(node2);
						node1->SetParent(node2);
						node1->SetChild(node2->GetChild());
					}
					else
					{
						//this section might need changing but more tests need to be done with 3+ drones to confirm
						//because node2->child section if I recall accounts for this
						if (node2->GetSibling()->GetParent() == node1)
						{
							node2->GetSibling()->SetParent(node2);
						}
						node1->SetChild(node2->GetChild());
					}
				}
				else
				{
					node1->SetParent(node2);
					node1->SetChild(node2->GetChild());
				}
			}
		}
		else
		{
			node1->SetChild(node2->GetChild());
		}

		if (node2->GetChild())
		{
			if (node2->GetChild() != node1)
			{
				if (node2->GetChild()->GetSibling() && node2->GetChild()->GetSibling() != node1)
				{
					if (node2->GetChild()->GetParent() == node2)
					{
						node2->GetChild()->SetParent(node1);
						node2->GetChild()->GetSibling()->SetParent(node1);
						node2->SetChild(tempChild);
					}
					else
					{
						node2->SetChild(tempChild);
					}
				}
				else if (node2->GetChild()->GetSibling())
				{
					if (node2->GetChild()->GetParent() == node2)
					{
						node2->GetChild()->SetParent(node1);
						node2->SetParent(node1);
						node2->SetChild(tempChild);
					}
					else
					{
						node2->SetChild(tempChild);
					}
				}
				else
				{
					if (node2->GetChild()->GetParent() == node2)
					{
						node2->GetChild()->SetParent(node1);
						node2->SetChild(tempChild);
					}
					else
					{
						node2->SetChild(tempChild);
					}
				}
			}
			else
			{
				if (node2->GetChild()->GetSibling())
				{
					if (node2->GetChild()->GetParent() == node2)
					{
						node2->SetParent(node1);
						node2->GetChild()->GetSibling()->SetParent(node1);
						node2->SetChild(tempChild);
					}
					else
					{
						node2->SetParent(node1);
						node2->SetChild(tempChild);
					}
				}
				else
				{
					node2->SetChild(tempChild);
				}
			}
		}
		else
		{
			node2->SetChild(tempChild);
		}

		node1->SetSibling(node2->GetSibling());
		node2->SetSibling(tempSibling);

		if (node1->GetParent() && node1->GetParent()->GetChild() && node1->GetParent()->GetChild() != node1)
		{
			node1->GetParent()->GetChild()->SetSibling(node1);
		}
		if (node2->GetParent() && node2->GetParent()->GetChild() && node2->GetParent()->GetChild() != node2)
		{
			node2->GetParent()->GetChild()->SetSibling(node2);
		}
	}
	//for when they are siblings of each other this adjust the pointers for this situation
	//which should rarely happen (only can happen in NodeTree comparisons where the child and sibling is backwards)
	else
	{
		if (node1->GetSibling() == node2)
		{
			node1->SetSibling(node2);
			node2->SetSibling(node1);
			if (node1->GetParent())
			{
				node1->GetParent()->SetChild(node2);
				if (node1->GetParent()->GetSibling())
				{
					node1->GetParent()->GetSibling()->SetChild(node2);
				}
			}
			if (node1->GetChild())
			{
				node1->GetChild()->SetParent(node2);
				if (node1->GetChild()->GetSibling())
				{
					node1->GetChild()->GetSibling()->SetParent(node2);
				}
			}
		}
		else
		{
			node1->SetSibling(node2);
			node2->SetSibling(node1);
			if (node2->GetParent())
			{
				node2->GetParent()->SetChild(node1);
				if (node2->GetParent()->GetSibling())
				{
					node2->GetParent()->GetSibling()->SetChild(node1);
				}
			}
			if (node2->GetChild())
			{
				node2->GetChild()->SetParent(node1);
				if (node2->GetChild()->GetSibling())
				{
					node2->GetChild()->GetSibling()->SetParent(node1);
				}
			}
		}
	}

	// Set NodeTree from node2 and go to the lead drone (by recursively going up through parents).
	NodeTreeSetter(node2);
}

NodeTree* NodeTree::ExecutePlan(NodeTree* tree)
{
	if (tree->GetDrone()->GetIsMe())
	{
		return tree;
	}
	else
	{
		if (tree && tree->GetChild() && tree->GetChild()->GetDrone())
		{
			if (ExecutePlan(tree->GetChild()) != NULL)
			{
				return ExecutePlan(tree->GetChild());
			}
		}

		if (tree->GetSibling()->GetDrone())
		{
			if (ExecutePlan(tree->GetSibling()) != NULL)
			{
				return ExecutePlan(tree->GetSibling());
			}
		}

		// Node location for landing due to a node location error.
		return NULL;
	}
}

int CheckNodeTreeDown(NodeTree* node1, NodeTree* node2)
{
	if (node1 && node1->GetDrone() && (node1->GetDrone()->GetInfo().GetBattery() - 10 >= node2->GetDrone()->GetInfo().GetBattery()))
	{
		// If node1's sibling has more battery than itself, perform a swap.
		if (node1->GetSibling() && node1->GetSibling()->GetDrone() && node1->GetDrone()->GetInfo().GetBattery() < node1->GetSibling()->GetDrone()->GetInfo().GetBattery())
		{
			//NOTE: Node swaps should be in this order with node2 first.
			globalNodeTree->SwapNodes(node2, node1->GetSibling(), true);
			globalNodeTree->UpdateLeadBits(globalNodeTree, true);
			node2 = globalNodeTree;
			globalNodeTree->GroupPoll(globalNodeTree);
			while (CheckNodeTreeDown(node1->GetSibling(), node2) != 1)
			{
				continue;
			}
		}
		else
		{
			globalNodeTree->SwapNodes(node2, node1, true);
			globalNodeTree->UpdateLeadBits(globalNodeTree, true);
			node2 = globalNodeTree;
			globalNodeTree->GroupPoll(globalNodeTree);
			while (CheckNodeTreeDown(node1, node2) != 1)
			{
				continue;
			}
		}
	}
	else if (node1 && node1->GetSibling() && node1->GetSibling()->GetDrone())
	{
		if (node1->GetSibling()->GetDrone()->GetInfo().GetBattery() - 10 >= node2->GetDrone()->GetInfo().GetBattery() && node1->GetDrone()->GetLevel() != node2->GetDrone()->GetLevel())
		{
			globalNodeTree->SwapNodes(node2, node1->GetSibling(), true);
			globalNodeTree->UpdateLeadBits(globalNodeTree, true);
			node2 = globalNodeTree;
			globalNodeTree->GroupPoll(globalNodeTree);
			while (CheckNodeTreeDown(node1->GetSibling(), node2) != 1)
			{
				continue;
			}
		}
	}

	if (node1 && node1->GetChild() && node1->GetChild()->GetDrone())
	{
		while (CheckNodeTreeDown(node1->GetChild(), node2) != 1)
		{
			continue;
		}
	}

	return 1;
}

void NodeTreeSetter(NodeTree* tree)
{
	if (tree->GetParent() && tree->GetParent()->GetDrone())
	{
		NodeTreeSetter(tree->GetParent());
	}
	else
	{
		globalNodeTree = tree;
	}
}

NodeTree* NodeTree::GetRoot(NodeTree* node)
{
	if (node->GetParent() && node->GetParent()->GetDrone())
	{
		return GetRoot(node->GetParent());
	}
	else
	{
		return node;
	}
}

double NodeTree::GetLowestBattery(NodeTree* node)
{
	double childMin = 100;
	double siblingMin = 100;
	double self = node->GetDrone()->GetInfo().GetBattery();

	if (node->GetChild() && node->GetChild()->GetDrone())
	{
		childMin = GetLowestBattery(node->GetChild());
	}

	if (node->GetSibling() && node->GetSibling()->GetDrone())
	{
		siblingMin = GetLowestBattery(node->GetSibling());
	}

	if (childMin <= siblingMin)
	{
		if (childMin <= self)
		{
			return childMin;
		}
		else
		{
			return self;
		}
	}
	else
	{
		if (siblingMin <= self)
		{
			return siblingMin;
		}
		else
		{
			return self;
		}
	}
}

void Enqueue(struct Queue* queue, std::string data)
{
	if (!data.empty() && queue->data.empty())
	{
		queue->data = data;
		queue->votes = 1;
	}
	else
	{
		if (queue->data == data)
		{
			queue->votes++;
		}
		else if (queue->front && !data.empty())
		{
			Enqueue(queue->front, data);
		}
		else if (!queue->front && !data.empty())
		{
			queue->front = CreateQueue(NULL, data);
			Enqueue(queue->front, data);
		}
	}
}

void DestroyQueue(struct Queue* queue)
{
	if (queue->front)
	{
		DestroyQueue(queue->front);
	}
	free(queue);
}

void AppendStringData(std::string data)
{
	if (!globalDroneQueue->front)
	{
		globalDroneQueue->front = CreateQueue(NULL, data);
		Enqueue(globalDroneQueue, data);
	}
	else
	{
		Enqueue(globalDroneQueue, data);
	}
}

std::string CheckVotes(std::string winningString, int highestVotes, int ties, struct Queue* queue)
{
	int votes = queue->votes;
	if (votes >= highestVotes)
	{
		if (votes == highestVotes)
		{
			ties = 1;
			if (queue->front)
			{
				winningString = CheckVotes(winningString, highestVotes, ties, queue->front);
				return winningString;
			}
			else
			{
				return "";
			}
		}
		else
		{
			ties = 0;
			winningString = queue->data;
			highestVotes = votes;
			if (queue->front)
			{
				winningString = CheckVotes(winningString, highestVotes, ties, queue->front);
				return winningString;
			}
			else
			{
				return winningString;
			}
		}
	}
	else
	{
		if (queue->front)
		{
			winningString = CheckVotes(winningString, highestVotes, ties, queue->front);
			return winningString;
		}
		else
		{
			if (ties == 1)
			{
				return "";
			}
			else
			{
				return winningString;
			}
		}
	}
}

std::string TreeToString(NodeTree* tree, std::string data)
{
	if (data != std::to_string(tree->GetDrone()->GetInfo().GetID()))
	{
		data = data + "," + std::to_string(tree->GetDrone()->GetInfo().GetID());
	}
	if (tree->GetSibling() && tree->GetSibling()->GetDrone())
	{
		data = data + "," + std::to_string(tree->GetSibling()->GetDrone()->GetInfo().GetID());
	}
	if (tree && tree->GetChild() && tree->GetChild()->GetDrone())
	{
		data = TreeToString(tree->GetChild(), data);
	}
	return data;
}

std::string NodeTree::MakeQueueData(NodeTree* tree)
{
	std::string data = std::to_string(tree->GetDrone()->GetInfo().GetID());
	data = TreeToString(tree, data);
	globalDroneQueue = CreateQueue(NULL, data);
	Enqueue(globalDroneQueue, data);
	data = std::to_string(globalNodeTree->ExecutePlan(globalNodeTree)->GetDrone()->GetInfo().GetID()) + " " + data;
	return data;
}

void NodeTree::GetQueueData(std::string data, int sid)
{
	while (!AllDone(recievetreelist) || !AllDone(sendtreelist))
	{
		for (int itr = 0; itr < recievetreelist.size(); itr++)
		{
			Socket::SetSockaddrIn(&Socket::remoteAddr, RemoteIPs[itr].c_str(), Socket::PORTNUM);
			//send data made from MakeQueueData
			// save and send node info
			if (sendtreelist.at(itr).isDone == 0)
			{
				std::cout << "Sent packet: " << data << std::endl;
				FileIO::WritePacket(&FileIO::sent_packet_file, data); // write the drone's id data to the file
				Socket::SendMessage(data.c_str());
				sleep(1);
			}
			// receive info and write to file
			for (int n = 1; n < 3; n++)
			{
				char* char_message = Socket::ReceiveMessage();
				std::string inboundstring = char_message ? char_message : "";
				if (inboundstring.length() > 1)
				{
					FileIO::WritePacket(&FileIO::rec_packet_file, inboundstring);
				}
				if (inboundstring.find(",") != std::string::npos)
				{
					int IDval;
					std::stringstream astream(inboundstring);
					astream >> IDval >> inboundstring;
					//get string from other drones to compare with and Queue the string
					for (int i = 0; i < recievetreelist.size(); i++)
					{
						if (recievetreelist.at(i).id == IDval)
						{
							if (recievetreelist.at(i).isDone == 0)
							{
								std::cout << "Received packet: " << inboundstring << std::endl;
								Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack2 " + std::to_string(sid)).c_str());
								FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack2 " + std::to_string(sid)); // write the drone sent data to file
								sleep(1);
								recievetreelist.at(i).isDone = 1;
								AppendStringData(inboundstring);
								break;
							}
							else
							{
								Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack2 " + std::to_string(sid)).c_str());
								FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack2 " + std::to_string(sid)); // write the drone sent data to file
								sleep(1);
								//drone was already added, break
								break;
							}
						}
					}
				}
				//recieved acknowledge
				else if (inboundstring.find(" ack2 ") != std::string::npos)
				{
					//get the id of the ack
					int ackid = 0, recid = 0;
					std::stringstream temp(inboundstring);
					temp >> recid;
					temp.ignore(1, ' ');
					temp.ignore(5, ' ');
					temp >> ackid;
					if (recid == sid)
					{
						for (int i = 0; i < sendtreelist.size(); i++)
						{
							if (sendtreelist.at(i).id == ackid)
							{
								if (sendtreelist.at(i).isDone == 0)
								{
									std::cout << "Received packet: " << inboundstring << std::endl;
									sendtreelist.at(i).isDone = 1;
									break;
								}
								else
								{
									//drone was already added, break
									break;
								}
							}
						}
					}
				}
				else {};
			}
		}
	}
	for (int i = 0; i < recievetreelist.size(); i++)
	{
		recievetreelist.at(i).isDone = 0;
	}
	for (int i = 0; i < sendtreelist.size(); i++)
	{
		sendtreelist.at(i).isDone = 0;
	}
}

// Returns the number of swaps and places an array containing swaps in swapstomake
// Note: drone id's at even indexes in the array will swap with the 
// drone id at the immediately following odd index, index 0 is even
int GetSwapsFromStrings(std::string mystring, std::string winningstring, int* swapstomake)
{
	// No swaps required
	if (mystring.compare(winningstring) == 0)
	{
		//std::cout << "\nI HAD THE SAME DATA :D \n";
		return 0;
	}
	else
	{
		std::vector<int> mystringints, winningstringints;
		int swapspot1, swapspot2, totalswaps;
		//changing data type for function.
		std::stringstream ss(mystring);
		int i;

		while (ss >> i)
		{
			mystringints.push_back(i);

			if (ss.peek() == ',')
			{
				ss.ignore(1, ',');
			}
		}

		std::stringstream ss2(winningstring);
		int j;
		while (ss2 >> j)
		{
			winningstringints.push_back(j);

			if (ss2.peek() == ',')
			{
				ss2.ignore(1, ',');
			}
		}

		std::cout << "\nI DIDN'T HAVE THE SAME DATA, FIXING \n";
		totalswaps = 0;
		for (swapspot1 = 0; mystringints != winningstringints; swapspot1++)
		{
			if (mystringints.at(swapspot1) == winningstringints.at(swapspot1))
			{
				//do nothing, they are the same at the moment
				continue;
			}

			for (swapspot2 = swapspot1;; ++swapspot2)
			{
				//is this the part that needs to swap?
				if (mystringints.at(swapspot2) == winningstringints.at(swapspot1))
				{
					// Add to swap array
					swapstomake[swapspot1 * 2] = mystringints.at(swapspot1);
					swapstomake[swapspot1 * 2 + 1] = mystringints.at(swapspot2);

					//get the corresponding droneID and swap in this drones node, and swap the vector
					//so it is accounted for
					std::swap(mystringints.at(swapspot1), mystringints.at(swapspot2));
					totalswaps++;
					break;
				}
				else
				{
					//do nothing and continue looking, the swap will happen
				}
			}
		}

		return totalswaps; // return the number of swaps to make
	}
}

// Old function, used to swap nodes in the global tree, see GetSwapsFromStrings and ExecuteSwap
void AdjustTree(std::string mystring, std::string winningstring)
{
	if (mystring.compare(winningstring) == 0)
	{
		//std::cout << "\nI HAD THE SAME DATA :D \n";
	}
	else
	{
		std::vector<int> mystringints, winningstringints;
		int swapspot1, swapspot2;
		//changing data type for function.
		std::stringstream ss(mystring);
		int i;
		while (ss >> i)
		{
			mystringints.push_back(i);

			if (ss.peek() == ',')
				ss.ignore(1, ',');
		}
		std::stringstream ss2(winningstring);
		int j;
		while (ss2 >> j)
		{
			winningstringints.push_back(j);

			if (ss2.peek() == ',')
				ss2.ignore(1, ',');
		}

		std::cout << "\nI DIDN'T HAVE THE SAME DATA, FIXING \n";
		for (swapspot1 = 0; mystringints != winningstringints; swapspot1++)
		{
			if (mystringints.at(swapspot1) == winningstringints.at(swapspot1))
			{
				//do nothing, they are the same at the moment
			}
			else
			{
				for (swapspot2 = swapspot1;; ++swapspot2)
				{
					//is this the part that needs to swap?
					if (mystringints.at(swapspot2) == winningstringints.at(swapspot1))
					{
						//get the corresponding droneID and swap in this drones node, and swap the vector
						//so it is accounted for
						globalNodeTree->SwapNodes(globalNodeTree->GetTreeSpotById(globalNodeTree, mystringints.at(swapspot1)), globalNodeTree->GetTreeSpotById(globalNodeTree, mystringints.at(swapspot2)), true);
						globalNodeTree->UpdateLeadBits(globalNodeTree, true);
						std::swap(mystringints.at(swapspot1), mystringints.at(swapspot2));
						break;
					}
					else
					{
						//do nothing and continue looking, the swap will happen
					}
				}
			}
		}
	}
}

//can be used to talk to neighbors of missing drone
NodeTree* NodeTree::GetTreeSpotById(NodeTree* treecheck, int id)
{
	if (treecheck->GetDrone()->GetInfo().GetID() == id)
	{
		return treecheck;
	}

	if (treecheck->GetSibling() && treecheck->GetSibling()->GetDrone())
	{
		if (treecheck->GetSibling()->GetDrone()->GetInfo().GetID() == id)
		{
			return treecheck->GetSibling();
		}
	}

	if (treecheck->GetChild() && treecheck->GetChild()->GetDrone()) {
		return GetTreeSpotById(treecheck->GetChild(), id);
	}
}

// Returns true if there is a drone in front of this drone that is swapping
bool NodeTree::IsSwappingDroneAhead(NodeTree* tree)
{
	// Check this drone
	if (!tree || !tree->GetDrone()) return false;

	// Check sibling
	if (tree->GetSibling() && tree->GetSibling()->GetDrone())
	{
		if (tree->GetSibling()->GetDrone()->GetInfo().IsSwapping()) return true;
	}

	// Check parent
	return IsSwappingDroneAhead(tree->GetParent());
}

void NodeTree::FillMissingDrone(NodeTree*)
{
	// Implement this...
}

// If swaps are suggested, finds and creates list of swaps
int NodeTree::CompareTreesForSwaps(NodeTree* mytree, int sid, int* swapstomake)
{
	int numvotes = 0;
	int tiecount = 0;
	std::string datastring = MakeQueueData(globalNodeTree);
	GetQueueData(datastring, sid);
	std::string winningstring;
	winningstring = CheckVotes(winningstring, numvotes, tiecount, globalDroneQueue);
	int IDval;
	std::string mystring = MakeQueueData(mytree);
	std::stringstream(mystring) >> IDval >> mystring;

	if (winningstring != "")
	{
		return GetSwapsFromStrings(mystring, winningstring, swapstomake);
	}
	else
	{
		std::cout << "Tie doing nothing\n";
		return 0;
	}
}

NodeTree* NodeTree::GetLeftNodeAtLevel(int level)
{
	NodeTree* current = this;

	for (int i = 1; i <= level; i++)
	{
		current = current->child;
	}

	return current;
}

NodeTree* NodeTree::GetRightNodeAtLevel(int level)
{
	NodeTree* current = this;
	current = current->child;
	current = current->sibling;

	for (int i = 1; i < level; i++)
	{
		current = current->child;
	}

	return current;
}

int NodeTree::GetMaxLevel()
{
	int temp = 0;
	int maxLevel = 0;

	NodeTree* current = this;
	while (current != NULL)
	{
		temp++;
		current = current->child;
	}
	maxLevel = temp;

	temp = 1;
	current = this->child->sibling;
	while (current != NULL)
	{
		temp++;
		current = current->child;
	}

	if (temp > maxLevel)
	{
		maxLevel = temp;
	}

	return maxLevel;
}

float NodeTree::GetNetworkFitness()
{
	float fitness = 0.0;

	NodeTree* current = this;
	while (current != NULL)
	{
		int level = floor(current->GetDrone()->GetInfo().GetID() / 2) + 1;
		fitness += (this->GetMaxLevel() - level + 1) * current->GetDrone()->GetInfo().GetBattery();
		current = current->child;
	}

	current = this->child->sibling;
	while (current != NULL)
	{
		int level = floor(current->GetDrone()->GetInfo().GetID() / 2) + 1;
		fitness += (this->GetMaxLevel() - level + 1) * current->GetDrone()->GetInfo().GetBattery();
		current = current->child;
	}

	return fitness;
}
