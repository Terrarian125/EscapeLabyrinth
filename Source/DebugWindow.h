#pragma once

class DebugWindow {
public:
	static void Update();

	//現在デバッグメニューが開いているかを取得
	static bool IsVisible() { return m_isOpen; }

	//マップ上に敵を表示するかどうかのフラグ
	static bool ShouldShowEnemy() { return m_showEnemyOnMap; }
private:
    static bool m_isOpen; //F1で切り替える表示フラグ
	static bool m_showEnemyOnMap;
};