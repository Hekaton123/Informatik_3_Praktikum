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
#include <time.h>


#include "SIMPLESOCKET.H"

using namespace std;

class myTCPclient : public TCPclient{
public:
	myTCPclient();
	int getSteps();
	int EveryField();
	int Random();
	int Sec();
	int Dia();


protected:

private:
	int last_X;
	int last_Y;
	int ** ocean;                                //Spielfeld des Clients
	string processSec(string message);           //Arbeitet mit den Antworten des Servers
	void nextSECStep();                          //Sucht das nächste noch nicht betrachtete Feld in zweier Schritten
	void nextSECStepAH();                        //Sucht nachdem ein Schiff zerstört wurde, das nächste noch nicht betrachtete Feld in zweier Schritten ausgehend vom ersten Treffer an dem Schiff
	bool used(int x, int y);                     //Überprüft ob die Stell (X/Y) schon betrachtet wurde
	int steps;                                   //Zählt die Anzahl der benötigten Züge
	int max_X;
	int max_Y;
	int direction;                               //Beschreibt die Ausrichtung des Schiffs, wenn ein treffer erzielt wurde
	void findShip();                             //Sucht die nächste mögliche Stelle, wenn nach einem treffer wieder das Wasser getroffen worden ist
    bool hasNeighbor();                          //Überprüft, ob das Schiff bereits einen treffer hat
    void destroy();
    int lastHit_X;
    int lastHit_Y;
    void reset();
};

myTCPclient::myTCPclient(){
	last_X = 0;
	last_Y = 0;
	max_X = 10;
	max_Y = 10;
	steps = 0;
	ocean = new int*[max_X];
	for(int i = 0; i < max_X; i++){
		ocean[i] = new int[max_Y];
	}
	direction = 0;
	lastHit_X = 0;
	lastHit_Y = 0;
}

int myTCPclient::getSteps(){
	return steps;
}

void myTCPclient::reset(){
	for(int y = 0; y < max_Y; y++){
		for(int x = 0; x < max_X; x++){
			ocean[x][y] = 0;
		}
	}
	last_X = 0;
	last_Y = 0;
	steps = 0;
	direction = 0;
	lastHit_X = 0;
	lastHit_Y = 0;
}

bool myTCPclient::used(int x, int y){
	if(x >= 0 && x < max_X && y >= 0 && y < max_Y){
	return !ocean[x][y] == 0;
	}
	return false;
}

bool myTCPclient::hasNeighbor(){
	bool tmp = false;

	int x = last_X;
	int y = last_Y;

	switch(direction){
			case 1:
				if((y + 2) < max_Y && ocean[x][y + 2] == 2){
					tmp = true;
				}
				break;
			case 2:
				if((x + 2) < max_X && ocean[x + 2][y] == 2){
					tmp = true;
				}
				break;
			case 3:
				if((y - 2) >= 0 && ocean[x][y - 2] == 2){
					tmp = true;
				}
				break;
			case 4:
				direction = 2;
				if((x - 2) >= 0 && ocean[x - 2][y] == 2){
					tmp = true;
				}
				break;
			default:
				break;
			}

	return tmp;
}

void myTCPclient::nextSECStep(){

	int x = last_X;
	int y = last_Y;

	while(used(x, y)){
		if((x + 2) < max_X){
				x = x + 2;
		}
		else if((y + 1) < max_Y){
			x = (max_X + 1) - (x + 2);
			if(x < 0){
				x = x * (-1);
			}
			y++;
		}
	}

		last_X = x;
		last_Y = y;
}

