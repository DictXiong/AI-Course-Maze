#pragma once

//#include "pch.h"
#include "maze.h"
#ifdef _MSC_VER 
#define min(a,b) (a<b? a:b)
#endif
using namespace std;


bool Helper::isFixedPoint(MapElem m)
{
	return m == MapElem::WALL || m == MapElem::TRAP || m == MapElem::UNDEF;
}
bool Helper::walkable(MapElem m)
{
	return m != MapElem::UNDEF && m != MapElem::WALL;
}
double Helper::V_TRAP = -1;
double Helper::V_DEST = 1;
double Helper::V_LUCKY = 0.5;
double Helper::PROB_S = 0.6;
double Helper::PROB_L = 0.2;
double Helper::PROB_R = 0.2;
double Helper::DISCOUNT = 0.99;
double Helper::EPSILON = 1;
double Helper::LEARNING_RATE = 0.9;

MazeElem::MazeElem(MapElem _type, double _reward, double _value) : type(_type), reward(_reward), value(_value) { prob = 0; }

void MazeElem::set(MapElem _type, double _reward, double _value)
{
	type = _type; reward = _reward; value = _value;
}

Maze::Maze(int r, int c) :row(r), col(c)
{
	_d = new MazeElem * [row];
	for (int i = 0; i < row; i++) {
		_d[i] = new MazeElem[col];
	}
	setLucky(r - 1, c - 1);
}
Maze::~Maze()
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
void Maze::clearEst()
{
	for (int i = 0; i != row; i++)
		for (int j = 0; j != col; j++)
			if (_d[i][j].type != MapElem::LUCKY || (i == row-1 && j == col-1)) _d[i][j].value = 0;
			else _d[i][j].value = Helper::V_LUCKY;
}

//用于设置迷宫点为特殊点
	
void Maze::setCell(int r, int c, MapElem m)
{
	switch (m)
	{
	case WALL: setWall(r, c); break;
	case ROAD: setRoad(r, c); break;
	case TRAP: setTrap(r, c); break;
	case LUCKY: setLucky(r, c); break;
	case UNDEF: throw(-1);
	}
}

void Maze::setWall(int r, int c)
{
	_d[r][c].set(WALL);
}

void Maze::setTrap(int r, int c)
{
	_d[r][c].set(TRAP, Helper::V_TRAP);
}

void Maze::setLucky(int r, int c)
{
	if (r == row - 1 && c == col - 1) _d[r][c].set(LUCKY, Helper::V_DEST);
	else _d[r][c].set(LUCKY, 0, Helper::V_LUCKY);
}

void Maze::setRoad(int r, int c)
{
	_d[r][c].set(ROAD);
}

//用于在迭代过程中更新预期值
void Maze::estPoint(int r, int c, double estpay)
{
	if (!Helper::isFixedPoint(_d[r][c].type)) 
	{
		_d[r][c].value = min(1-_d[r][c].reward, estpay);
	}
}
//检查点是否可走
bool Maze::lawful(int r, int c)
{
	return (((r >= 0) && (r < row) && (c >= 0) && (c < col)) && (Helper::walkable(_d[r][c].type)));
}
bool Maze::lawful(pair<int, int> pos)
{
	return lawful(pos.first, pos.second);
}

MazeElem Maze::getPoint(int r, int c)
{
	if (lawful(r, c)) return _d[r][c];
	else return MazeElem(WALL);
}
MazeElem Maze::getPoint(pair<int, int> pos)
{
	return getPoint(pos.first, pos.second);
}
int Maze::getRow()
{
	return row;
}
int Maze::getCol()
{
	return col;
}
vector< vector<MazeElem> > Maze::getMaze()
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
void Maze::print() {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {//?
			if (Helper::walkable(_d[i][j].type)) printf("%.2f\t", _d[i][j].value + _d[i][j].reward);
			else printf("0\t");
		}
		printf("\n");
	}
}
void Maze::reload()
{
	clearEst();
	for (int i = 0; i != row; i++)
		for (int j = 0; j != col; j++)
			setCell(i, j, _d[i][j].type);
}
