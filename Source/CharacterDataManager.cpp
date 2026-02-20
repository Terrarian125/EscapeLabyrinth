#include "CharacterDataManager.h"
#include <fstream>
#include <sstream>

std::map<std::string, CharacterData> CharacterDataManager::m_DataMap;

void CharacterDataManager::Load(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;

    //ヘッダーをスキップ
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> row;

        while (std::getline(ss, item, ',')) {
            row.push_back(item);
        }

        if (row.size() < 16) continue; //項目が足りない場合はスキップ

        CharacterData data;
        data.characterId = row[0];
        data.jobType = row[1];
        data.modelName = row[2];
        data.texFolder = row[3];
        data.voiceFolder = row[4];
        data.animIdle = row[5];
        data.animWalk = row[6];
        data.animRun = row[7];
        data.animAtkM1 = row[8];
        data.animAtkM2 = row[9];
        data.animAtkR = row[10];
        data.animMagic = row[11];
        data.animGuard = row[12];
        data.animDamage = row[13];
        data.animDead = row[14];
        data.animAbnormal = row[15];

        m_DataMap[data.characterId] = data;
    }
}

const CharacterData* CharacterDataManager::GetData(const std::string& id) {
    if (m_DataMap.count(id)) return &m_DataMap[id];
    return nullptr;
}