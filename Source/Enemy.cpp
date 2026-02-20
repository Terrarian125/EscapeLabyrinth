//#include "Enemy.h"
//
//Enemy::Enemy()
//{
//}
//
//Enemy::~Enemy()
//{
//}
//
// void Enemy::Update() {
//     float dt = Time::DeltaTime(); //Time.hの機能を利用
//
//     switch (state) {
//     case EnemyState::Chase:
//         UpdateChase();
//         //プレイヤーに接触
//         if (IsContactPlayer()) {
//             state = EnemyState::Eat;
//             stateTimer = 4.0f;
//             //Player::ReduceLife(); //ライフ減少処理
//         }
//         break;
//
//     case EnemyState::Eat:
//         stateTimer -= dt;
//         if (stateTimer <= 0) state = EnemyState::Chase;
//         break;
//
//     case EnemyState::Down:
//         stateTimer -= dt;
//         if (stateTimer <= 0) {
//             state = EnemyState::Roam;
//             ResetEyes(); //目を復活させる
//         }
//         break;
//     }
// }
//
//void Enemy::Draw()
//{
//}
//
// bool Enemy::CheckHit(VECTOR lineStart, VECTOR lineEnd) {
//     for (auto& eye : eyes) {
//         if (!eye.active) continue;
//
//         //モデルのボーン（目）の現在のワールド座標を取得
//         MATRIX m = MV1GetHierarchyWorldMatrix(modelHandle, eye.frameIndex);
//         VECTOR eyePos = VGet(m.m[3][0], m.m[3][1], m.m[3][2]);
//
//         //線分と球の当たり判定
//         if (HitCheck_Line_Sphere(lineStart, lineEnd, eyePos, eye.radius)) {
//             eye.active = false;
//             //全ての目がつぶれたかチェック
//             if (CheckAllEyesDestroyed()) {
//                 state = EnemyState::Down;
//                 stateTimer = 5.0f; //企画書の「一定時間動かない」
//             }
//             return true;
//         }
//     }
//     return false;
// }
