#include "EtestScene.h"
#include "Stage.h"
#include "Etest.h"
#include "../Library/ObjectManager.h"
#include "DxLib.h"

EtestScene::EtestScene() {
    //ステージを生成
    ObjectManager::Push(new Stage());
    //実験用オブジェクトを生成
    ObjectManager::Push(new Etest());
}

EtestScene::~EtestScene() {
    ObjectManager::DeleteAllGameObject();
}

void EtestScene::Update() {
    ObjectManager::Update();
    //タイトルへ戻る処理（デバッグ用）
    if (CheckHitKey(KEY_INPUT_T)) SceneManager::ChangeScene("TITLE");
}

void EtestScene::Draw() {
    ObjectManager::Draw();
}