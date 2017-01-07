#ifndef __MSWRENDERER_H__
#define __MSWRENDERER_H__

class MSWRenderer
{
private:
	static MSWRenderer* m_installer;

private:
	MSWRenderer() {}

public:
	static MSWRenderer* GetInstance();

	

	// ------------------------------ //
	// µå·ÎÀ× °ü·Ã
	// ------------------------------//
	bool BeginScene();
	void EndScene();


	// ------------------------------ //
	// Á¡ / ¼± / »ï°¢Çü 
	// ------------------------------ //
	void DrawPixel(int x, int y, int color);
	void DrawLine(int stX, int stY, int edX, int edY, int color);
	

	
};


#endif
