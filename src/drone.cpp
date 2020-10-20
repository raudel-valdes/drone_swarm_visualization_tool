// Get coordinates
// Send coordinates

// Get leader votes
// Send leader vote

// Calculate movement towards target
// Calculate movement in (or into) formation

// Decide on formation alignment (which side of the V to move on)
#include <string>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <array>
#include "drone.h"
#include "vector3d.h"
#include "droneinfo.h"
#include <iostream>
#include "fileio.h"

// Global
Vector3D startspot = Vector3D(50, 5, 100);
Vector3D leadspot = startspot;

// Function declarations
int Orientation(const Vector3D& target, const Vector3D& leadDrone, const Vector3D& point);

Drone::Drone(const Vector3D& target, const DroneInfo& position)
{
	this->target = target;
	this->info = position;
	this->elevation = this->info.GetLocation().getZ();
}

void Drone::Move(Autopilot_Interface& interface)
{
	// Get location
	Vector3D current_location = info.GetLocation();

	std::cout << "Location: " << current_location.getX() << " " << current_location.getY() << " " << current_location.getZ() << std::endl;
	std::cout << "waypoint: " << waypoint.getX() << " " << waypoint.getY() << " " << waypoint.getZ() << std::endl;
	std::cout << "target: " << target.getX() << " " << target.getY() << " " << target.getZ() << std::endl;

	// Movement vector
	Vector3D movement_vector;

	// Calculate waypoint
	//if(!(this->GetInfo().IsSwapping())){

	  // Normal speed
	  //movement_vector = (waypoint - current_location).UnitVector().Scale(DRONE_SPEED);

	//}
	// Drone is swapping
	//else{

	// Set speed based on whether drone is ahead or behind
	float speed;
	movement_vector = target - current_location;
	movement_vector = movement_vector.RotateZ(-90);
	Vector3D temp_vector = current_location + movement_vector;
	int result = Orientation(temp_vector, current_location, waypoint);
	if (result == -1) speed = DRONE_SPEED_BACKWARD_SWAP;
	else if (result == 1 && (waypoint - current_location).Magnitude() < DRONE_SPEED_BACKWARD_SWAP) speed = DRONE_SPEED_BACKWARD_SWAP;
	else if (result == 1 && (waypoint - current_location).Magnitude() > DRONE_SPEED_FORWARD_SWAP) speed = DRONE_SPEED_FORWARD_SWAP;
	else speed = DRONE_SPEED;

	// figure out if waypoint is to the left or right
	result = Orientation(target, current_location, waypoint);
	std::cout << "speed: " << speed << std::endl;
	temp_vector = target - current_location;
	//std::cout << "temp vector: " << temp_vector.getX() << " " << temp_vector.getY() << " " << temp_vector.getZ() << std::endl;
	Vector3D temp_waypoint = waypoint;
	Vector3D temp_target;

	// if waypoint is to the left
	if (result == 1)
	{
		// Rotate vector
		temp_vector = temp_vector.RotateZ(45);
		temp_target = current_location + temp_vector;

		// Move waypoint to front of line
		while (Orientation(temp_target, current_location, temp_waypoint) != -1 || (temp_waypoint - current_location).Magnitude() < DRONE_SPEED)
		{
			temp_waypoint += (target - temp_waypoint).UnitVector().Scale(DRONE_SPEED);
			//std::cout << "left temp waypoint: " << temp_waypoint.getX() << " " << temp_waypoint.getY() << " " << temp_waypoint.getZ() << std::endl;
			//std::cout << "temp target: " << temp_target.getX() << " " << temp_target.getY() << " " << temp_target.getZ() << std::endl;
			//std::cout << "Current location: " << current_location.getX() << " " << current_location.getY() << " " << current_location.getZ() << std::endl;
			//sleep(1);
		}
	}
	// waypoint centered or to the right
	else
	{
		// Rotate vector
		temp_vector = temp_vector.RotateZ(-45);
		temp_target = current_location + temp_vector;

		// Move waypoint to front of line
		while (Orientation(temp_target, current_location, temp_waypoint) != 1 || (temp_waypoint - current_location).Magnitude() < DRONE_SPEED)
		{
			temp_waypoint += (target - temp_waypoint).UnitVector().Scale(DRONE_SPEED);
			//std::cout << "right temp waypoint: " << temp_waypoint.getX() << " " << temp_waypoint.getY() << " " << temp_waypoint.getZ() << std::endl;
			//std::cout << "temp target: " << temp_target.getX() << " " << temp_target.getY() << " " << temp_target.getZ() << std::endl;
			//std::cout << "Current location: " << current_location.getX() << " " << current_location.getY() << " " << current_location.getZ() << std::endl;
			//sleep(1);
		}
	}

	// Create movement vector
	movement_vector = (temp_waypoint - current_location).UnitVector().Scale(speed);

	//}

	// Set location
	info.SetLocation(current_location + movement_vector);

	// Update location
	FileIO::WritePosition(&FileIO::position_file, info.GetLocation());

	// Check for undefined values
	//if(isnan(movement_vector.getX())) movement_vector.setX(0.0);
	//if(isnan(movement_vector.getY())) movement_vector.setY(0.0);
	//if(isnan(movement_vector.getZ())) movement_vector.setZ(0.0);

	// Send update the autopilot setpoint
	interface.move_setpoint(movement_vector.getX(), movement_vector.getY(), movement_vector.getZ());
}

