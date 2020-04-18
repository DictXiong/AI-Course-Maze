# AI Project / maze

使用MDP、Q-Learning以及SARSA算法来走迷宫的演示.

程序使用说明
------------

### Visual C++ 可视化程序

可视化界面从上到下由地图、设置面板和状态栏组成.

**注意: 所有修改地图及参数的操作, 都会立刻清空迭代结果.**

-   地图区显示地图与迭代结果, 可通过 "切换显示" 按钮切换.

    当仅显示地图时, 通过左击或右击可以编辑地图, 支持长按拖动批量修改. 左键可在路、墙之间切换, 右键可以在路、陷阱和奖励之间切换.

    当显示迭代结果时, 左键点击未显示方向的格子可以使其显示方向 (如果有), 而右键功能与上面一致.

    如果没有找到一条到达终点的路, 将会显示所有格点的决策方向; 否则, 只显示可行路线上的决策方向.

-   设置面板比较直观, 这里仅讲述一部分.

    **自动迭代**正向迭代, 直到找到一条可行的路, 或者超出了迭代次数限制 (这个限制是 $\mathrm{row}\times\mathrm{col}\div2$). 注意: 找到的路线不一定是最优解.

    **重置状态**清空迭代结果.

    **恢复默认**清空迭代结果、恢复默认地图、恢复默认参数.

-   状态栏分为两部分. 左侧显示智能体状态 "Ready" "Running" 和 "Runned", 右侧为到达当前结果所经历的正向迭代和反向迭代次数.

程序使用 MSVC v142 - VS 2019 编译.

### C++ 命令行程序

我们也提供了能够直接使用 `GCC` 编译器编译的源文件 `clitest.cpp`, 实现了基础功能. 命令行版本的程序共享同一套头文件, 设定了相同的初始地图, 因此二者结果具有一致性.

代码中使用了一些 C++11 标准的内容, 若需编译请合理设定参数.

地图设计
--------

我们设计了一张 $16\times16$ 的地图. 这张地图由路 (Road)、墙 (Wall)、陷阱 (Trap)、起点和终点组成. 每一个节点都具有估计值 (value) 和奖励 (reward) 两个属性, 估计值将在迭代过程更改, 是决策的依据和结果; 而奖励是提前设置好, 在过程中不可更改的值.

从定义上来说, 墙是不可走到的地方, 也就是说在智能体看来这一点根本就不存在, 永远都不可能走到.
智能体向某个方向走, 正常情况下出现三个可能的后继情况 (直行, 左偏和右偏), 若其中有墙, 那这个墙点会从后继列表中剔除, 剩余后继的可能性重新进行归一化.

图例中有奖励点, 且与其它点都不同, 具有初始的估计值.

在进行决策后, 地图格子上会显示估计值与决策方向. 使用寻路函数, 如果找到了一条可行的路, 则仅显示这条路上的决策方向; 如果没有找到, 则显示所有路上的决策方向.

核心算法
--------

我们实现了三种决策算法, 用于在某一给定的迷宫格点上进行决策 (下称 "决策函数"); 三者使用同一个迭代函数控制, 以完成对整个迷宫的迭代 (下称 "迭代函数"). 对于已得到的决策矩阵, 使用了一个路径探寻函数来获得迷宫的解法.

为方便表述, 记当前所处格子为 $S$, 采取的行动为 $A$, 模型参数为: 折扣因子 $\gamma$, 学习率 $\alpha$, $\epsilon$-greedy 概率 $\epsilon$.

### 决策函数

#### 马尔可夫决策 (MDP)

MDP 算法根据指定格点 $S$ 的后继点的现有预期收益值进行决策, 选择期望收益值最大的移动方向 $A^*$. 此外, 采用曼哈顿函数作为启发函数, 如果目标格子距离终点更远, 则对其预期收益施加折现惩罚. 其决策及更新公式如下. 

