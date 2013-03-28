#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <../rotation.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <Fl/Fl_Check_Button.H>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define MIDDLEX 475
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

	WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

struct Pair{
	int x;
	int y;
	Pair(int x1, int y1) {
		x = x1;
		y = y1;
	}
};
struct Command{
	DWORD time;
	char* cmd;
	int size;
	Command(char* c, int s, int delay)
	{
		size = s;
		cmd = new char[size];
		time = clock() + delay;
		for(int i=0; i<size; i++)
		{
			cmd[i] = c[i];
		}
	}
};
int angle1 = 0;
int angle2 = 0;
int angle3 = 0;
bool painting = false;
bool delay = false;
bool moving = false;

std::vector<Pair> pairs;
std::vector<Command*> command;

Lines* arms;

void SendCommand(char* cmd)
{
	
	//char *test = "test";
	//printf("%d\n", DHP[7]);
	iSendResult = send( ClientSocket, cmd, (int)strlen(cmd), 0 );
	printf("Sent %d bytes\n", iSendResult);
	//printf("SOCKET ERROR = %d\n", SOCKET_ERROR);
}
void sendToClient(){
	std::stringstream ss;
	for(int j=0; j<4; j++)
	{
		//char* temp=(char*)(&);
		ss << arms->lines[j]->dh->theta;
		ss << " ";
	}
	if (painting) ss<<'1'<<" ";
	else if (!painting) ss<<'0'<<" ";
	std::cout << ss.str();
	//SendCommand((char*)ss.str().c_str());
	
	
	Command* c;
	if(delay)
		c = new Command((char*)(ss.str().c_str()), (int)strlen(ss.str().c_str()), 2000);
	else 
		c = new Command((char*)(ss.str().c_str()), (int)strlen(ss.str().c_str()), 0);
	//std::cout << clock() << std::endl;
	command.push_back(c);
}

void Drawing::draw()
{
	fl_color(FL_WHITE);
	//draw_line(XX+0,0,0,150);
	for (int i=0; i<pairs.size();i++){
		fl_pie(pairs[i].x, pairs[i].y, 10, 10, 0, 360);
	}


	
	draw_line(arms->lines[1]->x1, arms->lines[1]->y1, arms->lines[1]->x2, arms->lines[1]->y2);
	fl_color(FL_WHITE);
	//draw_line(XX+0,150,0,100);
	draw_line(arms->lines[2]->x1, arms->lines[2]->y1, arms->lines[2]->x2, arms->lines[2]->y2);
	fl_color(FL_WHITE);
	//draw_line(XX+0,250,0,75);
	draw_line(arms->lines[3]->x1, arms->lines[3]->y1, arms->lines[3]->x2, arms->lines[3]->y2);


	fl_color(255, 255, 255);
	//int x1 = (int)arms->lines[0]->x1, BY-arms->lines[0]->y1;
	//int y1 = y()-5;
	fl_pie(BX+arms->lines[1]->x1-5, BY-arms->lines[1]->y1-5, 10, 10, 0, 360);

	//int x1 = x()-5;
	//int y1 = y()-5;
	fl_pie(BX+arms->lines[2]->x1-5, BY-arms->lines[2]->y1-5, 10, 10, 0, 360);

	//int x1 = x()-5;
	//int y1 = y()-5;
	fl_pie(BX+arms->lines[3]->x1-5, BY-arms->lines[3]->y1-5, 10, 10, 0, 360);
	//fl_arc(point(0).x,point(0).y,r+r,r+r,0,360);
	//fl_arc(100, 100, 200, 200, 0, 360);
	//fl_pie(0,0,200,200,0,360);

	if(painting)
		fl_pie(BX+arms->lines[3]->x2-10, BY-arms->lines[3]->y2-10, 20, 20, 0, 360);
	else
		fl_pie(BX+arms->lines[3]->x2-5, BY-arms->lines[3]->y2-5, 10, 10, 0, 360);

	
	fl_end_line();
}


class FillBoxBackground : public Fl_Widget {
public:
	FillBoxBackground(int x, int y, int W, int H) : Fl_Widget(x, y, W, H,0) { }

