//#include "pch.h"
#pragma once
#include "maze.cpp"


const double PROB_S = 0.6;
const double PROB_L = 0.2;
const double PROB_R = 0.2;
const double DISCOUNT = 0.95;
const double EPSILON = 1;
const double LEARNING_RATE = 0.9;


enum Direction
{
	UP = 0, UR, RIGHT, RD, DOWN, DL, LEFT, LU, null
};

const pair<int, int> DELTA[MAX_DIRECTION] = { make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(1,1),
	make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-1,-1) };

inline Direction littleLeft(Direction direction)
{
	return Direction((direction - 1 + MAX_DIRECTION) % MAX_DIRECTION);
}
inline Direction littleRight(Direction direction)
{
	return Direction((direction + 1 + MAX_DIRECTION) % MAX_DIRECTION);
}

//当前向 direction 走一格的坐标
inline pair<int, int> getAimPos(int r, int c, Direction direction)
{
	return make_pair(r + DELTA[direction].first, c + DELTA[direction].second);
}
inline pair<int, int> getAimPos(pair<int, int> pos, Direction direction)
{
	return getAimPos(pos.first, pos.second, direction);
}



typedef MazeElem Successor;
//algorithm中：0-MDP，1-QLearning，2-SARSA
class Agent {
private:
	Maze* _m = nullptr;
	Direction** decision;
	Direction(Agent::* iterfunc)(int, int);
	unsigned _cSeq = 0, _cRev = 0;
	//选择一个方向后, 直走/左前方/右前方的概率
public:
	Agent(Maze *maze, int algo = 0)
	{
		setMaze(maze); 
		setAlgo(algo);
	}
	~Agent() {
		for (int i = 0; i < _m->getRow(); i++) {
			delete[] decision[i];
		}
		delete[] decision;
	}

	void setMaze(Maze* maze)
	{
		if (_m != nullptr)
		{
			for (int i = 0; i < _m->getRow(); i++) {
				delete[] decision[i];
			}
			delete[] decision;
		}

		_m = maze;
		int r = _m->getRow(), c = _m->getCol();
		decision = new Direction * [r];
		for (int i = 0; i < r; i++) {
			decision[i] = new Direction[c];
			for (int j = 0; j < c; j++) {
				decision[i][j] = null;
			}
		}
	}

	void setAlgo(int i)
	{
		switch (i)
		{
		case 0:iterfunc = &Agent::MDPDecision; break;
		case 1:iterfunc = &Agent::QLearningDecision; break;
		case 2:iterfunc = &Agent::SARSADecision; break;
		default:throw(-1);
		}
	}

	Maze* getMaze()
	{
		return _m;
	}

