#include "Camera.h"
#include <math.h>
#include "../Library/Input.h"

Camera::Camera() {
    m_Eye = VGet(0.0f, 200.0f, -500.0f);
    m_Target = VGet(0.0f, 0.0f, 0.0f);
    m_Angle = 0.0f;
}

void Camera::Update() {
    //回転処理 (Q, E, ←, →)
    if (Input::IsKeepKeyDown(KEY_INPUT_Q) || Input::IsKeepKeyDown(KEY_INPUT_LEFT)) m_Angle -= 0.05f;
    if (Input::IsKeepKeyDown(KEY_INPUT_E) || Input::IsKeepKeyDown(KEY_INPUT_RIGHT)) m_Angle += 0.05f;

    //向いている方向のベクトルを計算
    //前方向 (Forward)
    float forwardX = sinf(m_Angle);
    float forwardZ = cosf(m_Angle);
    //右方向 (Right) - 前方向を90度(PI/2)ずらしたもの
    float rightX = sinf(m_Angle + DX_PI_F / 2.0f);
    float rightZ = cosf(m_Angle + DX_PI_F / 2.0f);

    float moveSpeed = 5.0f;

    //前後移動 (W, S) 向いている方向に進む
    if (!Input::IsKeepKeyDown(KEY_INPUT_LSHIFT)) {
        if (Input::IsKeepKeyDown(KEY_INPUT_W)) {
            m_Eye.x += forwardX * moveSpeed;
            m_Eye.z += forwardZ * moveSpeed;
        }
        if (Input::IsKeepKeyDown(KEY_INPUT_S)) {
            m_Eye.x -= forwardX * moveSpeed;
            m_Eye.z -= forwardZ * moveSpeed;
        }
    }

    //左右移動 (A, D) - 向いている方向の右横に進む
    if (Input::IsKeepKeyDown(KEY_INPUT_A)) {
        m_Eye.x -= rightX * moveSpeed;
        m_Eye.z -= rightZ * moveSpeed;
    }
    if (Input::IsKeepKeyDown(KEY_INPUT_D)) {
        m_Eye.x += rightX * moveSpeed;
        m_Eye.z += rightZ * moveSpeed;
    }

    //高さ（Y軸）の変更
    float heightSpeed = 5.0f;
    if (Input::IsKeepKeyDown(KEY_INPUT_LSHIFT)) {
        if (Input::IsKeepKeyDown(KEY_INPUT_W)) m_Eye.y += heightSpeed;
        if (Input::IsKeepKeyDown(KEY_INPUT_S)) m_Eye.y -= heightSpeed;
    }
    if (Input::IsKeepKeyDown(KEY_INPUT_UP))   m_Eye.y += heightSpeed;
    if (Input::IsKeepKeyDown(KEY_INPUT_DOWN)) m_Eye.y -= heightSpeed;

    //注視点の更新
    //カメラの現在地から、向いている方向に少し進んだ場所をターゲットにする
    m_Target.x = m_Eye.x + forwardX * 100.0f;
    m_Target.z = m_Eye.z + forwardZ * 100.0f;
    m_Target.y = m_Eye.y; //水平視線を維持
}

void Camera::Set() {
    SetCameraNearFar(1.0f, 100000.0f);
    SetCameraPositionAndTarget_UpVecY(m_Eye, m_Target);
}