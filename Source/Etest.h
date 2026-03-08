#pragma once
#include "../Library/GameObject.h"
#include "../Library/SceneBase.h"
#include "global.h"
#include <vector>
#include <queue>
#include <stack>
#include <map>

//アルゴリズムの種類
enum class AlgorithmType {
    None,
    BFS,        //1: 幅優先探索
    DFS,        //2: 深さ優先探索
    Dijkstra,   //3: ダイクストラ法
    AStar       //4: A*
};

//探索用ノード
struct TestNode {
    int x, z;
    float g; //スタートからのコスト
    float h; //ゴールまでの推定コスト
    TestNode* parent;

    float f() const { return g + h; }

    //比較用（優先度付きキューで使用）
    bool operator>(const TestNode& other) const { return f() > other.f(); }
};

class Etest : public GameObject {
public:
    Etest();
    ~Etest();
    void Update() override;
    void Draw() override;

private:
    void RunSearch(AlgorithmType type, bool toEnemy = false);
    void Reset();

    AlgorithmType m_currentAlgo = AlgorithmType::None;
    std::vector<VECTOR> m_path;          //最終的な経路
    std::vector<VECTOR> m_visitedNodes;  //探索した全てのマス
    bool m_isSearching = false;

    //アルゴリズム別コンテナ
    std::queue<TestNode*> m_openList;
    std::stack<TestNode*> m_dfsStack; //DFS用のスタック
    std::map<int, TestNode*> m_visitedMap;
    std::vector<TestNode*> m_allNodes; //メモリ解放用

    //A*用の優先度付きキュー（スコア F が小さい順に取り出す）
    struct CompareNode {
        bool operator()(TestNode* a, TestNode* b) {
            return a->f() > b->f(); //f = g + h
        }
    };
    std::priority_queue<TestNode*, std::vector<TestNode*>, CompareNode> m_aStarOpenList;

    //1ステップ進める関数
    void StepBFS();
    void StepDFS(); 
    void StepAStar();
    void StepDijkstra();

    VECTOR m_startPos;
    VECTOR m_goalPos;
};