#include "Etest.h"
#include "Stage.h"
#include "DxLib.h"
#include "Enemy.h"
#include "../Library/Input.h"

//カメラ操作用
static float g_CamX = 0.0f;
static float g_CamZ = 0.0f;
static float g_CamHeight = 8000.0f;
static bool g_CamInit = false;

Etest::Etest() {
    Reset();
}

Etest::~Etest() {
    for (auto node : m_allNodes) {
        if (node) delete node;
    }
}

void Etest::Reset() {
    m_isSearching = false; // 探索を止める

    // 各アルゴリズムのワークエリアをクリア
    while (!m_openList.empty()) m_openList.pop();
    while (!m_aStarOpenList.empty()) m_aStarOpenList.pop();
    while (!m_dfsStack.empty()) m_dfsStack.pop();

    //訪問済みマップをクリア
    m_visitedMap.clear();
    m_visitedNodes.clear();
    m_path.clear();

    // 動的メモリ一括解放
    for (auto node : m_allNodes) if (node) delete node;
    m_allNodes.clear();

    // 初期座標・カメラの再設定
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        m_startPos = stage->GetKeyPosition();
        m_goalPos = stage->GetExitPosition();

        if (!g_CamInit) {
            g_CamX = (STAGE_WIDTH * STAGE_SCALE) / 2.0f;
            g_CamZ = (STAGE_HEIGHT * STAGE_SCALE) / 2.0f;
            g_CamInit = true;
        }
    }
}

void Etest::Update() {
    // カメラ移動：WASD / ズーム：Shift + WS
    float moveSpeed = 50.0f;
    bool isShift = (Input::IsKeepKeyDown(KEY_INPUT_LSHIFT) > 0 || Input::IsKeepKeyDown(KEY_INPUT_RSHIFT) > 0);

    if (isShift) {
        if (Input::IsKeepKeyDown(KEY_INPUT_W)) g_CamHeight -= moveSpeed * 2.0f;
        if (Input::IsKeepKeyDown(KEY_INPUT_S)) g_CamHeight += moveSpeed * 2.0f;
    }
    else {
        if (Input::IsKeepKeyDown(KEY_INPUT_W)) g_CamZ += moveSpeed;
        if (Input::IsKeepKeyDown(KEY_INPUT_S)) g_CamZ -= moveSpeed;
        if (Input::IsKeepKeyDown(KEY_INPUT_A)) g_CamX -= moveSpeed;
        if (Input::IsKeepKeyDown(KEY_INPUT_D)) g_CamX += moveSpeed;
    }
    if (g_CamHeight < 100.0f) g_CamHeight = 100.0f;

    // キー入力による探索開始
    if (Input::IsKeyDown(KEY_INPUT_1)) RunSearch(AlgorithmType::BFS, isShift);
    if (Input::IsKeyDown(KEY_INPUT_2)) RunSearch(AlgorithmType::DFS, isShift);
    if (Input::IsKeyDown(KEY_INPUT_3)) RunSearch(AlgorithmType::Dijkstra, isShift);
    if (Input::IsKeyDown(KEY_INPUT_4)) RunSearch(AlgorithmType::AStar, isShift);
    if (Input::IsKeyDown(KEY_INPUT_ESCAPE)) Reset();

    // 1フレーム1ステップ実行
    if (m_isSearching) {
        if (m_currentAlgo == AlgorithmType::BFS) StepBFS();
        if (m_currentAlgo == AlgorithmType::DFS) StepDFS();
        if (m_currentAlgo == AlgorithmType::Dijkstra) StepDijkstra();
        if (m_currentAlgo == AlgorithmType::AStar) StepAStar();
    }
}

