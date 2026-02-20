#include "Stage.h"
#include "global.h"
#include "../Library/MyDxLib.h"
#include <algorithm>
#include "Screen.h"
#include "Player.h"

Stage::Stage() {
    GenerateMaze();
    m_floorGraph = LoadGraph("Data/Image/floor.png");
    m_wallGraph = LoadGraph("Data/Image/wall.png");
	m_bgGraph = LoadGraph("Data/Image/bg.png");
    m_keyGraph = LoadGraph("Data/Image/key.png"); //鍵の画像を用意
    m_hasKey = false;

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

    //敵の配置
    PlaceEnemies();
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
    //企画書：出口当たりの中腹ルートに配置
    //ステージ右下領域 (半分より右・下) を検索
    Rect area = { STAGE_WIDTH / 2, STAGE_HEIGHT / 2, STAGE_WIDTH / 2 - 2, STAGE_HEIGHT / 2 - 2 };

    int attempts = 0;
    while (attempts < 100) {
        int rx = GetRand(area.w) + area.x;
        int ry = GetRand(area.h) + area.y;

        if (m_mazeData[ry][rx] == 0) {
            //VECTOR pos = TileToWorld(rx, ry);
            //new Enemy(pos); //ここで敵を生成
            break;
        }
        attempts++;
    }
}

void Stage::DrawMinimap() {
    int mapSize = 15;
    int range = 7;
    int offsetX = 1100;
    int offsetY = 100;

    Player* player = FindGameObject<Player>();
    if (!player) return;

    VECTOR pPos = player->GetPosition();
    //座標からインデックスを計算
    int px = static_cast<int>((pPos.x + STAGE_SCALE / 2.0f) / STAGE_SCALE);
    int pz = static_cast<int>((pPos.z + STAGE_SCALE / 2.0f) / STAGE_SCALE);

    //壁と通路を描画
    for (int y = pz - range; y <= pz + range; y++) {
        for (int x = px - range; x <= px + range; x++) {
            if (x < 0 || x >= STAGE_WIDTH || y < 0 || y >= STAGE_HEIGHT) continue;

            unsigned int color = (m_mazeData[y][x] == 1) ? GetColor(150, 150, 150) : GetColor(30, 30, 30);

            int drawX = offsetX + (x - (px - range)) * mapSize;
            int drawY = offsetY + (y - (pz - range)) * mapSize;

            DrawBox(drawX, drawY, drawX + mapSize, drawY + mapSize, color, TRUE);
            DrawBox(drawX, drawY, drawX + mapSize, drawY + mapSize, GetColor(0, 0, 0), FALSE);
        }
    }

    //プレイヤーを描画
    //中心位置を計算
    int centerX = offsetX + range * mapSize + mapSize / 2;
    int centerY = offsetY + range * mapSize + mapSize / 2;

    //プレイヤーを赤い円で表示
    DrawCircle(centerX, centerY, mapSize / 3, GetColor(255, 0, 0), TRUE);

    //鍵を拾っていなければ、ミニマップに「黄色い〇」を出す
    if (!m_hasKey) {
        //ワールド座標からマスのインデックス(x, y)を正確に逆算
        int kx = static_cast<int>(m_keyPos.x / STAGE_SCALE);
        int kz = static_cast<int>(m_keyPos.z / STAGE_SCALE);

        //壁の描画ループで使っている「drawX/Y」と同じ計算式を使う
        int drawX = offsetX + (kx - (px - range)) * mapSize;
        int drawY = offsetY + (kz - (pz - range)) * mapSize;

        //マスの中心に点を出すために mapSize/2 を足す
        DrawCircle(drawX + mapSize / 2, drawY + mapSize / 2, mapSize / 4, GetColor(255, 255, 0), TRUE);
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

        //50ユニット以内に近づいたら拾ったことにする
        if (dist < 50.0f) {
            m_hasKey = true;
            //取得音
			//PlaySoundMem(LoadSoundMem("key_get.mp3"), DX_PLAYTYPE_BACK);
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

    //ライト設定（前回同様）
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

            //壁の描画
            if (m_mazeData[y][x] == 1) {
                //前面 (手前)
                DrawQuadGraph3D(
                    VGet(cx - half, scale, cz - half), VGet(cx + half, scale, cz - half),
                    VGet(cx + half, 0.0f, cz - half), VGet(cx - half, 0.0f, cz - half),
                    m_wallGraph, TRUE
                );
                //背面 (奥)
                DrawQuadGraph3D(
                    VGet(cx + half, scale, cz + half), VGet(cx - half, scale, cz + half),
                    VGet(cx - half, 0.0f, cz + half), VGet(cx + half, 0.0f, cz + half),
                    m_wallGraph, TRUE
                );
                //右面
                DrawQuadGraph3D(
                    VGet(cx + half, scale, cz - half), VGet(cx + half, scale, cz + half),
                    VGet(cx + half, 0.0f, cz + half), VGet(cx + half, 0.0f, cz - half),
                    m_wallGraph, TRUE
                );
                //左面
                DrawQuadGraph3D(
                    VGet(cx - half, scale, cz + half), VGet(cx - half, scale, cz - half),
                    VGet(cx - half, 0.0f, cz - half), VGet(cx - half, 0.0f, cz + half),
                    m_wallGraph, TRUE
                );
                //天面 (上)
                DrawQuadGraph3D(
                    VGet(cx - half, scale, cz + half), VGet(cx + half, scale, cz + half),
                    VGet(cx + half, scale, cz - half), VGet(cx - half, scale, cz - half),
                    m_wallGraph, TRUE
                );
            }
        }
    }
    if (!m_hasKey) {
        //ちょっと回転させたり上下させると「アイテム感」が出ます
        DrawBillboard3D(m_keyPos, 0.5f, 0.5f, 50.0f, 0.0f, m_keyGraph, TRUE);
    }
    DrawMinimap();
}