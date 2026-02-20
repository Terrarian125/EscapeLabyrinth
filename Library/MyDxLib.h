#include "DxLib.h"

//DrawBox3D 関数の宣言を追加
int DrawBox3D(VECTOR pos1, VECTOR pos2, unsigned int difColor, unsigned int spcColor, int fillFlag);

//3D空間に四角形ポリゴンとして画像を描画する関数
int DrawQuadGraph3D(VECTOR p1, VECTOR p2, VECTOR p3, VECTOR p4, int GrHandle, int TransFlag);

//DrawBox3D 関数の実装
int DrawBox3D(VECTOR pos1, VECTOR pos2, unsigned int difColor, unsigned int spcColor, int fillFlag) {
    //3D ボックスを描画するための実装
    DrawCube3D(pos1, pos2, difColor, spcColor, fillFlag);
    return 0; //成功を示す
}

 //画像付きの四角形（床や壁）を描画する実体
int DrawQuadGraph3D(VECTOR p1, VECTOR p2, VECTOR p3, VECTOR p4, int GrHandle, int TransFlag) {
    VERTEX3D v[6]; // 4頂点ではなく、三角形2枚分（6頂点）で確実に描く
    for (int i = 0; i < 6; i++) {
        v[i].norm = VGet(0, 1, 0);
        v[i].dif = GetColorU8(255, 255, 255, 255);
        v[i].spc = GetColorU8(0, 0, 0, 0);
    }
    // 1枚目の三角形 (p1-p2-p3)
    v[0].pos = p1; v[0].u = 0.0f; v[0].v = 0.0f;
    v[1].pos = p2; v[1].u = 1.0f; v[1].v = 0.0f;
    v[2].pos = p3; v[2].u = 1.0f; v[2].v = 1.0f;
    // 2枚目の三角形 (p3-p4-p1)
    v[3].pos = p3; v[3].u = 1.0f; v[3].v = 1.0f;
    v[4].pos = p4; v[4].u = 0.0f; v[4].v = 1.0f;
    v[5].pos = p1; v[5].u = 0.0f; v[5].v = 0.0f;

    DrawPolygon3D(v, 2, GrHandle, TransFlag); // 2ポリゴン（6頂点）描画
    return 0;
}
