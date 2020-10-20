#ifndef FILEIO_H_
#define FILEIO_H_

#include <string>
#include "drone.h"
#include "vector3d.h"

// FileIO code for writing to & from files goes here
class FileIO 
{
public:
	static void ReadIP(std::string* self, std::vector<std::string>* remotes);
	static void ReadID(std::string* self, std::vector<int>* otherslevels, std::vector<int>* others);
	static void WritePosition(std::ofstream* file, const Vector3D& position);
	static void WritePacket(std::ofstream* file, const std::string& packet);

	static void Initialize();  // This will create the 3 text files,
	static void CloseFiles();  // Close the files

	// http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	static bool exists(const std::string& name);
	static void create_file(const std::string& name);

	// private:
	static std::ofstream position_file;
	static std::ofstream rec_packet_file;
	static std::ofstream sent_packet_file;
};

#endif
