#pragma once
#include "../Library/SceneBase.h"
#include "Camera.h"
#include "SettingPanel.h"
#include "Animator.h"
#include <vector>
#include <string>

class CustomCharacterScene : public SceneBase
{
public:
	CustomCharacterScene();
	~CustomCharacterScene();
	void Update() override;
	void Draw() override;

	int selectIdx = 0;
	std::vector<GuiButton*> buttons;

private:
	// キャラクター切り替え用の内部関数
	void ChangePlayerCharacter(const std::string& charId);

	// 追加したメンバ
	Animator* m_Player;
	std::string m_CurrentCharId;

	// 既存の資産
	int hModel;
	int hModel_00;
	int hModel_01;

	int hBg;
	int hSound;

	VECTOR Pos; // 配置座標
	float bgScrollX;
	Camera m_Camera;
	bool isTransitioning; // シーン遷移予約フラグ
	bool isSceneChanging = false; // シーン切り替え中フラグ
	std::vector<GuiButton*> raceSubButtons; // 種族の選択肢ボタン用

	bool  showBodyMenu = false;// 体型編集メニュー表示フラグ
	float bodyWidthX = 1.0f;
	float bodyHeight = 1.0f;
	float bodyWidthZ = 1.0f;
};