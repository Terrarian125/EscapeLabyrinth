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
    if (Input::IsKeepKeyDown(KEY_INPUT_A))  m_angle -= rotSpeed;
    if (Input::IsKeepKeyDown(KEY_INPUT_D)) m_angle += rotSpeed;

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

	//デバッグ用に上昇と下降も追加
	if (Input::IsKeepKeyDown(KEY_INPUT_UP)) {
		nextPos.y += moveSpeed;
	}
	if (Input::IsKeepKeyDown(KEY_INPUT_DOWN)) {
		nextPos.y -= moveSpeed;
	}

    //出口に到達したかチェック
    if (m_pos.x < -100.0f) {
        SceneManager::ChangeScene("CLEAR"); //クリアシーンに
    }

    //当たり判定処理
    //壁で操作が止まらないように滑らせる
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        //X方向 だけ動かしてみて、壁に当たらないかチェック
        VECTOR nextPosX = m_pos;
        nextPosX.x = nextPos.x;

        bool canMoveX = true;
        float checkX[] = { nextPosX.x + playerRadius, nextPosX.x - playerRadius, nextPosX.x, nextPosX.x };
        float checkZ_forX[] = { m_pos.z, m_pos.z, m_pos.z + playerRadius, m_pos.z - playerRadius };

        for (int i = 0; i < 4; i++) {
            int mx = static_cast<int>((checkX[i] + STAGE_SCALE / 2.0f) / STAGE_SCALE);
            int mz = static_cast<int>((checkZ_forX[i] + STAGE_SCALE / 2.0f) / STAGE_SCALE);
            if (stage->GetMazeData(mx, mz) != 0) { canMoveX = false; break; }
        }
        if (canMoveX) m_pos.x = nextPos.x;

        //Z方向 だけ動かしてみて、壁に当たらないかチェック
        VECTOR nextPosZ = m_pos;
        nextPosZ.z = nextPos.z;

        bool canMoveZ = true;
        float checkX_forZ[] = { m_pos.x + playerRadius, m_pos.x - playerRadius, m_pos.x, m_pos.x };
        float checkZ[] = { nextPosZ.z, nextPosZ.z, nextPosZ.z + playerRadius, nextPosZ.z - playerRadius };

        for (int i = 0; i < 4; i++) {
            int mx = static_cast<int>((checkX_forZ[i] + STAGE_SCALE / 2.0f) / STAGE_SCALE);
            int mz = static_cast<int>((checkZ[i] + STAGE_SCALE / 2.0f) / STAGE_SCALE);
            if (stage->GetMazeData(mx, mz) != 0) { canMoveZ = false; break; }
        }
        if (canMoveZ) m_pos.z = nextPos.z;

        //Y方向は自由（デバッグ用）
        m_pos.y = nextPos.y;
    }
}