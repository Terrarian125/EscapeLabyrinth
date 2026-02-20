#include "Player.h"
#include "../Library/Input.h"
#include <math.h>
#include "global.h"
#include "Stage.h"

Player::Player() {
	float scale = STAGE_SCALE;//1マスのサイズ（単位はワールド座標）
    //最初の通路(1,1)の中心に配置（1マス100単位と想定）
    m_pos = VGet(STAGE_SCALE, 50.0f, STAGE_SCALE); //(1, 1)マスの中心
    m_angle = 0.0f;
}

void Player::Update() {
    float rotSpeed = 0.05f;
    float moveSpeed = 9.0f; //スケール400に合わせて速めに設定
    float playerRadius = 50.0f; //プレイヤーの当たり判定の大きさ（半径）

    //旋回
    if (Input::IsKeepKeyDown(KEY_INPUT_LEFT))  m_angle -= rotSpeed;
    if (Input::IsKeepKeyDown(KEY_INPUT_RIGHT)) m_angle += rotSpeed;

    //移動ベクトルの計算
    float nx = sinf(m_angle);
    float nz = cosf(m_angle);

    VECTOR nextPos = m_pos; //次の移動候補座標

    if (Input::IsKeepKeyDown(KEY_INPUT_W)) {
        nextPos.x += nx * moveSpeed;
        nextPos.z += nz * moveSpeed;
    }
    if (Input::IsKeepKeyDown(KEY_INPUT_S)) {
        nextPos.x -= nx * moveSpeed;
        nextPos.z -= nz * moveSpeed;
    }

    //当たり判定処理
    //Stageクラスを探してマップデータを参照する
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        //ワールド座標(float)をマップのインデックス(int)に変換
        //STAGE_SCALE (400) で割り、四捨五入（+0.5f）してマス目を特定
        int mapX = static_cast<int>((nextPos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE);
        int mapZ = static_cast<int>((nextPos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE);

        //移動先のマスが壁(1)でないなら、座標を更新する
        if (stage->GetMazeData(mapX, mapZ) == 0) {
            m_pos = nextPos;
        }
        //壁だった場合は m_pos を更新しない（＝止まる）
    }
}