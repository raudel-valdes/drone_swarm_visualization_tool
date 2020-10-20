#include <sstream>
#include <string>
#include "droneinfo.h"
#include "vector3d.h"
#include <iostream>

// ID:getX:getY:getZ:Battery:isLeadDrone
DroneInfo::DroneInfo(std::string Net, bool lead) 
{
	std::stringstream stream(Net);
	int id, state;
	double x, y, z, bat;
	bool swap;
	stream >> id >> x >> y >> z >> bat >> swap >> state;
	this->id = id;
	this->location = Vector3D(x, y, z);
	this->battery = bat;
	this->isLeadDrone = lead;
	this->SetIsSwapping(swap);
	this->SetSwappingState(state);
}

DroneInfo::DroneInfo(std::string Net) 
{
	std::stringstream stream(Net);
	bool lead, swap;
	int id, state;
	double x, y, z, bat;
	stream >> id >> x >> y >> z >> bat >> swap >> state >> lead;
	this->id = id;
	this->location = Vector3D(x, y, z);
	this->battery = bat;
	this->isLeadDrone = lead;
	this->SetIsSwapping(swap);
	this->SetSwappingState(state);
}

std::string DroneInfo::ToString() 
{
	std::stringstream stream;
	double x = location.getX();
	double y = location.getY();
	double z = location.getZ();

	// minimum values
	if (x < 0.1 && x > -0.1) x = 0.0;
	if (y < 0.1 && y > -0.1) y = 0.0;
	if (z < 0.1 && z > -0.1) z = 0.0;

	stream << id << " " << x << " " << y << " "
		<< z << " " << battery << " " << isSwapping << " "
		<< swappingState << " " << isLeadDrone;
	return stream.str();
}
