#include "Stage.h"
#include "global.h"
#include "../Library/MyDxLib.h"
#include <algorithm>

Stage::Stage() {
    GenerateMaze();
    m_floorGraph = LoadGraph("Assets/floor.png");
    m_wallGraph = LoadGraph("Assets/wall.png");
}

Stage::~Stage() {}

void Stage::GenerateMaze() {
    //1. 全てを壁(1)で初期化
    for (int y = 0; y < STAGE_HEIGHT; y++) {
        for (int x = 0; x < STAGE_WIDTH; x++) {
            m_mazeData[y][x] = 1;
        }
    }

    //2. 穴掘り開始 (奇数座標 (1,1) から開始するのが定石)
    Dig(1, 1);

    //3. ループ構造の作成
    //穴掘り法だけだと「一本道（木構造）」になり逃げ場がなくなるため、
    //意図的に壁を壊して周回ルートを作ります。
    BreakWalls(12);

    //4. 敵の配置
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

void Stage::WidenPaths()//ボツ
{
    //for (int y = 1; y < STAGE_HEIGHT - 1; y++) {
    //   for (int x = 1; x < STAGE_WIDTH - 1; x++) {
    //       if (m_mazeData[y][x] == 0) { //もし通路なら
    //           //上下左右をチェックして、30%くらいの確率で道にする
    //           if (GetRand(100) < 30) m_mazeData[y + 1][x] = 0;
    //           if (GetRand(100) < 30) m_mazeData[y][x + 1] = 0;
    //       }
    //   }
    //}
}

void Stage::Update() {}

void Stage::Draw() {
    float scale = STAGE_SCALE; // global.h で定義した 400.0f
    float half = scale / 2.0f;

    // --- 光の設定（DXライブラリの正しい関数名） ---
        // 1. ライトの向き（斜め下に向かって照らす）
    SetLightDirection(VGet(-1.0f, -1.0f, -1.0f));

    // 2. 環境光（影の部分を明るくする）
    // 引数は COLOR_F 構造体なので、GetColorDiffuse を使います
   //SetLightAmbColor(GetColorDiffuse(0.5f, 0.5f, 0.5f));

    // 3. 拡散光（光が当たっている部分を明るくする）
    //SetLightDifColor(GetColorDiffuse(1.0f, 1.0f, 1.0f));

    for (int y = 0; y < STAGE_HEIGHT; y++) {
        for (int x = 0; x < STAGE_WIDTH; x++) {
            float cx = x * scale;
            float cz = y * scale;

            // 床の描画
            // Zバッファのチラつき防止のため、y座標を 0.1f だけ上げます
            DrawQuadGraph3D(
                VGet(cx - half, 0.1f, cz - half), // 左手前
                VGet(cx + half, 0.1f, cz - half), // 右手前
                VGet(cx + half, 0.1f, cz + half), // 右奥
                VGet(cx - half, 0.1f, cz + half), // 左奥
                m_floorGraph, TRUE
            );

            // --- 2. 壁の描画 ---
            if (m_mazeData[y][x] == 1) {
                // 壁は単色の箱を明るく描画（壁画像を使いたい場合はここもQuadGraph3Dを4面分書く）
                VECTOR p1 = VGet(cx - half, 0.0f, cz - half);
                VECTOR p2 = VGet(cx + half, scale, cz + half);

                // 明るいグレー、エッジを白にして視認性を確保
                DrawBox3D(p1, p2, GetColor(180, 180, 180), GetColor(255, 255, 255), TRUE);

                // もし壁にも画像を貼るなら、例えば「正面」はこのようになります
                /*
                DrawQuadGraph3D(
                    VGet(cx - half, scale, cz - half), // 上左
                    VGet(cx + half, scale, cz - half), // 上右
                    VGet(cx + half, 0.0f,  cz - half), // 下右
                    VGet(cx - half, 0.0f,  cz - half), // 下左
                    m_wallGraph, TRUE
                );
                */
            }
        }
    }
}