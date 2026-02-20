#pragma once
#include <string>
#include <vector>
#include <map>

struct CharacterData {
    std::string characterId;
    std::string jobType;
    std::string modelName;
    std::string texFolder;
    std::string voiceFolder;
    //ƒ‚[ƒVƒ‡ƒ“
    std::string animIdle;
    std::string animWalk;
    std::string animRun;
    std::string animAtkM1;
    std::string animAtkM2;
    std::string animAtkR;
    std::string animMagic;
    std::string animGuard;
    std::string animDamage;
    std::string animDead;
    std::string animAbnormal;
};

class CharacterDataManager {
public:
    static void Load(const std::string& filePath);
    static const CharacterData* GetData(const std::string& id);

private:
    static std::map<std::string, CharacterData> m_DataMap;
};