$$\begin{gathered}
        A^*=\arg\max_{A}f(E(Q(S,A))+R)\\
        f(E(Q(S,A)+R)=\begin{cases}
        E(Q(S,A))+R,&\text{if}\  A=\text{Left or Up}\\
        \gamma (E(Q(S,A))+R),&\text{if }\  A=\text{Right or Down}\\
        \end{cases}\\
        Q(S,A):=f(E(Q(S,A)+R)
        \end{gathered}$$

#### Q-Learning

Q-Learning 算法根据指定点 $S$ 的后继点的现有预期收益值进行决策, 预期收益以 $\gamma$ 的比例折扣, 采用 $\epsilon$-greedy 选择移动方向, 并以 $\alpha$ 的学习率更新估计值. 在更新 $S$ 之前先对目标点进行更新, 求其预期收益最大值. 其决策及更新公式如下. 

$$\begin{gathered}
        A^*\xlongequal{\epsilon\text{-greedy}}\arg\max_A E(Q(S,A))\\
        Q(S,A):=(1-\alpha)Q(S,A)+\alpha[R+\gamma\max_{a}Q(S',a)]
        \end{gathered}$$

为确保取得目标点的 $\max$ 数值, 在更新估计值之前先使用 MDP 决策函数更新所有可能到达的 $S'$ 的估计值.

#### SARSA

SARSA 与 Q-Learning 基本相同, 区别在于: 其更新估计值时直接使用目标点现有估计值进行更新. 其决策及更新公式如下.

$$\begin{gathered}
        A^*\xlongequal{\epsilon\text{-greedy}}\arg\max_A E(Q(S,A))\\
        Q(S,A):=(1-\alpha)Q(S,A)+\alpha[R+\gamma Q(S',a)]
        \end{gathered}$$

### 迭代函数

迭代函数需要指定迭代次数与迭代模式. 迭代模式有前向迭代和后向迭代两种, 前者从起点处开始迭代, 后者从终点处开始迭代. 迭代方式为按斜线更新, 每次更新 $(\text{行号}+\text{列号})$ 相同的一条斜线上所有需要更新的点 (Road), 然后更新相邻的斜线. 在相同参数和决策函数下, 前向迭代和后向迭代的结果是一致的, 但后向迭代一般收敛较快.

### 寻路函数

经过迭代后, 我们得到的是一个 $\text{row}\times\text{col}$ 大小的决策矩阵, 每个格点上标注了所决策的方向 (上下左右). 寻路函数的目标是从中找出一条可行的路.

从起点开始, 对当前格点进行以下操作: 首先列出当前格点所决策方向的所有可能的后继 (最多有三个), 从其中选择一个概率最大、且没有走过的一个后继, 作为下一个格点重复本过程. 如果没有可行的后继, 那么往前退一格重复这个过程. 直到走到终点, 或者无路可走为止.

这样寻路的原因是: 决策可能在一个小区域内成环, 此时智能体试图利用左偏和右偏的可能性来绕开陷阱.寻路函数必须能够判断智能体的这种策略. 默认地图就构建了这种情况.

数据结构
--------

我们的核心代码主要实现了 `Maze` 和 `Agent` 两个类, 承担着存储地图、进行决策的任务. 它们分别位于 `maze.h` 与 `maze.cpp`、`agent.h` 与 `agent.cpp` 中. 其中, `Agent` 类的实现依赖于 `Maze` 类.

在介绍这两个类之前, 先介绍一些辅助性的部件.

-   `enum MapElem`: 枚举类型, 指定迷宫中某一格的类型, 即: 墙、普通的路、陷阱和奖励.

-   `class MazeElem`: 存储迷宫中一个格点的信息, 并具有别名 `Successor`. 它包含

    -   `MapElem type`: 该格点的类型

    -   `double value, reward`: 该格点的价值 (随迭代过程变化) 和奖励 (迭代中不变)

    -   `double prob`: 当 `MazeElem` 被作为某一决策的后继情况使用时, 该后继情况的可能性. 仅在 `Agent::getSuccessor(...)` 中使用, 稍后会详细叙述. 其它地方可忽略这个成员变量.

-   `bool isFixedPoint(MapElem)`: 判断该类型格点是否是固定点 (即: `MapElem::value` 不可被改变).

-   `bool walkable(MapElem)`: 判断该类型格点能否行走, 即: 这一点有定义且不能是墙.

-   `enum Direction`: 枚举了从一点出发的八个方向, 分别到达周围一圈的八个格子.

-   `Direction littleLeft(Direction)`: 返回方向的左边一个方向, 例如, 传入右, 传出右上.

-   `Direction liitleRight(Direction)`: 返回方向的右边一个方向, 例如, 传入下, 返回左下.

-   `std::pair<int,int> getAimPos(std::pair<int,int>,Direction)`: 传入的坐标, 向着给定的方向移动, 返回移动后的坐标.

-   一些全局变量, 将在涉及处详细叙述.

### `Maze` 类

-   `Maze(int,int)`: 构造函数, 指定迷宫的行数和列数.

-   `int row,col`: 记录地图的尺寸.

-   `MazeElem **_d`: 将被初始化为二维数组, 存储每一个格点的信息.

-   `void setWall(int,int)`, `void setTrap(...)`, `void setLucky(...)`,
    `void setRoad(...)`: 初始化迷宫中的一个格点为对应的类型 (终点借用了奖励的初始化方法). 初始化行为包括设置 `MazeElem::type`、`MazeElem::reward` 和 `MazeElem::value`. 按照所指定的, 陷阱使用全局变量 `V_TRAP`, 奖励使用全局变量 `V_LUCKY`, 终点使用全局变量 `V_DEST`.
    
-   `void setCell(int,int,MapElem)`: 根据传入的类型,
    在上面四个函数中选择对应的来执行.

-   `bool lawful(int,int)`: 检查该点是否可以走, 即这一点没有越界且
    `walkable`.

-   `void estPoint(int,int,double)`: 如果传入的坐标不是固定点,
    那么改变其 `value`. 改变时保证
    $\mathtt{value}+\mathtt{reward} <= 1$.

-   `void reload()`: 重新构造地图, 重新构造每一个格点.
    在改变与格点值有关的全局变量后, 使用这个函数来应用更改.

-   析构函数以及 `get`、`clear` 和 `print` 等函数.

### `Agent` 类

-   `Agent(Maze*,int)`: 构造函数, 传入迷宫并指定决策函数.

-   `Maze* _m`: 记录迷宫数据. 在构造时必须传入一个可用的迷宫,
    用来初始化这个值.

-   `Direction **decision`: 将被初始化二维数组, 记录每一个点的决策.

-   `Direction(Agent::* iterfunc)(int,int)`: 函数指针,
    指向类中提供的三个迭代算法中的一个, 标志着当前选择的迭代方法.

-   `unsigned _cSeq, _cRev`:
    记录到达当前结果所经历的正向迭代和反向迭代次数.

-   `void setMaze(Maze*)`: 更改迷宫, 并重新申请 `decision`,
    以适配新的迷宫.

-   `void setAlgo(int)`: 更改选定的算法. 其中 $0$ 对应 MDP, $1$ 对应
    Q-Learning, $2$ 对应 SARSA.

-   `vector<Successor> getSuccessor(int,int,Direction)`: 当前点, 向一个选定的方向行走时,
    由于结果的不确定性, 可能会移动到多个后继点.
    不同后继点的概率是由三个全局变量决定的, 分别是直行概率 `PROB_S`,
    左偏概率 `PROB_L` 和右偏概率 `PROB_R` (三者的和为 $1$).
    该函数会检查所有可能的后继点, 剔除其中不可行走的, 将概率重新归一化,
    并全部返回. 概率存储在 `Successor::prob` 中.
    
-   `MDPDecision(int,int)`
    `QLearningDecision(int,int)`
    `SARSADecision(int,int)`: 三个迭代函数.
    
-   `void iteration(unsigned,bool)`: 调用决策函数进行迭代.
    第一个参数指定迭代次数, 第二个参数指定是否为反向迭代.

-   `vector<pair<pair<int,int>,Direction>> getResult()`: 寻路函数.
    
-   析构函数以及一些平凡 `get`、`clear` 和 `print` 函数.