	void draw() {
		fl_color(64, 79, 89);
		int w1 = w();
		int h1 = h();
		int x1 = x();
		int y1 = y();
		//fl_rectf(x1, y1, w1, h1);
	}
	int handle(int event)
	{
		switch(event)
		{
		case FL_PUSH:
		case FL_DRAG:
			int x=Fl::event_x()-BX;
			int y=BY-Fl::event_y();
			printf("pos : %d %d\n", x, y);
			if(y>=0)
				arms->movetoPt(x,y);
		}
		return 1;
	}
};

class CreateBase : public Fl_Widget {
public:
	CreateBase(int x, int y, int W, int H) : Fl_Widget(x, y, W, H,0) { }

	void draw() {
		
		fl_color(255, 255, 255);
		int w1 = w();
		int h1 = h();
		int x1 = x();
		int y1 = y();
		fl_rectf(x1, y1, w1, h1);
	}
};

class CreateCircles : public Fl_Widget {
public:
	CreateCircles() : Fl_Widget(0, 0, 0, 0, 0) { }

	void draw() {
		//BX+arms->lines[0]->x1

		fl_color(255, 255, 255);
		//int x1 = (int)arms->lines[0]->x1, BY-arms->lines[0]->y1;
		//int y1 = y()-5;
		fl_pie(BX+arms->lines[1]->x1-5, BY-arms->lines[1]->y1-5, 10, 10, 0, 360);

		//int x1 = x()-5;
		//int y1 = y()-5;
		fl_pie(BX+arms->lines[2]->x1-5, BY-arms->lines[2]->y1-5, 10, 10, 0, 360);

		//int x1 = x()-5;
		//int y1 = y()-5;
		fl_pie(BX+arms->lines[3]->x1-5, BY-arms->lines[3]->y1-5, 10, 10, 0, 360);


	}
};

class CreateLine : public Fl_Widget {
public:
	CreateLine(int x1, int y1, int x2, int y2) : Fl_Widget(x1, y1, x2, y2,0) { }

	void draw() {
		fl_color(255, 255, 255);
		int x1 = x();
		int y1 = y();
		int x2 = w();
		int y2 = h();
		fl_line(x1, y1, x2, y2);
	}
};

class CreateBrush : public Fl_Widget {
public:
	CreateBrush(int x, int y) : Fl_Widget(x, y, 0, 0, 0) { }

	void draw() {
		fl_color(255, 255, 255);
		int x1 = x()-15;
		int y1 = y()-15;
		fl_pie(x1, y1, 30, 30, 0, 360);
	}
};

double inRange(double x)
{
	while(x>PI)
	{
		x -= 2*PI;
	}
	while(x<-PI)
	{
		x += 2*PI;
	}
	return x;

}

bool turnCCW(double theta0, double theta)
{
	theta0 = inRange(theta0);
	theta = inRange(theta);
	printf("Recieved %f %f\n", theta0*180/PI, theta*180/PI);
	if((theta0<0 && theta>0) || (theta0>0 && theta<0))
	{
		double the1 = abs(theta0 - theta);
		double the2 = (abs(inRange(PI-theta0)) + abs(inRange(PI-theta)));
		printf("THes %f %f\n", the1*180/PI, the2*180/PI);
		if(the2>the1)
			return (theta0>0 && theta<0);
		else
			return (theta0<0 && theta>0);

	}
	else
	{
		if(theta0>theta)
			return true;
		else
			return false;
	}
}