void Etest::RunSearch(AlgorithmType type, bool toEnemy) {
    Reset();
    m_currentAlgo = type;

    Stage* stage = FindGameObject<Stage>();
    if (!stage) return;

    // ターゲット設定（エネミー or 出口）
    if (toEnemy) {
        Enemy* enemy = FindGameObject<Enemy>();
        m_goalPos = enemy ? enemy->GetPosition() : stage->GetExitPosition();
    }
    else {
        m_goalPos = stage->GetExitPosition();
    }

    // グリッド変換
    int startX = (int)(m_startPos.x / STAGE_SCALE);
    int startZ = (int)(m_startPos.z / STAGE_SCALE);
    int goalX = (int)(m_goalPos.x / STAGE_SCALE);
    int goalZ = (int)(m_goalPos.z / STAGE_SCALE);

    // 開始ノード生成
    float h = (type == AlgorithmType::AStar) ? (float)(abs(startX - goalX) + abs(startZ - goalZ)) : 0.0f;
    TestNode* startNode = new TestNode{ startX, startZ, 0, h, nullptr };

    // 各種コンテナの初期化
    if (type == AlgorithmType::BFS) m_openList.push(startNode);
    else if (type == AlgorithmType::DFS) m_dfsStack.push(startNode);
    else if (type == AlgorithmType::Dijkstra || type == AlgorithmType::AStar) m_aStarOpenList.push(startNode);

    m_visitedMap[startX * 1000 + startZ] = startNode;
    m_allNodes.push_back(startNode);
    m_isSearching = true;
}

void Etest::StepBFS() {
    if (m_openList.empty()) { m_isSearching = false; return; }

    Stage* stage = FindGameObject<Stage>();
    int goalX = (int)(m_goalPos.x / STAGE_SCALE);
    int goalZ = (int)(m_goalPos.z / STAGE_SCALE);

    TestNode* current = m_openList.front();
    m_openList.pop();

    m_visitedNodes.push_back(VGet(current->x * STAGE_SCALE, 5, current->z * STAGE_SCALE));

    // ゴール到達：親を辿って経路復元
    if (current->x == goalX && current->z == goalZ) {
        for (TestNode* p = current; p; p = p->parent) m_path.push_back(VGet(p->x * STAGE_SCALE, 10, p->z * STAGE_SCALE));
        m_isSearching = false;
        return;
    }

    int dx[] = { 1, -1, 0, 0 }, dz[] = { 0, 0, 1, -1 };
    for (int i = 0; i < 4; i++) {
        int nx = current->x + dx[i], nz = current->z + dz[i];
        if (stage->GetMazeData(nx, nz) != 1) { // 侵入可能判定
            int hash = nx * 1000 + nz;
            if (m_visitedMap.find(hash) == m_visitedMap.end()) {
                TestNode* next = new TestNode{ nx, nz, 0, 0, current };
                m_openList.push(next);
                m_visitedMap[hash] = next;
                m_allNodes.push_back(next);
            }
        }
    }
}

void Etest::StepDFS() {
    if (m_dfsStack.empty()) { m_isSearching = false; return; }

    Stage* stage = FindGameObject<Stage>();
    int goalX = (int)(m_goalPos.x / STAGE_SCALE);
    int goalZ = (int)(m_goalPos.z / STAGE_SCALE);

    // LIFO：最新のノードを抽出
    TestNode* current = m_dfsStack.top();
    m_dfsStack.pop();

    m_visitedNodes.push_back(VGet(current->x * STAGE_SCALE, 5, current->z * STAGE_SCALE));

    // ゴール判定
    if (current->x == goalX && current->z == goalZ) {
        for (TestNode* p = current; p; p = p->parent) m_path.push_back(VGet(p->x * STAGE_SCALE, 10, p->z * STAGE_SCALE));
        m_isSearching = false;
        return;
    }

    // 隣接走査：未訪問ならスタックへ（探索の深さを優先）
    int dx[] = { 0, 0, 1, -1 }, dz[] = { 1, -1, 0, 0 };
    for (int i = 0; i < 4; i++) {
        int nx = current->x + dx[i], nz = current->z + dz[i];
        if (stage->GetMazeData(nx, nz) != 1) {
            int hash = nx * 1000 + nz;
            if (m_visitedMap.find(hash) == m_visitedMap.end()) {
                TestNode* next = new TestNode{ nx, nz, 0, 0, current };
                m_dfsStack.push(next);
                m_visitedMap[hash] = next;
                m_allNodes.push_back(next);
            }
        }
    }
}

