#ifndef NODETREE_H_
#define NODETREE_H_

#include "drone.h"
#include "droneinfo.h"
#include "vector3d.h"
#include <math.h>

#define ACCEPTABLE_Z_OFFSET		1.0
#define ACCEPTABLE_Y_OFFSET		1.0
#define ACCEPTABLE_X_OFFSET		1.0

struct StatusList
{
	int id;
	int level;
	bool isDone;
};

class NodeTree
{
public:
	NodeTree() {};
	NodeTree(Drone* drone, NodeTree* parent, NodeTree* sibling, NodeTree* child);
	NodeTree(const NodeTree& copy);
	// TODO: Implement destructor.
	~NodeTree();
	// Gets the lowest battery value in the tree.
	// TODO: Make this private and add a public method GetLowestBattery() that calls GetLowestBattery(this).
	double GetLowestBattery(NodeTree*);
	// Adds the given Drone to the tree based on the Drone's level.
	// TODO: Make this private and add a public method AddDrone(Drone*) that calls AddDrone(this, drone).
	void AddDrone(NodeTree*, Drone*);
	// Finds the given Drone in the tree and updates its information.
	// TODO: Make this private and add a public method UpdateDrone(Drone*) that calls UpdateDrone(this, drone).
	void UpdateDrone(NodeTree*, Drone*);
	void GroupPoll(NodeTree*); //adjust the NodeTree by accounting for batterylife differences
	// TODO: Implement FillMissingDrone().
	void FillMissingDrone(NodeTree*); 
	int CompareTreesForSwaps(NodeTree*, int, int*); // compares trees and returns the number of swaps and creates list of swaps if any
	// Creates the globalDroneQueue with the data in the NodeTree.
	std::string MakeQueueData(NodeTree*);
	void GetQueueData(std::string, int); //gets the datastrings from other drones and increments votes
	// Swaps the given nodes. If test is true, the drone data won't be changed.
	void SwapNodes(NodeTree*, NodeTree*, bool);
	// Gets the node who's drone has an isMe bit of 1.
	// TODO: Rename this.
	NodeTree* ExecutePlan(NodeTree*); 
	NodeTree* GetTreeSpotById(NodeTree*, int); //returns the NodeTree from the spot of the drone that has the same id as the integer parameter
	NodeTree* GetLeftNodeAtLevel(int);
	NodeTree* GetRightNodeAtLevel(int);
	// Gets the root node of the tree.
	NodeTree* GetRoot(NodeTree*);
	float GetNetworkFitness();
	int GetMaxLevel();
	// Returns true if the drone in the given node is in an acceptable position relative to the given coordinates.
	bool IsDronePositionAcceptable(NodeTree*, float*, float*, float*);
	// Makes the given node the lead and sets the lead bit of all nodes below it to false.
	// TODO: Make this private and add a public method SetToLead() that calls UpdateLeadBits(this, true).
	void UpdateLeadBits(NodeTree*, bool);
	bool IsSwappingDroneAhead(NodeTree* tree); // Returns true if there is a drone ahead or sibling that is swapping

	Drone* GetDrone() { return drone; };
	void SetDrone(Drone* drone) { this->drone = drone; }
	NodeTree* GetParent() { return parent; }
	void SetParent(NodeTree* parent) { this->parent = parent; }
	NodeTree* GetChild() { return child; }
	void SetChild(NodeTree* child) { this->child = child; }
	NodeTree* GetSibling() { return sibling; }
	void SetSibling(NodeTree* sibling) { this->sibling = sibling; }

private:
	NodeTree* parent;
	NodeTree* sibling;
	NodeTree* child;
	Drone* drone;
};

extern std::vector<StatusList> recievetreelist, sendtreelist;
extern std::vector<std::string> RemoteIPs;
extern NodeTree* globalNodeTree;

#endif
