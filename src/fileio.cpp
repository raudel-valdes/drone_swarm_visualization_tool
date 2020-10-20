// FileIO code for writing to & from files goes here
#include <iostream>
#include <fstream>
#include "fileio.h"
#include <time.h>

std::ofstream FileIO::position_file;
std::ofstream FileIO::rec_packet_file;
std::ofstream FileIO::sent_packet_file;

// WritePosition()
void FileIO::WritePosition(std::ofstream* file, const Vector3D& position) 
{
	time_t rawtime;
	struct tm* currenttime;

	time(&rawtime);
	currenttime = localtime(&rawtime);

	*file << currenttime->tm_hour << ":" << currenttime->tm_min << ":" << currenttime->tm_sec
		<< " " << position.getX() << " " << position.getY()
		<< " " << position.getZ() << std::endl;
}

// WriteReceivedPacket()
void FileIO::WritePacket(std::ofstream* file, const std::string& packet) 
{
	*file << packet << std::endl;
}

// Initialize()
// This will create the 3 text files,
void FileIO::Initialize() 
{
	position_file.open("./logs/position.txt");
	rec_packet_file.open("./logs/packet_received.txt");
	sent_packet_file.open("./logs/packet_sent.txt");

	if (!(position_file.is_open() || rec_packet_file.is_open() || sent_packet_file.is_open())) 
	{
		std::cout << "Error opening file" << std::endl;
	}
}

// Close files
void FileIO::CloseFiles() 
{
	position_file.close();
	rec_packet_file.close();
	sent_packet_file.close();
}

// for now format files such that the first line is your ip. later on, once
// BATMAN is fully implemented,
// we can have a list of all the ip's and self is automatically removed
void FileIO::ReadIP(std::string* self, std::vector<std::string>* remotes) 
{
	std::ifstream file("./params/ip.txt");
	std::string str;

	std::getline(file, *self);

	while (std::getline(file, str)) 
	{
		// if(str.find_first_not_of("\t\n ") != std::string::npos) {
		//    std::cout << "found a non whitespace line" << std::endl;
		remotes->push_back(str);
		//    }
	}
}

void FileIO::ReadID(std::string* self, std::vector<int>* otherslevels, std::vector<int>* others) 
{
	std::ifstream file("./params/droneID.txt");
	int level; int id;
	if (file.is_open())
	{
		std::getline(file, *self);
		while (file >> id >> level) 
		{
			// if(str.find_first_not_of("\t\n ") != std::string::npos) {
			//    std::cout << "found a non whitespace line" << std::endl;
			otherslevels->push_back(level);
			others->push_back(id);
			//    }
		}
		file.close();
	}
	else
	{
		std::cout << "Unable to open file";
	}
}

bool FileIO::exists(const std::string& name) 
{
	std::ifstream infile(name);
	return infile.good();
}

void FileIO::create_file(const std::string& name) 
{
	std::ofstream outfile(name);
	outfile << "" << std::endl;
	outfile.close();
}
