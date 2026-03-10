#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GuiButton.h"
#include "SettingPanel.h"

///<summary>
///タイトルシーン
///
///タイトルを表示して、キーを押したらプレイシーンに移行する。
///</summary>
class StageSelectScene : public SceneBase
{
public:
	StageSelectScene();
	~StageSelectScene();
	void Update() override;
	void Draw() override;
	//int hImage;
	//int LogoBg;
	//int Logo;
	int Op_Music;

	///フェード用
	float alpha;
	float fadeSpeed;
	float ChangeTimer;
	int hSound;

	int selectIdx = 0;
	std::vector<GuiButton*> buttons;

	bool screenPattern;//画面パターン選択用

	bool isExitDialogVisible = false;
	std::vector<GuiButton*> exitButtons;

private:
	std::vector<GuiButton*> menuButtons;
	int currentSelect = 0; //キーボード選択用インデックス
	SettingPanel* mySettingPanel;
};