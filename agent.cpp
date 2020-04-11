//#include "pch.h"
#pragma once
#include "maze.h"

enum Direction
{
	UP = 0, UR, RIGHT, RD, DOWN, DL, LEFT, LU, null
};
inline Direction littleLeft(Direction direction)
{
	return Direction((direction - 1 + MAX_DIRECTION) % MAX_DIRECTION);
}
inline Direction littleRight(Direction direction)
{
	return Direction((direction + 1 + MAX_DIRECTION) % MAX_DIRECTION);
}

const std::pair<int, int> DELTA[MAX_DIRECTION] = { std::make_pair(-1,0), std::make_pair(-1,1), std::make_pair(0,1), std::make_pair(1,1),
	std::make_pair(1,0), std::make_pair(1,-1), std::make_pair(0,-1), std::make_pair(-1,-1) };


//��ǰ�� direction ��һ�������
inline std::pair<int, int> getAimPos(int r, int c, Direction direction)
{
	return std::make_pair(r + DELTA[direction].first, c + DELTA[direction].second);
}
inline std::pair<int, int> getAimPos(std::pair<int, int> pos, Direction direction)
{
	return getAimPos(pos.first, pos.second, direction);
}

typedef MazeElem Successor;
//algorithm�У�0-MDP��1-QLearning��2-SARSA
class Agent {
private:
	Maze* _m = nullptr;
	Direction** decision;
	Direction(Agent::* iterfunc)(int, int);
	unsigned _cSeq = 0, _cRev = 0;
	//ѡ��һ�������, ֱ��/��ǰ��/��ǰ���ĸ���
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

	//����һ��ѡ���ķ���, ��ÿ��ܵĺ�����
	vector<Successor> getSuccessor(int r, int c, Direction direction)
	{
		vector<Successor> ans;
		Successor tmp;
		double totalProb = 0;//��һ������
		pair<int, int> nextpos;

		nextpos = getAimPos(r, c, direction);//ֱ��
		tmp = _m->getPoint(nextpos);
		tmp.prob = Helper::PROB_S;
		if (Helper::walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleLeft(direction));//��ǰ��
		tmp = _m->getPoint(nextpos);
		tmp.prob = Helper::PROB_L;
		if (Helper::walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleRight(direction));//��ǰ��
		tmp = _m->getPoint(nextpos);
		tmp.prob = Helper::PROB_R;
		if (Helper::walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		for (auto& i : ans)//��һ��
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
			//�˴�ʹ�������پ�����Ϊ��������������ƶ�������������ر�����һ���ۿ�
			if ((Direction(2 * i) == Direction::UP) || (Direction(2 * i) == Direction::LEFT)) {
				pay[i] = pay[i] * Helper::DISCOUNT;
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
		_m->estPoint(r, c, 10*V_TRAP);
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
			//Ϊ��ú�̵�����Qֵ�����ڿ��ܵ���ĵ���ʹ��MDP���¹���ֵ��֮��ֱ�ӽ�����ֵ��Ϊmax Q(s')�������
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
			pay[i / 2] = (1 - Helper::LEARNING_RATE) * _m->getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + Helper::LEARNING_RATE * j.prob * (j.reward + Helper::DISCOUNT * j.value);
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
		//����epsilon-greedy
		if ((rand() / RAND_MAX) < Helper::EPSILON) {
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
		pair<int, int> nextpos;
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!_m->lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			auto succ = getSuccessor(r, c, Direction(i));
			pay[i / 2] = (1 - Helper::LEARNING_RATE) * _m->getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + Helper::LEARNING_RATE * j.prob * (j.reward + Helper::DISCOUNT * j.value);
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
		if ((rand() / RAND_MAX) < Helper::EPSILON) {
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
							if (!Helper::isFixedPoint(_m->getPoint(r, c).type)) {
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
							if (!Helper::isFixedPoint(_m->getPoint(r, c).type)) {
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
		while (_m->lawful(x, y) && ((x != row - 1) || (y != col - 1)))
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
				else if (tmp.type == TRAP) whole[i][j] == '#';
			}
		}
		whole[row - 1][col - 1] = '$';
		int x = 0;
		int y = 0;
		int i = 0;
		while (_m->lawful(x,y) && ((x != row-1 ) || (y != col-1))) {
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
