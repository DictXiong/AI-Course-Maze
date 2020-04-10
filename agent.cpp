#include "pch.h"
#include "maze.cpp"

//algorithm中：0-MDP，1-QLearning，2-SARSA
typedef MazeElem Successor;
class Agent {
private:
	Maze maze;
	Direction** decision;
	Direction(Agent::* iterfunc)(int, int);
	//选择一个方向后, 直走/左前方/右前方的概率
	double probs, probl, probr;
	int algorithm;
	double discount;
	double epsilon;
	double learning_rate;
public:
	Agent(int r, int c, double ps, double pl, double pr, int algo = 0, double disc = 0.95, double eps = 1, double lr = 1)
		:probs(ps), probl(pl), probr(pr), maze(r, c), algorithm(algo), discount(disc), epsilon(eps), learning_rate(lr) {
		decision = new Direction * [r];
		for (int i = 0; i < r; i++) {
			decision[i] = new Direction[c];
			for (int j = 0; j < c; j++) {
				decision[i][j] = UP;
			}
		}
		if (algorithm == 0) {
			iterfunc = &Agent::MDPDecision;
		}
		else if (algorithm == 1) {
			iterfunc = &Agent::QLearningDecision;
		}
		else if (algorithm == 2) {
			iterfunc = &Agent::SARSADecision;
		}
		else {
			printf("Wrong Algorithm Code.\n");
			throw(-1);
		}
	}
	~Agent() {
		for (int i = 0; i < maze.getRow(); i++) {
			delete[] decision[i];
		}
		delete[] decision;
	}

	void clearEst()
	{
		maze.clearEst();
	}
	//输入的坐标应从1开始 //这四个函数让我感觉: 是不是这四个不应该在Maze里实现? 或者Agent应当继承Maze?
	void setWall(int r, int c)
	{
		maze.setWall(r - 1, c - 1);
	}

	void setTrap(int r, int c)
	{
		maze.setTrap(r - 1, c - 1);
	}

	void setLucky(int r, int c)
	{
		maze.setLucky(r - 1, c - 1);
	}

	void setRoad(int r, int c)
	{
		maze.setRoad(r - 1, c - 1);
	}

