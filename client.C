/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

#include <string>
#include <iostream>
#include <math.h>
#include <unistd.h> //contains various constants
#include <sstream>


#include "SIMPLESOCKET.H"

using namespace std;

class myTCPclient : public TCPclient{
public:
	myTCPclient();
	string processSec(string message);              //Arbeitet mit den Antworten des Servers
	string processDia(string message);              //Arbeitet mit den Antworten des Servers
	int getSteps();


protected:

private:
	int last_X;
	int last_Y;
	int ** ocean;                                //Spielfeld des Clients
	void nextSECStep();                          //Sucht das nächste noch nicht betrachtete Feld in zweier Schritten
	void nextDIAStep();                          //Sucht das nächste noch nicht betrachtete Feld in einer festgelegten Diagonalen
	bool used(int x, int y);                     //Überprüft ob die Stell (X/Y) schon betrachtet wurde
	int steps;                                   //Zählt die Anzahl der benötigten Züge
	int max_X;
	int max_Y;
	int direction;                               //Beschreibt die Ausrichtung des Schiffs, wenn ein treffer erzielt wurde
	int getlX();
	int getlY();
	int getmX();
	int getmY();
	void findShip();                             //Sucht die nächste mögliche Stelle, wenn nach einem treffer wieder das Wasser getroffen worden ist
    bool hasNeighbor();                          //Überprüft, ob das Schiff bereits einen treffer hat
    void destroy();
    //int phase;                                   //Abstand Schritte, bei der diagonalen Variante
};

myTCPclient::myTCPclient(){
	last_X = 0;
	last_Y = 0;
	max_X = 10;
	max_Y = 10;
	steps = 0;
	ocean = (int**) new int[max_X];
	for(int i = 0; i < max_X; i++){
		ocean[i] = new int[max_Y];
	}
	direction = 0;
	//phase = 0;
}

int myTCPclient::getSteps(){
	return steps;
}

int myTCPclient::getlX(){
	return last_X;
}

int myTCPclient::getlY(){
	return last_Y;
}

int myTCPclient::getmX(){
	return max_X;
}

int myTCPclient::getmY(){
	return max_Y;
}

bool myTCPclient::used(int x, int y){
	return !ocean[x][y] == 0;
}

bool myTCPclient::hasNeighbor(){
	bool tmp = false;
	switch(direction){
			case 1:
				if(ocean[last_X][last_Y + 2] == 2){
					tmp = true;
				}
				break;
			case 2:
				if(ocean[last_X + 2][last_Y] == 2){
					tmp = true;
				}
				break;
			case 3:
				if(ocean[last_X][last_Y - 2] == 2){
						tmp = true;
				}
				break;
			case 4:
				direction = 2;
				if(ocean[last_X - 2][last_Y] == 2){
						tmp = true;
				}
				break;
			default:
				break;
			}
	/*if(ocean[last_X][last_Y - 1] == 2 || ocean[last_X - 1][last_Y] == 2 || ocean[last_X][last_Y + 1] == 2 || ocean[last_X + 1][last_Y] == 2){
		tmp = true;
	}*/https://fh-bielefeld.zoom.us/j/98276110795?pwd=clIwaUJIQUxuMTdQSGdibHlEaTZmQT09
	return tmp;
}

void myTCPclient::nextSECStep(){
	bool a = false;
	int x = 0;
	int y = 0;

	if(steps > 0){
		for(int i = 0; i < max_Y; i++){
				for(int j = 0; j < max_X; j++){
					if(!used(max_X, max_Y)){
						if(!a){
							a = true;
						}
						else{
							x = j;
							y = i;
							i = max_X;
							j = max_Y;
						}
					}
					else{
						a = false;
					}
				}
			}
	}


	last_X = x;
	last_Y = y;
}

void myTCPclient::nextDIAStep(){
	    bool a = false;
		int x = 0;
		int y = 0;
		int count = 0;

		for(int i = 0; i < max_Y; i++){
			for(int j = 0; j < max_X; j++){
				if(!used(max_X, max_Y)){
					if(!a){
						count++;
					}
					else if(count == 4){
						x = j;
					    y = i;
						i = max_X;
						j = max_Y;
					}
				}
				else if(count == 4 && used(max_X, max_Y)){
					a = false;
				}
				else{
					a = false;
				}
			}
		}
		last_X = x;
		last_Y = y;
}

void myTCPclient::findShip(){
	int x = last_X;
	int y = last_Y;
	if(hasNeighbor()){
		switch(direction){
		case 1:
			direction = 3;
			while(used(x, y)){
				y++;
			}
			y++;
			break;
		case 2:
			direction = 4;
			while(used(x, y)){
				x--;
			}
			x--;
			break;
		case 3:
			direction = 1;
			while(used(x, y)){
				y--;
			}
			y--;
			break;
		case 4:
			direction = 2;
			while(used(x, y)){
				x++;
			}
			x++;
			break;
		default:
			break;
		}
	}
	else{

		bool possible = false;

		while(direction < 4 && !possible){
				direction++;
				switch(direction){
				case 1:
					if(!used(last_X, last_Y - 1) && last_Y >= 0){
						y--;
						possible = true;
					}
					else{
						direction++;
					}
					break;
				case 2:
					if(!used(last_X - 1, last_Y) && last_X >= 0){
						x--;
						possible = true;
					}
					else{
						direction++;
					}
					break;
				case 3:
					if(!used(last_X, last_Y + 1) && last_Y < max_Y){
						y++;
						possible = true;
					}
					else{
						direction++;
					}
					break;
				case 4:
					if(!used(last_X + 1, last_Y) && last_X < max_X){
						x++;
						possible = true;
					}
					else{
						direction = 1;
					}
					break;
				default:
					break;
				}
			}

	}

	last_X = x;
	last_Y = y;
}