void myTCPclient::nextSECStepAH(){

	int x = lastHit_X;
	int y = lastHit_Y;

	while(used(x, y)){
		if((x + 2) < max_X){
			x = x + 2;
		}
		else if((y + 1) < max_Y){
			x = (max_X + 1) - (x + 2);
			if(x < 0){
				x = x * (-1);
			}
			y++;
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
			while((y + 1) < max_Y && used(x, y)){
				y++;
			}
			break;
		case 2:
			direction = 4;
			while((x + 1) < max_X && used(x, y)){
				x++;
			}
			break;
		case 3:
			direction = 1;
			while((y - 1) >= 0 && used(x, y)){
				y--;
			}
			break;
		case 4:
			direction = 2;
			while((x - 1) >= 0 && used(x, y)){
				x--;
			}
			break;
		default:
			break;
		}
	}
	else{

		switch(direction){         //X und Y auf Position des Treffers setzen
		case 1:
			y++;
		  break;
		case 2:
			x++;
		  break;
		case 3:
			y--;
		  break;
		case 4:
			x--;
		  break;
		default:
		  break;

		}


		bool possible = false;

		if(direction == 4){
			direction = 1;
		}
		else{
			direction++;
		}

		while(direction <= 4 && !possible){
				switch(direction){
				case 1:
					if(!used(x, y - 1) && (y - 1) >= 0){
						y--;
						possible = true;

					}
					else{
						direction++;
					}
					break;
				case 2:
					if(!used(x - 1, y) && (x - 1) >= 0){
						x--;
						possible = true;

					}
					else{
						direction++;
					}
					break;
				case 3:
					if(!used(x, y + 1) && (y + 1) < max_Y){
						y++;
						possible = true;

					}
					else{
						direction++;
					}
					break;
				case 4:
					if(!used(x + 1, y) && (x + 1) < max_X){
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


	if(direction == 0){                                  //Ausrichtung des Schiffs noch unbekannt
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
		        lastHit_X = last_X;
		        lastHit_Y = last_Y;

	 }
		    else if(direction == 1){                            //Ausrichtung nach Oben
		    	if(!used(x, y - 1) && (y - 1) >= 0){
		    		y--;
		        }
		    	else{
		    		direction = 3;
		    		while(used(x, y)){
		    			y++;
		    		}
		    	}
		    }
		    else if(direction == 2){                            //Ausrichtung nach Links
		    	if(!used(x - 1, y) && (x - 1) >= 0){
		    		x--;
		    	}
		    	else{
		    		direction = 4;
		    		while(used(x, y)){
		    			x++;
		    		}
		    	}
		    }
		    else if(direction == 3){                            //Ausrichtung nach Unten
		    	if(!used(x, y + 1) && (y + 1) < max_Y){
		    	   	y++;
		    	}
		    	else{
		    		direction = 1;
		    		while(used(x, y)){
		    		   y--;
		    		}
		    	}
		    }
		    else if(direction == 4){                            //Ausrichtung nach Rechts
		    	if(!used(x + 1, y) && (x + 1) < max_X){
		    	    x++;
		    	}
		    	else{
		    		direction = 2;
		    		while(used(x, y)){
		    			x--;
		    		}

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
		nextSECStepAH();

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

int myTCPclient::EveryField(){
	int count = 0;

	string msg;

	msg = "new game";
	sendData(msg);
	sleep(0.3);

	for(int y = 0; y < 10; y++){
		for(int x = 0; x < 10; x++){
			ostringstream tmp;

			tmp << "shoot " << x + 1 << " " << y + 1;
			msg = tmp.str();
			sendData(msg);
			sleep(0.3);
			count++;

			//cout << "Sends: " << msg << endl;
			msg = receive(32);
			sleep(0.3);
			//cout << "Response: " << msg << endl;

			if(msg.compare(0, 19, "ALL_SHIPS_DESTROYED") == 0 || msg.compare(0, 9, "GAME_OVER") == 0){
				y = 10;
				x = 10;
				//sendData("BYEBYE");
				//msg = receive(32);
			}
		}
	}

	return count;
}

int myTCPclient::Random(){
	reset();
	srand(time(NULL));

	string msg;

	msg = "new game";
	sendData(msg);
	sleep(0.3);

	int count = 0;
	bool finish = false;

	int x;
	int y;

	do{
		ostringstream tmp;

	    x = rand() % 10;
	    y = rand() % 10;

	    if(!used(x, y)){
	        tmp << "shoot " << x + 1 << " " << y + 1;
	    	msg = tmp.str();
	    	sendData(msg);
	    	sleep(0.3);
	    	count++;
	    	ocean[x][y] = 1;
            //cout << "Sends: " << msg << endl;

	    	msg = receive(32);
	    	sleep(0.3);
	    	//cout << "Response: " << msg << endl;
	    }
        if(msg.compare(0, 19, "ALL_SHIPS_DESTROYED") == 0 || msg.compare(0, 9, "GAME_OVER") == 0){
        	//sendData("BYEBYE");
        	//msg = receive(32);
			finish = true;
		}
	} while (!finish);

	return count;
}

int myTCPclient::Sec(){
	reset();
	steps = 0;

	bool running = false;
	string res;
	string msg;

	msg = "new game";
	sendData(msg);
	msg = receive(32);
	sleep(0.3);

	ostringstream tmp;
	tmp << "shoot " << 1 << " " << 1;
	running = sendData(tmp.str());
	msg = receive(32);


	do{
		res = processSec(msg);
		running = sendData(res);
		//cout << "Sends: " << res << endl;
		sleep(0.5);
	    msg = receive(32);
	    //cout << "Response: " << msg << endl;
	    sleep(0.5);

	    if(msg.compare(0, 19, "ALL_SHIPS_DESTROYED") == 0 || msg.compare(0, 9, "GAME_OVER") == 0){
	    	//sendData("BYEBYE");
	    	//msg = receive(32);
	    	running = false;
	    }
	} while(running);



	return steps;
}

int main() {
	myTCPclient c;
	string host = "localhost";
	string msg;
	string res;
	int count;

	//connect to host
	c.conn(host , 2015);

	for(int i = 0; i < 50; i++){
		count = c.EveryField();
		cout << count << endl;
	}

}

