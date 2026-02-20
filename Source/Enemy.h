#pragma once
#include "../Library/GameObject.h"
#include <vector>

//企画書P6のステート定義
enum class EnemyState {
    Wait, Chase, Roam, Down, Eat
};

struct Eye {
    int frameIndex;   //モデルのボーン番号
    bool active;      //まだ破壊されていないか
    float radius;     //当たり判定の大きさ
};

class Enemy : public GameObject {
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;

    //プレイヤーからの攻撃判定用
    bool CheckHit(VECTOR lineStart, VECTOR lineEnd);

private:
    int modelHandle;
    EnemyState state;
    std::vector<Eye> eyes;
    float stateTimer; //ダウン時間や食事時間の管理用

    void UpdateChase(); //A*探索による追跡ロジック
    void UpdateDown();  //一定時間動かない処理
};