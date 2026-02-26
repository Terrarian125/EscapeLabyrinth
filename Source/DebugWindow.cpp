#include "DebugWindow.h"
#include "../ImGui/imgui.h"
#include "../Library/Input.h"
#include "Stage.h"
#include "Player.h"
#include "Enemy.h"

bool DebugWindow::m_isOpen = false;
bool DebugWindow::m_showEnemyOnMap = false;

void DebugWindow::Update() {
    //F1キーで開閉切り替え
    if (Input::IsKeyUP(KEY_INPUT_F1)) {
        m_isOpen = !m_isOpen;
    }

    if (!m_isOpen) return;

    ImGui::Begin("Debug Menu", &m_isOpen);

    //ミニマップ
    if (ImGui::CollapsingHeader("Minimap Options")) {
        ImGui::Checkbox("Show Enemy on Minimap", &m_showEnemyOnMap);
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
    if (enemy) {
        if (ImGui::CollapsingHeader("Enemy")) {

            //現在のステートを表示
            const char* stateNames[] = { "Wait", "Chase", "Roam", "Down", "Eat" };
            ImGui::Text("State: %s", stateNames[(int)enemy->GetState()]);

            ImGui::Text("Current State: %d", (int)enemy->GetState());
            if (ImGui::Button("Force Down")) {
                //enemy->SetState(EnemyState::Down); //Enemy側にSetterが必要
            }

            //座標操作
            VECTOR ePos = enemy->GetPosition();
            float pos[3] = { ePos.x, ePos.y, ePos.z };
            if (ImGui::DragFloat3("Enemy Pos", pos, 1.0f)) {
                enemy->SetPosition(VGet(pos[0], pos[1], pos[2]));
            }
        }
    }

    ImGui::End();
}