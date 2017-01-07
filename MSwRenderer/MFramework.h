#ifndef __MFRAMEWORK_H__
#define __MFRAMEWORK_H__

/*
	MFramework 
	Singletone class.
	����� / ������Ʈ�� ����մϴ�.

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