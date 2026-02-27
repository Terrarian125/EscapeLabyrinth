#include "Enemy.h"
#include "Player.h"
#include "Stage.h"
#include "DxLib.h"
#include <cmath>

Enemy::Enemy() : modelHandle(-1), state(EnemyState::Chase), stateTimer(0.0f) {
    m_pos = VGet(0.0f, 0.0f, 0.0f);
    m_targetPos = m_pos; 
    m_testTargetPos = m_pos;
    m_isPathfindingTest = false;
    //敵のモデルをロード
    modelHandle = MV1LoadModel("Data/Model/Enemy.mv1");
    MV1SetScale(modelHandle, VGet(0.5f, 0.5f, 0.5f)); //サイズは適宜調整
}

Enemy::~Enemy() {
    MV1DeleteModel(modelHandle);
}

void Enemy::Update() {
    //プレイヤーの座標を取得
    Player* player = FindGameObject<Player>();
    if (!player) return;

    switch (state) {
    case EnemyState::Wait:  UpdateWait();  break;
    case EnemyState::Roam:  UpdateRoam();  break;
    case EnemyState::Chase: UpdateChase(); break;
    case EnemyState::Eat:   UpdateEat();   break;
    case EnemyState::Down:  UpdateDown();  break;
    }

    //モデルの位置を更新
    MV1SetPosition(modelHandle, m_pos);
}

void Enemy::UpdateWait()
{
}

void Enemy::UpdateRoam()
{
}

void Enemy::UpdateChase() {
    Stage* stage = FindGameObject<Stage>();
    Player* player = FindGameObject<Player>();
    if (!stage) return;

    //目標地点の決定
    VECTOR finalTargetPos;
    if (m_isPathfindingTest) {
        finalTargetPos = m_testTargetPos;
    }
    else {
        if (!player) return;
        finalTargetPos = player->GetPosition();
    }

    //目的地ロックロジック
    //現在の目標マス(m_targetPos)に十分近いかチェック
    float distToNextNode = VSize(VSub(m_targetPos, m_pos));

    //もし目標マスにほぼ到着しているなら、次の目標マスを計算する
    if (distToNextNode < 10.0f) {
        //現在のマス座標
        int curX = static_cast<int>(floorf((m_pos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE));
        int curZ = static_cast<int>(floorf((m_pos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE));

        //最終目的地のマス座標
        int tx = static_cast<int>(floorf((finalTargetPos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE));
        int tz = static_cast<int>(floorf((finalTargetPos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE));

        int nextX = curX;
        int nextZ = curZ;

        //次の隣接マスを決める
        if (curX != tx) {
            int dx = (tx > curX) ? 1 : -1;
            if (stage->GetMazeData(curX + dx, curZ) == 0) nextX = curX + dx;
        }
        if (nextX == curX && curZ != tz) {
            int dz = (tz > curZ) ? 1 : -1;
            if (stage->GetMazeData(curX, curZ + dz) == 0) nextZ = curZ + dz;
        }

        //次のマスの中心を目標としてロックする
        m_targetPos = VGet(nextX * STAGE_SCALE, 0.0f, nextZ * STAGE_SCALE);
    }

    //移動処理
    // ロックされた m_targetPos に向かって進む
    VECTOR moveVec = VSub(m_targetPos, m_pos);
    float d = VSize(moveVec);
    if (d > 0.1f) {
        float speed = 5.0f; //速度
        if (d < speed) {
            m_pos = m_targetPos; // 行き過ぎ防止
        }
        else {
            m_pos = VAdd(m_pos, VScale(VNorm(moveVec), speed));
        }

        //向きの更新
        float angle = atan2f(moveVec.x, moveVec.z);
        MV1SetRotationXYZ(modelHandle, VGet(0.0f, angle + DX_PI_F, 0.0f));
    }
}

void Enemy::UpdateEat()
{
}

void Enemy::UpdateDown()
{
    stateTimer -= 0.016f; //1フレーム(60fps)分減らす

    //ダウン中はモデルを横倒しにするデバッグ演出
    MV1SetRotationXYZ(modelHandle, VGet(DX_PI_F / 2.0f, 0, 0));

    if (stateTimer <= 0) {
        MV1SetRotationXYZ(modelHandle, VGet(0, 0, 0)); // 復帰
        state = EnemyState::Wait; //復帰後は待機へ
    }
}

void Enemy::StartAStarTest() {
    Stage* stage = FindGameObject<Stage>();
    if (!stage) return;

    //鍵の位置に移動
    this->m_pos = stage->GetKeyPosition();
    this->m_targetPos = this->m_pos;//中間目標を現在の位置にリセット
    //目標地点を出口に設定
    this->m_testTargetPos = stage->GetExitPosition();

    //跡モードに切り替えて、テストフラグを立てる
    this->state = EnemyState::Chase;
    this->m_isPathfindingTest = true;
}

void Enemy::Draw() {
    MV1DrawModel(modelHandle);
}