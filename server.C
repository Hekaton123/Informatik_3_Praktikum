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
	/**
	 * \brief Konstruktor
	 *
	 * \param port int Nummer des Clients
	 *
	 * \param maxDataSizeRecv int maximale Größe der Nachricht, die vom Server Empfangen werden kann
	 */
	myTCPserver(int port, int maxDataSizeRecv) : TCPserver(port, maxDataSizeRecv){;}

protected:


private:
	/**
	 * \brief ocean dient als Pointer zu einem Objekt der Klasse World.
	 */
	TASK3::World *ocean;
	/**
	 * \brief Antwortet dem Client auf eine Nachricht.
	 *
	 * \param input string enthält die Nachricht des Clients.
	 *
	 * \return string enthält die Antwort des Servers.
	 */
	string myResponse(string input);
	/**
	 * \brief Ausführung eines Schusses.
	 *
	 * \param x int ist X-Koordinate.
	 *
	 * \param y int ist Y-Koordinate.
	 *
	 * \return string gibt an, was getroffen wurde.
	 */
	string hit(int x, int y);
	/**
	 * \brief führt die print-Methode der Klasse World aus.
	 */
	void game();

};


string myTCPserver::myResponse(string input){
	if(input.compare(0, 8, "new game") == 0){
		delete ocean;
		ocean = new TASK3::World();
		game();
		return string("Created new Game\n");
	}
	else if(input.compare(0, 5, "shoot") == 0){
		int x, y;
		sscanf(input.c_str(), "shoot %i %i", &x, &y);
		return hit(x, y);
	}
	else{
	return string("Command not found\n");
	}
}

using namespace TASK3;

string myTCPserver::hit(int x, int y){
	if(x > 10 || y > 10){
		return "Not on field";
	}
	ShootResult tmp = ocean->shoot(x, y);
	game();
	switch(tmp){                                 //Dient zum "umschreiben" von shootResult in string
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
	ocean->printBoard();
}


int main(){
	srand(time(nullptr));
	myTCPserver srv(2015,25);
	srv.run();
}


