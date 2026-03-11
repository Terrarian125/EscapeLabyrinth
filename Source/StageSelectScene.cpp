#include "StageSelectScene.h"
#include <DxLib.h>
#include "../Library/Input.h"
#include "../Library/SceneManager.h"
#include "../Library/GuiButton.h"
#include "../Library/GameSetting.h"

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
            //GameSetting::SetDifficulty(i); //難易度設定
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