size_t split(const std::string& txt, std::vector<std::string>* strs)
{
	char delimiter = ':';
	size_t pos = txt.find(delimiter);
	size_t initialPos = 0;
	strs->clear();

	// Decompose statement
	while (pos != std::string::npos)
	{
		strs->push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(delimiter, initialPos);
	}

	// Add the last one
	strs->push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs->size();
}

// Parses the string contained in net[] so the needed information can be
// extracted and used when needed.
std::string ParseNet(std::string input_str, int value_to_return)
{
	std::string delimiter = ":";
	std::string token;
	size_t pos = 0;
	int i = 0;

	// Searches the string and breaks it apart based on the ':' delimiter
	// Stores relevant information in appropriate variable.
	// while there exists the delimiter in the string
	while ((pos = input_str.find(delimiter)) != std::string::npos)
	{
		token = input_str.substr(0, pos);  // portion of the string before the delimiter

		/* ??? */
		if (value_to_return == i)
		{
			return token;
		}

		input_str.erase(0, pos + delimiter.length());  // erase the string up to the delimiter
		i++;
	}

	return "-1";
}

// This just computes the distance of each drone from the target
double Drone::ComputeDistanceToTarget(const std::string& net, const Vector3D& target)
{
	Vector3D coordinate(strtod(ParseNet(net, 1).c_str(), NULL),  // Must convert to c_str for strtod.
		strtod(ParseNet(net, 2).c_str(), NULL),
		strtod(ParseNet(net, 3).c_str(), NULL));

	return sqrt(pow((coordinate.getX() - target.getX()), 2) +
		pow((coordinate.getY() - target.getY()), 2) +
		pow((coordinate.getZ() - target.getZ()), 2));
}

