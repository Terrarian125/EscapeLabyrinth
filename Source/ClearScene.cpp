#include "ClearScene.h"
#include "../Library/Input.h"
#include   "../Library/GuiButton.h"
#include "../Library/Input.h"

ClearScene::ClearScene()
{
	m_bgGraph = LoadGraph("Data/Image/clear_bg.png");
	m_clearGraph = LoadGraph("Data/Image/clear_text.png");
	m_clearSound = LoadSoundMem("Data/Sound/clear_bgm.wav");
	PlaySoundMem(m_clearSound, DX_PLAYTYPE_LOOP);

    int bx = 100,
        by = 300,
        bw = 300,
        bh = 60;

    auto bTback = new GuiButton(bx, by, bw, bh, "タイトルへ");
    bTback->onClick = []() { SceneManager::ChangeScene("TITLE"); };
    buttons.push_back(bTback);

    auto bContinue = new GuiButton(bx, by + 80, bw, bh, "つづきから");
    bContinue->onClick = []() { /* ロード処理 */ };
    buttons.push_back(bContinue);
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update() {
    if (Input::IsKeyUP(KEY_INPUT_R)) {
		SceneManager::ChangeScene("TITLE"); // Rキーでタイトルに戻る
    }
}

void ClearScene::Draw() {
	DrawGraph(0, 0, m_bgGraph, FALSE); //背景
	DrawGraph(400, 100, m_clearGraph, TRUE); //クリアの文字
}