	//对于一个选定的方向, 获得可能的后继情况
	vector<Successor> getSuccessor(int r, int c, Direction direction)
	{
		vector<Successor> ans;
		Successor tmp;
		double totalProb = 0;//归一化因子
		pair<int, int> nextpos;

		nextpos = getAimPos(r, c, direction);//直行
		tmp = maze.getPoint(nextpos);
		tmp.prob = probs;
		if (walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleLeft(direction));//左前方
		tmp = maze.getPoint(nextpos);
		tmp.prob = probl;
		if (walkable(tmp.type)) ans.push_back(tmp), totalProb += tmp.prob;

		nextpos = getAimPos(r, c, littleRight(direction));//右前方
		tmp = maze.getPoint(nextpos);
		tmp.prob = probr;
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
			if (!maze.lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			auto succ = getSuccessor(r, c, Direction(i));
			if (succ.empty()) return Direction(0); // watchout! this line must be added into other algorithms 
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
				pay[i] = pay[i] * discount;
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
		//	estpay /= discount;
		//}
		maze.estPoint(r, c, estpay);
		return direction;
	}
	Direction QLearningDecision(int r, int c) {
		//maze.estPoint(r, c, 0);
		double pay[MAX_DIRECTION / 2];
		pair<int, int> nextpos;
		double last_value = 0;
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!maze.lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			//为求得后继点的最大Q值，对于可能到达的点先使用MDP更新估计值，之后直接将估计值作为max Q(s')纳入计算
			nextpos = getAimPos(r, c, Direction(i));
			if (maze.lawful(nextpos)) {
				last_value = maze.getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (maze.getPoint(nextpos.first, nextpos.second).value < last_value) {
					maze.estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			nextpos = getAimPos(r, c, littleLeft(Direction(i)));
			if (maze.lawful(nextpos)) {
				last_value = maze.getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (maze.getPoint(nextpos.first, nextpos.second).value < last_value) {
					maze.estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			nextpos = getAimPos(r, c, littleRight(Direction(i)));
			if (maze.lawful(nextpos)) {
				last_value = maze.getPoint(nextpos.first, nextpos.second).value;
				MDPDecision(nextpos.first, nextpos.second);
				if (maze.getPoint(nextpos.first, nextpos.second).value < last_value) {
					maze.estPoint(nextpos.first, nextpos.second, last_value);
				}
			}
			auto succ = getSuccessor(r, c, Direction(i));
			pay[i / 2] = (1 - learning_rate) * maze.getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + learning_rate * j.prob * (j.reward + discount * j.value);
			}
		}
		Direction direction = null;
		double estpay = -INF;
		vector<Direction> directions;
		pair<int, int>cur_pos = make_pair(r, c);
		for (int i = 0; i != MAX_DIRECTION / 2; i++) {
			if (pay[i] > -INF / 2) {
				directions.push_back(Direction(2 * i));
			}
			if (pay[i] > estpay * (1 + EPS)) {
				direction = Direction(2 * i);
				estpay = pay[i];
			}
			//由于浮点数有一定误差，此处用小量EPS进行校正，即当两数比值在(1-EPS,1+EPS)内时认为两浮点数相等
			else if (pay[i] > estpay * (1 - EPS)) {
				nextpos = getAimPos(cur_pos, Direction(2 * i));
				if (maze.lawful(nextpos)) {
					nextpos = getAimPos(nextpos, decision[nextpos.first][nextpos.second]);
					if (nextpos != cur_pos) {
						direction = Direction(2 * i);
						estpay = pay[i];
					}
				}
			}
		}
		//采用epsilon-greedy
		if ((rand() / RAND_MAX) < epsilon) {
			maze.estPoint(r, c, estpay);
			return direction;
		}
		else {
			double rand_d = rand() / (RAND_MAX + 1);
			double rand_div = 1 / directions.size();
			for (int i = 0; i < directions.size(); i++) {
				if (rand_d < (i + 1) * rand_div) {
					maze.estPoint(r, c, pay[directions[i] / 2]);
					return directions[i];
				}
			}
		}
	}
	Direction SARSADecision(int r, int c) {
		double pay[MAX_DIRECTION / 2];
		for (Direction i = UP; i < MAX_DIRECTION; i = Direction(i + 2))
		{
			if (!maze.lawful(getAimPos(r, c, i)))
			{
				pay[i / 2] = -INF;
				continue;
			}
			auto succ = getSuccessor(r, c, Direction(i));
			pay[i / 2] = (1 - learning_rate) * maze.getPoint(r, c).value;
			for (auto j : succ) {
				pay[i / 2] = pay[i / 2] + learning_rate * j.prob * (j.reward + discount * j.value);
			}
		}
		Direction direction = null;
		double estpay = -INF;
		vector<Direction> directions;
		for (int i = 0; i != MAX_DIRECTION / 2; i++) {
			if (pay[i] > -INF / 2) {
				directions.push_back(Direction(2 * i));
			}
			if (pay[i] >= estpay) {
				direction = Direction(2 * i);
				estpay = pay[i];
			}
		}
		if ((rand() / RAND_MAX) < epsilon) {
			maze.estPoint(r, c, estpay);
			return direction;
		}
		else {
			double rand_d = rand() / (RAND_MAX + 1);
			double rand_div = 1 / directions.size();
			for (int i = 0; i < directions.size(); i++) {
				if (rand_d < (i + 1) * rand_div) {
					maze.estPoint(r, c, pay[directions[i] / 2]);
					return directions[i];
				}
			}
		}
	}
	void iteration(bool reverse_iter = false) {
		int row = maze.getRow();
		int col = maze.getCol();
		int r = 0;
		int c = 0;
		if (reverse_iter) {
			for (int s = row + col - 2; s >= 0; s--) {
				r = 0;
				c = s - r;
				while (r < row) {
					if (maze.lawful(r, c)) {
						decision[r][c] = (this->*iterfunc)(r, c);
					}
					r++;
					c--;
					if (c < 0) {
						break;
					}
				}
			}
		}
		else {
			for (int s = 0; s < row + col - 1; s++) {
				r = 0;
				c = s - r;
				while (r < row) {
					if (c < col) {
						decision[r][c] = (this->*iterfunc)(r, c);
					}
					r++;
					c--;
					if (c < 0) {
						break;
					}
				}
			}
		}
	}
	pair< vector< vector<MazeElem> >, vector< pair<pair<int, int>, Direction> >>   getResult()
	{
		int row = maze.getRow();
		int col = maze.getCol();
		vector< pair<pair<int, int>, Direction> > ans;
		int i = 0, x = 0, y = 0;
		while ((x < row - 1) || (y < col - 1))
		{
			ans.push_back(make_pair(make_pair(x, y), decision[x][y]));
			switch (decision[x][y])
			{
			case LEFT: y--; break;
			case RIGHT: y++; break;
			case UP: x--; break;
			case DOWN: x++; break;
			}
			i++;
			if (i > row * col) {
				break;
			}
		}
		return make_pair(maze.getMaze(), ans);
	}

	void print() {
		maze.print();
	}
	void printRoute() {
		int row = maze.getRow();
		int col = maze.getCol();
		char** whole = new char* [row];
		for (int i = 0; i < row; i++) {
			whole[i] = new char[col + 1];
			whole[i][col] = '\0';
			for (int j = 0; j < col; j++) {
				whole[i][j] = 0;
				auto tmp = maze.getPoint(i, j);
				if (tmp.type == WALL) whole[i][j] = 'H';
				else if (tmp.type == TRAP) whole[i][j] == '#';
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
};