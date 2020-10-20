/****************************************************************************
 *
 *   Copyright (c) 2014 MAVlink Development Team. All rights reserved.
 *   Author: Trent Lukaczyk, <aerialhedgehog@gmail.com>
 *           Jaycee Lock,    <jaycee.lock@gmail.com>
 *           Lorenz Meier,   <lm@inf.ethz.ch>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

 /**
  * @file mavlink_control.cpp
  *
  * @brief An example offboard control process via mavlink
  *
  * This process connects an external MAVLink UART device to send an receive data
  *
  * @author Trent Lukaczyk, <aerialhedgehog@gmail.com>
  * @author Jaycee Lock,    <jaycee.lock@gmail.com>
  * @author Lorenz Meier,   <lm@inf.ethz.ch>
  *
  */



  // ------------------------------------------------------------------------------
  //   Includes
  // ------------------------------------------------------------------------------

#include "mavlink_control.h"
#include <time.h>

std::vector<NodeTree*> swaps;

NodeTree* globalNodeTree; //global NodeTree for the drone
std::vector<StatusList> recievetreelist; //vector status list for recieved drones
std::vector<StatusList> sendtreelist; //vector status list for drones that acknowledged messages
std::vector<std::string> RemoteIPs; //remoteIPs which are loaded from ip.txt, this is used for communications

// All done function checks the entire list and returns 1 if all of them are done.
bool AllDone(std::vector<StatusList> listcheck)
{
	for (int i = 0; i < listcheck.size(); i++)
	{
		if (listcheck.at(i).isDone == 0)
		{
			return 0;
		}
	}
	return 1;
}

// ------------------------------------------------------------------------------
//   TOP
// ------------------------------------------------------------------------------
int top(int argc, char** argv)
{
	// --------------------------------------------------------------------------
	//   PARSE THE COMMANDS
	// --------------------------------------------------------------------------

	// Default input arguments from command line

	// do the parse, will throw an int if it fails

	// --------------------------------------------------------------------------
	//   PORT and THREAD STARTUP
	// --------------------------------------------------------------------------

	/*
	 * Instantiate a serial port object
	 *
	 * This object handles the opening and closing of the offboard computer's
	 * serial port over which it will communicate to an autopilot.  It has
	 * methods to read and write a mavlink_message_t object.  To help with read
	 * and write in the context of pthreading, it gaurds port operations with a
	 * pthread mutex lock.
	 *
	 */

// For Mac OS
#ifdef __APPLE__
	char* uart_name = (char*)"/dev/tty.usbmodem1";
#else
	char* uart_name = (char*)"/dev/ttyS0";
#endif

	int baudrate = 57600;
	parse_commandline(argc, argv, uart_name, baudrate);
	Serial_Port serial_port(uart_name, baudrate);
	Autopilot_Interface autopilot_interface(&serial_port);
	serial_port_quit = &serial_port;
	autopilot_interface_quit = &autopilot_interface;
	signal(SIGINT, quit_handler);
	serial_port.start();
	autopilot_interface.start();
	commands(autopilot_interface);
	autopilot_interface.stop();
	serial_port.stop();
	/*
	 * Instantiate an autopilot interface object
	 *
	 * This starts two threads for read and write over MAVlink. The read thread
	 * listens for any MAVlink message and pushes it to the current_messages
	 * attribute.  The write thread at the moment only streams a position target
	 * in the local NED frame (mavlink_set_position_target_local_ned_t), which
	 * is changed by using the method update_setpoint().  Sending these messages
	 * are only half the requirement to get response from the autopilot, a signal
	 * to enter "offboard_control" mode is sent by using the enable_offboard_control()
	 * method.  Signal the exit of this mode with disable_offboard_control().  It's
	 * important that one way or another this program signals offboard mode exit,
	 * otherwise the vehicle will go into failsafe.
	 *
	 */


	 /*
	  * Setup interrupt signal handler
	  *
	  * Responds to early exits signaled with Ctrl-C.  The handler will command
	  * to exit offboard mode if required, and close threads and the port.
	  * The handler in this example needs references to the above objects.
	  *
	  */


	  /*
	   * Start the port and autopilot_interface
	   * This is where the port is opened, and read and write threads are started.
	   */



	   // --------------------------------------------------------------------------
	   //   RUN COMMANDS
	   // --------------------------------------------------------------------------

	   /*
		* Now we can implement the algorithm we want on top of the autopilot interface
		*/



		// --------------------------------------------------------------------------
		//   THREAD and PORT SHUTDOWN
		// --------------------------------------------------------------------------

		/*
		 * Now that we are done we can stop the threads and close the port
		 */



		 // --------------------------------------------------------------------------
		 //   DONE
		 // --------------------------------------------------------------------------

		 // woot!
	return 0;
}


