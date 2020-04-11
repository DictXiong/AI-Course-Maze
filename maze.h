#pragma once

//#include "pch.h"
#include<cstdio>
#include<vector>
using namespace std;

const int MAX_DIRECTION = 8;
const int INF = 0X3F4F5F6F;
const double EPS = 1e-6;

enum MapElem
{
	UNDEF, WALL, ROAD, TRAP, LUCKY //����, WALL, TRAP �ǹ̶���, value ֵ���øı�. 
};

class Helper
{
public:
	static bool isFixedPoint(MapElem m);
	static bool walkable(MapElem m);

	static double V_TRAP;
	static double V_DEST;
	static double V_LUCKY;

	static double PROB_S;
	static double PROB_L;
	static double PROB_R;
	static double DISCOUNT;
	static double EPSILON;
	static double LEARNING_RATE;
};

class MazeElem
{
public:
	MapElem type;
	double value, reward;
	double prob; //special usage
	MazeElem(MapElem _type = ROAD, double _reward = 0);
};

class Maze {
private:
	int row;
	int col;
	MazeElem** _d;
public:
	Maze(int r, int c);
	~Maze();

	//�������Ԥ��ֵ
	void clearEst();

	//���������Թ���Ϊ�����
	void setCell(int r, int c, MapElem m);
	void setWall(int r, int c);
	void setTrap(int r, int c);
	void setLucky(int r, int c);
	void setRoad(int r, int c);

	//�����ڵ��������и���Ԥ��ֵ
	void estPoint(int r, int c, double estpay);

	//�����Ƿ����
	bool lawful(int r, int c);
	bool lawful(pair<int, int> pos);
	MazeElem getPoint(int r, int c);
	MazeElem getPoint(pair<int, int> pos);
	int getRow();
	int getCol();
	vector< vector<MazeElem> > getMaze();
	void print();
	void reload();
};
