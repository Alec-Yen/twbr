#include <iostream>
#include "RoboClaw.cpp"


using namespace std;
//#define serial 
#define address 0x80;
//rc = Roboclaw("/dev/serial0", 38400)

int main(){
	RoboClaw robo("/dev/serial0",38400);
	while(1) robo.ForwardM1(address, 60);
	return 0;
}