// ------------------------------------------------------------------------------
//   COMMANDS
// ------------------------------------------------------------------------------

void commands(Autopilot_Interface& api)
{
	// --------------------------------------------------------------------------
	//   START OFFBOARD MODE
	// --------------------------------------------------------------------------

	api.enable_offboard_control();
	usleep(100); // give some time to let it sink in

	// now the autopilot is accepting setpoint commands


	// --------------------------------------------------------------------------
	//   SEND OFFBOARD COMMANDS
	// --------------------------------------------------------------------------
	printf("SEND OFFBOARD COMMANDS\n");

	// initialize command data strtuctures
	mavlink_set_position_target_local_ned_t sp;
	mavlink_set_position_target_local_ned_t ip = api.initial_position;

	// autopilot_interface.h provides some helper functions to build the command


	// Example 1 - Set Velocity
//	set_velocity( -1.0       , // [m/s]
//				  -1.0       , // [m/s]
//				   0.0       , // [m/s]
//				   sp        );

	// Example 2 - Set Position
	set_position(ip.x - 5.0, // [m]
		ip.y - 5.0, // [m]
		ip.z, // [m]
		sp);


	// Example 1.2 - Append Yaw Command
	set_yaw(ip.yaw, // [rad]
		sp);

	// SEND THE COMMAND
	// api.update_setpoint(sp);
	// NOW pixhawk will try to move

	// Wait for 8 seconds, check position
	for (int i = 0; i < 8; i++)
	{
		mavlink_local_position_ned_t pos = api.current_messages.local_position_ned;
		printf("%i CURRENT POSITION XYZ = [ % .4f , % .4f , % .4f ] \n", i, pos.x, pos.y, pos.z);
		sleep(1);
	}

	printf("\n");


	// --------------------------------------------------------------------------
	//   STOP OFFBOARD MODE
	// --------------------------------------------------------------------------

	//api.disable_offboard_control();

	// now pixhawk isn't listening to setpoint commands


	// --------------------------------------------------------------------------
	//   GET A MESSAGE
	// --------------------------------------------------------------------------
	printf("READ SOME MESSAGES \n");

	// copy current messages
	Mavlink_Messages messages = api.current_messages;

	// local position in ned frame
	mavlink_local_position_ned_t pos = messages.local_position_ned;
	printf("Got message LOCAL_POSITION_NED (spec: https://pixhawk.ethz.ch/mavlink/#LOCAL_POSITION_NED)\n");
	printf("    pos  (NED):  %f %f %f (m)\n", pos.x, pos.y, pos.z);

	// hires imu
	mavlink_highres_imu_t imu = messages.highres_imu;
	printf("Got message HIGHRES_IMU (spec: https://pixhawk.ethz.ch/mavlink/#HIGHRES_IMU)\n");
	printf("    ap time:     %llu \n", imu.time_usec);
	printf("    acc  (NED):  % f % f % f (m/s^2)\n", imu.xacc, imu.yacc, imu.zacc);
	printf("    gyro (NED):  % f % f % f (rad/s)\n", imu.xgyro, imu.ygyro, imu.zgyro);
	printf("    mag  (NED):  % f % f % f (Ga)\n", imu.xmag, imu.ymag, imu.zmag);
	printf("    baro:        %f (mBar) \n", imu.abs_pressure);
	printf("    altitude:    %f (m) \n", imu.pressure_alt);
	printf("    temperature: %f C \n", imu.temperature);

	printf("\n");


	// --------------------------------------------------------------------------
	//   END OF COMMANDS
	// --------------------------------------------------------------------------
}

// Updates the local target of the drone
void update_local_target(mavlink_set_position_target_local_ned_t sp, Autopilot_Interface& api)
{
	// --------------------------------------------------------------------------
	//   START OFFBOARD MODE
	// --------------------------------------------------------------------------

	api.enable_offboard_control();
	usleep(100); // give some time to let it sink in

	// --------------------------------------------------------------------------
	//   SEND OFFBOARD COMMANDS
	// --------------------------------------------------------------------------

	// SEND THE COMMAND
	api.update_setpoint(sp); // Will be sent to the pixhawk when the write thread gets to it
}


