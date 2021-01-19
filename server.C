/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.H"
#include "TASK3.C"

class myTCPserver : public TCPserver{
public:
	myTCPserver(int port, int maxDataSizeRecv) : TCPserver(port, maxDataSizeRecv){;}


protected:
	string myResponse(string input);

private:
	TASK3::World ocean;
	string hit(int x, int y);
	void game();

};


string myTCPserver::myResponse(string input){
	if(input.compare(0, 8, "new game") == 0){
		//ocean = new TASK3::World();
		game();
		return string("Created new Game\n");
	}
	else if(input.compare(0, 5, "shoot") == 0){
		int x, y;
		sscanf(input.c_str(), "shoot %i %i", &x, &y);
		return hit(x, y);
	}
	else if(input.compare(0, 8, "Not found") == 0){
		//ocean = new World();
		return string("Wait for new command\n");
	}
	else{
	return string("Command not found\n");
	}
}

using namespace TASK3;

string myTCPserver::hit(int x, int y){
	ShootResult tmp = ocean.shoot(x, y);
	game();
	switch(tmp){
	case WATER:
		return "WATER\n";
		break;
	case SHIP_HIT:
		return "SHIP_HIT\n";
		break;
	case SHIP_DESTROYED:
		return "SHIP_DESTROYED\n";
		break;
	case ALL_SHIPS_DESTROYED:
		return "ALL_SHIPS_DESTROYED\n";
			break;
	case GAME_OVER:
		return "GAME_OVER\n";
		break;
	default:
		return "Not Found\n";
	}
}


void myTCPserver::game(){
	ocean.printBoard();
}


int main(){
	srand(time(nullptr));
	myTCPserver srv(2025,25);
	srv.run();
}


