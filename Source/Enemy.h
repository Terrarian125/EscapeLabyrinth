#pragma once
#include "../Library/GameObject.h"
#include <vector>
#include "global.h"

//ステート定義
enum class EnemyState {
    Wait,     //待機（周囲を警戒）
    Roam,    //徘徊（目的地を決めずに移動）
    Chase,   //追跡（プレイヤーや特定の目標へ）
    Eat,       //食事（プレイヤーへの攻撃成功後、再び襲うまでの間）
    Down    //ダウン（プレイヤーにやられたとき復活まで）
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
    void UpdateWait();
    void UpdateRoam();
    void UpdateChase();
    void UpdateEat();
    void UpdateDown();
    void StartAStarTest();
    void Draw() override;

    //プレイヤーからの攻撃判定用
    bool CheckHit(VECTOR lineStart, VECTOR lineEnd);


    void SetPosition(VECTOR pos) { m_pos = pos; }//座標を設定する関数
    VECTOR GetPosition() const { return m_pos; }//座標を取得する関数（DebugWindow用）
    EnemyState GetState() const { return state; }//状態を取得する関数（DebugWindow用）
    VECTOR GetTargetPos() const { return m_targetPos; } //現在の目標座標を返す
    float GetStateTimer() const { return stateTimer; }

    //状態をセットする関数
    void SetState(EnemyState nextState, float duration = 5.0f) {
        state = nextState;
        stateTimer = duration; // ダウン時間や食事時間をセット

        //テストモード中に他のステートにされたらフラグを折るなどのケア
        if (nextState != EnemyState::Chase) m_isPathfindingTest = false;
    }
private:
    int modelHandle;
    EnemyState state;
    std::vector<Eye> eyes;
    float stateTimer; //ダウン時間や食事時間の管理用

    VECTOR m_targetPos; //UpdateChase内で次に向かうマスの中心を入れている変数
    VECTOR m_testTargetPos;//A*テスト用の目標
    bool m_isPathfindingTest = false;//テスト中フラグ
};