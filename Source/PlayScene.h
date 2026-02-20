#pragma once
#include "../Library/SceneBase.h"
#include "global.h"

//‘O•ûéŒ¾
class Player;
class Camera;

class PlayScene : public SceneBase {
public:
    PlayScene();
    ~PlayScene();
    void Update() override;
    void Draw() override;

private:
    Player* m_player = nullptr;
    Camera* m_camera = nullptr;
};