// ------------------------------------------------------------------------------
//   Parse Command Line
// ------------------------------------------------------------------------------
// throws EXIT_FAILURE if could not open the port
void parse_commandline(int argc, char** argv, char*& uart_name, int& baudrate)
{
	// string for command line usage
	const char* commandline_usage = "usage: mavlink_serial -d <devicename> -b <baudrate>";

	// Read input arguments
	// argv[0] is "mavlink"
	for (int i = 1; i < argc; i++)
	{
		// Help
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			printf("%s\n", commandline_usage);
			throw EXIT_FAILURE;
		}

		// UART device ID
		if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0)
		{
			if (argc > i + 1)
			{
				uart_name = argv[i + 1];

			}
			else
			{
				printf("%s\n", commandline_usage);
				throw EXIT_FAILURE;
			}
		}

		// Baud rate
		if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--baud") == 0)
		{
			if (argc > i + 1)
			{
				baudrate = atoi(argv[i + 1]);
			}
			else
			{
				printf("%s\n", commandline_usage);
				throw EXIT_FAILURE;
			}
		}
	}
}


// ------------------------------------------------------------------------------
//   Quit Signal Handler
// ------------------------------------------------------------------------------
// this function is called when you press Ctrl-C
void quit_handler(int sig)
{
	printf("\n");
	printf("TERMINATING AT USER REQUEST\n");
	printf("\n");

	// autopilot interface
	try
	{
		autopilot_interface_quit->handle_quit(sig);
	}
	catch (int error) {}

	// serial port
	try
	{
		serial_port_quit->handle_quit(sig);
	}
	catch (int error) {}

	// end program here
	exit(0);
}