	//对于一个选定的方向, 获得可能的后继情况
	vector<Successor> getSuccessor(int r, int c, Direction direction)
	{
		vector<Successor> ans;
		Successor tmp;
		double totalProb = 0;//归一化因子
		pair<int, int> nextpos;

		nextpos = getAimPos(r, c, direction);//直行
		tmp = _m->getPoint(nextpos);
		tmp.prob = PROB_S;
		if (walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleLeft(direction));//左前方
		tmp = _m->getPoint(nextpos);
		tmp.prob = PROB_L;
		if (walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleRight(direction));//右前方
		tmp = _m->getPoint(nextpos);
		tmp.prob = PROB_R;
		if (walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		for (auto& i : ans)//归一化
		{
			i.prob /= totalProb;
		}

		return ans;
	}
	Direction MDPDecision(int r, int c) {
		double pay[MAX_DIRECTION / 2];
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!_m->lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			auto succ = getSuccessor(r, c, Direction(i));
			if (succ.empty()) return null; // watchout! this line must be added into other algorithms 
			double value = 0;
			pay[i / 2] = 0;
			for (auto j : succ) {
				pay[i / 2] += j.prob * (j.reward + j.value);
			}
		}
		Direction direction = null;
		double estpay = -INF;
		//bool discount_flag = false;
		for (int i = 0; i != MAX_DIRECTION / 2; i++) {
			//此处使用曼哈顿距离作为启发函数，如果移动后更靠近起点则回报会有一定折扣
			if ((Direction(2 * i) == Direction::UP) || (Direction(2 * i) == Direction::LEFT)) {
				pay[i] = pay[i] * DISCOUNT;
				//discount_flag = true;
			}
			if (pay[i] > estpay) {
				direction = Direction(i * 2);
				estpay = pay[i];
			}
			//else{
			//	discount_flag = false;
			//}
		}
		//if (discount_flag) {
		//	estpay /= DISCOUNT;
		//}
		_m->estPoint(r, c, estpay);
		return direction;
	}
	Direction QLearningDecision(int r, int c) {
		//maze.estPoint(r, c, 0);
		double pay[MAX_DIRECTION / 2];
		pair<int, int> nextpos;
		double last_value = 0;
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!_m->lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			//为求得后继点的最大Q值，对于可能到达的点先使用MDP更新估计值，之后直接将估计值作为max Q(s')纳入计算
			nextpos = getAimPos(r, c, Direction(i));
			if (_m->lawful(nextpos)) {
				last_value = _m->getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (_m->getPoint(nextpos.first, nextpos.second).value < last_value) {
					_m->estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			nextpos = getAimPos(r, c, littleLeft(Direction(i)));
			if (_m->lawful(nextpos)) {
				last_value = _m->getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (_m->getPoint(nextpos.first, nextpos.second).value < last_value) {
					_m->estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			nextpos = getAimPos(r, c, littleRight(Direction(i)));
			if (_m->lawful(nextpos)) {
				last_value = _m->getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (_m->getPoint(nextpos.first, nextpos.second).value < last_value) {
					_m->estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			auto succ = getSuccessor(r, c, Direction(i));
			pay[i / 2] = (1 - LEARNING_RATE) * _m->getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + LEARNING_RATE * j.prob * (j.reward + DISCOUNT * j.value);
			}
		}
		Direction direction = null;
		double estpay = -INF;
		vector<Direction> directions;
		pair<int, int>cur_pos = make_pair(r, c);
		for (int i = 0; i != MAX_DIRECTION / 2; i++) {
			nextpos = getAimPos(cur_pos, Direction(2 * i));
			if (_m->lawful(nextpos)) {
				nextpos = getAimPos(nextpos, decision[nextpos.first][nextpos.second]);
				if (nextpos != cur_pos) {
					if (pay[i] > -INF / 2) {
						directions.push_back(Direction(2 * i));
					}
					if (pay[i] > estpay) {
						direction = Direction(2 * i);
						estpay = pay[i];
					}
				}
			}
		}
		//采用epsilon-greedy
		if ((rand() / RAND_MAX) < EPSILON) {
			_m->estPoint(r, c, estpay);
			return direction;
		}
		else {
			double rand_d = rand() / (RAND_MAX + 1);
			double rand_div = 1 / directions.size();
			for (int i = 0; i < directions.size(); i++) {
				if (rand_d < (i + 1) * rand_div) {
					_m->estPoint(r, c, pay[directions[i] / 2]);
					return directions[i];
				}
			}
		}
	}
	Direction SARSADecision(int r, int c) {
		double pay[MAX_DIRECTION / 2];
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!_m->lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			auto succ = getSuccessor(r, c, Direction(i));
			pay[i / 2] = (1 - LEARNING_RATE) * _m->getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + LEARNING_RATE * j.prob * (j.reward + DISCOUNT * j.value);
			}
		}
		Direction direction = null;
		double estpay = -INF;
		vector<Direction> directions;
		pair<int, int>cur_pos = make_pair(r, c);
		for (int i = 0; i != MAX_DIRECTION / 2; i++) {
			auto nextpos = getAimPos(cur_pos, Direction(2 * i));
			if (_m->lawful(nextpos)) {
				nextpos = getAimPos(nextpos, decision[nextpos.first][nextpos.second]);
				if (nextpos != cur_pos) {
					if (pay[i] > -INF / 2) {
						directions.push_back(Direction(2 * i));
					}
					if (pay[i] > estpay) {
						direction = Direction(2 * i);
						estpay = pay[i];
					}
				}
			}
		}
		if ((rand() / RAND_MAX) < EPSILON) {
			_m->estPoint(r, c, estpay);
			return direction;
		}
		else {
			double rand_d = rand() / (RAND_MAX + 1);
			double rand_div = 1 / directions.size();
			for (int i = 0; i < directions.size(); i++) {
				if (rand_d < (i + 1) * rand_div) {
					_m->estPoint(r, c, pay[directions[i] / 2]);
					return directions[i];
				}
			}
		}
	}
	void iteration(unsigned times, bool reverse_iter = false) 
	{
		int row = _m->getRow();
		int col = _m->getCol();
		int r = 0;
		int c = 0;
		for (unsigned i = 0; i != times; i++)
			if (reverse_iter) 
			{
				_cRev++;
				for (int s = row + col - 2; s >= 0; s--) 
				{
					r = 0;
					c = s - r;
					while (r < row) 
					{
						if (c < col) {
							if (isFixedPoint(_m->getPoint(r, c).type)) {
								decision[r][c] = (this->*iterfunc)(r, c);
							}
						}
						r++;
						c--;
						if (c < 0) break;
					}
				}
			}
			else 
			{
				_cSeq++;
				for (int s = 0; s < row + col - 1; s++) 
				{
					r = 0;
					c = s - r;
					while (r < row)
					{
						if (c < col) {
							if (isFixedPoint(_m->getPoint(r, c).type)) {
								decision[r][c] = (this->*iterfunc)(r, c);
							}
						}
						r++;
						c--;
						if (c < 0) break;
					}
				}
			}
	}
	vector< pair<pair<int, int>, Direction> >   getResult()
	{
		int row = _m->getRow();
		int col = _m->getCol();
		vector< pair<pair<int, int>, Direction> > ans;
		int i = 0, x = 0, y = 0;
		if (decision[x][y] == null) return ans;
		while ((x < row - 1) || (y < col - 1))
		{
			ans.push_back(make_pair(make_pair(x, y), decision[x][y]));
			switch (decision[x][y])
			{
			case LEFT: y--; break;
			case RIGHT: y++; break;
			case UP: x--; break;
			case DOWN: x++; break;
			case null: throw(-1);
			}
			i++;
			if (i > row * col) {
				break;
			}
		}
		return ans;
	}

	void clearResult()
	{
		int r = _m->getRow();
		int c = _m->getCol();
		for (int i = 0; i != r; i++)
			for (int j = 0; j != c; j++)
				decision[i][j] = null;
		_cSeq = _cRev = 0;
	}

	void printRoute() {
		int row = _m->getRow();
		int col = _m->getCol();
		char** whole = new char* [row];
		for (int i = 0; i < row; i++) {
			whole[i] = new char[col + 1];
			whole[i][col] = '\0';
			for (int j = 0; j < col; j++) {
				whole[i][j] = 0;
				auto tmp = _m->getPoint(i, j);
				if (tmp.type == WALL) whole[i][j] = 'H';
				else if (tmp.type == TRAP) whole[i][j] = '#';
			}
		}
		whole[row - 1][col - 1] = '$';
		int x = 0;
		int y = 0;
		int i = 0;
		while ((x < row - 1) || (y < col - 1)) {
			if (decision[x][y] == Direction::LEFT) {
				whole[x][y--] = '-';
			}
			else if (decision[x][y] == Direction::RIGHT) {
				whole[x][y++] = '-';
			}
			else if (decision[x][y] == Direction::UP) {
				whole[x--][y] = '|';
			}
			else if (decision[x][y] == Direction::DOWN) {
				whole[x++][y] = '|';
			}
			i++;
			if (i > row * col) {
				break;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				printf("%c    ", whole[i][j]);
			}
			printf("\n\n");
		}
	}

	pair<int, int> getCount()
	{
		return make_pair(_cSeq, _cRev);
	}
};
