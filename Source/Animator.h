#pragma once
#include "../Library/GameObject.h"
#include "CharacterDataManager.h"

class Animator : public GameObject {
public:
    Animator();
    virtual ~Animator();

    void Initialize(const std::string& charId);
    void Update() override;
    void Draw() override;

    //モーション切り替え用
    void ChangeAnim(const std::string& animFileName);

    //モデルの大きさを変える時用 (3軸対応)
    void SetScale(float x, float y, float z);

private:
    int m_ModelHandle = -1;
    int m_AttachIndex = -1; //アタッチ番号
    float m_AnimTime = 0.0f;
    std::string m_CurrentCharId;
    const CharacterData* m_Data = nullptr;
    float m_ScaleX = 1.0f;
    float m_ScaleY = 1.0f;
    float m_ScaleZ = 1.0f;
};