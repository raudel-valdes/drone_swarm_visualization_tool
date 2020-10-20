#ifndef DRONEINFO_H_
#define DRONEINFO_H_

#include <string>
#include "vector3d.h"

// Swapping types
enum swap_t
{
	NOT_SWAPPING = 0,
	STARTED_SWAPPING = 1,
	POSITION_1 = 2,
	POSITION_2 = 3,
	POSITION_3 = 4,
	FINISHED_SWAPPING = 5
};

class DroneInfo
{
public:
	DroneInfo() {}
	DroneInfo(std::string Net, bool lead);
	DroneInfo(std::string Net);

	inline bool IsAlive() { return battery > 0; }
	std::string ToString();
	inline void SetLocation(const Vector3D& location) { this->location = location; }
	inline void SetLocation(float x, float y, float z) { location = Vector3D(x, y, z); }
	inline void SetBattery(float batteryLevel) { battery = batteryLevel; }
	inline double GetBattery() const { return battery; }
	inline Vector3D GetLocation() const { return location; }
	inline void SetIsLeadDrone(bool isLeadDrone) { this->isLeadDrone = isLeadDrone; }
	inline bool IsLead() { return isLeadDrone; }
	int GetID() { return id; }
	void SetIsSwapping(bool isSwapping) { this->isSwapping = isSwapping; }
	bool IsSwapping() const { return isSwapping; }
	int GetSwappingState() const { return swappingState; }
	void SetSwappingState(int swappingState) { this->swappingState = swappingState; }
	void SetIsRightSide(bool isRightSide) { this->isRightSide = isRightSide; }
	bool IsRightSide() { return isRightSide; }

private:
	Vector3D location;
	// The drone's remaining battery life.
	double battery;
	bool isLeadDrone;
	int id;
	// True if the drone is currently swapping.
	bool isSwapping;
	int swappingState;
	bool isRightSide;
};

#endif
