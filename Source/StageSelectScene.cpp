#include "StageSelectScene.h"
#include <DxLib.h>
#include "../Library/Input.h"
#include "../Library/SceneManager.h"
#include "../Library/GuiButton.h"
#include "../Library/GameSetting.h"
#include "DebugWindow.h"
#include "global.h"

StageSelectScene::StageSelectScene()
{
    Op_Music = LoadSoundMem("Data/Music/OP.mp3");
    int btnImg_bTxt = LoadGraph("data/image/btnImg_bStageTxt.png");
    int btnImg_bBack = LoadGraph("data/image/btnImg_bBack.png");
    int btnImg_bStage00 = LoadGraph("data/image/btnImg_bStage00.png");
    int btnImg_bStage01 = LoadGraph("data/image/btnImg_bStage01.png");
    int btnImg_bStage02 = LoadGraph("data/image/btnImg_bStage02.png");
    int btnImg_bStage03 = LoadGraph("data/image/btnImg_bStage03.png");

    auto bTxt = new GuiButton(240, 20, 800, 200, "難易度を選択");
    bTxt->SetImage(btnImg_bTxt);
    bTxt->onClick = [bTxt]() { bTxt->Fall(); };
    buttons.push_back(bTxt);

    //戻るボタン
    auto bBack = new GuiButton(20, 20, 120, 50, "戻る"); //小さめに左上へ
    bBack->SetImage(btnImg_bBack);
    bBack->onClick = []() { SceneManager::ChangeScene("TITLE"); };
    buttons.push_back(bBack);

    int btnCount = 4;        //配置するボタンの数
    int bw = 220;            //ボタンの幅
    int bh = 400;            //ボタンの高さ
    int startY = 250;        //Y軸の開始位置
    int screenWidth = 1280;  //画面の横幅
    int totalBtnWidth = bw * btnCount;
    int spacing = (screenWidth - totalBtnWidth) / (btnCount + 1);

    const char* labels[] = { "イージー", "ノーマル", "ハード", "ヘル" };
    //画像ハンドルを配列にまとめて、ループで順番に割り当てられるようにする
    int diffImages[] = { btnImg_bStage00, btnImg_bStage01, btnImg_bStage02, btnImg_bStage03 };

    for (int i = 0; i < btnCount; i++) {
        int bx = spacing + (bw + spacing) * i;
        auto btn = new GuiButton(bx, startY, bw, bh, labels[i]);

        //各難易度ボタンに個別の画像をセット
        btn->SetImage(diffImages[i]);

        btn->onClick = [i]() {
            // i: 0=Easy, 1=Normal, 2=Hard, 3=Hell
            g_currentDifficulty = static_cast<Difficulty>(i);

            // ミニマップの設定（DebugWindowの変数を直接書き換える）
            if (g_currentDifficulty == Difficulty::Easy) {
                DebugWindow::m_showMinimap = true;
                DebugWindow::m_minimapOffsetX = 956;
                DebugWindow::m_minimapOffsetY = 0;
                DebugWindow::m_minimapTileSize = 12;
                DebugWindow::m_minimapRange = 10;
                g_showCompass = true; // コンパスON
            }
            else if (g_currentDifficulty == Difficulty::Normal) {
                DebugWindow::m_showMinimap = true;
                DebugWindow::m_minimapOffsetX = 1014;
                DebugWindow::m_minimapOffsetY = 12;
                DebugWindow::m_minimapTileSize = 17;
                DebugWindow::m_minimapRange = 7;
				g_showCompass = true; // コンパスON
            }
            else {
                // Hard, Hell は表示なし
                DebugWindow::m_showMinimap = false;
                g_showCompass = false;
            }

            SceneManager::ChangeScene("PLAY");
            };
        buttons.push_back(btn);
    }
}

StageSelectScene::~StageSelectScene()
{
}

void StageSelectScene::Update()
{
}

void StageSelectScene::Draw()
{
}
