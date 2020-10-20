#ifndef DRONE_H_
#define DRONE_H_

#include "droneinfo.h"
#include "vector3d.h"
#include "autopilot_interface.h"

// Distance, in meters, between drones in the formation
#define DISTANCE_BETWEEN_DRONES		5.0
#define DRONE_SPEED					5.0
#define DRONE_SPEED_FORWARD_SWAP	6.0
#define DRONE_SPEED_BACKWARD_SWAP	4.0
#define DRONE_VERTICAL_OFFSET		2.0

class Drone
{
public:
	Drone(const Vector3D&, const DroneInfo&);

	void CalculateWaypoint(const DroneInfo&);
	void Move(Autopilot_Interface& interface);
	static double ComputeDistanceToTarget(const std::string&, const Vector3D&);

	static void MoveDronesVFormation(std::string[], const Vector3D&, std::string, double, const Vector3D&, const int);
	static void AvoidCollision(std::string[], const std::vector<Vector3D>&, const int);
	static void NetOut(std::string[], int, const Vector3D&, int, int);

	Vector3D GetWaypoint() { return waypoint; }
	void UpdateWaypointVertical(float);
	Vector3D GetTarget() { return target; }
	DroneInfo GetInfo() { return info; }
	DroneInfo* GetInfoPointer() { return &info; }
	int GetLevel() { return level; }
	void SetLevel(int level) { this->level = level; }
	bool GetIsMe() { return isMe; }
	void SetIsMe(bool isMe) { this->isMe = isMe; }

private:
	DroneInfo info;
	int level;
	bool isMe;
	float elevation;
	Vector3D waypoint;
	Vector3D target;
};

#endif
