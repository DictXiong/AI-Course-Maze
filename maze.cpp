#pragma once

#include "pch.h"
#include<cstdio>
#include<ctime>
#include<cstdlib>
#include<vector>
using namespace std;

const int MAX_DIRECTION = 8;
const int INF = 0X3F4F5F6F;
const double EPS = 1e-6;
const double V_TRAP = -1;
const double V_DEST = 1;
const double V_LUCKY = 0.1;


enum MapElem
{
	UNDEF, WALL, ROAD, TRAP, LUCKY //其中, WALL, TRAP 是固定点, value 值不得改变. 
};
inline bool isFixedPoint(MapElem m)
{
	return m == WALL || m == TRAP || m == LUCKY || m == UNDEF;
}
inline bool walkable(MapElem m)
{
	return m != UNDEF && m != WALL;
}

class MazeElem
{
public:
	MapElem type;
	double value, reward;
	double prob; //special usage
	MazeElem(MapElem _type = ROAD, double _reward = 0) : type(_type), reward(_reward), value(0) {}
};

/* replaced by MazeElem
//对于一个选定的方向, 可能的后继情况.
class Successor
{
public:
	double value, prob, reward;
	Successor(double v, double p, double r=0) :value(v), prob(p), reward(r){};
	Successor()
	{
		value = -INF, prob = 0, reward=0;
	}
};
*/

class Maze {
private:
	int row;
	int col;
	MazeElem** _d;
public:
	Maze(int r, int c) :row(r), col(c)
	{
		_d = new MazeElem * [row];
		for (int i = 0; i < row; i++) {
			_d[i] = new MazeElem[col];
		}
		setLucky(r - 1, c - 1);
	}
	~Maze()
	{
		for (int i = 0; i < row; i++) {
			delete[] _d[i];
		}
		if (_d != NULL)
		{
			delete[] _d;
		}
	}

	//用于清空预测值
	void clearEst()
	{
		for (int i = 0; i != row; i++)
			for (int j = 0; j != col; j++)
				//if (!isFixedPoint(_d[i][j].type))
				_d[i][j].value = 0;
	}

	//用于设置迷宫点为特殊点
	
	void setCell(int r, int c, MapElem m)
	{
		switch (m)
		{
		case WALL: setWall(r, c); break;
		case ROAD: setRoad(r, c); break;
		case TRAP: setTrap(r, c); break;
		case LUCKY: setLucky(r, c); break;
		}
	}

	void setWall(int r, int c)
	{
		_d[r][c] = MazeElem(WALL);
	}

	void setTrap(int r, int c)
	{
		_d[r][c] = MazeElem(TRAP, V_TRAP);
	}

	void setLucky(int r, int c)
	{
		if (r == row - 1 && c == col - 1) _d[r][c] = MazeElem(LUCKY, V_DEST);
		else _d[r][c] = MazeElem(LUCKY, V_LUCKY);
	}

	void setRoad(int r, int c)
	{
		_d[r][c] = MazeElem(ROAD);
	}

	//用于在迭代过程中更新预期值
	void estPoint(int r, int c, double estpay)
	{
		if (!isFixedPoint(_d[r][c].type)) {
			_d[r][c].value = estpay;
		}
	}
	//检查点是否可走
	inline bool lawful(int r, int c)
	{
		return (((r >= 0) && (r < row) && (c >= 0) && (c < col)) && (walkable(_d[r][c].type)));
	}
	inline bool lawful(pair<int, int> pos)
	{
		return lawful(pos.first, pos.second);
	}

	MazeElem getPoint(int r, int c)
	{
		if (lawful(r, c)) return _d[r][c];
		else return MazeElem(WALL);
	}
	inline MazeElem getPoint(pair<int, int> pos)
	{
		return getPoint(pos.first, pos.second);
	}
	int getRow()
	{
		return row;
	}
	int getCol()
	{
		return col;
	}
	vector< vector<MazeElem> > getMaze()
	{
		vector<vector<MazeElem>> ans;
		ans.reserve(row);
		for (int i = 0; i != row; i++)
		{
			vector<MazeElem> tmp;
			tmp.reserve(col);
			for (int j = 0; j != col; j++)
			{
				tmp.push_back(_d[i][j]);
			}
			ans.push_back(tmp);
		}
		return ans;
	}
	void print() {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {//?
				if (walkable(_d[i][j].type)) printf("%.2f\t", _d[i][j].value + _d[i][j].reward);
				else printf("0\t");
			}
			printf("\n");
		}
	}
};

/*
int main() {
	srand(static_cast<unsigned>(time(NULL)));
	
	int times = 15;
	double prob1 = 0.6;
	double prob2 = 0.2;
	double prob3 = 0.2;
	double discount = 0.95;
	int algorithm = 0;
	double epsilon = 1;
	double learning_rate = 0.9;
	Agent Robot(row, col, prob1, prob2, prob3, algorithm, discount, epsilon, learning_rate);
	
	//Robot.print();
	for (int i = 1; i <= times; i++) {
		Robot.iteration(true);
		if (i % 1) {
			continue;
		}
		printf("\nAfter %d iterations:\n", i);
		Robot.print();
		printf("\n");
		Robot.printRoute();
	}
	//printf("\n");
	//Robot.printRoute();
	//Agent rob(5, 4, 0.6, 0.2, 0.2, 0.9);
	//rob.setPoint(1, 1, -1);
	//rob.setPoint(3, 2, -1);
	//rob.setPoint(4, 3, 1);
	/*Agent rob1(row, col, 0.6, 0.2, 0.2, 0.9);
	Agent rob2(row, col, 0.6, 0.2, 0.2, 0.9);
	rob1.setPoint(5, 4, 1);
	rob1.setPoint(5, 2, -1);
	rob1.setPoint(4, 2, 0, 1);
	rob1.setPoint(3, 2, 0, 1);
	rob1.setPoint(3, 3, 0, 1);
	rob1.setPoint(2, 3, 0, 1);
	rob1.print();
	for (int i = 1; i <= times; i++) {
		rob1.iteration();
		if (i % 5) {
			continue;
		}
		printf("\nAfter %d iterations:\n", i);
		rob1.print();
	}
	printf("\n");
	rob1.printRoute();
	//printf("\n************************\n\n");
	//rob2.setPoint(5, 4, 1);
	//rob2.setPoint(5, 2, -1);
	//rob2.setPoint(4, 2, 0, 1);
	//rob2.setPoint(3, 2, 0, 1);
	//rob2.setPoint(3, 3, 0, 1);
	//rob2.setPoint(2, 3, 0, 1);
	//rob2.print();
	//for (int i = 1; i <= times; i++) {
	//	rob2.iteration(true);
	//	if (i % 5) {
	//		continue;
	//	}
	//	printf("\nAfter %d iterations:\n", i);
	//	rob2.print();
	//}

	return 0;
}
*/