Pos* Line::rot(double deg, double x_axis, double y_axis)
{
	Matrix* mat = new Matrix(4,1);
	mat->data[0][0] = x1-x_axis;
	mat->data[1][0] = y1-y_axis;
	mat->data[2][0] = 0;
	mat->data[3][0] = 1;
	Matrix* result = rotate(mat, deg, x_axis,y_axis);
	//if((ln != 0 &&result->data[1][0]<230) || (ln==0 && result->data[1][0]<280))
	if((ln != 0 &&result->data[1][0]<-50) || (ln==0 && result->data[1][0]<0))
	{
		//printf("eeeeeeeeee1111111111111111111111\n");
		return 0;
	}
	
	/*
	printf("=======================\n");
	for(int i=0; i<result->row; i++)
	{
		for(int j=0; j<result->col; j++)
		{
			printf("%f ", result->data[i][j]);
		}
		printf("\n");
	}
	*/
	Matrix* mat2 = new Matrix(4,1);
	mat2->data[0][0] = x2-x_axis;
	mat2->data[1][0] = y2-y_axis;
	mat2->data[2][0] = 0;
	mat2->data[3][0] = 1;
	Matrix* result2 = rotate(mat2, deg, x_axis,y_axis);
	//if((ln != 0 &&result2->data[1][0]<230) || (ln==0 && result2->data[1][0]<280))
	
	if((ln != 0 &&result2->data[1][0]<-50) || (ln==0 && result2->data[1][0]<0))
	{
		//printf("322222222222222222222222222222222\n");
		return 0;
	}
	
	//printf("4333333333333333333333333333\n");
	/*
	x1 = result->data[0][0];
	y1 = result->data[1][0];
	x2 = result2->data[0][0];
	y2 = result2->data[1][0];
	*/

	/*
	printf("=======================\n");
	for(int i=0; i<result2->row; i++)
	{
		for(int j=0; j<result2->col; j++)
		{
			printf("%f ", result2->data[i][j]);
		}
		printf("\n");
	}
	*/
	return new Pos(result->data[0][0],result->data[1][0],result2->data[0][0],result2->data[1][0]);
}

void Lines::rotate(double degree, int axis)
{
	printf("rotate %f along %d axis\n", degree, axis);
	switch(axis)
	{
		case 0:
			{
			//printf("AFEADSFASDFADSGFDSFADSF%d\n", lines[0]->rot(degree, lines[axis]->x1, lines[axis]->y1));
			//Pos* p0=lines[0]->rot(degree, lines[axis]->x1, lines[axis]->y1);
			//Pos* p1=lines[1]->rot(degree, lines[axis]->x1, lines[axis]->y1);
			//Pos* p2=lines[2]->rot(degree, lines[axis]->x1, lines[axis]->y1);

			lines[1]->dh->theta += degree;
			
			
			break;
			}
		case 1:
			{
			lines[2]->dh->theta += degree;
			
			break;
			}
		case 2:
			{
			lines[3]->dh->theta += degree;	
			
			break;
			}
	}

	////
	Matrix* T1 = getTmat(lines[1]->dh->theta, lines[0]->dh->leng, 0);			//this leng can be -
	Matrix* T2 = getTmat(lines[2]->dh->theta, lines[1]->dh->leng, 0);			//
	Matrix* T3 = getTmat(lines[3]->dh->theta, lines[2]->dh->leng, 0);						//
	Matrix* T4 = getTmat(0, lines[3]->dh->leng, 0);
	//Matrix* T3 = getTmat(0, lines[1]->dh->leng, 0);						//

	Matrix* mat = new Matrix(4,1);
	mat->data[0][0] = 0;
	mat->data[1][0] = 0;
	mat->data[2][0] = 0;
	mat->data[3][0] = 1;
	Matrix* result = T1->multiply(mat);
	lines[1]->x1 = result->data[0][0];
	lines[1]->y1 = result->data[1][0];
	/*
	printf("=======================2\n");
	for(int i=0; i<result->row; i++)
	{
		for(int j=0; j<result->col; j++)
		{
			printf("%f ", result->data[i][j]);
		}
		printf("\n");
	}
	*/
			
	//arms->updateXY();
	//lines[1]->updateAxis(lines[0]->x2, lines[0]->y2);
	result = (T1->multiply(T2))->multiply(mat);
	lines[1]->x2 = result->data[0][0];
	lines[1]->y2 = result->data[1][0];
	lines[2]->x1 = result->data[0][0];
	lines[2]->y1 = result->data[1][0];
	/*
	printf("=======================2\n");
	for(int i=0; i<result->row; i++)
	{
		for(int j=0; j<result->col; j++)
		{
			printf("%f ", result->data[i][j]);
		}
		printf("\n");
	}
	*/
			
	result = ((T1->multiply(T2))->multiply(T3))->multiply(mat);
	lines[2]->x2 = result->data[0][0];
	lines[2]->y2 = result->data[1][0];
	lines[3]->x1 = result->data[0][0];
	lines[3]->y1 = result->data[1][0];
	/*
	printf("=======================2\n");
	for(int i=0; i<result->row; i++)
	{
		for(int j=0; j<result->col; j++)
		{
			printf("%f ", result->data[i][j]);
		}
		printf("\n");
	}
	*/
			
	result = (((T1->multiply(T2))->multiply(T3))->multiply(T4))->multiply(mat);
	lines[3]->x2 = result->data[0][0];
	lines[3]->y2 = result->data[1][0];
	/*
	printf("=======================2\n");
	for(int i=0; i<result->row; i++)
	{
		for(int j=0; j<result->col; j++)
		{
			printf("%f ", result->data[i][j]);
		}
		printf("\n");
	}
	*/






	////
	if(painting && !moving){
		bool dup=false;
		for(int i=0; i<pairs.size(); i++)
		{
			if(pairs[i].x == (int)(BX+arms->lines[3]->x2-5) && pairs[i].y == (int)(BY-arms->lines[3]->y2-5))
			{
				dup = true;
				break;
			}
		}
		if(!dup)
		{
			printf("added in drawing %d\n", moving);
			pairs.push_back(Pair(BX+arms->lines[3]->x2-5, BY-arms->lines[3]->y2-5));
		}
	}
	if(!moving)
	{
		Fl::redraw();
		Fl::check();
	}
}

