#pragma once

//#include "pch.h"
#include "maze.h"
using namespace std;


bool Helper::isFixedPoint(MapElem m)
{
	return m == WALL || m == TRAP || m == LUCKY || m == UNDEF;
}
bool Helper::walkable(MapElem m)
{
	return m != UNDEF && m != WALL;
}
double Helper::V_TRAP = -1;
double Helper::V_DEST = 1;
double Helper::V_LUCKY = 0.1;
double Helper::PROB_S = 0.6;
double Helper::PROB_L = 0.2;
double Helper::PROB_R = 0.2;
double Helper::DISCOUNT = 0.99;
double Helper::EPSILON = 1;
double Helper::LEARNING_RATE = 0.9;

MazeElem::MazeElem(MapElem _type, double _reward) : type(_type), reward(_reward), value(0) { prob = 0; }

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

//�������Ԥ��ֵ
void Maze::clearEst()
{
	for (int i = 0; i != row; i++)
		for (int j = 0; j != col; j++)
			//if (!isFixedPoint(_d[i][j].type))
			_d[i][j].value = 0;
}

//���������Թ���Ϊ�����
	
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
	_d[r][c] = MazeElem(WALL);
}

void Maze::setTrap(int r, int c)
{
	_d[r][c] = MazeElem(TRAP, Helper::V_TRAP);
}

void Maze::setLucky(int r, int c)
{
	if (r == row - 1 && c == col - 1) _d[r][c] = MazeElem(LUCKY, Helper::V_DEST);
	else _d[r][c] = MazeElem(LUCKY, Helper::V_LUCKY);
}

void Maze::setRoad(int r, int c)
{
	_d[r][c] = MazeElem(ROAD);
}

//�����ڵ��������и���Ԥ��ֵ
void Maze::estPoint(int r, int c, double estpay)
{
	if (!Helper::isFixedPoint(_d[r][c].type)) {
		_d[r][c].value = estpay;
	}
}
//�����Ƿ����
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
