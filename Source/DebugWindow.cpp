#include "DebugWindow.h"
#include "../ImGui/imgui.h"
#include "../Library/Input.h"
#include "Stage.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"

//初期化
bool DebugWindow::m_isOpen = false;
bool DebugWindow::m_showEnemyOnMap = false;
int DebugWindow::m_minimapOffsetX = 1014;
int DebugWindow::m_minimapOffsetY = 12;
int DebugWindow::m_minimapTileSize = 17;
int DebugWindow::m_minimapRange = 7;
bool DebugWindow::m_showMinimap = true;

void DebugWindow::Update() {
    //F1キーで開閉切り替え
    if (Input::IsKeyUP(KEY_INPUT_F1)) {
        m_isOpen = !m_isOpen;
    }
    if (!m_isOpen) return;

    ImGui::Begin("Debug Menu", &m_isOpen);

    //Cameraのデバッグセクションを追加
    Camera* camera = FindGameObject<Camera>();
    if (camera) {
        if (ImGui::CollapsingHeader("Camera")) {
            bool isDebug = camera->IsDebugMode();
            if (ImGui::Checkbox("Top-down Debug View (F2)", &isDebug)) {
                camera->SetDebugMode(isDebug);
            }
        }
    }
    //ミニマップ
    if (ImGui::CollapsingHeader("Minimap Options")) {
        ImGui::Checkbox("Show Minimap", &m_showMinimap);//表示・非表示を切り替え
        ImGui::Checkbox("Show Enemy on Minimap", &m_showEnemyOnMap);//ミニマップに敵をうつす
        ImGui::SliderInt("Pos X", &m_minimapOffsetX, 0, 1280);//位置変更
        ImGui::SliderInt("Pos Y", &m_minimapOffsetY, 0, 720);
        ImGui::SliderInt("Tile Size", &m_minimapTileSize, 2, 30);//マップの大きさ
        ImGui::SliderInt("View Range", &m_minimapRange, 2, 30);//描画範囲の大きさ
    }

    //Playerのデバッグ
    Player* player = FindGameObject<Player>();
    if (player) {
        if (ImGui::CollapsingHeader("Player")) {
            VECTOR pos = player->GetPosition();
            float pPos[3] = { pos.x, pos.y, pos.z };
            if (ImGui::DragFloat3("Position", pPos, 1.0f)) {
                player->SetPosition(VGet(pPos[0], pPos[1], pPos[2]));
            }
        }
    }

    //Stageのデバッグ
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        if (ImGui::CollapsingHeader("Stage")) {
            //鍵フラグ
            bool hasKey = stage->GetHasKey();
            if (ImGui::Checkbox("Has Key", &hasKey)) {
                stage->SetHasKey(hasKey);
            }

            //当たり判定のオンオフ
            bool collision = stage->GetCollisionEnable(); //stageから現在の状態をとる
            if (ImGui::Checkbox("Wall Collision", &collision)) {
                stage->SetCollisionEnable(collision); //stageの状態を書き換える
            }
        }
    }

    //Enemy
    Enemy* enemy = FindGameObject<Enemy>();
    //Stage* stage = FindGameObject<Stage>(); // 座標取得用にStageも取得

    if (enemy) {
        if (ImGui::CollapsingHeader("Enemy AI Controller")) {

            // 1. 現在のステートを文字列で分かりやすく表示
            const char* stateNames[] = { "Wait", "Roam", "Chase", "Eat", "Down" };
            int currentState = (int)enemy->GetState();
            ImGui::Text("State: %s", stateNames[currentState]);

            // 2. ステートタイマーの表示（ProgressBarにすると視覚的にわかりやすい）
            // maxを5.0fとして、残り時間を表示
            ImGui::ProgressBar(enemy->GetStateTimer() / 5.0f, ImVec2(0, 0), "Timer");

            // 3. 各ステートへの強制遷移ボタン
            if (ImGui::Button("Force WAIT")) { enemy->SetState(EnemyState::Wait); }
            ImGui::SameLine();
            if (ImGui::Button("Force ROAM")) { enemy->SetState(EnemyState::Roam); }

            if (ImGui::Button("Force CHASE")) { enemy->SetState(EnemyState::Chase); }
            ImGui::SameLine();
            if (ImGui::Button("Force DOWN")) { enemy->SetState(EnemyState::Down, 5.0f); }

            ImGui::Separator();

            // 4. A* 経路探索テストモード (鍵 -> 出口)
            if (stage) {
                if (ImGui::Button("A* Test: Key -> Exit Route")) {
                    enemy->StartAStarTest();
                }
            }

            ImGui::Separator();

            // 5. 座標操作（既存のもの）
            VECTOR ePos = enemy->GetPosition();
            float pos[3] = { ePos.x, ePos.y, ePos.z };
            if (ImGui::DragFloat3("Enemy Pos", pos, 5.0f)) {
                enemy->SetPosition(VGet(pos[0], pos[1], pos[2]));
            }
        }
    }

    ImGui::End();
}