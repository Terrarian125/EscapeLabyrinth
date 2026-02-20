#include "PlayScene.h"
#include "Player.h"
#include "Camera.h"
#include "Stage.h"
#include "../Library/Input.h"
#include "../Library/SceneManager.h"

PlayScene::PlayScene() {
    new Stage();
    m_player = new Player();
    m_camera = new Camera();
}

PlayScene::~PlayScene() {
    //CameraがGameObject継承でないならここで消す
    if (m_camera) { delete m_camera; m_camera = nullptr; }
}

void PlayScene::Update() {
    //ライトを有効化（全方位から照らす）
    //SetOutDirectLight(VGet(-1.0f, -1.0f, -1.0f)); //斜め上からの光
    //SetAmbientLight(GetColor(150, 150, 150));      //全体を明るく底上げ（環境光）

    if (!m_player || !m_camera) return;

    //カメラをプレイヤーの位置に同期
    m_camera->m_Eye = VAdd(m_player->GetPosition(), VGet(0, 40, 0));
    m_camera->m_Angle = m_player->GetAngle();

    //注視点をプレイヤーの正面に設定
    float tx = sinf(m_camera->m_Angle);
    float tz = cosf(m_camera->m_Angle);
    m_camera->m_Target = VAdd(m_camera->m_Eye, VGet(tx * 100, 0, tz * 100));

    m_camera->Set();

    if (Input::IsKeyDown(KEY_INPUT_ESCAPE)) {
        SceneManager::ChangeScene("TITLE");
    }
}

void PlayScene::Draw() {
    DrawString(10, 10, "Move: WASD / Rotate: Left,Right", GetColor(255, 255, 255));
}