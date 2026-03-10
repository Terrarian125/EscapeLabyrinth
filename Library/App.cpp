#include "App.h"
#include "SceneManager.h"
#include "ObjectManager.h"
//#include "Time.h"
#include <DxLib.h>
#include "../Source/Camera.h"
#include "../Library/GameObject.h"
void AppInit()
{
	ObjectManager::Init();
	SceneManager::Init();
//	Time::Init();
	Camera* camera = new Camera();
	ObjectManager::Push(camera);
}

void AppUpdate()
{
	SceneManager::Update();
	ObjectManager::Update();
}

void AppDraw()
{
//	Time::Refresh();
	SceneManager::Draw();
	ObjectManager::Draw();

	//先にカメラの座標を適用する
	Camera* cam = FindGameObject<Camera>();
	if (cam) cam->Set();

	//その後に描画を呼ぶ
	SceneManager::Draw();
	ObjectManager::Draw();
}

void AppRelease()
{
	ObjectManager::Release();
	SceneManager::Release();
}

bool AppIsExit()
{
	return SceneManager::IsExit();
}