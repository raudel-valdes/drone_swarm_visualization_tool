/************************************************************************************
Project DroneSwarm
************************************************************************************/
Table of Contents:
  I:   Download Project
  II:  Build Project
  III: Parameter Setup
  IV:  Run with BATMAN
  V:   Run drone as dummyDrone
  VI:  MAVLink/MAVProxy Setup and Test
	
/************************************************************************************
I: Download
************************************************************************************/
  1) Open Terminal
  2) Change Directory (cd) to Desired File Location
  3) git clone --recursive https://[user]@bitbucket.org/CECSDroneswarm/cecsdroneswarmfall2016.git
  4) Input Password
  5) Verify Success

/************************************************************************************
II: Build:
************************************************************************************/
  Note: A makefile is included with the project to facilitate the compiling of the 
	necessary project files. All build files are contained within the "./build"
	folder. Object files are generated and stored in "./build/obj/". The "drones"
	executable is located in "./build". All files should build without Warnings
	or Errors.

  1) From Terminal Change Directory to DroneSwarm Root Folder
  2) Type: make
	Note: 

/************************************************************************************
III: Parameter Setup:
************************************************************************************/
  Note: Before DroneSwarm can be ran a few parametes must be edited. You will need
	the desired IP address of your drone for the BATMAN ADHOC network. This
	address is used to set-up the local BATMAN network and to connect to other
	drones on the network.
 
  1) Via Terminal or Desktop Navigate to DroneSwarm Root Folder
  2) Open droneSwarm.sh in a Text Editor
  3) Under "Drone Parameters" Edit DRONE_IP to Match the Desired Address.
	Example: "sudo export DRONE_IP=192.168.2.2/16"
  4) Save and Exit File
  5) Repeat Steps 2-4 for drone.sh

  *Legacy: Must also edit ./params/ip.txt and droneID.txt. Each line in the
	   documents represents a single drone, and the same drone must occupy the
	   same line in each document. 
		Example: Drone 1 -> ip.txt(Line 1) & droneID.txt(Line 1)
			 Drone 2 -> ip.txt(Line 2) & droneID.txt(Line 2)
			 .
			 .
			 .
			 Drone N -> ip.txt(Line N) & droneID.txt(Line N)

	   ip.txt holds the ip addresses for all the drones
		example input: 192.168.2.1

	   droneID.txt:holds the ID and Level of each drone
		example input: 1 1

	1) Edit ip.txt Line 1 with your DRONE_IP
	2) Edit droneID.txt Line 1 with your DRONE_ID space Drone Level
	3) Edit ip.txt Line 2 with Drone 2 IP
	4) Edit droneID.txt Line 2 with Drone 2 ID and level
	5) Repeat Steps 3 and 4 for Remaining Drones

/************************************************************************************
IV: Run with BATMAN Protocol:
************************************************************************************/
  NOTE: The droneSwarm.sh file is used to setup the ADHOC network and execute 
	"./build/drones".  The ADHOC setup and connection takes approximatley 30
	seconds to complete. The BATMAN ADHOC connection can be verified from another
	terminal with the ifconfig command.

  1) From Terminal Navigate to DroneSwarm Root Folder
  2) Execute by typing: ./droneSwarm.sh

/************************************************************************************
V:  Run drone as dummyDrone:
************************************************************************************/
  1) From Terminal Navigate to DroneSwarm Root Folder
  2) git checkout dummyDrone
	NOTE: If changes have been made to current branch commit them or use git stash
	      to stash changes on stack.  dummyDrone does not connect to Pixhawk.

/************************************************************************************
VI:  MAVLink/MAVProxy Setup and Test:
************************************************************************************/
A) MavLink Getting Started:

  Reference: https://ardupilot.github.io/MAVProxy/html/getting_started/index.html

  Download/Install for Pi3:

  1) Install Dependency Packages:

    sudo apt-get install /
  	python-dev /
  	python-opencv /
	python-wxgtk3.0 /
	python-pip /
	python-matplotlib /
	python-pygame /
	python-lxml /
	libxml2-dev

  2) Install MAVProxy:

    sudo pip install MAVProxy

  3) Verify MAVLink is on sysytem PATH:

    echo "export PATH=$PATH:$HOME/.local/bin" >> ~/.bashrc

B) MAVProxy PI3 Config

  1) Disable the OS control of the serial port
	Use the Raspberry Pi configuration utility for this.

	a) Type: sudo raspi-config
	b) Select “Advanced Options”
	c) Select “Serial” to disable OS use of the serial connection
	d) Reboot the Raspberry Pi

  2) edit /boot/config.txt
	a) sudo nano /boot/config.txt
	b) scroll to end of document and create new line
	c) insert: enable_uart=1
	d) save and exit

C) MAVProxy PI3 Connection

  NOTE: TCP/IP Connection is used here to communicate with the Connected Pixhawk 
        and Mission Planner.

  mavproxy.py --master=/dev/ttyS0 --baudrate 57600 --out <M-PlannerPC_IP>:14550 
  --aircraft drone1

 Example: mavproxy.py --master=/dev/ttyS0 --baudrate 57600 --out 192.168.1.139:14550 
	  --aircraft drone1

/************************************************************************************
VII: 
************************************************************************************/