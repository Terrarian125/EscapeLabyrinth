#include "Animator.h"
#include "../Library/Input.h"

Animator::Animator() {}
Animator::~Animator() {
    //0以上の有効なハンドルの場合のみ削除
    if (m_ModelHandle >= 0) {
        MV1DeleteModel(m_ModelHandle);
        m_ModelHandle = -1;
    }
}

void Animator::Initialize(const std::string& charId) {
    const CharacterData* data = CharacterDataManager::GetData(charId);
    if (!data) return;

    if (m_ModelHandle != -1) MV1DeleteModel(m_ModelHandle);

    std::string modelPath = "Data\\Character\\BaseModel\\" + data->modelName;
    m_ModelHandle = MV1LoadModel(modelPath.c_str());

    std::string idlePath = "Data\\Character\\Motion\\" + data->animIdle;
    int animModel = MV1LoadModel(idlePath.c_str());

    m_AttachIndex = MV1AttachAnim(m_ModelHandle, 0, animModel);

    MV1SetPosition(m_ModelHandle, VGet(0, 0, 0));
    //初期スケール適用
    SetScale(m_ScaleX, m_ScaleY, m_ScaleZ);
}

void Animator::ChangeAnim(const std::string& animFileName) {
    if (m_ModelHandle == -1) return;
    std::string animPath = "Data\\Character\\Motion\\" + animFileName;
    int animModel = MV1LoadModel(animPath.c_str());

    if (m_AttachIndex != -1) MV1DetachAnim(m_ModelHandle, m_AttachIndex);
    m_AttachIndex = MV1AttachAnim(m_ModelHandle, 0, animModel);
}

void Animator::SetScale(float x, float y, float z) {
    m_ScaleX = x; m_ScaleY = y; m_ScaleZ = z;
    if (m_ModelHandle != -1) {
        MV1SetScale(m_ModelHandle, VGet(m_ScaleX, m_ScaleY, m_ScaleZ));
    }
}

void Animator::Update() {
    if (m_ModelHandle == -1 || m_AttachIndex == -1) return;

    m_AnimTime += 0.5f;
    float totalTime = MV1GetAttachAnimTotalTime(m_ModelHandle, m_AttachIndex);
    if (m_AnimTime >= totalTime) m_AnimTime = 0.0f;

    MV1SetAttachAnimTime(m_ModelHandle, m_AttachIndex, m_AnimTime);
}

void Animator::Draw() {
    if (m_ModelHandle == -1) return;
    MV1DrawModel(m_ModelHandle);
}