void myTCPclient::destroy(){
	int x = last_X;
	int y = last_Y;


	if(direction == 0){                                  //Ausrichtung des Schiffs unbekannt
		        if(!used(x, last_Y - 1) && (y - 1) >= 0){
		    		direction = 1;
		    		y--;
		    	}
		    	else if(!used(x - 1, y) && (x - 1) >= 0){
		    		direction = 2;
		    	    x--;
		    	}
		    	else if(!used(x, y + 1) && (y + 1) < max_Y){
		    		direction = 3;
		    	    y++;
		    	}
		    	else if(!used(x + 1, y) && (x + 1) < max_X){
		 	        direction = 4;
		            x++;
		    }
	 }
		    else if(direction == 1){                            //Ausrichtung nach Oben
		    	if(!used(x, y - 1) && (y - 1) >= 0){
		    		y--;
		        }
		    }
		    else if(direction == 2){                            //Ausrichtung nach Links
		    	if(!used(x - 1, y) && (x - 1) >= 0){
		    	    x--;
		    	}
		    }
		    else if(direction == 3){                            //Ausrichtung nach Unten
		    	if(!used(x, y + 1) && (y + 1) < max_Y){
		    	   	y++;
		    	}
		    }
		    else if(direction == 4){                            //Ausrichtung nach Rechts
		    	if(!used(x + 1, y) && (x + 1) < max_X){
		    	    x++;
		    	}
		    }

	last_X = x;
	last_Y = y;

}

string myTCPclient::processSec(string message){
	ostringstream tmp;

	if(message.compare(0, 5, "WATER") == 0){
		ocean[last_X][last_Y] = 1;
		if(direction == 0){
			nextSECStep();
		}
		else{
			findShip();
		}

		steps++;
		tmp << "shoot " << last_X + 1 << " " << last_Y + 1;
	}
	else if(message.compare(0, 8, "SHIP_HIT") == 0){
		ocean[last_X][last_Y] = 2;
		steps++;
	    destroy();

	    tmp << "shoot " << last_X + 1 << " " << last_Y + 1;

	}
	else if(message.compare(0, 14, "SHIP_DESTROYED") == 0){
		ocean[last_X][last_Y] = 2;
		direction = 0;
		steps++;
		nextSECStep();

		tmp << "shoot " << last_X + 1 << " " << last_Y + 1;
	}
	else if(message.compare(0, 19, "ALL_SHIPS_DESTROYED") == 0){
		tmp << "BYEBYE";

	}
	else if(message.compare(0, 16, "Created new Game") == 0){
		tmp << "shoot " << 1 << " " << 1;

	}
	else if(message.compare(0, 9, "GAME_OVER") == 0){
		tmp << "BYEBYE";

	}
	else{
		tmp << "Not found";

	}

	string response = tmp.str();

	return response;
}

string myTCPclient::processDia(string message){
	ostringstream tmp;

	if(message.compare(0, 5, "WATER") == 0){
		ocean[last_X][last_Y] = 1;
		if(direction == 0){
			nextDIAStep();
		}
		else{
			findShip();
		}

		steps++;
		tmp << "shoot " << last_X + 1 << " " << last_Y + 1;
	}
	else if(message.compare(0, 8, "SHIP_HIT") == 0){
		ocean[last_X][last_Y] = 2;
		steps++;
	    destroy();

	    tmp << "shoot " << last_X + 1 << " " << last_Y + 1;

	}
	else if(message.compare(0, 14, "SHIP_DESTROYED") == 0){
		ocean[last_X][last_Y] = 2;
		direction = 0;
		steps++;
		nextDIAStep();

		tmp << "shoot " << last_X + 1 << " " << last_Y + 1;
	}
	else if(message.compare(0, 19, "ALL_SHIPS_DESTROYED") == 0){
		tmp << "BYEBYE";

	}
	else if(message.compare(0, 16, "Created new Game") == 0){
		tmp << "shoot " << 1 << " " << 1;

	}
	else if(message.compare(0, 9, "GAME_OVER") == 0){
		tmp << "BYEBYE";

	}
	else{
		tmp << "Not found";

	}

	string response = tmp.str();

	return response;
}

int main() {
	myTCPclient c;
	string host = "localhost";
	string msg;
	string res;
	int count;

	//connect to host
	c.conn(host , 2025);
	msg = "new game";
	c.sendData(msg);
	sleep(1);


	//Auführung  Client
	bool goOn = true;

	while(goOn){
		msg = c.receive(32);
		cout << "Response: " << msg << endl;
		res = c.processSec(msg);
		goOn = c.sendData(res);
		cout << "Sends: " << res << endl;
		if(res.compare(0, 6, "BYEBYE") == 0){
			goOn = false;
		}
		sleep(1);
	}
	count = c.getSteps();



}