void Etest::StepAStar() {
    if (m_aStarOpenList.empty()) { m_isSearching = false; return; }

    Stage* stage = FindGameObject<Stage>();
    int goalX = (int)(m_goalPos.x / STAGE_SCALE);
    int goalZ = (int)(m_goalPos.z / STAGE_SCALE);

    // スコア(f = g + h)が最小のノードを抽出
    TestNode* current = m_aStarOpenList.top();
    m_aStarOpenList.pop();

    m_visitedNodes.push_back(VGet(current->x * STAGE_SCALE, 5, current->z * STAGE_SCALE));

    if (current->x == goalX && current->z == goalZ) {
        for (TestNode* p = current; p; p = p->parent) m_path.push_back(VGet(p->x * STAGE_SCALE, 10, p->z * STAGE_SCALE));
        m_isSearching = false;
        return;
    }

    int dx[] = { 1, -1, 0, 0 }, dz[] = { 0, 0, 1, -1 };
    for (int i = 0; i < 4; i++) {
        int nx = current->x + dx[i], nz = current->z + dz[i];
        if (stage->GetMazeData(nx, nz) != 1) {
            int hash = nx * 1000 + nz;
            float newG = current->g + 1.0f; // 移動コスト累計

            // 未訪問 or 既存ルートより低コストなら更新
            if (m_visitedMap.find(hash) == m_visitedMap.end() || newG < m_visitedMap[hash]->g) {
                float h = (float)(abs(nx - goalX) + abs(nz - goalZ)); // マンハッタン距離
                TestNode* next = new TestNode{ nx, nz, newG, h, current };
                m_aStarOpenList.push(next);
                m_visitedMap[hash] = next;
                m_allNodes.push_back(next);
            }
        }
    }
}

void Etest::StepDijkstra() {
    // A*コンテナを流用（h=0として運用）
    if (m_aStarOpenList.empty()) { m_isSearching = false; return; }

    Stage* stage = FindGameObject<Stage>();
    int goalX = (int)(m_goalPos.x / STAGE_SCALE);
    int goalZ = (int)(m_goalPos.z / STAGE_SCALE);

    // 実績コスト(g)が最小のノードを抽出
    TestNode* current = m_aStarOpenList.top();
    m_aStarOpenList.pop();

    m_visitedNodes.push_back(VGet(current->x * STAGE_SCALE, 5, current->z * STAGE_SCALE));

    if (current->x == goalX && current->z == goalZ) {
        for (TestNode* p = current; p; p = p->parent) m_path.push_back(VGet(p->x * STAGE_SCALE, 10, p->z * STAGE_SCALE));
        m_isSearching = false;
        return;
    }

    int dx[] = { 1, -1, 0, 0 }, dz[] = { 0, 0, 1, -1 };
    for (int i = 0; i < 4; i++) {
        int nx = current->x + dx[i], nz = current->z + dz[i];
        if (stage->GetMazeData(nx, nz) != 1) {
            int hash = nx * 1000 + nz;
            float newG = current->g + 1.0f;

            if (m_visitedMap.find(hash) == m_visitedMap.end() || newG < m_visitedMap[hash]->g) {
                //全方位に等しく広がる
                TestNode* next = new TestNode{ nx, nz, newG, 0.0f, current };
                m_aStarOpenList.push(next);
                m_visitedMap[hash] = next;
                m_allNodes.push_back(next);
            }
        }
    }
}

void Etest::Draw() {
    // デバッグカメラ適用
    SetCameraPositionAndTargetAndUpVec(VGet(g_CamX, g_CamHeight, g_CamZ), VGet(g_CamX, 0.0f, g_CamZ), VGet(0.0f, 0.0f, 1.0f));

    // 走査済みマス
    for (const auto& v : m_visitedNodes) DrawCube3D(VAdd(v, VGet(-20, -5, -20)), VAdd(v, VGet(20, 5, 20)), GetColor(0, 100, 255), GetColor(255, 255, 255), TRUE);

    // 確定経路
    if (m_path.size() > 1) {
        for (size_t i = 0; i < m_path.size() - 1; i++) DrawLine3D(m_path[i], m_path[i + 1], GetColor(255, 0, 0));
    }

    // エネミー強調表示
    Enemy* e = FindGameObject<Enemy>();
    if (e != nullptr) { // ★ここで this(e) が nullptr でないか絶対チェック
        VECTOR ep = e->GetPosition();
        SetUseZBuffer3D(FALSE);

        DrawSphere3D(ep, 60.0f, 8, GetColor(255, 255, 0), GetColor(0, 0, 0), FALSE);
        DrawCapsule3D(ep, VAdd(ep, VGet(0, 300, 0)), 40.0f, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), FALSE);

        SetUseZBuffer3D(TRUE);
    }

    DrawFormatString(10, 100, GetColor(255, 255, 255), "1:BFS 2:DFS 3:Dijk 4:A* / ESC:Reset");
}