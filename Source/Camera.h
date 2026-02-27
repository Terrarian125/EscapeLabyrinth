#pragma once
#include <DxLib.h>
#include "../Library/GameObject.h"

class Camera : public GameObject {
public:
    Camera();
    void Update() override; //入力による移動などを書く
    void Set();    //実際にDXLibのカメラを適用する
    VECTOR m_Eye;    //位置
    VECTOR m_Target; //注視点
    float  m_Angle;  //回転角度（ラジアン）

    void SetDebugMode(bool sw) { m_IsDebugMode = sw; }
    bool IsDebugMode() const { return m_IsDebugMode; }
private:
    bool m_IsDebugMode = false;//デバッグ見下ろしカメラ用
};