#pragma once
#include "../Library/SceneBase.h"
#include "../Library/GuiButton.h"

class ClearScene : public SceneBase {
public:
	ClearScene();
	~ClearScene();
    void Update() override;
    void Draw() override;

	std::vector<GuiButton*> buttons;
private:
	int m_bgGraph;//”wŒi
	int m_clearGraph;//ƒNƒŠƒA‚Ì•¶š
	int m_clearSound;//BGM
};