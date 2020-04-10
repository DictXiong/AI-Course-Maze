#include<iostream>
#include<cstdio>
#include "agent.cpp"

using namespace std;

int main()
{
	srand(static_cast<unsigned>(time(NULL)));
	int row = 16;
	int col = 16;
	int times = 15;
	Maze maze(row,col);
	vector<pair<int, int>> walls = { make_pair(1,2), make_pair(2,2), make_pair(2,6), make_pair(2,8), make_pair(3,2),
	make_pair(3,3), make_pair(3,4), make_pair(3,5), make_pair(3,6), make_pair(3,8), make_pair(3,9), make_pair(3,10),
	make_pair(3,11), make_pair(3,12), make_pair(3,13), make_pair(3,14), make_pair(4,14), make_pair(5,2), make_pair(5,3),
	make_pair(5,4), make_pair(5,5), make_pair(5,6), make_pair(5,7), make_pair(5,8), make_pair(5,11), make_pair(5,12), make_pair(5,14),
	make_pair(6,8), make_pair(6,9), make_pair(6,10), make_pair(6,11), make_pair(6,14), make_pair(7,2), make_pair(7,3), make_pair(7,4),
	make_pair(7,5), make_pair(7,6), make_pair(7,2), make_pair(7,8), make_pair(7,9), make_pair(7,14), make_pair(8,1), make_pair(8,2),
	make_pair(8,6), make_pair(8,8), make_pair(8,11), make_pair(8,12), make_pair(8,14), make_pair(9,1), make_pair(9,4), make_pair(9,6),
	make_pair(9,8), make_pair(9,12), make_pair(9,14), make_pair(10,2), make_pair(10,4), make_pair(10,5), make_pair(10,6), make_pair(10,8),
	make_pair(10,10), make_pair(10,12), make_pair(10,13), make_pair(10,14), make_pair(11,6), make_pair(11,8), make_pair(11,10), make_pair(11,12),
	make_pair(12,2), make_pair(12,3), make_pair(12,4), make_pair(12,8), make_pair(12,12), make_pair(12,13), make_pair(12,14), make_pair(12,2), make_pair(13,2),
	make_pair(13,4), make_pair(13,5), make_pair(13,6), make_pair(13,7), make_pair(13,8), make_pair(13,10), make_pair(14,2), make_pair(14,4),
	make_pair(14,8), make_pair(14,9), make_pair(14,10), make_pair(14,11), make_pair(14,12), make_pair(14,13), make_pair(14,14), make_pair(15,6) };
	for (auto i : walls) {
		maze.setWall(i.first, i.second);
	}
	maze.setTrap(5, 10);
	maze.setTrap(7, 1);
	maze.setTrap(8, 9);
	maze.setTrap(11, 11);
	maze.setTrap(12, 15);
	maze.setLucky(row-1, col-1);
	//maze.print();
	Agent Robot(&maze,0);
	for (int i = 1; i <= times; i++) {
		Robot.iteration(true);
		if (i % 5) {
			continue;
		}
		printf("\nAfter %d iterations:\n", i);
		maze.print();
		printf("\n");
		Robot.printRoute();
	}
} 
