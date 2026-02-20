#pragma once
#include "../Library/GameObject.h"

class Player : public GameObject {
public:
    Player();
    void Update() override;
    void Draw() override {}

    VECTOR GetPosition() const { return m_pos; }
    float GetAngle() const { return m_angle; }

private:
    VECTOR m_pos;   //À•W
    float m_angle;  //Œü‚«
};