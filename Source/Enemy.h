#pragma once
#include "../Library/GameObject.h"
#include <vector>
#include "global.h"

//ステート定義
enum class EnemyState {
    Wait, Chase, Roam, Down, Eat
};

struct Eye {
    int frameIndex;   //モデルのボーン番号
    bool active;      //まだ破壊されていないか
    float radius;     //当たり判定の大きさ
};

struct Node {
    int x, y;
    float g, h; //g: スタートからのコスト, h: ゴールまでの推定距離
    Node* parent;

    float f() const { return g + h; } //合計スコア
};

class Enemy : public GameObject {
public:
    Enemy();
    ~Enemy();
    void Update() override;
    void Draw() override;

    //プレイヤーからの攻撃判定用
    bool CheckHit(VECTOR lineStart, VECTOR lineEnd);


    void SetPosition(VECTOR pos) { m_pos = pos; }//座標を設定する関数
    VECTOR GetPosition() const { return m_pos; }//座標を取得する関数（DebugWindow用）
    EnemyState GetState() const { return state; }//状態を取得する関数（DebugWindow用）
    VECTOR GetTargetPos() const { return m_targetPos; } //現在の目標座標を返す

private:
    int modelHandle;
    EnemyState state;
    std::vector<Eye> eyes;
    float stateTimer; //ダウン時間や食事時間の管理用

    void UpdateChase(); //A*探索による追跡ロジック
    void UpdateDown();  //一定時間動かない処理
    VECTOR m_targetPos; //UpdateChase内で次に向かうマスの中心を入れている変数
};