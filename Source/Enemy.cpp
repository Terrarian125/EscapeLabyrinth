#include "Enemy.h"
#include "Player.h"
#include "Stage.h"
#include "DxLib.h"
#include <cmath>

Enemy::Enemy() : modelHandle(-1), state(EnemyState::Chase), stateTimer(0.0f) {
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
    case EnemyState::Chase:
        UpdateChase();
        break;
    case EnemyState::Down:
        stateTimer -= 0.016f; //Time::DeltaTime()があれば差し替え
        if (stateTimer <= 0) state = EnemyState::Chase;
        break;
    }

    //モデルの位置を更新
    MV1SetPosition(modelHandle, m_pos);
}

void Enemy::UpdateChase() {
    Stage* stage = FindGameObject<Stage>();
    Player* player = FindGameObject<Player>();
    if (!player || !stage) return;

    //座標計算ロジック
    //現在の座標から今どのマスにいるか
    int curX = static_cast<int>((m_pos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE);
    int curZ = static_cast<int>((m_pos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE);

    //プレイヤーがどのマスにいるか
    VECTOR pPos = player->GetPosition();
    int targetX = static_cast<int>((pPos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE);
	int targetZ = static_cast<int>((pPos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE);

    //次に進むべきマスを決定（簡易A*的ロジック）
    int nextX = curX;
    int nextZ = curZ;

    //X軸の移動を試みる
    if (curX != targetX) {
        int dirX = (targetX > curX) ? 1 : -1;
        if (stage->GetMazeData(curX + dirX, curZ) == 0) { //通路なら
            nextX = curX + dirX;
        }
    }
    //Xが壁だった、またはX移動が不要ならZ軸を試みる
    if (nextX == curX && curZ != targetZ) {
        int dirZ = (targetZ > curZ) ? 1 : -1;
        if (stage->GetMazeData(curX, curZ + dirZ) == 0) { //通路なら
            nextZ = curZ + dirZ;
        }
    }

    //次のマスの中心を目標座標にする
    VECTOR nextTargetPos = VGet(nextX * STAGE_SCALE, 0.0f, nextZ * STAGE_SCALE);
    VECTOR moveVec = VSub(nextTargetPos, m_pos);

    //目標に向かって少しずつ移動
    float dist = VSize(moveVec);
    if (dist > 5.0f) {
        m_pos = VAdd(m_pos, VScale(VNorm(moveVec), 6.0f)); //速度
    }

    //向きの更新
    if (dist > 1.0f) {
        float angle = atan2f(moveVec.x, moveVec.z);
        MV1SetRotationXYZ(modelHandle, VGet(0.0f, angle + DX_PI_F, 0.0f));
    }
}

void Enemy::Draw() {
    MV1DrawModel(modelHandle);
}