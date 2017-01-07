#ifndef __MFRAMEWORK_H__
#define __MFRAMEWORK_H__

/*
	MFramework 
	Singletone class.
	드로잉 / 업데이트를 담당합니다.

*/


#include <Windows.h>
class MFramework
{
private:
	static  MFramework* m_instance;


private:
	MFramework();
public:
	static MFramework* GetInstance();
	
	bool SetupFramework();
	void ReleaseFramework();

	// ----------------------- //
	// Game Loop
	// ----------------------- //
	void GameLoop();

private:
	// -------------------------- //
	// Scene Render & update
	// ------------------------- //
	void SceneRender();
	void SceneUpdate();
};
#endif