// Returns 1 if point is left of formation vector, -1 if to the right, and 0 if
// aligned
int Orientation(const Vector3D& target, const Vector3D& leadDrone, const Vector3D& point)
{
	double result =
		((target.getX() - leadDrone.getX()) * (point.getY() - leadDrone.getY())) -
		((target.getY() - leadDrone.getY()) * (point.getX() - leadDrone.getX()));

	if (result == 0)
	{
		return 0;
	}
	else if (result > 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

void Drone::UpdateWaypointVertical(float z)
{
	elevation += z;
}

void Drone::CalculateWaypoint(const DroneInfo& leadDrone)
{
	// Update lead spot
	leadspot = (this->target - leadspot).UnitVector().Scale(DRONE_SPEED) + leadspot;

	// if the drone is the leader, go straight for the target
	if (this->info.IsLead())
	{
		this->waypoint = leadspot;

		// Adjust for altitude
		this->waypoint.setZ(this->elevation);

		return;
	}

	// If not lead, update the lead spot to the lead drone
	if (!leadDrone.IsSwapping() || leadDrone.GetSwappingState() >= POSITION_2) leadspot = leadDrone.GetLocation() + (this->target - leadDrone.GetLocation()).UnitVector().Scale(DRONE_SPEED);

	// Formation vector is just a vector from the lead drone to the target.
	// It is the overall vector the drones follow.
	Vector3D formationVector = this->target - leadspot;

	// calculate which side of the V this drone is on and will follow
	Vector3D VectorToUse;
	if ((this->info.IsRightSide()))
	{
		VectorToUse = formationVector.RotateZ((45 / 2) + 180).UnitVector().Scale(DISTANCE_BETWEEN_DRONES);
	}
	else
	{
		VectorToUse = formationVector.RotateZ((-45 / 2) + 180).UnitVector().Scale(DISTANCE_BETWEEN_DRONES);
	}

	// the vector from this to the lead drone
	//Vector3D VectorToLead = leadDrone.GetLocation() - this->info.GetLocation();
	// project the vector the the lead drone
	//Vector3D prjVecToLeadOnVForm = VectorToLead.Project(VectorToUse);

	// Adjust for distance
	//prjVecToLeadOnVForm = prjVecToLeadOnVForm.UnitVector().Scale(DISTANCE_BETWEEN_DRONES);

	this->waypoint = Vector3D(leadspot + VectorToUse);

	// Adjust for altitude
	this->waypoint.setZ(this->elevation);
}

// Starts calculating the positions of each drone and where they need to go in
// order to fly in a V formation
void Drone::MoveDronesVFormation(std::string net[],
	const Vector3D& formationVector,
	std::string leadDrone,
	double formationDistance,
	const Vector3D& target,
	const int droneCount)
{
	Vector3D lead_coords(strtod(ParseNet(leadDrone, 1).c_str(), NULL),
		strtod(ParseNet(leadDrone, 2).c_str(), NULL),
		strtod(ParseNet(leadDrone, 3).c_str(), NULL));

	Vector3D coords[droneCount];
	for (int i = 0; i < droneCount; ++i)
	{
		coords[i] = Vector3D(strtod(ParseNet(net[i], 1).c_str(), NULL),
			strtod(ParseNet(net[i], 2).c_str(), NULL),
			strtod(ParseNet(net[i], 3).c_str(), NULL));
	}

	int formationSpeed = 4;

	// rotated formation vectors
	Vector3D v1 = formationVector.RotateZ(45 / 2 + 180);
	Vector3D v2 = formationVector.RotateZ(-45 / 2 + 180);

	std::vector<Vector3D> base_change = Vector3D::Repmat(formationVector, droneCount);

	// determines which side of the lead drone each drone is on
	double sides[droneCount];
	for (int i = 0; i <= droneCount; i++)
	{
		sides[i] = -formationVector.getX() * (coords[i].getY() - target.getY()) + formationVector.getY() * (coords[i].getX() - target.getX());
	}

	// the below p may be able to use arrays instead of vectors
	// int left_count = std::count_if(sides.begin(), sides.end(), [](int i)
	// {return i > 0});

	// vformChange contains the correct vectors dependent on side for each drone
	Vector3D vformChange[droneCount];
	for (int i = 0; i < droneCount; ++i)
	{
		if (sides[i] < 0)
		{
			vformChange[i] = v1;
		}
		else
		{
			vformChange[i] = v2;
		}
	}

	//!!! int l1 = v1follows.size();
	//!!! int l2 = v2follows.size();

	// matrix of distance between vector to the lead drone
	std::vector<Vector3D> vector_to_lead(droneCount);
	for (int i = 0; i < droneCount; ++i)
	{
		vector_to_lead[i] = lead_coords - coords[i];
	}

	// projection of vectorToLeadDrone onto vformChange
	std::vector<Vector3D> proj_vector_to_lead_fv(droneCount);
	for (int i = 0; i < droneCount; ++i)
	{
		proj_vector_to_lead_fv[i] = vector_to_lead[i].Project(vformChange[i]);
	}

	// swap x and y if sides < 0
	for (int i = 0; i < droneCount; ++i)
	{
		if (sides[i] < 0)
		{
			double temp = proj_vector_to_lead_fv[i].getX();
			proj_vector_to_lead_fv[i].setX(proj_vector_to_lead_fv[i].getY());
			proj_vector_to_lead_fv[i].setY(temp);
		}
	}

	// matrix droneVector = lead_coords - projVtldFV - coords
	std::vector<Vector3D> droneVector(droneCount);
	for (int i = 0; i < droneCount; ++i)
	{
		droneVector[i] = lead_coords - proj_vector_to_lead_fv[i] - coords[i];
	}

	// declares matrix state changes of numDrones x 3
	// coder.varsize('StateChanges', [droneCount 3]);

	// base_change is now a unit vector
	for (auto vector : base_change)
	{
		vector = vector.UnitVector();
	}

	// droneVector is now a unit vector
	for (auto& vector : droneVector)
	{
		vector = vector.UnitVector();
	}

	// StateChanges = combines moving in formationVector direction with moving
	// into formation
	// StateChanges = (base_change(1:droneCount,1:3) * formationSpeed) +
	// droneVector(1 : droneCount, 1 : 3);
	std::vector<Vector3D> StateChanges(droneCount);
	for (int i = 0; i < droneCount; i++)
	{
		StateChanges[i] = base_change[i] * formationSpeed + droneVector[i];
	}

	//******************************************************************************************
	// TODO
	// CollisionAvoidence:
	//******************************************************************************************
	// Ignoring collision avoidance for now
	std::vector<Vector3D> collisionAvoidenceVector(droneCount);
	// collisionAvoidenceVector = avoidCollisions(net, StateChanges, droneCount);
	// we pass in StateChanges here, aka what we want the drones to do
	// make collisionAvoidance a unit vector

	// make all vectors in collision AvoidanceVector unit size
	for (auto& vector : collisionAvoidenceVector)
	{
		vector = vector.UnitVector();
	}

	// same as before but include collision avoidance movement
	// StateChanges = combines moving in formationVector direction with moving
	// into formation
	// as well as include the collision avoidance movement.
	// StateChanges =
	//                  (base_change(1:droneCount,1:3) * formationSpeed)
	//                  + droneVector(1:droneCount,1:3)
	//                  + collisionAvoidenceVector(1:droneCount, 1:3);
	for (int i = 0; i < droneCount; ++i)
	{
		StateChanges[i] = base_change[i] * formationSpeed + droneVector[i] + collisionAvoidenceVector[i];
	}
}

// There are code for dead drones. It just moves dead drones down 1 unit on
// the z axis.sim only, so skipping.
void Drone::AvoidCollision(std::string net[],
	const std::vector<Vector3D>& changeVector,
	const int droneCount)
{
	Vector3D coords[droneCount];
	for (int i = 0; i < droneCount; ++i)
	{
		coords[i] = Vector3D(strtod(ParseNet(net[i], 1).c_str(), NULL),
			strtod(ParseNet(net[i], 2).c_str(), NULL),
			strtod(ParseNet(net[i], 3).c_str(), NULL));
	}

	int minCollisionDistance = 12;

	// creating a matrix, livingDrones, of only the living drones
	std::vector<Vector3D> livingDrones(droneCount);
	for (int i = 0; i < droneCount; ++i)
	{
		if (strtod(ParseNet(net[i], 5).c_str(), NULL) == 1)
		{
			livingDrones.push_back(coords[i]);
		}
	}

	int num_alive = livingDrones.size();

	// distance vector with size m(m-1)/2
	const int distance_size = num_alive * (num_alive - 1) / 2;
	std::vector<double> distance(distance_size);
	// Distance in Euclidean 3 space
	for (int i = 0; i < distance_size; ++i)
	{
		distance[i] = pow(livingDrones[i].Magnitude(), 2);
	}

	// tempMatrix is a lower triangular matrix
	// where everything below the diagonal is 1
	// 0 0 0
	// 1 0 0
	// 1 1 0
	int tempMatrix[num_alive][num_alive];
	for (int row = 1; row < num_alive; row++)
	{
		for (int column = 0; column < row; column++)
		{
			tempMatrix[row][column] = 1;
		}
	}

	// %# get the indices of the 1's
	// [rowIdx,colIdx] = find(tmp);

	// %# create the output
	// out = [D',livingDrones(rowIdx,:),livingDrones(colIdx,:)];

	// This is most definitely not right
	const int unknown_size = 4;  // magic number, need to comeback and fix
	double outputMatrix[distance.size()][unknown_size];
	for (int i = 0; i < distance.size(); ++i)
	{
		outputMatrix[i][0] = distance[i];
		outputMatrix[i][1] = livingDrones[i].getX();
		outputMatrix[i][2] = livingDrones[i].getY();
		outputMatrix[i][3] = livingDrones[i].getZ();
	}

	std::vector<bool> selectCollisions;

	// add a 1 to selectCollisions if the distance is less than min, else a 0
	for (int i = 0; i < distance_size; ++i)
	{
		bool value = distance[i] < minCollisionDistance;
		selectCollisions.push_back(value);
	}

	double collisions[distance_size][unknown_size];
	for (int i = 0, j = 0; i < distance_size; ++i)
	{
		if (selectCollisions[i])
		{
			collisions[j][0] = distance[i];
			collisions[j][1] = livingDrones[i].getX();
			collisions[j][2] = livingDrones[i].getY();
			collisions[j][3] = livingDrones[i].getZ();
			j++;
		}
	}

	double collisionDroneMatrix[distance_size][2];
	for (int i = 0; i < distance_size; ++i)
	{
		collisionDroneMatrix[i][0] = collisions[i][7];
		collisionDroneMatrix[i][1] = collisions[i][14];
	}

	std::vector<Vector3D> vbpt(num_alive);
	for (int i = 0; i < num_alive; ++i)
	{
		vbpt[i] = Vector3D(
			strtod(ParseNet(net[int(collisionDroneMatrix[i][0])], 1).c_str(), NULL) -
			strtod(ParseNet(net[int(collisionDroneMatrix[i][1])], 1).c_str(), NULL),
			strtod(ParseNet(net[int(collisionDroneMatrix[i][0])], 2).c_str(), NULL) -
			strtod(ParseNet(net[int(collisionDroneMatrix[i][1])], 2).c_str(), NULL),
			strtod(ParseNet(net[int(collisionDroneMatrix[i][0])], 3).c_str(), NULL) -
			strtod(ParseNet(net[int(collisionDroneMatrix[i][1])], 3).c_str(), NULL));
	}

	std::vector<double> magnitudeVbpt(num_alive);
	for (int i = 0; i < num_alive; ++i)
	{
		magnitudeVbpt[i] = vbpt[i].Magnitude();
	}

	// unit vbpt
	std::vector<Vector3D> uvbpt(num_alive);
	for (int i = 0; i < num_alive; ++i)
	{
		uvbpt[i] = vbpt[i].UnitVector();
	}

	std::vector<Vector3D> chvt(num_alive);
	for (int i = 0; i < num_alive; ++i)
	{
		chvt[i] = uvbpt[i] * (minCollisionDistance - magnitudeVbpt[i]);
	}

	std::vector<Vector3D> collisionAvoidVector(num_alive);
	for (int i = 0; i < num_alive; ++i)
	{
		collisionAvoidVector[i] = Vector3D(0, 0, 0);
		//(*collisionAvoidVector)[int(collisionDroneMatrix[i][0])].getX()
		//+ (*chvt)[i].getX()
	}

	for (int i = 0; i < distance.size(); i++)
	{
		collisionAvoidVector[int(collisionDroneMatrix[i][0])] += chvt[i];
		collisionAvoidVector[int(collisionDroneMatrix[i][1])] -= chvt[i];
	}
}

// This is the first function from the Matlab Drones file.
// Basically, it just takes in information and then determines the lead drone
// based upon who is closest.
// Then it calls the appropriate formation function based upon the formation
// number passed to it.
// Since there is only the V Formation, that is the function that is called.
// net[] each row contains the information that is sent for each drone.
// The format assumed in the file is a string of format
// "DroneID:XCoordinate:YCoordinate:ZCoordinate:IsAlive:BatteryLife"
// I.E. "1:50.42:39.76:85.92:1:75"
// DroneID = 1
// XCoordinate=50.42
// YCoordinate=39.76
// ZCoordinate=85.92
// IsAlive = 1 - means drone is alive
// BatteryLife = 75 or 75%
void Drone::NetOut(std::string net[], int state, const Vector3D& target, const int droneCount, int formationNumber)
{
	// Set initial variable to hold point to point distances
	double distances[droneCount];
	for (int n = 0; n <= droneCount; n++)
	{
		distances[n] = ComputeDistanceToTarget(net[n], target);
	}

	// Stores the found minimal distance from drone to target
	double minDist = -1.0;
	for (int i = 0; i < droneCount + 1; i++)
	{
		if (minDist != -1.0 && distances[i] < minDist)
		{
			minDist = distances[i];
		}
	}

	// Finds the lead drone based upon distance from target
	std::string leadDrone = "-1";
	double leadDistance = minDist;

	for (int m = 0; m < droneCount + 1; m++)
	{
		if (minDist == distances[m])
		{
			leadDrone = net[m];
		}
	}

	// Create lead drone coordinates and formation vector to be passed to
	// formation creation
	Vector3D lead_coords = Vector3D(strtod(ParseNet(leadDrone, 1).c_str(), NULL),
		strtod(ParseNet(leadDrone, 2).c_str(), NULL),
		strtod(ParseNet(leadDrone, 3).c_str(), NULL));

	Vector3D formationVector = Vector3D(target - lead_coords);

	// Choose which formation to utilize based upon passed in value
	switch (formationNumber)
	{
	case 1:
		MoveDronesVFormation(net, formationVector, leadDrone, leadDistance,
			target, droneCount);
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;

	default:
		break;
	}
}
