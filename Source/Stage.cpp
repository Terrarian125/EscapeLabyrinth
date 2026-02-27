#include "Stage.h"
#include "global.h"
#include "../Library/MyDxLib.h"
#include <algorithm>
#include "Screen.h"
#include "Player.h"
#include "Enemy.h"
#include "DebugWindow.h"

Stage::Stage() {
    GenerateMaze();
	m_collisionEnable = true;//当たり判定は最初は有効にしておく
    m_floorGraph = LoadGraph("Data/Image/floor.png");
    m_wallGraph = LoadGraph("Data/Image/wall.png");
	m_bgGraph = LoadGraph("Data/Image/bg.png");
    m_keyGraph = LoadGraph("Data/Image/key.png"); //鍵の画像を用意
	m_seKeyGet = LoadSoundMem("Data/Sound/key_get.wav"); //鍵を拾う音
    m_hasKey = false;
    m_doorGraph = LoadGraph("Data/Image/door.png"); //閉じた扉の画像
	m_doorOpenGraph = LoadGraph("Data/Image/door_open.png"); //開いた扉の画像
	m_seDoorOpen = LoadSoundMem("Data/Sound/door_open.wav"); //扉が開く音
    //出口を (1, 1) のすぐ近くの外壁に設置
    m_exitX = 0;
    m_exitY = 1;
    m_mazeData[m_exitY][m_exitX] = 2; //出口を2として区別する

	m_compass.Init();//コンパスの初期化

    //鍵の配置ロジック
    bool placed = false;
    //右下の方から逆順にループを回して、最初に見つかった通路(0)に置く
    for (int y = STAGE_HEIGHT - 2; y > 0 && !placed; y--) {
        for (int x = STAGE_WIDTH - 2; x > 0 && !placed; x--) {
            if (m_mazeData[y][x] == 0) { //そこが通路なら
                //マスの中心に配置
                m_keyPos = VGet(x * STAGE_SCALE, 20.0f, y * STAGE_SCALE);
                placed = true;
            }
        }
    }
    PlaceEnemies();
}

Stage::~Stage() {}

void Stage::GenerateMaze() {
    //全てを壁(1)で初期化
    for (int y = 0; y < STAGE_HEIGHT; y++) {
        for (int x = 0; x < STAGE_WIDTH; x++) {
            m_mazeData[y][x] = 1;
        }
    }

    //穴掘り開始 (奇数座標 (1,1) から開始するのが定石)
    Dig(1, 1);

    //ループ構造の作成
    //穴掘り法だけだと「一本道（木構造）」になり逃げ場がなくなるため、
    //意図的に壁を壊して周回ルートを作ります。
    BreakWalls(12);
}

void Stage::Dig(int x, int y) {
    m_mazeData[y][x] = 0; //現在地を通路にする

    //掘り進める方向（上下左右）をシャッフル
    int directions[] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; i++) {
        int r = GetRand(3);
        std::swap(directions[i], directions[r]);
    }

    //方向ベクトル（2マスずつチェック）
    int dx[] = { 0, 0, 2, -2 };
    int dy[] = { 2, -2, 0, 0 };

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[directions[i]];
        int ny = y + dy[directions[i]];

        //2マス先が画面内かつ壁のままであれば、その方向を掘る
        if (nx > 0 && nx < STAGE_WIDTH - 1 && ny > 0 && ny < STAGE_HEIGHT - 1) {
            if (m_mazeData[ny][nx] == 1) {
                //間（1マス先）も通路にする
                m_mazeData[y + dy[directions[i]] / 2][x + dx[directions[i]] / 2] = 0;
                Dig(nx, ny);
            }
        }
    }
}



void Stage::BreakWalls(int breakCount) {
    int count = 0;
    while (count < breakCount) {
        //外壁（0列目、最大列目など）を避けてランダムに選択
        int rx = GetRand(STAGE_WIDTH - 3) + 1;
        int ry = GetRand(STAGE_HEIGHT - 3) + 1;

        if (m_mazeData[ry][rx] == 1) {
            //上下または左右が通路なら、ここを壊すと道が繋がる
            bool horizontal = (m_mazeData[ry][rx - 1] == 0 && m_mazeData[ry][rx + 1] == 0);
            bool vertical = (m_mazeData[ry - 1][rx] == 0 && m_mazeData[ry + 1][rx] == 0);

            if (horizontal || vertical) {
                m_mazeData[ry][rx] = 0;
                count++;
            }
        }
    }
}

void Stage::PlaceEnemies() {
    Enemy* enemy = new Enemy();

    //鍵の座標を初期位置としてセット
    enemy->SetPosition(m_keyPos);

    //ObjectManagerに登録
    ObjectManager::Push(enemy);
}

