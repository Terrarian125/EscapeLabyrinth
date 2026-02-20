#include "CustomCharacterScene.h"
#include <DxLib.h>  
#include "../Library/Input.h" 
#include "Screen.h"
#include "../ImGui/imgui.h"
#include "../Library/GuiButton.h"
#include "../Library/GameSetting.h"
#include "../Library/MyDxlib.h"
#include "CharacterDataManager.h"
#include "Animator.h"

CustomCharacterScene::CustomCharacterScene() : bgScrollX(0.0f)
{
    //CSVデータのロード (元に戻しました)
    CharacterDataManager::Load("Data\\Character\\Character.csv");

    //読み込み
    hBg = LoadGraph("Data\\Image\\Bg.jpg");
    hSound = LoadSoundMem("Data\\Music\\BGM.mp3");
    PlaySoundMem(hSound, DX_PLAYTYPE_LOOP);

    // Animatorの生成
    m_Player = new Animator();
    ChangePlayerCharacter("TEST_01");

    //ボタン配置
    int bx = 50,
        by = 20,
        bw = 150,
        bh = 60;

    auto bCC_00 = new GuiButton(bx, by, bw, bh, "種族");
    bCC_00->onClick = [this]() {
        if (m_CurrentCharId == "TEST_01") {
            ChangePlayerCharacter("NPC_SATO");
        }
        else {
            ChangePlayerCharacter("TEST_01");
        }
        };
    buttons.push_back(bCC_00);

    auto bCC_01 = new GuiButton(bx + (by + bw), by, bw, bh, "体型タイプ");
    bCC_01->onClick = []() {};
    buttons.push_back(bCC_01);

    auto bCC_02 = new GuiButton(bx + (by + bw) * 2, by, bw, bh, "顔/髪");
    bCC_02->onClick = []() {};
    buttons.push_back(bCC_02);

    auto bCC_03 = new GuiButton(bx + (by + bw) * 3, by, bw, bh, "体");
    bCC_03->onClick = [this]() {
        showBodyMenu = !showBodyMenu; // スライダー表示切替
        };
    buttons.push_back(bCC_03);

    auto bCC_04 = new GuiButton(bx + (by + bw) * 4, by, bw, bh, "追加パーツ");
    bCC_04->onClick = [this]() {};
    buttons.push_back(bCC_04);

    auto bCC_05 = new GuiButton(bx + (by + bw) * 5, by, bw, bh, "外部からインポート");
    bCC_05->onClick = [this]() {};
    buttons.push_back(bCC_05);

    auto bCC_06 = new GuiButton(bx + (by + bw) * 6, by, bw, bh, "名前をつけて完成");
    bCC_06->onClick = [this]() {};
    buttons.push_back(bCC_06);

    auto bCC_Exit = new GuiButton(bx, 620, bw, bh, "戻る");
    bCC_Exit->onClick = [this]() {SceneManager::ChangeScene("TITLE"); };
    buttons.push_back(bCC_Exit);
}

CustomCharacterScene::~CustomCharacterScene()
{
    DeleteGraph(hBg);
    DeleteSoundMem(hSound);
    // delete m_Player; // クラッシュ回避のためコメントアウト
}

void CustomCharacterScene::ChangePlayerCharacter(const std::string& charId)
{
    m_CurrentCharId = charId;
    if (m_Player) {
        m_Player->Initialize(charId);
    }
}

void CustomCharacterScene::Update()
{
    m_Camera.Update();

    bgScrollX -= 0.1f;
    if (bgScrollX <= -Screen::WIDTH) {
        bgScrollX = 0.0f;
    }

    if (m_Player) m_Player->Update();

    //Cキーによるモーション切り替え
    const CharacterData* data = CharacterDataManager::GetData(m_CurrentCharId);
    if (data && m_Player) {
        if (Input::IsKeyDown(KEY_INPUT_C)) {
            m_Player->ChangeAnim(data->animRun);
        }
        if (Input::IsKeyUP(KEY_INPUT_C)) {
            m_Player->ChangeAnim(data->animIdle);
        }
    }
}

void CustomCharacterScene::Draw()
{
    DrawExtendGraph((int)bgScrollX, 0, (int)bgScrollX + Screen::WIDTH, Screen::HEIGHT, hBg, FALSE);
    DrawExtendGraph((int)bgScrollX + Screen::WIDTH, 0, (int)bgScrollX + Screen::WIDTH * 2, Screen::HEIGHT, hBg, FALSE);

    if (hBg == -1) {
        DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, GetColor(150, 150, 150), TRUE);
    }

    m_Camera.Set();

    for (int z = 0; z < 10; z++) {
        for (int x = 0; x < 10; x++) {
            unsigned int color = ((x + z) % 2 == 0) ? GetColor(100, 100, 100) : GetColor(150, 150, 150);
            float posX = (float)x * 50.0f - 250.0f;
            float posZ = (float)z * 50.0f - 250.0f;
            DrawBox3D(VGet(posX, -1.0f, posZ), VGet(posX + 50.0f, 0.0f, posZ + 50.0f), color, GetColor(255, 255, 255), TRUE);
        }
    }

    if (m_Player) {
        m_Player->Draw();
    }

    // ボタン描画
    for (auto b : buttons) {
        if (b) b->Draw();
    }

    // Kenshi風スライダーメニュー
    if (showBodyMenu) {
		// ウィンドウのサイズ指定
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);

        // ウィンドウの出現位置
        ImGui::SetNextWindowPos(ImVec2(Screen::WIDTH - 900, 300), ImGuiCond_Once);

        ImGui::Begin("Character Body Edit", &showBodyMenu);

        ImGui::PushItemWidth(-150); // ラベルの分だけ空けて最大まで広げる

        // 横幅（肩幅など全体）
        if (ImGui::SliderFloat("Width (X)", &bodyWidthX, 0.5f, 2.0f)) {
            if (m_Player) m_Player->SetScale(bodyWidthX, bodyHeight, bodyWidthZ);
        }

        // 身長
        if (ImGui::SliderFloat("Height (Y)", &bodyHeight, 0.5f, 2.0f)) {
            if (m_Player) m_Player->SetScale(bodyWidthX, bodyHeight, bodyWidthZ);
        }

        // 厚み（Z）: 腰回りやガタイの良さを出す
        if (ImGui::SliderFloat("Thickness (Z)", &bodyWidthZ, 0.5f, 2.0f)) {
            if (m_Player) m_Player->SetScale(bodyWidthX, bodyHeight, bodyWidthZ);
        }

        ImGui::PopItemWidth();

        ImGui::Separator(); // 区切り線

        if (ImGui::Button("Reset to Default", ImVec2(120, 30))) {
            bodyWidthX = bodyHeight = bodyWidthZ = 1.0f;
            if (m_Player) m_Player->SetScale(1.0f, 1.0f, 1.0f);
        }

        ImGui::End();
    }
}