void Lines::movetoPt(double x, double y)
{
	moving = true;
	//Before rotation, check it is a possible point
	
	if(pow(x-lines[1]->x1, 2) + pow(y-lines[1]->y1,2) > pow((double)(L0+L1+L2), 2))
	{
		printf("Impossible point\n");
		moving = false;
		return;
	}
	
	//Line 0
	if(pow(x-lines[1]->x2, 2) + pow(y-lines[1]->y2, 2) > pow((double)(L1+L2), 2))	//checking outter circle
	{
		double theta0 = acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		//double theta1 = asin((double)(L1+L2)/(sqrt(pow(x,2) + pow(y,2))));
		double theta1 = acos((pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)+pow((double)(L0),2)-pow((double)(L1+L2),2))/(2*L0*sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		double theta = acos((lines[1]->x2-lines[1]->x1)/L0);
		//printf("AAA0 %f %f %f\n", theta0*180/PI, theta1*180/PI, theta*180/PI);

		if(y<lines[1]->y1)
		{
			theta0 = -theta0;	//-acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
			//theta1 = -theta1;
		}
		if(lines[1]->y1 > lines[1]->y2)
		{
			theta = -theta;
		}
		if(theta0 > theta)
		{
			rotate((theta0-theta1-theta)*180/PI, 0);
		}
		else
		{
			rotate((theta0+theta1-theta)*180/PI, 0);
		}
	}
	else if(pow(x-lines[1]->x2, 2) + pow(y-lines[1]->y2, 2) < pow((double)(L1-L2), 2))	//checking inner circle
	{
		//double theta0 = acos((x)/(sqrt(pow(x,2) + pow(y,2))));
		double theta0 = acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		//double theta1 = asin((sqrt(pow((double)(L1-L2),2) - (pow(x-lines[0]->x2,2) + pow(y-lines[0]->y2,2))))/((double)(L1)));
		double theta1 = acos((pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)+pow((double)(L0),2)-pow((double)(L1-L2),2))/(2*L0*sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		double theta = acos((lines[1]->x2-lines[1]->x1)/L0);

		//printf("AAA0 %f %f %f\n", theta0*180/PI, theta1*180/PI, theta*180/PI);
		if(y<lines[1]->y1)
		{
			theta0 = -theta0;	//-acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
			//theta1 = -theta1;
		}
		if(lines[1]->y1 > lines[1]->y2)
		{
			theta = -theta;
		}
		if(theta0 > theta)
		{
			//printf("CW\n");
			rotate((theta0-theta1-theta)*180/PI, 0);
		}
		else
		{
			//printf("CCW\n");
			rotate((theta0+theta1-theta)*180/PI, 0);
		}

	}

	//Line 1
	if(pow(x-lines[2]->x2, 2) + pow(y-lines[2]->y2, 2) != pow((double)(L2), 2))
	{
		double theta0 = acos((x-lines[2]->x1)/(sqrt(pow(x-lines[2]->x1,2) + pow(y-lines[2]->y1,2))));
		//double theta1 = atan((double)L2/L1);
		double theta1 = acos((int)(((pow(x-lines[2]->x1,2) + pow(y-lines[2]->y1,2)+pow((double)L1,2)-pow((double)L2,2))/(2*L1*sqrt(pow(x-lines[2]->x1,2) + pow(y-lines[2]->y1,2))))*100000000)/100000000.0);	//when it become 1.0000, it is not exactly 1.0000. so need to floor it at low decimal point? test "arms->movetoPt(0,140);"
		double theta = acos((lines[2]->x2 - lines[2]->x1)/L1);
		//double theta2 = acos((sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)) - L1)/L2);
		if(y<lines[2]->y1)
		{

			theta0 = -theta0;	//-acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
			//theta1 = -theta1;
		}
		if(lines[2]->y1 > lines[2]->y2)
		{
			theta = -theta;
		}
		if(theta0 < 0)
			theta0 += 2*PI;

		//printf("distance %f\n", sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)));
		//printf("%f\n", (pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)+pow((double)L1,2)-pow((double)L2,2)));
		//printf("%f %f %f\n", x-lines[1]->x1, y-lines[1]->y1, 2*L1*sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)));
		//printf("%f\n", (pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2)+pow((double)L1,2)-pow((double)L2,2))/(2*L1*sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		//printf("%f\n", acos(1.000000));
		//printf("AAA1 %f %f %f\n", theta0*180/PI, theta1*180/PI, theta*180/PI);
		
		//if(theta0>theta && (theta0 <= PI))
		if(turnCCW(theta0, theta))
		{

			//printf("CCW\n");
			rotate((theta0-theta1-theta)*180/PI, 1);
		}
		else
		{
			//printf("CW\n");
			rotate((theta0+theta1-theta)*180/PI, 1);
		}

	}
	//double theta0 = acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
	//double theta = acos((lines[1]->x2 - lines[1]->x1)/L1);
	//printf("AAA %f %f\n", theta0*180/PI, theta*180/PI);
		
	//Line 2
	double theta0 = acos((x-lines[3]->x1)/(sqrt(pow(x-lines[3]->x1,2) + pow(y-lines[3]->y1,2))));
	double theta = acos((lines[3]->x2 - lines[3]->x1)/L2);
	if(y<lines[3]->y1)
	{
		theta0 = -theta0;	//-acos((x-lines[1]->x1)/(sqrt(pow(x-lines[1]->x1,2) + pow(y-lines[1]->y1,2))));
		//theta1 = -theta1;
	}
	if(lines[3]->y1 > lines[3]->y2)
	{
		theta = -theta;
	}
	//printf("AAA2 %f %f\n", theta0*180/PI, theta*180/PI);
	//printf("AAA2 %f\n", (theta0-theta)*180/PI);
	if(theta0>theta)
	{
		rotate((theta0-theta)*180/PI, 2);
	}
	else
	{
		rotate((theta0-theta)*180/PI, 2);
	}
		
	if(painting){
		bool dup=false;

		for(int i=0; i<pairs.size(); i++)
		{
			if(pairs[i].x == (int)(BX+arms->lines[3]->x2-5) && pairs[i].y == (int)(BY-arms->lines[3]->y2-5))
			{
				dup = true;
				break;
			}
		}
		if(!dup)
		{
			printf("added in moving\n");
			pairs.push_back(Pair(BX+arms->lines[3]->x2-5, BY-arms->lines[3]->y2-5));
		}
	}
	moving = false;
	sendToClient();
	Fl::redraw();
	Fl::check();
}

void j1_cc_callback(Fl_Widget*, void* v) {
	arms->rotate(1, 0);
	sendToClient();
}

void j1_cl_callback(Fl_Widget*, void* v) {
	arms->rotate(-1, 0);
	sendToClient();
}

void j2_cc_callback(Fl_Widget*, void* v) {
	arms->rotate(1, 1);
	sendToClient();
}

void j2_cl_callback(Fl_Widget*, void* v) {
	arms->rotate(-1, 1);
	sendToClient();
}

void j3_cc_callback(Fl_Widget*, void* v) {
	arms->rotate(1, 2);
	sendToClient();
}

void j3_cl_callback(Fl_Widget*, void* v) {
	arms->rotate(-1, 2);
	sendToClient();
}

void x_plus_callback(Fl_Widget*, void* v) {
	arms->movetoPt(arms->lines[3]->x2+1,arms->lines[3]->y2);
	sendToClient();
}

void x_minus_callback(Fl_Widget*, void* v) {
	arms->movetoPt(arms->lines[3]->x2-1,arms->lines[3]->y2);
	sendToClient();
}

void y_plus_callback(Fl_Widget*, void* v) {
	arms->movetoPt(arms->lines[3]->x2,arms->lines[3]->y2+1);
	sendToClient();
}

void y_minus_callback(Fl_Widget*, void* v) {
	arms->movetoPt(arms->lines[3]->x2,arms->lines[3]->y2-1);
	sendToClient();
}

void paint_callback(Fl_Widget*, void* v) {
	painting = !painting;
	if(painting){
		printf("Begin painting\n");

	}
	if(!painting){
		printf("End painting\n");
	}
	Fl::redraw();
	Fl::check();
}
void delay_callback(Fl_Widget* w, void* v) {
	delay = !delay;
}
DWORD WINAPI Thread1(void *parameter){
    while(true)
	{
		if(command.size() >0 && command[0]->time <= clock())
		{
			SendCommand(command[0]->cmd);
			command.erase(command.begin());
		}
		Sleep(10);
	}
	return 0;
}

int __cdecl main(int argc, char **argv) {

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

	Fl_Double_Window window(10, 10, 950, 692, "PaintBot 3.0 Server");
	
	Fl_Box drawSpace(0, 0, 950, 700, "");
	Fl_PNG_Image backgroundPNG("background.png");
	drawSpace.image(backgroundPNG);
	
	FillBoxBackground fillBoxBackground(31, 31, 888, 486);

	Drawing d(1,1,4,5);

	CreateBase base(450,470, 50,30);

	arms = new Lines();
	
	CreateCircles joint1Circ();

	//CreateCircle joint2Circ(BX+arms->lines[1]->x1, BY-arms->lines[1]->y1);
	//CreateCircle joint3Circ(BX+arms->lines[2]->x1, BY-arms->lines[2]->y1);
  
	fl_color(50, 40, 255);

	Fl_Button j1_cc(300, 615, 50, 30, "J1 -");
	Fl_Button j1_cl(350, 615, 50, 30, "J1 +");

	j1_cc.callback(j1_cc_callback, NULL);
	j1_cl.callback(j1_cl_callback, NULL);
	
	Fl_Button j2_cc(300, 585, 50, 30, "J2 -");
	Fl_Button j2_cl(350, 585, 50, 30, "J2 +");

	j2_cc.callback(j2_cc_callback, NULL);
	j2_cl.callback(j2_cl_callback, NULL);

	Fl_Button j3_cc(300, 555, 50, 30, "J3 -");
	Fl_Button j3_cl(350, 555, 50, 30, "J3 +");

	j3_cc.callback(j3_cc_callback, NULL);
	j3_cl.callback(j3_cl_callback, NULL);

	Fl_Button x_minus(565, 585, 50, 30, "Left");
	Fl_Button x_plus(615, 585, 50, 30, "Right");

	x_plus.callback(x_plus_callback, NULL);
	x_minus.callback(x_minus_callback, NULL);

	Fl_Button y_minus(590, 615, 50, 30, "Down");
	Fl_Button y_plus(590, 555, 50, 30, "Up");

	y_plus.callback(y_plus_callback, NULL);
	y_minus.callback(y_minus_callback, NULL);

	Fl_Check_Button paint(445, 585, 60, 30, "Paint");
	paint.labelcolor(FL_WHITE);
	Fl_Check_Button delay(445, 625, 60, 30, "Delay");
	delay.labelcolor(FL_WHITE);
	paint.callback(paint_callback, NULL);
	delay.callback(delay_callback, NULL);

	j1_cc.show();
	j1_cl.show();

	j2_cc.show();
	j2_cl.show();

	j3_cc.show();
	j3_cl.show();

	paint.show();

	window.show();

	Fl_Offscreen test = fl_create_offscreen(950, 692);
	fl_begin_offscreen(test);

	HANDLE t1 = CreateThread(0, 0, Thread1, NULL, 0, 0);
	return Fl::run();
}
