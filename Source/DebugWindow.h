#pragma once
#include "Camera.h"

class DebugWindow {
public:
	static void Update();

	//現在デバッグメニューが開いているかを取得
	static bool IsVisible() { return m_isOpen; }



	static void SetCameraPtr(Camera* ptr) { m_cameraPtr = ptr; }
	static Camera* m_cameraPtr;

	//ミニマップ関連
	static bool ShouldShowEnemy() { return m_showEnemyOnMap; }	//マップ上に敵を表示するかどうかのフラグ
	static int GetMapOffsetX() { return m_minimapOffsetX; }
	static int GetMapOffsetY() { return m_minimapOffsetY; }
	static int m_minimapOffsetX;
	static int m_minimapOffsetY;
	static int GetMapSize() { return m_minimapTileSize; }	//ミニマップそのものの大きさを返す関数
	static int m_minimapTileSize;//ミニマップそのもののサイズ変更
	static int GetMapRange() { return m_minimapRange; } //描画範囲取得用
	static int m_minimapRange;
	static bool ShouldShowMinimap() { return m_showMinimap; }
	static bool m_showMinimap;
	static bool ShouldShowCollision() { return m_showCollision; }
private:
    static bool m_isOpen; //F1で切り替える表示フラグ
	static bool m_showEnemyOnMap;
	static bool m_showCollision;
};