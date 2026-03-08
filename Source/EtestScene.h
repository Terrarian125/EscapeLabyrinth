#pragma once
#pragma once
#include "../Library/SceneBase.h"

class EtestScene : public SceneBase {
public:
    EtestScene();
    virtual ~EtestScene();
    void Update() override;
    void Draw() override;
};