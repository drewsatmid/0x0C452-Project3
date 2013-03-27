#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <math.h>
#define PI 3.14159265

#define L0 150
#define L1 100
#define L2 75
/*
int BX = 500;
int BY = 750;
*/
int BX = 475;
int BY = 470;
int XX = 200;	//

int orgX = 0;
int orgY = 0;

struct Pos{
	double x1;
	double y1;
	double x2;
	double y2;
	Pos(double x_1, double y_1,double x_2, double y_2) {
		x1 = x_1;
		y1 = y_1;
		x2 = x_2;
		y2 = y_2;
	}
};
class Matrix
{
public:
	int row;
	int col;
	double data[4][4];	//**
	Matrix(int r, int c)
	{
		row=r;
		col=c;
		//initialize data with row, col
		for(int i=0; i<row; i++)
		{
			for(int j=0; j<col; j++)
			{
				data[i][j] = 0;
			}
		}
	}
	Matrix* multiply(Matrix* mat)
	{
		Matrix* result = new Matrix(row, mat->col);
		for(int i=0; i<row; i++)
		{
			for(int j=0; j<mat->col; j++)
			{
				//result.data[i][j]
				for(int k=0; k<row; k++)
				{
					result->data[i][j] += data[i][k]*mat->data[k][j];
					//printf("i = %d, j = %d, mat : %f, line : %f, result = %f\n", i, j, data[i][k], mat->data[k][j], data[i][k]*mat->data[k][j]);
				}
			}
		}

		return result;
	}
};
Matrix* getTmat(double degree, double orx, double ory)
{
	Matrix* Rot = new Matrix(4,4);
	Rot->data[0][0] = cos(degree*PI/180);
	Rot->data[0][1] = -sin(degree*PI/180);
	Rot->data[0][2] = 0;
	Rot->data[1][0] = sin(degree*PI/180);
	Rot->data[1][1] = cos(degree*PI/180);
	Rot->data[1][2] = 0;
	Rot->data[2][0] = 0;
	Rot->data[2][1] = 0;
	Rot->data[2][2] = 1;
	Rot->data[0][3] = orx;	
	Rot->data[1][3] = ory;
	Rot->data[2][3] = 0;	
	Rot->data[3][0] = 0;
	Rot->data[3][1] = 0;
	Rot->data[3][2] = 0;
	Rot->data[3][3] = 1;
	return Rot;
}
Matrix* rotate(Matrix* line, double degree, double orx, double ory)	
{

	//printf("aaaaaaaaa %f\n", degree);
	Matrix* Rot = new Matrix(4,4);
	Rot->data[0][0] = cos(degree*PI/180);
	Rot->data[0][1] = -sin(degree*PI/180);
	Rot->data[0][2] = 0;
	Rot->data[1][0] = sin(degree*PI/180);
	Rot->data[1][1] = cos(degree*PI/180);
	Rot->data[1][2] = 0;
	Rot->data[2][0] = 0;
	Rot->data[2][1] = 0;
	Rot->data[2][2] = 1;
	Rot->data[0][3] = orx;	
	Rot->data[1][3] = ory;
	Rot->data[2][3] = 0;	
	Rot->data[3][0] = 0;
	Rot->data[3][1] = 0;
	Rot->data[3][2] = 0;
	Rot->data[3][3] = 1;

	return Rot->multiply(line);
}
class Drawing: public Fl_Widget
{
	void draw();
	/*
	{
		
		fl_color(FL_BLACK);
		//draw_line(XX+0,0,0,150);
		//draw_line(
		fl_color(FL_BLUE);
		draw_line(XX+0,150,0,100);
		fl_color(FL_RED);
		draw_line(XX+0,250,0,75);
		//fl_arc(point(0).x,point(0).y,r+r,r+r,0,360);
		//fl_arc(100, 100, 200, 200, 0, 360);
		fl_pie(0,0,200,200,0,360);
		fl_end_line();

	}
	*/
	void draw_line(int x1, int y1, int x2, int y2)
	{
		//fl_line(BX+x1, BY-y1, BX+x1+x2, BY-y1-y2);
		fl_line(BX+x1, BY-y1, BX+x2, BY-y2);
	}
	public:
	Drawing(int X, int Y, int W, int H):Fl_Widget(X,Y,W,H){}
};

class DHparam
{
public:
	double leng;	//a_i
	double theta;	//theta_i
	DHparam(double l, double t)
	{
		leng = l;
		theta = t;
	}
};

class Line
{
public:
	int ln;
	DHparam* dh;
	double x1;	//for display(fixed)
	double y1;
	double x2;
	double y2;
	Line(double a1,double b1,double a2,double b2, int l)
	{
		x1 = a1;
		x2 = a2;
		y1 = b1;
		y2 = b2;
		ln = l;
	}
	Line(double l, double t)
	{
		dh = new DHparam(l,t);
	}
	Pos* rot(double deg, double x_axis, double y_axis);


};


class Lines
{

public:
	Line* lines[4];

	Lines()
	{
		//lines[0] = new Line(-25,280,-25,430,0);
		//lines[1] = new Line(-25,430,-25,530,1);
		//lines[2] = new Line(-25,530,-25,605,2);
		//lines[0] = new Line(0,0,0,150,0);
		//lines[1] = new Line(0,150,0,250,1);
		//lines[2] = new Line(0,250,0,325,2);
		lines[0] = new Line(0, 0);	//base
		lines[1] = new Line(150,90);
		lines[2] = new Line(100,0);
		lines[3] = new Line(75,0);
		//lines[1]->updateAxis(0, 0);
		//lines[2]->updateAxis(150,0);
		//lines[3]->updateAxis(250,0);
		rotate(0, 0);
		//updateXY();
		
		
	}
	void rotate(double degree, int axis);
	void movetoPt(double x, double y);
	
	void updateXY()
	{
		double angle=0;
		//lines[0]->x1 = lines[0]->axisX;	//only axisX can be changed for lines[0]	//sliding
		//lines[0]->y1 = lines[0]->axisY;
		for(int i=0; i<4; i++)
		{
			angle+= lines[i]->dh->theta;
			lines[i]->x2 = lines[i]->x1 + lines[i]->dh->leng*cos(angle*PI/180);
			lines[i]->y2 = lines[i]->y1 + lines[i]->dh->leng*sin(angle*PI/180);
			if(i<3)
			{
				lines[i+1]->x1 = lines[i]->x1 + lines[i]->dh->leng*cos(angle*PI/180);
				lines[i+1]->y1 = lines[i]->y1 + lines[i]->dh->leng*sin(angle*PI/180);
			}
			//x2=x1 + dh->leng*cos(dh->theta*PI/180);
			//y2=y1 + dh->leng*sin(dh->theta*PI/180);
			//printf("%f %f %f %f %f\n",angle*PI/180, lines[i]->x1,lines[i]->y1,lines[i]->x2,lines[i]->y2);
			//lines[1]->dh->theta
			char* ch = (char*)(&lines[i]->dh->theta);
			

		}
	}
};