// ------------------------------------------------------------------------------
//   Main
// ------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//checks for ip.txt file
	if (!FileIO::exists("./params/ip.txt"))
	{
		FileIO::create_file("./params/ip.txt");
		std::cout << "Created ip.txt\nEdit it with first line this ip, other lines other ips." << std::endl;
		//checks for droneID.txt file 
		if (!FileIO::exists("./params/droneID.txt"))
		{
			FileIO::create_file("./params/droneID.txt");
			std::cout << "Created droneID.txt\nEdit it with first line this droneID and level, other lines other droneIDs and levels separated by a space, be sure all drones are in this with the correct levels.\n Also make sure the order in the ip.txt file corresponds to the order in this file." << std::endl;
		}
		return 1;
	}

	//checks for droneID.txt file if ip.txt does exist
	if (!FileIO::exists("./params/droneID.txt"))
	{
		FileIO::create_file("./params/droneID.txt");
		std::cout << "Created droneID.txt\nEdit it with first line this droneID and level, other lines other droneIDs and levels separated by a space, be sure all drones are in this with the correct levels.\n Also make sure the order in the ip.txt file corresponds to the order in this file." << std::endl;
		return 1;
	}

	char* uart_name = (char*)"/dev/ttyS0";
	int baudrate = 57600;
	parse_commandline(argc, argv, uart_name, baudrate);
	Serial_Port serial_port(uart_name, baudrate);
	Autopilot_Interface autopilot_interface(&serial_port);
	serial_port_quit = &serial_port;
	autopilot_interface_quit = &autopilot_interface;
	signal(SIGINT, quit_handler);
	serial_port.start();
	autopilot_interface.start();



	NodeTree* Selftree; //NodeTree used for temporary storage in case of ties in voting.
	int SelfID, SelfLevel; //stored self variables
	int action; //used as a boolean for the return of NodeTree comparisons
	std::string selfdata; //string used for initialization of SelfID and SelfLevel
	std::vector<int> RemoteIDs(0), RemoteLevels(0); //Vectors of the Remote drone's information 
	std::string SelfIP; //This drone's ip address when loaded from ip.txt

	FileIO::ReadIP(&SelfIP, &RemoteIPs); //Reads the ip.txt file and stores the first line in SelfIP and the others in the RemoteIPs vector
	//Note: be sure that the ip addresses lines on ip.txt correspond to the lines in droneID.txt
	FileIO::ReadID(&selfdata, &RemoteLevels, &RemoteIDs); //Reads the IDs and Levels from the droneID.txt file similar to ReadIP  
	std::stringstream stream(selfdata); //used to set the SelfID and SelfLevel

	stream >> SelfID >> SelfLevel; //here

	//Fill the StatusList vectors with the information gathered
	for (int i = 0; i < RemoteIDs.size(); i++)
	{
		StatusList addentry;
		addentry.id = RemoteIDs.at(i);
		addentry.level = RemoteLevels.at(i);
		addentry.isDone = 0;
		recievetreelist.push_back(addentry);
		sendtreelist.push_back(addentry);
	}

	std::cout << "Self IP: " << SelfIP << std::endl;

	// create the socket
	if (Socket::Init(SelfIP.c_str()) != 0)
	{
		std::cout << "Socket initialization failed." << std::endl;
	}

	// Creates the output files
	FileIO::Initialize();
	mavlink_set_position_target_local_ned_t sp;
	mavlink_set_position_target_local_ned_t ip = autopilot_interface.initial_position;

	mavlink_local_position_ned_t pos = autopilot_interface.current_messages.local_position_ned;
	mavlink_battery_status_t bat = autopilot_interface.current_messages.battery_status;
	std::cout << "Battery Life: " << std::to_string((int8_t)bat.battery_remaining) << std::endl;
	// mavlink_position_target_local_ned_t tar_ned = autopilot_interface.position_target_local_ned;  
	//mavlink_position_target_global_int_t tar_gpsi = autopilot_interface.position_target_global_int;
	// mavlink_global_position_int_t pos_gpsi = autopilot_interface.global_position_int;

	// Example 1 - Set Velocity
	set_velocity(0.0, // [m/s]
		0.0, // [m/s]
		0.0, // [m/s]
		sp);

	// Example 2 - Set Position
	set_position(ip.x, // [m]
		ip.y, // [m]
		ip.z, // [m]
		sp);


	// Example 1.2 - Append Yaw Command
	set_yaw(ip.yaw, // [rad]
		sp);

	// SEND THE COMMAND
	autopilot_interface.update_setpoint(sp);

	Vector3D self(ip.x, ip.y, ip.z);
	Vector3D target(50, 1000, 100);
	//Vector3D target_gpsi(tar_gpsi.x, tar_gpsi.y, tar_gpsi.z);
	//Vector3D self_gpsi(pos_gpsi.x, pos_gpsi.y, pos_gpsi.z);
	// Network lead drone ************************* will need to change in future (see next comment)
	DroneInfo selfDroneInfo;
	if (SelfLevel == 1)
	{
		selfDroneInfo = DroneInfo(std::to_string(SelfID) + " " + std::to_string(ip.x) + " " + std::to_string(ip.y) + " " + std::to_string(ip.z) + " 50 " + std::to_string(0) + " " + std::to_string(NOT_SWAPPING), true);

		//selfDroneInfo.SetLocation(self);//(pos.x, pos.y,pos.z);
	}
	else if (SelfID == 2)
	{
		selfDroneInfo = DroneInfo(std::to_string(SelfID) + " " + std::to_string(ip.x) + " " + std::to_string(ip.y) + " " + std::to_string(ip.z) + " 75 " + std::to_string(0) + " " + std::to_string(NOT_SWAPPING), false);
	}
	else if (SelfID == 3)
	{
		selfDroneInfo = DroneInfo(std::to_string(SelfID) + " " + std::to_string(ip.x) + " " + std::to_string(ip.y) + " " + std::to_string(ip.z) + " 90 " + std::to_string(0) + " " + std::to_string(NOT_SWAPPING), false);
	}
	else
	{
		selfDroneInfo = DroneInfo(std::to_string(SelfID) + " " + std::to_string(ip.x) + " " + std::to_string(ip.y) + " " + std::to_string(ip.z) + " 100 " + std::to_string(0) + " " + std::to_string(NOT_SWAPPING), false);
	}

	DroneInfo leadDrone(selfDroneInfo); //Lead drone this needs to be looked into, this may need to be the lead drone in the tree
	Drone* selfDrone = new Drone(target, selfDroneInfo); //Initialize selfDrone which is this drone
	globalNodeTree = new NodeTree(NULL, NULL, NULL, NULL); //construct globalNodeTree
	selfDrone->SetIsMe(1); //set the isMe bit to 1 
	selfDrone->SetLevel(SelfLevel); //set the level of the drone to add to tree
	globalNodeTree->AddDrone(globalNodeTree, selfDrone); //Adds the drone to the tree

	//Initialization of trees all trees need to send and recieve from each other
	while (!AllDone(recievetreelist) || !AllDone(sendtreelist))
	{
		// set the socket address, each Remote drone gets a communication session
		for (int itr = 0; itr < RemoteIPs.size(); itr++)
		{
			Socket::SetSockaddrIn(&Socket::remoteAddr, RemoteIPs[itr].c_str(), Socket::PORTNUM); //sets the socket remote ip address
			//std::cout << "Send to: " << RemoteIPs[itr] << std::endl;

			//has this drone sent an acknowledge for this message(from past loop)
			if (sendtreelist.at(itr).isDone == 0)
			{
				std::string info = selfDrone->GetInfo().ToString(); //get your droneinfo string to send
				std::cout << "Sent Initialize Packet: " << info << std::endl;
				FileIO::WritePacket(&FileIO::sent_packet_file, info); // write the drone's sent information to the file
				Socket::SendMessage(info.c_str()); //send through socket
				sleep(1);
			}

			for (int n = 1; n < 3; n++)
			{
				DroneInfo recdroneinf; //for storing recieved information
				char* char_message = Socket::ReceiveMessage(); //get message
				std::string message = char_message ? char_message : "";
				if (message.length() > 2) //checks if message is something valid (shouldnt be smaller than 2)
				{
					FileIO::WritePacket(&FileIO::rec_packet_file, message);
				}

				//get correct message, shouldnt have a comma
				if (message.find(",") == std::string::npos)
				{
					//recieved acknowledge 1
					if (message.find(" ack1 ") != std::string::npos)
					{
						//get the id of the ack
						int ackid = 0, recid = 0;
						std::stringstream temp(message);
						temp >> recid;
						temp.ignore(1, ' '); //ignore the acknowledge part of the message, just get the id's
						temp.ignore(5, ' ');
						temp >> ackid;
						if (recid == SelfID) //was this ment for this drone?
						{
							for (int i = 0; i < sendtreelist.size(); i++)
							{
								if (sendtreelist.at(i).id == ackid) //check the drone off the send list since he replied an acknowledge
								{
									if (sendtreelist.at(i).isDone == 0)
									{
										std::cout << "Received Initialize Packet: " << message << std::endl;
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
					//makes sure message length is long enough to be valid and have DroneInfo
					else if (message.length() > 7 && message.find(" ack2 ") == std::string::npos)
					{
						recdroneinf = DroneInfo(message); //store information from the message using DroneInfo constructor

						//check the drone off the recieved tree list
						for (int i = 0; i < recievetreelist.size(); i++)
						{
							if (recievetreelist.at(i).id == recdroneinf.GetID())
							{
								//first time recieving for this drone during this messaging section
								if (recievetreelist.at(i).isDone == 0)
								{
									std::cout << "Received NEW Initialize Packet: " << message << std::endl;
									Drone* someDrone = new Drone(target, recdroneinf);//make a new drone to add to tree
									Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)).c_str());//send Acknowledge 1
									FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)); // write the drone sent data to file			  
									sleep(1);
									recievetreelist.at(i).isDone = 1; //this drone is checked off the list
									someDrone->SetLevel(recievetreelist.at(i).level); //set the level to put in the tree
									someDrone->SetIsMe(0); //set the isMe bit to 0 (since it is a remote drone)
									globalNodeTree->AddDrone(globalNodeTree, someDrone); //add to the tree
									break;
								}
								else
								{
									//already recieved, send acknowledgement 1 again (maybe can also update the drone in the tree by using function UpdateDrone)
									Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)).c_str());
									FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)); // write the drone sent data to file
									sleep(1);
									//drone was already added, break
									break;
								}
							}
						}
					}
					else {}; //ignore the message
				}
			}
		}
	}

	//reset the isDone bits of both lists
	for (int i = 0; i < recievetreelist.size(); i++)
	{
		recievetreelist.at(i).isDone = 0;
	}
	for (int i = 0; i < sendtreelist.size(); i++)
	{
		sendtreelist.at(i).isDone = 0;
	}

	// This section is used to initially organize the tree
	// However, it's probably not necessary since the tree will organize during flight
	/*
	Selftree = NodeTree(*globalNodeTree);//use copy constructor to save a current copy of globalNodeTree
	globalNodeTree->GroupPoll(globalNodeTree);//adjust globalNodeTree for battery differences
	action = globalNodeTree->CompareTrees(SelfID);*//*compare the NodeTree with other drone's NodeTree, if there is a tie among the determined
	NodeTree then action will be 0*/

	/*
	if(action == 1)//things are fine if there wasn't a tie
	{
	}
	else//there was a tie
	{
	globalNodeTree = &Selftree;//Don't adjust the tree, go back to the stored globalNodeTree
	}
	*/

	// Swap variables
	int lastswapflag = 0;//Initialize the lastswap flag
	int swapstoexecute = 0;//When swaps are in Queue, this flag prevents Queue from being overwritten
	int currentswap = 0;
	int listofswaps[RemoteIDs.size() * 2 * 2] = { 0 }; // List of swaps to be made. Four times the number of id's in case every drone swaps and extra drones are added
	bool swapinprogress = false;

	selfDrone = globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID)->GetDrone();//set this drone spot from the drone representing this drone in the tree

	// Timer to first swap
	// Gives the drones a chance to organize the formation
	time_t start = time(0);

	while (true)
	{
		// update self data
		std::cout << "Head drone is: " << globalNodeTree->GetDrone()->GetInfo().GetID() << std::endl;
		selfDrone->CalculateWaypoint(globalNodeTree->GetDrone()->GetInfo()); // calculate the drone's next waypoint
		selfDrone->Move(autopilot_interface); // move the drone
		// update tree information of all the drones by communicating this works exactly like the initialization while loop, but instead of initializing, it just updates the drone information for all the drones
		while (!AllDone(recievetreelist) || !AllDone(sendtreelist))
		{
			for (int itr = 0; itr < RemoteIPs.size(); itr++) // set the socket address
			{
				Socket::SetSockaddrIn(&Socket::remoteAddr, RemoteIPs[itr].c_str(), Socket::PORTNUM);
				if (sendtreelist.at(itr).isDone == 0)
				{
					// save and send current info
					//selfDrone=globalNodeTree->GetTreeSpotById(globalNodeTree,SelfID)->GetDrone();
					std::string info = selfDrone->GetInfo().ToString();
					std::cout << "Sent Update Packet: " << info << std::endl;
					FileIO::WritePacket(&FileIO::sent_packet_file, info); // write the drone's position to the file
					Socket::SendMessage(info.c_str());
					sleep(1);
				}
				for (int n = 1; n < 3; n++)
				{
					DroneInfo recdroneinf;
					// receive info and write to file
					char* char_message = Socket::ReceiveMessage();
					std::string message = char_message ? char_message : "";
					if (message.length() > 2)
					{
						FileIO::WritePacket(&FileIO::rec_packet_file, message);
					}

					//get correct message
					if (message.find(",") == std::string::npos)
					{
						//recieved acknowledge 1
						if (message.find(" ack1 ") != std::string::npos)
						{
							//get the id of the ack
							int ackid = 0, recid = 0;
							std::stringstream temp(message);
							temp >> recid;
							temp.ignore(1, ' ');
							temp.ignore(5, ' ');
							temp >> ackid;
							if (recid == SelfID)
							{
								for (int i = 0; i < sendtreelist.size(); i++)
								{
									if (sendtreelist.at(i).id == ackid)
									{
										if (sendtreelist.at(i).isDone == 0)
										{
											std::cout << "Received Update Packet: " << message << std::endl;
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
						//is this message drone information
						else if (message.find(" ack2 ") == std::string::npos)
						{
							recdroneinf = DroneInfo(message);

							for (int i = 0; i < recievetreelist.size(); i++)
							{
								if (recievetreelist.at(i).id == recdroneinf.GetID())
								{
									if (recievetreelist.at(i).isDone == 0)
									{
										std::cout << "Received New Drone Packet: " << message << std::endl;
										Drone* someDrone = new Drone(target, recdroneinf); //Make a drone for this information
										Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)).c_str()); //send Acknowledge 1
										FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)); // write the drone sent data to file
										sleep(1);
										recievetreelist.at(i).isDone = 1;
										someDrone->SetIsMe(0);
										globalNodeTree->UpdateDrone(globalNodeTree, someDrone); //Update this drone in the tree
										break;
									}
									else
									{
										Socket::SendMessage((std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)).c_str());//send acknowledge 1
										FileIO::WritePacket(&FileIO::sent_packet_file, std::to_string(recievetreelist.at(i).id) + " ack1 " + std::to_string(SelfID)); // write the drone sent data to file
										//drone was already updated, break
										break;
									}
								}
							}
						}

						// Create else if message for receiving a swap complete packet here

						else {}; //ignore message
					}
					// Set bits for missed acknowledgements
					else
					{
						sendtreelist.at(itr).isDone = 1;
						recievetreelist.at(itr).isDone = 1;
					}
				}
			}
		}

		//reset the isDone bits
		for (int i = 0; i < recievetreelist.size(); i++)
		{
			recievetreelist.at(i).isDone = 0;
		}
		for (int i = 0; i < sendtreelist.size(); i++)
		{
			sendtreelist.at(i).isDone = 0;
		}

		std::cout << "Network fitness: " << globalNodeTree->GetNetworkFitness() << std::endl;
		//Ranking::levelorderRank(globalNodeTree);

		// Find swaps if necessary
		if (swapstoexecute == 0)
		{
			// Reset list
			if (lastswapflag == 0)
			{
				memset(listofswaps, 0, RemoteIDs.size());
			}
			currentswap = 0;

			//checks if lowest battery is greater than 20%
			if (globalNodeTree->GetLowestBattery(globalNodeTree) > 20.0)
			{
				//do the same normal group decision making process
				Selftree = new NodeTree(*globalNodeTree); //save a copy
				globalNodeTree->GroupPoll(globalNodeTree);
				action = globalNodeTree->CompareTreesForSwaps(Selftree, SelfID, listofswaps);
				std::cout << "actions " << action << std::endl;

				if (action > 0)
				{
					swapstoexecute = action;
				}

				// Reset tree for actual swaps
				globalNodeTree = Selftree;
				globalNodeTree->UpdateLeadBits(globalNodeTree, true);
				selfDrone = globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID)->GetDrone();
			}
			else
			{
				if (lastswapflag == 0)
				{
					std::cout << "\n\n LAST SWAP \n\n";
					//last group decision making procedure
					Selftree = new NodeTree(*globalNodeTree);
					globalNodeTree->GroupPoll(globalNodeTree);
					action = globalNodeTree->CompareTreesForSwaps(Selftree, SelfID, listofswaps);

					if (action > 0)
					{
						swapstoexecute = action;
					}

					// Reset tree for actual swaps
					globalNodeTree = Selftree;
					globalNodeTree->UpdateLeadBits(globalNodeTree, true);
					selfDrone = globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID)->GetDrone();
					lastswapflag = 1;//set the last swap flag
				}
				else
				{
				}
			}
		}
		// Execute next swap
		else if (!swapinprogress && difftime(time(0), start) > 30)
		{
			std::cout << "Started Swapping" << std::endl;

			swapinprogress = true;

			// Code to execute next swap here
			// use currentswap, swapstoexecute, and listofswaps to do this

			// Rise out of formation
			if (selfDrone->GetInfo().GetID() == listofswaps[currentswap])
			{
				std::cout << "its an up" << std::endl;
				selfDrone->UpdateWaypointVertical(DRONE_VERTICAL_OFFSET);
				selfDrone->GetInfoPointer()->SetIsSwapping(true);
				selfDrone->GetInfoPointer()->SetSwappingState(STARTED_SWAPPING);
			}
			// Drop out of formation
			else if (selfDrone->GetInfo().GetID() == listofswaps[currentswap + 1])
			{
				std::cout << "its a down" << std::endl;
				selfDrone->UpdateWaypointVertical(-1 * DRONE_VERTICAL_OFFSET);
				selfDrone->GetInfoPointer()->SetIsSwapping(true);
				selfDrone->GetInfoPointer()->SetSwappingState(STARTED_SWAPPING);
			}
		}
		// Wait for swap to finish
		else if (swapinprogress && difftime(time(0), start) > 30)
		{
			std::cout << "Currently swapping" << std::endl;

			// Is this drone going above the formation?
			if (selfDrone->GetInfo().GetID() == listofswaps[currentswap])
			{
				// Check if high enough out of formation
				if (selfDrone->GetInfo().GetSwappingState() == STARTED_SWAPPING && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() >= STARTED_SWAPPING)
				{
					std::cout << "stage 1 up" << std::endl;

					// Check if waypoint reached
					float z = selfDrone->GetWaypoint().getZ();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), NULL, NULL, &z))
					{
						std::cout << "position reached" << std::endl;

						globalNodeTree->SwapNodes(globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap]), globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1]), false);
						globalNodeTree = globalNodeTree->GetRoot(globalNodeTree);
						globalNodeTree->UpdateLeadBits(globalNodeTree, true);
						selfDrone = globalNodeTree->GetTreeSpotById(globalNodeTree, selfDrone->GetInfo().GetID())->GetDrone();

						//globalNodeTree = globalNodeTree->GetRoot(globalNodeTree);
						selfDrone->GetInfoPointer()->SetSwappingState(POSITION_1);
					}
				}
				// Check if drone reached new x,y coordinate
				else if (selfDrone->GetInfo().GetSwappingState() == POSITION_1 && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() >= POSITION_1)
				{
					std::cout << "stage 2 up" << std::endl;

					// Check if waypoint is reached
					float x = selfDrone->GetWaypoint().getX();
					float y = selfDrone->GetWaypoint().getY();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), &x, &y, NULL))
					{
						selfDrone->GetInfoPointer()->SetSwappingState(POSITION_2);
						selfDrone->UpdateWaypointVertical(-1 * DRONE_VERTICAL_OFFSET);
					}
				}
				// Check if back in formation
				else if (selfDrone->GetInfo().GetSwappingState() == POSITION_2 && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() >= POSITION_2)
				{
					std::cout << "stage 3 up" << std::endl;

					// Check if waypoint reached
					float z = selfDrone->GetWaypoint().getZ();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), NULL, NULL, &z))
					{
						selfDrone->GetInfoPointer()->SetSwappingState(FINISHED_SWAPPING);
					}
				}
				// Check if done
				else if (selfDrone->GetInfo().GetSwappingState() == FINISHED_SWAPPING && (globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() >= FINISHED_SWAPPING || globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() == NOT_SWAPPING))
				{
					std::cout << "done up" << std::endl;

					selfDrone->GetInfoPointer()->SetIsSwapping(false);
					selfDrone->GetInfoPointer()->SetSwappingState(NOT_SWAPPING);
					swapstoexecute--;
					currentswap += 2;
					swapinprogress = false;
				}
			}
			// Is this drone going below the formation?
			else if (selfDrone->GetInfo().GetID() == listofswaps[currentswap + 1])
			{
				// Check if low enough out of formation
				if (selfDrone->GetInfo().GetSwappingState() == STARTED_SWAPPING && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() >= STARTED_SWAPPING)
				{
					std::cout << "stage 1 down" << std::endl;

					// Check if waypoint reached
					float z = selfDrone->GetWaypoint().getZ();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), NULL, NULL, &z))
					{
						globalNodeTree->SwapNodes(globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap]), globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1]), false);
						globalNodeTree = globalNodeTree->GetRoot(globalNodeTree);
						globalNodeTree->UpdateLeadBits(globalNodeTree, true);
						selfDrone = globalNodeTree->GetTreeSpotById(globalNodeTree, selfDrone->GetInfo().GetID())->GetDrone();

						selfDrone->GetInfoPointer()->SetSwappingState(POSITION_1);
					}
				}
				// Check if drone reached new x,y coordinate
				else if (selfDrone->GetInfo().GetSwappingState() == POSITION_1 && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() >= POSITION_1)
				{
					std::cout << "stage 2 down" << std::endl;

					// Check if waypoint is reached
					float x = selfDrone->GetWaypoint().getX();
					float y = selfDrone->GetWaypoint().getY();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), &x, &y, NULL))
					{
						selfDrone->GetInfoPointer()->SetSwappingState(POSITION_2);
						selfDrone->UpdateWaypointVertical(DRONE_VERTICAL_OFFSET);
					}
				}
				// Check if back in formation
				else if (selfDrone->GetInfo().GetSwappingState() == POSITION_2 && globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() >= POSITION_2)
				{
					std::cout << "stage 3 down" << std::endl;

					// Check if waypoint reached
					float z = selfDrone->GetWaypoint().getZ();

					if (globalNodeTree->IsDronePositionAcceptable(globalNodeTree->GetTreeSpotById(globalNodeTree, SelfID), NULL, NULL, &z))
					{
						selfDrone->GetInfoPointer()->SetSwappingState(FINISHED_SWAPPING);
					}
				}
				// Check if done
				else if (selfDrone->GetInfo().GetSwappingState() == FINISHED_SWAPPING && (globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() >= FINISHED_SWAPPING || globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() == NOT_SWAPPING))
				{
					std::cout << "done down" << std::endl;

					selfDrone->GetInfoPointer()->SetIsSwapping(false);
					selfDrone->GetInfoPointer()->SetSwappingState(NOT_SWAPPING);
					swapstoexecute--;
					currentswap += 2;
					swapinprogress = false;
				}
			}
			else
			{
				if ((globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() >= FINISHED_SWAPPING || globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap])->GetDrone()->GetInfo().GetSwappingState() == NOT_SWAPPING) && (globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() >= FINISHED_SWAPPING || globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1])->GetDrone()->GetInfo().GetSwappingState() == NOT_SWAPPING))
				{
					std::cout << "next swap" << std::endl;

					globalNodeTree->SwapNodes(globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap]), globalNodeTree->GetTreeSpotById(globalNodeTree, listofswaps[currentswap + 1]), false);
					globalNodeTree = globalNodeTree->GetRoot(globalNodeTree);
					//globalNodeTree->UpdateLeadBits(globalNodeTree, true);
					swapstoexecute--;
					currentswap += 2;
					swapinprogress = false;
				}
			}
		}
	}

	autopilot_interface.stop();
	serial_port.stop();
	return 0;
}