void Stage::DrawMinimap() {
    int mapSize = DebugWindow::GetMapSize();
    int range = DebugWindow::GetMapRange();
    int offsetX = DebugWindow::GetMapOffsetX();
    int offsetY = DebugWindow::GetMapOffsetY();

    Player* player = FindGameObject<Player>();
    if (!player) return;

    VECTOR pPos = player->GetPosition();

    //ワールド座標 → マス座標
    int px = static_cast<int>((pPos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE);
    int pz = static_cast<int>((pPos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE);

    //周囲描画
    for (int y = pz - range; y <= pz + range; y++) {
        for (int x = px - range; x <= px + range; x++) {

            if (x < 0 || x >= STAGE_WIDTH || y < 0 || y >= STAGE_HEIGHT)
                continue;

            int data = m_mazeData[y][x];
            unsigned int color;

            if (data == 1) {
                color = GetColor(150, 150, 150);   //壁
            }
            else if (data == 2) {
                color = GetColor(0, 255, 255);     //扉
            }
            else {
                color = GetColor(30, 30, 30);      //通路
            }

            //プレイヤーとの差分を使う
            int dx = x - px;
            int dy = y - pz;

            //Xを反転
            int drawX = offsetX + (range - dx) * mapSize;
            int drawY = offsetY + (range + dy) * mapSize;

            DrawBox(drawX, drawY,
                drawX + mapSize, drawY + mapSize,
                color, TRUE);

            DrawBox(drawX, drawY,
                drawX + mapSize, drawY + mapSize,
                GetColor(0, 0, 0), FALSE);
        }
    }

    //プレイヤーは常に中央
    int centerX = offsetX + range * mapSize + mapSize / 2;
    int centerY = offsetY + range * mapSize + mapSize / 2;

    DrawCircle(centerX, centerY, mapSize / 3,
        GetColor(255, 0, 0), TRUE);

    //鍵表示（未取得時）
    if (!m_hasKey) {
        int kx = static_cast<int>(m_keyPos.x / STAGE_SCALE);
        int kz = static_cast<int>(m_keyPos.z / STAGE_SCALE);

        int dx = kx - px;
        int dy = kz - pz;

        int drawX = offsetX + (range - dx) * mapSize;
        int drawY = offsetY + (range + dy) * mapSize;

        DrawCircle(drawX + mapSize / 2,
            drawY + mapSize / 2,
            mapSize / 4,
            GetColor(255, 255, 0),
            TRUE);
    }

    //DebugWindowのチェックボックスがONの時だけエネミーを描画
    if (DebugWindow::ShouldShowEnemy()) {
        Enemy* enemy = FindGameObject<Enemy>();
        if (enemy) {
            VECTOR ePos = enemy->GetPosition();
            //VECTOR tPos = enemy->GetTargetPos(); // 目標地点を取得
            int ex = static_cast<int>(ePos.x / STAGE_SCALE);
            int ez = static_cast<int>(ePos.z / STAGE_SCALE);

            int dx = ex - px;
            int dy = ez - pz;

            //X反転を適用した座標計算
            int drawX = offsetX + (range - dx) * mapSize;
            int drawY = offsetY + (range + dy) * mapSize;

            DrawCircle(drawX + mapSize / 2, drawY + mapSize / 2, mapSize / 3,
                GetColor(255, 0, 255), TRUE); //敵は紫
        }
    }
}

void Stage::Update() {
    //背景スクロール速度
    bgScrollX -= 0.02f;

    //画像1枚分（Screen::WIDTH）左に消えたら位置を0に戻す
    if (bgScrollX <= -Screen::WIDTH) {
        bgScrollX = 0.0f;
    }

    if (m_hasKey) return;

    Player* player = FindGameObject<Player>();
    if (player) {
        VECTOR pPos = player->GetPosition();
        //プレイヤーと鍵の距離を測る
        float dist = VSize(VSub(pPos, m_keyPos));

		//距離が近ければ鍵を拾う
        if (dist < 100.0f) {
            m_hasKey = true;
            //取得音
            PlaySoundMem(m_seKeyGet, DX_PLAYTYPE_BACK);
			PlaySoundMem(m_seDoorOpen, DX_PLAYTYPE_BACK);//扉が開く音も同時に鳴らす
        }
    }
}

void Stage::Draw() {
    //1枚目
    DrawExtendGraph((int)bgScrollX, 0, (int)bgScrollX + 1280, 720, m_bgGraph, FALSE);
    //2枚目（右側に連結）
    DrawExtendGraph((int)bgScrollX + 1280, 0, (int)bgScrollX + 2560, 720, m_bgGraph, FALSE);

    //3D描画のためのZバッファ初期化
    ClearDrawScreenZBuffer();

    float scale = STAGE_SCALE;
    float half = scale / 2.0f;

    //ライト設定
    SetLightDirection(VGet(-1.0f, -1.0f, -1.0f));
    COLOR_F amb = { 0.5f, 0.5f, 0.5f, 1.0f };
    SetLightAmbColor(amb);

    for (int y = 0; y < STAGE_HEIGHT; y++) {
        for (int x = 0; x < STAGE_WIDTH; x++) {
            float cx = x * scale;
            float cz = y * scale;

            //床の描画
            DrawQuadGraph3D(
                VGet(cx - half, 0.1f, cz - half), VGet(cx + half, 0.1f, cz - half),
                VGet(cx + half, 0.1f, cz + half), VGet(cx - half, 0.1f, cz + half),
                m_floorGraph, TRUE
            );

            //壁と扉の描画判定
            int data = m_mazeData[y][x];
            if (data == 1 || data == 2) {

                int useGraph = (data == 2) ? m_doorGraph : m_wallGraph;
                //出口かつ鍵を持っていれば描画をスキップ
                if (data == 2 && m_hasKey) {
                    useGraph = m_doorOpenGraph;
                }

                //前面
                DrawQuadGraph3D(VGet(cx - half, scale, cz - half), VGet(cx + half, scale, cz - half),
                    VGet(cx + half, 0.0f, cz - half), VGet(cx - half, 0.0f, cz - half),
                    useGraph, TRUE);

                //背面 (奥)
                DrawQuadGraph3D(VGet(cx + half, scale, cz + half), VGet(cx - half, scale, cz + half), VGet(cx - half, 0.0f, cz + half), VGet(cx + half, 0.0f, cz + half), useGraph, TRUE);
                //右面
                DrawQuadGraph3D(VGet(cx + half, scale, cz - half), VGet(cx + half, scale, cz + half), VGet(cx + half, 0.0f, cz + half), VGet(cx + half, 0.0f, cz - half), useGraph, TRUE);
                //左面
                DrawQuadGraph3D(VGet(cx - half, scale, cz + half), VGet(cx - half, scale, cz - half), VGet(cx - half, 0.0f, cz - half), VGet(cx - half, 0.0f, cz + half), useGraph, TRUE);
                //天面 (上)
                DrawQuadGraph3D(VGet(cx - half, scale, cz + half), VGet(cx + half, scale, cz + half), VGet(cx + half, scale, cz - half), VGet(cx - half, scale, cz - half), useGraph, TRUE);
            }
        }
    }

    //鍵の描画
    if (!m_hasKey) {
		static float keyRotation = 0.0f;
		keyRotation += 0.05f;//回転速度
		float size = 100.0f;//鍵の大きさ
		float height = 40.0f;//鍵の地面からの高さ
		float halfSize = size / 2.0f;//鍵の半分の大きさ
		float s = sinf(keyRotation) * halfSize;//回転に合わせて左右に揺れる量
		float c = cosf(keyRotation) * halfSize;//回転に合わせて前後に揺れる量

        VECTOR p1 = VAdd(m_keyPos, VGet(-c, size + height, -s));
        VECTOR p2 = VAdd(m_keyPos, VGet(c, size + height, s));
        VECTOR p3 = VAdd(m_keyPos, VGet(c, height, s));
        VECTOR p4 = VAdd(m_keyPos, VGet(-c, height, -s));

        DrawQuadGraph3D(p1, p2, p3, p4, m_keyGraph, TRUE);
    }

	//コンパスの描画
    Player* player = FindGameObject<Player>();
    if (player) {
        std::vector<CompassTarget> targets;

        //鍵を追加（拾っていない場合）
        if (!m_hasKey) {
            targets.push_back({ m_keyPos, m_keyGraph });
        }

        //出口を追加
        targets.push_back({ VGet(m_exitX * STAGE_SCALE, 0, m_exitY * STAGE_SCALE), m_doorGraph });

        //描画実行
        m_compass.Draw(player->GetAngle(), player->GetPosition(), targets);
    }
    if (DebugWindow::ShouldShowMinimap()) {
        DrawMinimap();
    }
}

//外部（敵のAIなど）から道かどうかを判定するための関数
int Stage::GetMazeData(int x, int y) const {
    if (!m_collisionEnable) return 0;
    if (x < 0 || x >= STAGE_WIDTH || y < 0 || y >= STAGE_HEIGHT) return 1;

    int data = m_mazeData[y][x];
    if (data == 2) {
        return m_hasKey ? 0 : 1;
    }
    return data;
}