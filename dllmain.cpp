#include <vector>
#include <d3d9.h>
#include <d3dx9core.h>
#include "detours.h"
#include "HackProcess.h"
#include "CECross.h"
#include "ModelRecs.h"

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")
#pragma warning( disable : 4101 )
#pragma warning( disable : 4244 )

#define PATT_VTABLE		"\xAD\x7D\x28\x71\xB1\x5D\x28\x71\x8D\x5D\x28\x71\xC1\x3B\x2A\x71"
#define MASK_VTABLE		"xxxxxxxxxxxxxxxx"

#define RED(COLORCODE)			((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)			((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)		((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)		((int) COLORCODE & 0xFF )
#define COLORCODE(r,g,b,a)		((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))
#define RGBA(colorcode)			RED(colorcode), GREEN(colorcode), BLUE(colorcode), ALPHA(colorcode)

#define MENU_GOLD COLORCODE(218, 165, 32, 255)
#define CHEAT_ORANGE    COLORCODE( 255, 100, 000, 255 )
#define CHEAT_YELLOW    COLORCODE( 255, 255, 0, 255 )
#define CHEAT_PURPLE    COLORCODE( 55, 25, 128, 255 )
#define CHEAT_RED       COLORCODE( 255, 0, 0, 255 )
#define CHEAT_GREEN		COLORCODE( 0, 255, 0, 255 )
#define CHEAT_BLUE		COLORCODE( 0, 0, 255, 255 )
#define CHEAT_BLACK		COLORCODE( 0, 0, 0, 255 )
#define CHEAT_WHITE		COLORCODE( 255, 255, 255, 255 )
#define CHEAT_GREY		COLORCODE( 120, 120, 120, 255 )

using namespace std;

typedef HRESULT(WINAPI* pDrawIndexedPrimitive)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, int, UINT, UINT, UINT, UINT);
typedef HRESULT(WINAPI* pEndScene)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* pReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* pSetRenderState)(LPDIRECT3DDEVICE9, D3DRENDERSTATETYPE, DWORD);
typedef HRESULT(WINAPI* pCreateQuery)(LPDIRECT3DDEVICE9, D3DQUERYTYPE ,IDirect3DQuery9**);

pDrawIndexedPrimitive oDrawIndexedPrimitive;
pEndScene oEndScene;
pReset oReset;
pSetRenderState oSetRenderState;
pCreateQuery oCreateQuery;

D3DVIEWPORT9 vpt;

LPDIRECT3DTEXTURE9 blue = NULL;
LPDIRECT3DTEXTURE9 green = NULL; 
LPDIRECT3DTEXTURE9 pTx = NULL;
LPDIRECT3DTEXTURE9 pTxB = NULL;

LPDIRECT3DTEXTURE9 yellow = NULL;
LPDIRECT3DTEXTURE9 red = NULL; 
LPDIRECT3DTEXTURE9 pTx1 = NULL;
LPDIRECT3DTEXTURE9 pTxY = NULL;

LPDIRECT3DTEXTURE9 white = NULL; 
LPDIRECT3DTEXTURE9 pTx_white = NULL;
D3DLOCKED_RECT d3dlr_white;

LPDIRECT3DTEXTURE9 glass = NULL; 
LPDIRECT3DTEXTURE9 pTx_glass = NULL;
D3DLOCKED_RECT d3dlr_glass;

LPDIRECT3DTEXTURE9 hands = NULL;
LPDIRECT3DTEXTURE9 pTx_hands = NULL;
D3DLOCKED_RECT d3dlr_hands;

D3DLOCKED_RECT d3dlr;
D3DLOCKED_RECT d3dlr1;
D3DLOCKED_RECT d3dlrB;
D3DLOCKED_RECT d3dlrY;

LPDIRECT3DPIXELSHADER9 re_blue;

//IDirect3DDevice9* Device;

//IDirect3DPixelShader9;

//ID3DXLine* p_Line;
LPD3DXLINE p_Line = NULL;
LPD3DXFONT pFont = NULL;
char strbuff[260];
char strbuff2[260];

//int crosshairToggle = 0;
bool crosshairToggle = true;
bool wallHack = true;
bool gotMethods = false;
bool AsusHack = false;
bool bunnyhop = false;
bool esp = false;

bool jumped;
bool legit_bhop_enabled = true;

bool shad = true;

const DWORD playerbase = 0x541C94;
const DWORD mflags = 0x34C;

int SRCBLENDINT=5; 
int DESTBLENDINT=2;

int leet = 1;

char value1[250];
char value2[250];

#define F6_Key 0x75

#define tt_col_novis D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 100 + 0, rand() % 30 + 0, 255)
#define tt_col_vis D3DCOLOR_RGBA(rand() % 245 + 0, rand() % 255 + 0, rand() % 46 + 0, 255)

#define ct_col_novis D3DCOLOR_RGBA(rand() % 53 + 0, rand() % 46 + 0, rand() % 255 + 0, 255)
#define ct_col_vis D3DCOLOR_RGBA(rand() % 46 + 0, rand() % 255 + 0, rand() % 63 + 0, 255)

//Shaders
IDirect3DPixelShader9* RedShader;
IDirect3DPixelShader9* GreenShader;
IDirect3DPixelShader9* YellowShader;
IDirect3DPixelShader9* BlueShader;
IDirect3DPixelShader9* WhiteShader;
IDirect3DPixelShader9* PurpleShader;
IDirect3DPixelShader9* CyanShader;
IDirect3DPixelShader9* Invis;
//IDirect3DPixelShader9* Walls;

RECT Rect1;
bool clicking;
bool holdclicking;
//int cursorx, cursory;

POINT Pos;
RECT wRect;
bool showMenu = false;
int cursorx, cursory;

BYTE oldcspos[3];
BYTE UnlockMouse[3] = {0xC2, 0x08, 0x00};

bool SpriteCreated1 = false;
bool SpriteCreated2 = false;
LPDIRECT3DTEXTURE9 IMAGE1;
LPD3DXSPRITE SPRITE1 = NULL;
LPDIRECT3DTEXTURE9 IMAGE2;
LPD3DXSPRITE SPRITE2 = NULL;
IDirect3DPixelShader9 *sAdd = NULL;
IDirect3DPixelShader9 *sAdd2 = NULL;
char ShaderAdd[] = "ps.1.0 def c0, 0.0f, 0.0f, 0.0f, 0.0f tex t0 add r0, c0, t0";
char ShaderAdd2[] = "ps.2.0 def c0, 0.0f, 0.0f, 0.0f, 0.0f tex t0 add r0, c0, t0";
ID3DXBuffer *ShaderBufferAdd = NULL;
ID3DXBuffer *ShaderBufferAdd2 = NULL;
LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9 pStateBlock2 = NULL;
float pSize = 40; //pic size
float pSize2 = 40; //pic size

int crosshaire = 1;

DWORD HUILO;

CHackProcess fProcess;

int MenyX = 120;
int MenyY = 120;

//==========CVars==========
	bool esp_active, esp_name, esp_health, esp_steamid, esp_distance, esp_enemyonly, esp_visibilty, esp_2dbox, esp_3dbox, esp_playerlight;
	bool esp_plus, esp_square;

	bool misc_bunnyhop, misc_namesteal;

	//float rem_recoil = 0;
	//float rem_spread = 0;

	bool tabAim = false;
	bool tabESP = false;
	bool tabMis = false;
	bool tabCre = false;
	bool IsActive = false;

	int MenuX = 120;
	int MenuY = 120;
//==========CVars==========



bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for( ; *szMask; ++szMask, ++pData, ++bMask )
			if( *szMask == 'x' && *pData != *bMask )
				return false;

	return ( *szMask ) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
	for( DWORD i = NULL; i < dwLen; i++ )
		if( bDataCompare( (BYTE*) ( dwAddress + i ), bMask, szMask ) )
			return (DWORD)( dwAddress + i );

	return 0;
}

void GetOldBytes()
{
  HMODULE hMyMod = NULL;
                        hMyMod = GetModuleHandle("USER32.dll");

                        if(hMyMod)
                        {
                            DWORD dwTemp = NULL;
                            dwTemp =    (DWORD)GetProcAddress(hMyMod, "SetCursorPos");

                            if(dwTemp)
                            {
                                DWORD dwProtect;
                                VirtualProtect((void*)dwTemp, 3, PAGE_EXECUTE_READWRITE, &dwProtect);
                                memcpy(oldcspos, (void*)dwTemp, 3);
                                VirtualProtect((void*)dwTemp, 3, dwProtect, &dwProtect);
                            }
                        }
}

void hkSetCursorPos(bool lock)
    {
        if(!lock)
        {
                HMODULE hMyMod = NULL;
                        hMyMod = GetModuleHandle("USER32.dll");

                        if(hMyMod)
                        {
                            DWORD dwTemp = NULL;
                            dwTemp =    (DWORD)GetProcAddress(hMyMod, "SetCursorPos");

                            if(dwTemp)
                            {
                                DWORD dwProtect;
                                VirtualProtect((void*)dwTemp, 3, PAGE_EXECUTE_READWRITE, &dwProtect);
                                memcpy((void*)dwTemp, &UnlockMouse, 3);
                                VirtualProtect((void*)dwTemp, 3, dwProtect, &dwProtect);
                            }
                        }
        }
        else
        {
        HMODULE hMyMod = NULL;
                        hMyMod = GetModuleHandle("USER32.dll");

                        if(hMyMod)
                        {
                            DWORD dwTemp = NULL;
				 
                            dwTemp =    (DWORD)GetProcAddress(hMyMod, "SetCursorPos");

                            if(dwTemp)
                            {
                                DWORD dwProtect;
                                VirtualProtect((void*)dwTemp, 3, PAGE_EXECUTE_READWRITE, &dwProtect);
                                memcpy((void*)dwTemp, &oldcspos, 3);
								
                                VirtualProtect((void*)dwTemp, 3, dwProtect, &dwProtect);
                            }
                        }
        }
    }

int isMouseIn(int x, int y, int width, int height)
{
    POINT Mouse;
    GetCursorPos(&Mouse);

//
	cursorx = Mouse.x - 05 - Rect1.left; 
	cursory = Mouse.y - 30 - Rect1.top; 

    if(GetAsyncKeyState(0x1)&1)
    {
        clicking = true;
    }
    else
    {
        clicking = false;
    }
    if(GetAsyncKeyState(0x1))
    {
        holdclicking = true;
    }
    else
    {
        holdclicking = false;
    }
//

    if (Mouse.x > x + Rect1.left && Mouse.y > y + Rect1.top)
    {
        if (Mouse.x < x + Rect1.left + width && Mouse.y < y + height + Rect1.top)
        {
            return true;
        }
    }
    return false;
} 

POINT Clicking() 
{ 
	int cursorx, cursory; //dont need to be global
    POINT Pos;
    HWND hWnd = GetActiveWindow(); //use it in a class, so you dont need to get it always
	
    if(GetCursorPos(&Pos))
	{
		if(ScreenToClient(hWnd,&Pos))
		{
			cursorx = Pos.x - 05;//why u use -5 & -30?
			cursory = Pos.y - 30;
		}
	}
    if(GetAsyncKeyState(VK_LBUTTON)) 
    { 
      	Pos.x = cursorx;
		Pos.y = cursory;
		return Pos;
    } 
    else 
    { 
		Pos.x = -1;
		Pos.y = -1;
		return Pos;
    } 
}

//bool GenerateCham(IDirect3DDevice9 * pDevInter, LPDIRECT3DTEXTURE9 * texture, DWORD colorARGB);

HRESULT GenerateShader( LPDIRECT3DDEVICE9 pDev, IDirect3DPixelShader9** Shader, float r, float g, float b)
//HRESULT GenerateShader( IDirect3DDevice9* Device, IDirect3DPixelShader9** Shader, float r, float g, float b)
{
	char ShaderAsmBuf[256];
	ID3DXBuffer* ShaderBuf = NULL;
	//sprintf_s(ShaderAsmBuf, "ps_2_0\ndef c0, %f, %f, %f, %f\nmov oC0,c0", r, g, b, 1.0f);
	sprintf_s( ShaderAsmBuf, "ps.1.1\ndef c0, %f, %f, %f, %f\nmov r0,c0", r, g, b, 1.0f );
	if(FAILED(D3DXAssembleShader(ShaderAsmBuf, (strlen(ShaderAsmBuf)+1), NULL, NULL, 0, &ShaderBuf, NULL)))
	{
		return E_FAIL;
	}
	return pDev->CreatePixelShader((const DWORD*)ShaderBuf->GetBufferPointer(), Shader);
}

HRESULT GenerateShader2( LPDIRECT3DDEVICE9 pDev, IDirect3DPixelShader9** Shader, float r, float g, float b, float a)
//HRESULT GenerateShader( IDirect3DDevice9* Device, IDirect3DPixelShader9** Shader, float r, float g, float b)
{
	char ShaderAsmBuf[256];
	ID3DXBuffer* ShaderBuf = NULL;
	//sprintf_s(ShaderAsmBuf, "ps_2_0\ndef c0, %f, %f, %f, %f\nmov oC0,c0", r, g, b, a);
	sprintf_s( ShaderAsmBuf, "ps.1.1\ndef c0, %f, %f, %f, %f\nmov r0,c0", r, g, b, a);
	if(FAILED(D3DXAssembleShader(ShaderAsmBuf, (strlen(ShaderAsmBuf)+1), NULL, NULL, 0, &ShaderBuf, NULL)))
	{
		return E_FAIL;
	}
	return pDev->CreatePixelShader((const DWORD*)ShaderBuf->GetBufferPointer(), Shader);
}

HRESULT GenerateShader3(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DPixelShader9 **pShader, float r, float g, float b, float a)
{
    char szShader[256];
    ID3DXBuffer *pShaderBuf = NULL;
    sprintf_s(szShader, "ps_3_0\ndef c0, %f, %f, %f, %f\nmov oC0, c0", r, g, b, a);
    D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);

	if(FAILED(D3DXAssembleShader(szShader, (strlen(szShader)+1), NULL, NULL, 0, &pShaderBuf, NULL)))
	{
		return E_FAIL;
	}
	return pD3Ddev->CreatePixelShader((const DWORD*)pShaderBuf->GetBufferPointer(), pShader);
}  

void Shaders(LPDIRECT3DDEVICE9 pDev)
{
	GenerateShader(pDev, &RedShader, 1.0f, 0.0f, 0.0f);
	GenerateShader(pDev, &GreenShader, 0.3f, 1.0f, 0.0f);
	GenerateShader(pDev, &BlueShader, 0.0f, 0.0f, 1.0f);
	GenerateShader(pDev, &YellowShader, 1.0f, 1.0f, 0.0f);
	GenerateShader(pDev, &WhiteShader, 1.0f, 1.0f, 1.0f);
	GenerateShader(pDev, &PurpleShader, 1.0f, 0.0f, 1.0f);
	GenerateShader(pDev, &CyanShader, 0.0f, 1.0f, 1.0f);
	//GenerateShader2(pDev, &Invis, 1.0f, 1.0f, 1.0f, 0.0f);
	//GenerateShader2(pDev, &Walls, 1.0f, 1.0f, 1.0f, wallls);
}

void DrawLine(float x, float y, float xx, float yy, int r, int g, int b, int a)
{
	D3DXVECTOR2 dLine[2];

	p_Line->SetWidth( 1 );

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	p_Line->Draw( dLine, 2, D3DCOLOR_ARGB(a, r , g, b));

}

void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a) 
{ 
    D3DXVECTOR2 vLine[2]; 

    p_Line->SetWidth( w ); 
     
    vLine[0].x = x + w/2; 
    vLine[0].y = y; 
    vLine[1].x = x + w/2; 
    vLine[1].y = y + h; 

    p_Line->Begin( ); 
    p_Line->Draw( vLine, 2, D3DCOLOR_RGBA( r, g, b, a ) ); 
    p_Line->End( ); 
} 

void DrawBox(float x, float y, float width, float height, float px, int r, int g, int b, int a) 
{ 
	D3DXVECTOR2 points[5];  
	points[0] = D3DXVECTOR2(x,		y);  
	points[1] = D3DXVECTOR2(x+width,	y);  
	points[2] = D3DXVECTOR2(x+width,	y+height);  
	points[3] = D3DXVECTOR2(x,		y+height);  
	points[4] = D3DXVECTOR2(x,		y);  
	p_Line->SetWidth(1);
	p_Line->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
} 

void DrawRadarBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa) 
{ 
    DrawBox(x,y,w,h, 1,r ,g ,b ,a);    
    FillRGB(x+1,y+1,w-1,h-1,rr,gg,bb,aa);
	DrawLine((x+w),(y+h),x,y,r,g,b,a);
	DrawLine((x+w),y,x,(y+h),r,g,b,a);
	//DrawLine((x+w),(y+h),x,y,0,0,0,255);
	//DrawLine((x+w),y,x,(y+h),0,0,0,255);
}

void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa) 
{ 
    DrawBox(x,y,w,h, 1,r ,g ,b ,a);    
    FillRGB(x+1,y+1,w-1,h-1,rr,gg,bb,aa);   
}

void DrawGUIBox2(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa) 
{
    POINT Mous_pos;

	//bool isclicked;

	if(isMouseIn(x, y, w, h))
	{
		DrawBox(x ,y ,w ,h ,1 ,r ,g ,b ,a);
	}
	else
	{
		DrawBox(x,y,w,h, 1,r ,255 ,b ,a);
	}
    FillRGB(x+1,y+1,w-1,h-1,rr,gg,bb,aa);
}

void ShadowBox(float x, float y, float width, float height, int r, int g, int b)
{
	D3DXVECTOR2 points[5];

	points[0] = D3DXVECTOR2(x - 1, y - 1);
	points[1] = D3DXVECTOR2(x + width + 1, y - 1);
	points[2] = D3DXVECTOR2(x + width + 1, y + height + 1);
	points[3] = D3DXVECTOR2(x - 1, y + height + 1);
	points[4] = D3DXVECTOR2(x - 1, y - 1);

	p_Line->SetAntialias(false);
	p_Line->SetWidth(1);

	p_Line->Draw(points, 5, D3DCOLOR_ARGB(255, 0, 0, 0));

	points[0] = D3DXVECTOR2(x + 1, y + 1);
	points[1] = D3DXVECTOR2(x + width - 1, y + 1);
	points[2] = D3DXVECTOR2(x + width - 1, y + height - 1);
	points[3] = D3DXVECTOR2(x + 1, y + height - 1);
	points[4] = D3DXVECTOR2(x + 1, y + 1);

	p_Line->SetAntialias(false);
	p_Line->SetWidth(1);

	p_Line->Draw(points, 5, D3DCOLOR_ARGB(255, 0, 0, 0));

	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);

	p_Line->SetAntialias(false);
	p_Line->SetWidth(1);

	p_Line->Draw(points, 5, D3DCOLOR_ARGB(255, r, g, b));
}

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev) 
{ 
    //We create our rectangle to draw on screen 
    D3DRECT BarRect = { x, y, x + w, y + h };  
    //We clear that portion of the screen and display our rectangle 
    dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0); 
} 

void DrawBorderBox( int x, int y, int w, int h, int thickness, D3DCOLOR Colour, IDirect3DDevice9 *pDevice) 
{ 
    //Top horiz line 
    DrawFilledRect( x, y, w, thickness,  Colour, pDevice ); 
    //Left vertical line 
    DrawFilledRect( x, y, thickness, h, Colour, pDevice ); 
    //right vertical line 
    DrawFilledRect( (x + w), y, thickness, h, Colour, pDevice ); 
    //bottom horiz line 
    DrawFilledRect( x, y + h, w+thickness, thickness, Colour, pDevice ); 
}

void DrawRectA(IDirect3DDevice9* Unidade, int baseX, int baseY, int baseW, int baseH, D3DCOLOR Cor)
{
	D3DRECT BarRect = { baseX, baseY, baseX + baseW, baseY + baseH }; 
	Unidade->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Cor, 0,  0); 
}

int DrawShadowString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	RECT Fonts;
	Fonts.left = x+1;
	Fonts.top = y;
	RECT Fonts1;
	Fonts1.left = x-1;
	Fonts1.top = y;
	RECT Fonts2;
	Fonts2.left = x;
	Fonts2.top = y+1;
	RECT Fonts3;
	Fonts3.left = x;
	Fonts3.top = y-1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP , D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}

void SteamBase(IDirect3DDevice9* Unidade, int baseX, int baseY, int baseW, int baseH, D3DCOLOR Linha2, D3DCOLOR Linha1, D3DCOLOR Linha3)
{
	DrawRectA(Unidade, baseX+1, baseY+1, baseW+1, baseH+1, Linha1); 

	DrawRectA(Unidade, baseX, baseY-1, baseW+1 , baseH, Linha2);

	DrawRectA(Unidade, baseX+1, baseY, baseW , baseH , Linha3);
}

void SteamFundo(IDirect3DDevice9* Unidade, char *Texto,int baseX, int baseY, int baseW, int baseH, D3DCOLOR Linha2, D3DCOLOR Linha1, D3DCOLOR Linha3)
{
	DrawRectA(Unidade, baseX+1, baseY+1, baseW+1, baseH+1, Linha1); 

	DrawRectA(Unidade, baseX, baseY-1, baseW+1 , baseH, Linha2);

	DrawRectA(Unidade, baseX+1, baseY, baseW , baseH , Linha3);

	DrawShadowString(Texto, baseX + baseW - 170, baseY+1, 210, 210, 210, pFont);
}

void SteamBotao(IDirect3DDevice9* Unidade,  char *Texto,int baseX, int baseY, int baseW, int baseH, D3DCOLOR Linha2, D3DCOLOR Linha1, D3DCOLOR Linha3)
{
	DrawRectA(Unidade, baseX+1, baseY+1, baseW+1, baseH+1, Linha1); 

	DrawRectA(Unidade, baseX, baseY-1, baseW+1 , baseH, Linha2);

	DrawRectA(Unidade, baseX+1, baseY, baseW , baseH , Linha3);

	DrawShadowString(Texto, baseX + baseW - 65, baseY+4, 210, 210, 210, pFont);
}

void SteamCheckBox(IDirect3DDevice9* Unidade,  char *Texto,char *Texto1,int baseX, int baseY, int baseW, int baseH, D3DCOLOR Linha2, D3DCOLOR Linha1, D3DCOLOR Linha3)
{
	DrawRectA(Unidade, baseX+1, baseY+1, baseW+1, baseH+1, Linha1); 

	DrawRectA(Unidade, baseX, baseY-1, baseW+1 , baseH, Linha2);

	DrawRectA(Unidade, baseX+1, baseY, baseW , baseH , Linha3);

	DrawShadowString(Texto, baseX + baseW - 10, baseY, 255, 255, 255, pFont);
	DrawShadowString(Texto1, baseX + baseW + 10, baseY+1, 255, 255, 255, pFont);
}

void DrawMouse(int mousex, int mousey)
{
	FillRGB(mousex - 1, mousey - 1, 1, 12, 255, 255, 255, 255);
	FillRGB(mousex, mousey - 1, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex, mousey, 1, 10, 0, 0, 0, 255);
	FillRGB(mousex, mousey + 10, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 1, mousey, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 1, mousey + 1, 1, 8, 0, 0, 0, 255);
	FillRGB(mousex + 1, mousey + 9, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 2, mousey + 1, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 2, mousey + 2, 1, 6, 0, 0, 0, 255);
	FillRGB(mousex + 2, mousey + 8, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 3, mousey + 2, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 3, mousey + 3, 1, 6, 0, 0, 0, 255);
	FillRGB(mousex + 3, mousey + 9, 1, 2, 255, 255, 255, 255);
	FillRGB(mousex + 4, mousey + 3, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 4, mousey + 4, 1, 7, 0, 0, 0, 255);
	FillRGB(mousex + 4, mousey + 11, 1, 2, 255, 255, 255, 255);
	FillRGB(mousex + 5, mousey + 4, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 5, mousey + 5, 1, 2, 0, 0, 0, 255);
	FillRGB(mousex + 5, mousey + 7, 1, 2, 255, 255, 255, 255);
	FillRGB(mousex + 5, mousey + 9, 1, 4, 0, 0, 0, 255);
	FillRGB(mousex + 5, mousey + 13, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 6, mousey + 5, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 6, mousey + 6, 1, 1, 0, 0, 0, 255);
	FillRGB(mousex + 6, mousey + 7, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 6, mousey + 9, 1, 2, 255, 255, 255, 255);
	FillRGB(mousex + 6, mousey + 11, 1, 2, 0, 0, 0, 255);
	FillRGB(mousex + 6, mousey + 13, 1, 1, 255, 255, 255, 255);
	FillRGB(mousex + 7, mousey + 6, 1, 2, 255, 255, 255, 255);
	FillRGB(mousex + 7, mousey + 11, 1, 2, 255, 255, 255, 255);
}

struct menu{
	int BaseX;
	int BaseY;
	int BaseW;
	int BaseH;
} Janela;

struct TestMenu{
	int X;
	int Y;
	int W;
	int H;
} Menyu;

bool MouseClick(int x, int y, int w, int h)
{
	POINT Pos;
	RECT mRect;
	HWND hWnd = ::GetActiveWindow();

	GetWindowRect(hWnd, (LPRECT)&mRect);
	GetCursorPos(&Pos);
	int cursorx1 = Pos.x - 05 - mRect.left;
	int cursory1 = Pos.y - 30 - mRect.top;
	bool Clicked;

	if (cursorx1 > x && cursorx1 < x + w && cursory1 > y && cursory1 < y + h && GetAsyncKeyState(VK_LBUTTON) & 1)
		Clicked = true;
	else
		Clicked = false;

	return Clicked;
}

//=-===========================================
int DrawString(int x, int y, DWORD dwColor, char* String, ID3DXFont* ifont)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	RECT Fonts;
	Fonts.left = x+1;
	Fonts.top = y;
	RECT Fonts1;
	Fonts1.left = x-1;
	Fonts1.top = y;
	RECT Fonts2;
	Fonts2.left = x;
	Fonts2.top = y+1;
	RECT Fonts3;
	Fonts3.left = x;
	Fonts3.top = y-1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP , D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, RED(dwColor), GREEN(dwColor), BLUE(dwColor)));
	return 0;
}

void FillRGBA(float x, float y, float w, float h, DWORD dwColor)
{ 
    D3DXVECTOR2 vLine[2]; 

    p_Line->SetWidth( w ); 
     
    vLine[0].x = x + w/2; 
    vLine[0].y = y; 
    vLine[1].x = x + w/2; 
    vLine[1].y = y + h; 

    p_Line->Begin( ); 
    p_Line->Draw( vLine, 2, D3DCOLOR_RGBA( RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor) ) ); 
    p_Line->End( ); 
} 

void Border(int x, int y, int w, int h, DWORD dwColor)
{
	FillRGBA(x - 1, y - 1, w + 2, 1, dwColor);
	FillRGBA(x - 1, y, 1, h - 1, dwColor); 
	FillRGBA(x + w, y, 1, h - 1, dwColor);
	FillRGBA(x - 1, y + h - 1, w + 2, 1, dwColor);
}

void AddTab(int x, int y, int w, int h, DWORD dwColor, DWORD oColor, char* tText)
{
	FillRGBA(x, y, w, h, dwColor);
	Border(x, y, w, h, CHEAT_BLACK);
	DrawString(x + 65, y + 15, oColor, tText, pFont);
}

void CloseButton(int x, int y, int w, int h, DWORD dwColor)
{
	FillRGBA(x, y, w, h, dwColor);
	DrawString(x + 7, y + 2, CHEAT_WHITE, "X", pFont);


	if (MouseClick(x, y, w, h))
	{
		IsActive = !IsActive;
	}
}

void DrawListbox(int x, int y, int w, int h, int letters, DWORD dwColor, DWORD textColor, char* fmt)
{
	FillRGBA(x, y, w, h, CHEAT_GREY);
	Border(x, y, w, h, CHEAT_BLACK);
	DrawString(x, y - 15, textColor, fmt, pFont);
}

void DrawCheckBox(int x, int y, char* fmt, bool &option)
{
	POINT Pos;
	RECT mRect;
	HWND hWnd = ::GetActiveWindow();

	GetWindowRect(hWnd, (LPRECT)&mRect);
	GetCursorPos(&Pos);

	int cursorx1 = Pos.x - 05 - mRect.left;
	int cursory1 = Pos.y - 30 - mRect.top;

	Border(x, y, 15, 15, MENU_GOLD);
	DrawString(x + 35, y, CHEAT_WHITE, fmt, pFont);


	if (cursorx1 > x && cursorx1 < x + 15 && cursory1 > y && cursory1 < y + 15)
	{
		if (GetAsyncKeyState(0x01) & 1)
		{
			option = !option;
		}
	}
	if (option)
	{
		//gDrawing.DrawString(x + 4, y, COLORCODE(180, 190, 191, 255), "X");
		FillRGBA(x + 1, y + 1, 13, 12, MENU_GOLD);
	}
}

void DrawButton(int x, int y, int w, int h, char* tText, char* action)
{
	POINT Pos;
	RECT mRect;
	HWND hWnd = ::GetActiveWindow();

	GetWindowRect(hWnd, (LPRECT)&mRect);
	GetCursorPos(&Pos);

	int cursorx = Pos.x - 05 - mRect.left;
	int cursory = Pos.y - 30 - mRect.top;

	FillRGBA(x, y, w, h, CHEAT_GREY);
	Border(x, y, w, h, CHEAT_BLACK);
	DrawString(x + w / 2, y + 5, CHEAT_WHITE, tText, pFont);
	if (cursorx > x && cursory > y  && cursorx < x + w && cursory < y + h)
	{
		Border(x, y, w, h, CHEAT_WHITE);
		if (GetAsyncKeyState(0x01) & 1)
		{
			if (action == "SAVE")
			{

			}

			if (action == "LOAD")
			{

			}

			if (action == "BEEP")
			{
				Beep(500, 750);
			}
		}
	}
}
//=-===========================================

bool MouseIn(int x, int y, int w, int h)
{
	POINT Cur1;
	GetCursorPos(&Cur1);

	if (Cur1.x > x && Cur1.x < x + w && Cur1.y > y && Cur1.y < y + h)
	{
		return true;
	}

	return false;
}

void GetMouse()
{
	POINT Cur;
	GetCursorPos(&Cur);
	FillRGBA(Cur.x, Cur.y, 5, 5, CHEAT_BLACK);

	POINT Pos1;
	RECT mRect;
	HWND hWnd = GetActiveWindow();

	GetWindowRect(hWnd, (LPRECT)&mRect);
	GetCursorPos(&Pos1);
	int cursorx1 = Pos.x - 05 - mRect.left;
	int cursory1 = Pos.y - 30 - mRect.top;

	DrawString(cursorx1, cursory1, CHEAT_WHITE, "X", pFont);
}

bool Move[8][2];
POINT Cursor;
bool Mover(int &X, int &Y, int W, int H, int index)
{
	for (int i = 0; i < 8; i++)
		if (Move[i][0] && i != index)
			return false;

	POINT Pos1;
	RECT mRect;
	HWND hWnd = ::GetActiveWindow();

	GetWindowRect(hWnd, (LPRECT)&mRect);
	GetCursorPos(&Pos1);
	int cursorx1 = Pos1.x - 05 - mRect.left;
	int cursory1 = Pos1.y - 30 - mRect.top;

	if (GetAsyncKeyState(VK_LBUTTON) < 0)
	{
		if (cursorx1 > X && cursorx1 < X + W && cursory1 >(Y - 20 - 3) && cursory1 < (Y - 20 - 3) + H || Move[index][0])
		{
			Move[index][0] = true;

			if (!Move[index][1])
			{
				Cursor.x = cursorx1 - X;
				Cursor.y = cursory1 - Y;
				Move[index][1] = true;

			}
		}
		else
		{
			Move[index][0] = false;
			Move[index][1] = false;
		}
	}

	if (GetAsyncKeyState(VK_LBUTTON) == 0 && Move[index][0])
	{
		Move[index][0] = false;
		Move[index][1] = false;
	}

	if (Move[index][0])
	{
		X = cursorx1 - Cursor.x;
		Y = cursory1 - Cursor.y;
	}

	return true;
}

void TabOperator(int x, int y, int w, int h)
{
	/*if (MouseClick(x + 5, y + 70, 230, 70))
	{
		tabAim = !tabAim;
		tabESP = false;
		tabMis = false;
		tabCre = false;
	}*/

	if (MouseClick(x + 5, y + 150, 230, 70))
	{
		tabAim = false;
		tabESP = !tabESP;
		tabMis = false;
		tabCre = false;
	}
	/*if (MouseClick(x + 5, y + 230, 230, 70))
	{
		tabAim = false;
		tabESP = false;
		tabCre = false;
		tabMis = !tabMis;
	}

	if (MouseClick(x + 5, y + 310, 230, 70))
	{
		tabAim = false;
		tabESP = false;
		tabMis = false;
		tabCre = !tabCre;
	}*/


	/*if (tabAim)
	{
		tabMis = false;
		tabESP = false;
		tabCre = false;
		DrawListbox(x + 240, y + 70, 350, 310, 0, COLORCODE(180, 190, 191, 255), CHEAT_WHITE, "");
	}*/

	if (tabESP)
	{
		tabAim = false;
		tabMis = false;
		tabCre = false;
		DrawListbox(x + 240, y + 70, 350, 310, 0, COLORCODE(180, 190, 191, 255), CHEAT_WHITE, "");

		DrawCheckBox(x + 250, y + 80, "Chams", wallHack);
		DrawCheckBox(x + 250, y + 100, "Asus WH", AsusHack);
		DrawCheckBox(x + 250, y + 120, "Pseudo ESP", esp);
		DrawCheckBox(x + 250, y + 160, "BunnyHop", bunnyhop);
		DrawCheckBox(x + 250, y + 200, "Plus Cross", esp_plus);
		DrawCheckBox(x + 250, y + 220, "Square Cross", esp_square);
	}

	/*if (tabMis)
	{
		tabAim = false;
		tabESP = false;
		tabCre = false;
		DrawListbox(x + 240, y + 70, 350, 310, 0, COLORCODE(180, 190, 191, 255), CHEAT_WHITE, "");
		
		DrawCheckBox(x + 250, y + 80, "Bunny Hop", misc_bunnyhop);
		DrawCheckBox(x + 250, y + 100, "Name Stealer", misc_namesteal);
	}

	if (tabCre)
	{
		tabAim = false;
		tabESP = false;
		tabMis = false;
		DrawListbox(x + 240, y + 70, 350, 310, 0, COLORCODE(180, 190, 191, 255), CHEAT_WHITE, "");
		DrawString(x + 250, y + 70, CHEAT_GREEN, "Credits:", pFont);
		DrawString(x + 250, y + 85, CHEAT_GREEN, "Horizon		- Project Creator", pFont);
		DrawString(x + 250, y + 100, CHEAT_GREEN, "Sasha		- Coding Help", pFont);
		DrawString(x + 250, y + 115, CHEAT_GREEN, "Element		- Menu Help", pFont);
	}*/
}

void RenderTab(int x, int y, int w, int h)
{
	//DrawListbox(x + 2, y + 2, w / 4, h - 4, 0, COLORCODE(180, 190, 191, 255), CHEAT_WHITE, "");//Не трогать
	//AddTab(x + 5, y + 70, 230, 70, CHEAT_BLACK, MENU_GOLD, "Aimbot");
	AddTab(x + 5, y + 150, 230, 70, CHEAT_BLACK, MENU_GOLD, "Visuals");
	//AddTab(x + 5, y + 230, 230, 70, CHEAT_BLACK, MENU_GOLD, "Extras");
	//AddTab(x + 5, y + 310, 230, 70, CHEAT_BLACK, MENU_GOLD, "Credits");
}

void MenuRender(int x, int y, int w, int h, char* lbl)
{
	FillRGBA(x, y, w, h, MENU_GOLD); // Main BG
	FillRGBA(x, y, w, 60, CHEAT_BLACK); // 
	DrawString(x + w / 1.5 - 14, y + 9, MENU_GOLD, lbl, pFont);

	//GetMouse();

	TabOperator(x, y, w, h);
	RenderTab(x, y, w, h);
	Mover(MenuX, MenuY, w, h, 0);
	DrawButton(x + 100, y + 30, 50, 24, "l33t", "BEEP");
}

__inline void MenuTest()
{
	if(GetAsyncKeyState(VK_DELETE)&1)
		IsActive = !IsActive;

	if (IsActive)
		MenuRender(MenuX, MenuY, 600, 390, "TestHook");
}


__inline void DoMenu(LPDIRECT3DDEVICE9 pDev)
{
	HWND hWnd = GetActiveWindow();

	Menyu.X = 120;//MenyX
	Menyu.Y = 120;//MenyY
	Menyu.W = 100;
	Menyu.H = 40;

	//Mover(MenyX, MenyY, 506, 20, 0);

    if(GetAsyncKeyState(VK_DELETE)&1)
    {
        showMenu = !showMenu;
    }

    if(showMenu)
    {
		//Header
		//DrawGUIBox(Menyu.X, Menyu.Y - 21, 403, 20, 255, 100, 0, 255, 100, 100, 100, 150);
		DrawGUIBox(Menyu.X - 1, Menyu.Y - 21, 506, 62, 255, 100, 0, 255, 100, 100, 100, 150);

		DrawGUIBox(Menyu.X + 485, Menyu.Y - 19, 18, 17, 255, 0, 255, 255, 100, 100, 100, 150);

		//DrawShadowString("D3DHook", Menyu.X + (506/3), Menyu.Y - 20, 255, 255, 255, pFont);//Watermark
        //Our ghetto menu item
		DrawGUIBox(Menyu.X, Menyu.Y, Menyu.W, Menyu.H, 255, 0, 0, 255, 100, 100, 100, 150);
		if(wallHack)
		{
			DrawShadowString("Chams", Menyu.X + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else
		{
			DrawShadowString("Chams", Menyu.X + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 255, 0, 0, pFont);
		}
//================================================================================================================
		DrawGUIBox(Menyu.X + Menyu.W + 1, Menyu.Y, Menyu.W, Menyu.H, 255, 0, 0, 255, 100, 100, 100, 150);
		if(crosshaire == 0)
		{
			DrawShadowString("Crosshair: off", Menyu.X + Menyu.W - 15 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 255, 0, 0, pFont);
		}
		else if(crosshaire == 1)
		{
			DrawShadowString("Crosshair: 1", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else if(crosshaire == 2)
		{
			DrawShadowString("Crosshair: 2", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0,255, 0, pFont);
		}
		else if(crosshaire == 3)
		{
			DrawShadowString("Crosshair: 3", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else if(crosshaire == 4)
		{
			DrawShadowString("Crosshair: 4", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else if(crosshaire == 5)
		{
			DrawShadowString("Crosshair: 5", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else if(crosshaire == 6)
		{
			DrawShadowString("Crosshair: 6", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else if(crosshaire == 7)
		{
			DrawShadowString("Crosshair: 7", Menyu.X + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
//================================================================================================================
		DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + 2, Menyu.Y, Menyu.W, Menyu.H, 255, 0, 0, 255, 100, 100, 100, 150);
		if(AsusHack)
		{
			DrawShadowString("Asus WH", Menyu.X + Menyu.W + Menyu.W + 1 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else
		{
			DrawShadowString("Asus WH", Menyu.X + Menyu.W + Menyu.W + 1 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 255, 0, 0, pFont);
		}
//================================================================================================================
		DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + Menyu.W + 3, Menyu.Y, Menyu.W, Menyu.H, 255, 0, 0, 255, 100, 100, 100, 150);
		if(bunnyhop)
		{
			DrawShadowString("Bunny Hop", Menyu.X + Menyu.W + Menyu.W + Menyu.W + 1 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else
		{
			DrawShadowString("Bunny Hop", Menyu.X + Menyu.W + Menyu.W + Menyu.W + 1 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 255, 0, 0, pFont);
		}
//================================================================================================================
		DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4, Menyu.Y, Menyu.W, Menyu.H, 255, 0, 0, 255, 100, 100, 100, 150);
		if(esp)
		{
			DrawShadowString("Pseudo ESP", Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 0, 255, 0, pFont);
		}
		else
		{
			DrawShadowString("Pseudo ESP", Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W - 10 + (Menyu.W / 4), Menyu.Y + (Menyu.H / 4), 255, 0, 0, pFont);
		}
        //DrawFilledRect(60, 60, 20, 20, 0xFFFFFF00, pDev);

/*									DEBUG									*/
		/*
		DrawGUIBox(20, 16, 20, 20, 255, 255, 0, 255, 0, 0, 0, 0);
		DrawShadowString("x+", 20, 16, 255, 255, 255, pFont);

		DrawGUIBox(20, 38, 20, 20, 255, 255, 0, 255, 0, 0, 0, 0);
		DrawShadowString("x+", 20, 38, 255, 255, 255, pFont);

		DrawGUIBox(20, 60, 20, 20, 255, 255, 0, 255, 0, 0, 0, 0);
		DrawShadowString("x+", 20, 60, 255, 255, 255, pFont);

		DrawGUIBox(20, 82, 20, 20, 255, 255, 0, 255, 0, 0, 0, 0);
		DrawShadowString("x+", 20, 82, 255, 255, 255, pFont);

		if(cursorx > 20 && cursory > 16
			&& cursorx < 20 + 20 && cursory < 16 + 20)
		{
			DrawGUIBox(20, 16, 20, 20, 0, 255, 0, 255, 0, 0, 0, 0);
		}
 
		if(cursorx > 20 && cursory > 38
			&& cursorx < 20 + 20 && cursory < 38 + 20)
		{
			DrawGUIBox(20, 38, 20, 20, 0, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > 20 && cursory > 60
			&& cursorx < 20 + 20 && cursory < 60 + 20)
		{
			DrawGUIBox(20, 60, 20, 20, 0, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > 20 && cursory > 82
			&& cursorx < 20 + 20 && cursory < 82 + 20)
		{
			DrawGUIBox(20, 82, 20, 20, 0, 255, 0, 255, 0, 0, 0, 0);
		}
		*/
/*									DEBUG									*/
 
        /* This is to correct for a problem with getcursorpos and windowed mode */
        GetWindowRect(hWnd, &wRect);
 
        //Cursor pos (Pos.x/Pos.y)
        GetCursorPos(&Pos);
 
        //Windowed correction
        cursorx = Pos.x - wRect.left - 03;
        cursory = Pos.y - wRect.top - 26;
 
        //Draw cursor
		DrawMouse( cursorx, cursory);
		/*DrawRectA(pDev, cursorx, cursory - 5, 1, 11, D3DCOLOR_RGBA(255, 0, 0, 255));
		DrawRectA(pDev, cursorx - 5, cursory, 11, 1, D3DCOLOR_RGBA(255, 0, 0, 255));
		DrawRectA(pDev, cursorx - 1, cursory  - 6, 3,1, D3DCOLOR_RGBA(255, 0 ,0 ,255));
		DrawRectA(pDev, cursorx - 1, cursory  + 6, 3,1, D3DCOLOR_RGBA(255, 0 ,0 ,255) );
		DrawRectA(pDev, cursorx - 1, cursory  - 8, 3,1, D3DCOLOR_RGBA(255, 0 ,0 ,255) );
		DrawRectA(pDev, cursorx - 1, cursory  + 8, 3,1, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 2, cursory  - 8, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 2, cursory  + 6, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 2, cursory  - 8, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 2, cursory  + 6, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 6, cursory  - 1, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 6, cursory  - 1, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 8, cursory  - 1, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 8, cursory  - 1, 1,3, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 8, cursory  - 2, 3,1, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 6, cursory  - 2, 3,1, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx - 8, cursory  + 2, 3,1, D3DCOLOR_RGBA(255, 0, 0, 255) );
		DrawRectA(pDev, cursorx + 6, cursory  + 2, 3,1, D3DCOLOR_RGBA(255, 0, 0, 255) );*/


		/*if(cursorx > Menyu.X - 1 && cursory > Menyu.Y - 21
			&& cursorx < Menyu.X + 405 && cursory < Menyu.Y + 62)
		{
			DrawGUIBox(Menyu.X - 1, Menyu.Y - 21, 405, 62, 255, 0, 255, 255, 0, 0, 0, 0);
		}*/
		if(cursorx > Menyu.X && cursory > Menyu.Y
			&& cursorx < Menyu.X + Menyu.W && cursory < Menyu.Y + Menyu.H)
		{
			DrawGUIBox(Menyu.X, Menyu.Y, Menyu.W, Menyu.H, 255, 255, 0, 255, 0, 0, 0, 0);
		}
 
		if(cursorx > Menyu.X + Menyu.W + 1 && cursory > Menyu.Y
			&& cursorx < Menyu.X + Menyu.W + Menyu.W + 1 && cursory < 160)
		{
			DrawGUIBox(Menyu.X + Menyu.W + 1, Menyu.Y, Menyu.W, Menyu.H, 255, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > Menyu.X + Menyu.W + Menyu.W + 2 && cursory > Menyu.Y
			&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + 2 && cursory < 160)
		{
			DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + 2, Menyu.Y, Menyu.W, Menyu.H, 255, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > Menyu.X + Menyu.W + Menyu.W + Menyu.W + 3 && cursory > Menyu.Y
			&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 3 && cursory < 160)
		{
			DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + Menyu.W + 3, Menyu.Y, Menyu.W, Menyu.H, 255, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4 && cursory > Menyu.Y
			&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4 && cursory < 160)
		{
			DrawGUIBox(Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4, Menyu.Y, Menyu.W, Menyu.H, 255, 255, 0, 255, 0, 0, 0, 0);
		}

		if(cursorx > Menyu.X + 485 && cursory > Menyu.Y - 19
			&& cursorx < Menyu.X + 485 + 18 && cursory < Menyu.Y - 19 + 17)
		{
			DrawGUIBox(Menyu.X + 485, Menyu.Y - 19, 18, 17, 0, 255, 255, 255, 0, 0, 0, 0);
			DrawShadowString("Close Menu", cursorx + 20, cursory - 10, 255, 255, 255, pFont);
		}

		/*if(GetAsyncKeyState(0x2))
		{
			//Menyu.X - 1, Menyu.Y - 21, 405, 62

			if(cursorx > Menyu.X - 1 && cursory > Menyu.Y - 21
				&& cursorx < Menyu.X + 405 && cursory < Menyu.Y + 62)
			{
				Menyu.X = Menyu.X + cursorx;
				Menyu.Y = Menyu.Y + cursory;
			}
		}*/

        //Mouse 1
        if(GetAsyncKeyState(0x1)&1)
        {
            if(cursorx > Menyu.X && cursory > Menyu.Y
                && cursorx < Menyu.X + Menyu.W && cursory < Menyu.Y + Menyu.H)
            {
				wallHack = !wallHack;
                //Toggle/Action here
            }
            if(cursorx > Menyu.X + Menyu.W + 1 && cursory > Menyu.Y
                && cursorx < Menyu.X + Menyu.W + Menyu.W + 1 && cursory < 160)
            {
				crosshaire++;
				if(crosshaire > 7)
				{
					crosshaire = 0;
				}
				//crosshairToggle = !crosshairToggle;
                //Toggle/Action here
            }
			if(cursorx > Menyu.X + Menyu.W + Menyu.W + 2 && cursory > Menyu.Y
				&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + 2 && cursory < 160)
			{
				AsusHack = !AsusHack;
			}
			if(cursorx > Menyu.X + Menyu.W + Menyu.W + Menyu.W + 3 && cursory > Menyu.Y
				&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 3 && cursory < 160)
			{
				bunnyhop = !bunnyhop;
			}
			if(cursorx > Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4 && cursory > Menyu.Y
				&& cursorx < Menyu.X + Menyu.W + Menyu.W + Menyu.W + Menyu.W + Menyu.W + 4 && cursory < 160)
			{
				esp = !esp;
			}
			if(cursorx > Menyu.X + 485 && cursory > Menyu.Y - 19
				&& cursorx < Menyu.X + 485 + 18 && cursory < Menyu.Y - 19 + 17)
			{
				showMenu = !showMenu;
			}
        }
    }
}

HRESULT WINAPI hkSetRenderState(LPDIRECT3DDEVICE9 pDev, D3DRENDERSTATETYPE State, DWORD Value)
    {
        __asm nop

        
        if(State == D3DRS_FOGENABLE){
            Value = false;
        }

        if(State == D3DRS_RANGEFOGENABLE){
            Value = false;
        }
        
        if(State == D3DRS_FOGCOLOR){
            Value = D3DCOLOR_RGBA(0,0,0,0);
        }
        
        if(State == D3DRS_FOGTABLEMODE){
            Value = D3DFOG_NONE;
        }
        
        if(State == D3DRS_FOGVERTEXMODE){
            Value = D3DFOG_NONE;
        }
        
        if(State == D3DRS_FOGSTART){
            Value = 0.0;
        }
        
        if(State == D3DRS_FOGEND){
            Value = 0.0;
        }

        if(State == D3DRS_FOGDENSITY){
            Value = 0.0;
        }

		/*pDev->SetRenderState(D3DRS_FOGENABLE, false);
		pDev->SetRenderState(D3DRS_RANGEFOGENABLE, false);
		pDev->SetRenderState(D3DRS_FOGCOLOR,D3DCOLOR_RGBA(0,0,0,0));
		pDev->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_NONE);
		pDev->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_NONE);
		pDev->SetRenderState(D3DRS_FOGSTART,0.0);
		pDev->SetRenderState(D3DRS_FOGEND,0.0);
		pDev->SetRenderState(D3DRS_FOGDENSITY,0.0);*/

        HRESULT hRet = oSetRenderState(pDev, State, Value);
        return hRet;
    }

void SetLightChams(float A, float R, float G, float B, IDirect3DDevice9 *pDevice)
{
	//D3DMATERIAL9 pMaterial;
	//ZeroMemory(&pMaterial, sizeof(D3DMATERIAL9));
	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); //Enable Lighting
	////Ambient
	//pMaterial.Ambient.a = (A/255);
	//pMaterial.Ambient.r = (R/255);
	//pMaterial.Ambient.g = (G/255);
	//pMaterial.Ambient.b = (B/255);
	////Diffuse
	//pMaterial.Diffuse.a = (A/255);
	//pMaterial.Diffuse.r = (R/255);
	//pMaterial.Diffuse.g = (G/255);
	//pMaterial.Diffuse.b = (B/255);
	////Specular
	//pMaterial.Specular.a = (A/255);
	//pMaterial.Specular.r = (R/255);
	//pMaterial.Specular.g = (G/255);
	//pMaterial.Specular.b = (B/255);
	//pDevice->SetMaterial(&pMaterial);
}



HRESULT WINAPI hkDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDev, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	bool test = true;
	
	if(shad)
	{
		Shaders(pDev);
		shad = !shad;
	}

	if (AsusHack) 
	{
		LPDIRECT3DVERTEXBUFFER9 Stream_Data;
		UINT offset = 0;
		UINT stride = 0;
		if (pDev->GetStreamSource(0, &Stream_Data, &offset, &stride) == S_OK)
			Stream_Data->Release();

		if (stride == 48) 
		{
			/*pDev->SetRenderState( D3DRS_SRCBLEND, 0x5 );
			pDev->SetRenderState( D3DRS_DESTBLEND, 0x2 );
			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );*/
			pDev->SetRenderState( D3DRS_SRCBLEND, SRCBLENDINT);
			pDev->SetRenderState( D3DRS_DESTBLEND, DESTBLENDINT);
			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			//pDev->SetPixelShader(Walls);
		}

		if (test)
		{
			//-------------------CT CHAMS---------------------------------//
			if (antiterror)
			{

				DWORD dwOldZEnable = D3DZB_TRUE; 
				pDev->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable); 
				pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);

				/*pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetPixelShader(BlueShader);
				//Device->DrawIndexedPrimitive( PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetPixelShader(GreenShader);*/
			}

//-------------------T CHAMS---------------------------------//
			if (terror)
			{
				DWORD dwOldZEnable = D3DZB_TRUE; 
				pDev->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable); 
				pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);

				/*pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetPixelShader(RedShader);
				//Device->DrawIndexedPrimitive( PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetPixelShader(YellowShader);*/
			}

		}
	}

	if (esp)
	{




		//LPDIRECT3DVERTEXBUFFER9 Stream_Data;
		//UINT offset = 0;
		//UINT stride = 0;
		//if (pDev->GetStreamSource(0, &Stream_Data, &offset, &stride) == S_OK)
		//	Stream_Data->Release();

		//if (stride == 32) 
		//{
		//	//-------------------CT ESP---------------------------------//
		//	if (NumVertices == 343 && primCount == 344  || 
		//		NumVertices == 475 && primCount == 510  || 
		//		NumVertices == 814 && primCount == 945  || 
		//		NumVertices == 1424 && primCount == 1858 || 
		//		NumVertices == 2130 && primCount == 3004 || 
		//		NumVertices == 3206 && primCount == 4872 ||
		//		//////////////////////
		//		NumVertices == 369 && primCount == 297  ||
		//		NumVertices == 513 && primCount == 451  ||
		//		NumVertices == 899 && primCount == 910  ||
		//		NumVertices == 1609 && primCount == 1794 ||
		//		NumVertices == 2487 && primCount == 3006 ||
		//		NumVertices == 3887 && primCount == 4974 ||
		//		//////////////////////
		//		NumVertices == 324 && primCount == 372  ||
		//		NumVertices == 466 && primCount == 550  ||
		//		NumVertices == 811 && primCount == 989  ||
		//		NumVertices == 1404 && primCount == 1919 ||
		//		NumVertices == 2196 && primCount == 3061 ||
		//		NumVertices == 3304 && primCount == 5003 ||
		//		//////////////////////
		//		NumVertices == 409 && primCount == 433  ||
		//		NumVertices == 499 && primCount == 533  ||
		//		NumVertices == 929 && primCount == 1007  ||
		//		NumVertices == 1498 && primCount == 1822 ||
		//		NumVertices == 2245 && primCount == 2998 ||
		//		NumVertices == 3417 && primCount == 5030)
		//	{
		//	    if (SpriteCreated2 == FALSE)
		//		{
		//			D3DXAssembleShader(ShaderAdd2, sizeof(ShaderAdd2), NULL, NULL, 0, &ShaderBufferAdd2, NULL);
		//			pDev->CreatePixelShader((const DWORD*)ShaderBufferAdd2->GetBufferPointer(), &sAdd2);
		//			ShaderBufferAdd2->Release();

		//			//D3DXCreateTextureFromFile( pDev, "counterterrorist.png", &IMAGE2 );
		//			SpriteCreated2 = TRUE;
		//		}

		//		D3DXCreateSprite( pDev, &SPRITE2 );
  //  
		//		SPRITE2->Begin( D3DXSPRITE_ALPHABLEND | -1);
		//		SPRITE2->Draw( IMAGE2, NULL, NULL, NULL, 0xFFFFFFFF );
		//		SPRITE2->End( ); 
		//		if( SPRITE2 != NULL ){ SPRITE2->Release(); SPRITE2 = NULL; }

		//		LPDIRECT3DSTATEBLOCK9 pStateBlock2 = NULL;
		//		pDev->CreateStateBlock( D3DSBT_ALL, &pStateBlock2 );   
		//		pStateBlock2->Capture(); 

		//		pDev->SetTexture( 0, IMAGE2);
		//		pDev->SetPixelShader(sAdd2);
		//		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//<-
		//		pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		//		pDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		//		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		//		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

		//		pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pSize2));
		//		pDev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&pSize2));
		//		pDev->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&pSize2));
		//		pDev->DrawPrimitive(D3DPT_POINTLIST, 0, 1);

		//		pStateBlock2->Apply();
		//		pStateBlock2->Release();
		//	}

		//	//-------------------T ESP---------------------------------//
		//	if (NumVertices == 408 && primCount == 344  || 
		//		NumVertices == 554 && primCount == 509  || 
		//		NumVertices == 835 && primCount == 899  || 
		//		NumVertices == 1488 && primCount == 1819 || 
		//		NumVertices == 2292 && primCount == 3015 || 
		//		NumVertices == 3210 && primCount == 4503 ||
		//		//////////////////////
		//		NumVertices == 389 && primCount == 386  ||
		//		NumVertices == 526 && primCount == 594  ||
		//		NumVertices == 911 && primCount == 996  ||
		//		NumVertices == 1588 && primCount == 2013 ||
		//		NumVertices == 2261 && primCount == 3003 ||
		//		NumVertices == 3561 && primCount == 5066 ||
		//		//////////////////////
		//		NumVertices == 368 && primCount == 377  ||
		//		NumVertices == 563 && primCount == 590  ||
		//		NumVertices == 873 && primCount == 986  ||
		//		NumVertices == 1510 && primCount == 1871 ||
		//		NumVertices == 2274 && primCount == 3070 ||
		//		NumVertices == 3265 && primCount == 5015 ||
		//		//////////////////////
		//		NumVertices == 323 && primCount == 338  ||
		//		NumVertices == 454 && primCount == 519  ||
		//		NumVertices == 735 && primCount == 899  ||
		//		NumVertices == 1311 && primCount == 1812 ||
		//		NumVertices == 1992 && primCount == 2996 ||
		//		NumVertices == 3087 && primCount == 4911)
		//	{
		//	    if (SpriteCreated1 == FALSE)
		//		{
		//			D3DXAssembleShader(ShaderAdd, sizeof(ShaderAdd), NULL, NULL, 0, &ShaderBufferAdd, NULL);
		//			pDev->CreatePixelShader((const DWORD*)ShaderBufferAdd->GetBufferPointer(), &sAdd);
		//			ShaderBufferAdd->Release();

		//			//D3DXCreateTextureFromFile( pDev, "terrorist.png", &IMAGE1 );
		//			SpriteCreated1 = TRUE;
		//		}

		//		D3DXCreateSprite( pDev, &SPRITE1 );
  //  
		//		SPRITE1->Begin( D3DXSPRITE_ALPHABLEND | -1);
		//		SPRITE1->Draw( IMAGE1, NULL, NULL, NULL, 0xFFFFFFFF );
		//		SPRITE1->End( ); 
		//		if( SPRITE1 != NULL ){ SPRITE1->Release(); SPRITE1 = NULL; }

		//		LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
		//		pDev->CreateStateBlock( D3DSBT_ALL, &pStateBlock );   
		//		pStateBlock->Capture(); 

		//		pDev->SetTexture( 0, IMAGE1);
		//		pDev->SetPixelShader(sAdd);
		//		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//<-
		//		pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		//		pDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		//		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		//		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);

		//		pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
		//		pDev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pSize));
		//		pDev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&pSize));
		//		pDev->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&pSize));
		//		pDev->DrawPrimitive(D3DPT_POINTLIST, 0, 1);

		//		pStateBlock->Apply();
		//		pStateBlock->Release();
		//	}
		//}		
	}

	if (wallHack) 
	{
		LPDIRECT3DVERTEXBUFFER9 Stream_Data;
		UINT offset = 0;
		UINT stride = 0;
		if (pDev->GetStreamSource(0, &Stream_Data, &offset, &stride) == S_OK)
			Stream_Data->Release();
		/*if (stride == 32) 
		{
			if (NumVertices == 1286 )	
			{
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				pDev->SetTexture(0, hands);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}
		}*/
		/*
		pDev->SetRenderState(D3DRS_FOGSTART, 9999999);
		pDev->SetRenderState(D3DRS_FOGEND, 9999999);
		pDev->SetRenderState(D3DRS_FOGENABLE, false);
		*/

		/*pDev->SetRenderState(D3DRS_FOGENABLE, false);
		pDev->SetRenderState(D3DRS_RANGEFOGENABLE, false);
		pDev->SetRenderState(D3DRS_FOGCOLOR,D3DCOLOR_RGBA(0,0,0,0));
		pDev->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_NONE);
		pDev->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_NONE);
		pDev->SetRenderState(D3DRS_FOGSTART,0.0);
		pDev->SetRenderState(D3DRS_FOGEND,0.0);
		pDev->SetRenderState(D3DRS_FOGDENSITY,0.0);*/
		
		/*if(stride == 64) //Hostage Chams
		{
			if (NumVertices == 227 && primCount == 363  || //Face3
				NumVertices == 346 && primCount == 592  ||
				NumVertices == 523 && primCount == 943  ||
				//////////////////////
				NumVertices == 247 && primCount == 406 || //Face4
				NumVertices == 392 && primCount == 691 ||
				NumVertices == 547 && primCount == 986)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetPixelShader(PurpleShader);
				//Device->DrawIndexedPrimitive( PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetPixelShader(CyanShader);
			}
		}*/

		if (stride == 32) 
		{
//------------------Hostage CHAMS-----------------------------//
			/*if (NumVertices == 275 && primCount == 300  || //Body1
				NumVertices == 390 && primCount == 474  || 
				NumVertices == 698 && primCount == 916  || 
				NumVertices == 1181 && primCount == 1486 || 
				NumVertices == 1329 && primCount == 1770 || 
				NumVertices == 1694 && primCount == 2482 ||
				//////////////////////
				NumVertices == 434 && primCount == 429  || //Body2
				NumVertices == 684 && primCount == 760  || 
				NumVertices == 895 && primCount == 1106  || 
				NumVertices == 1309 && primCount == 1802 || 
				NumVertices == 1579 && primCount == 2256 || 
				NumVertices == 1689 && primCount == 2474 ||
				//////////////////////
				NumVertices == 314 && primCount == 351  || //Body3
				NumVertices == 421 && primCount == 517  || 
				NumVertices == 665 && primCount == 874  || 
				NumVertices == 1120 && primCount == 1378 || 
				NumVertices == 1325 && primCount == 1766 || 
				NumVertices == 1689 && primCount == 2474 ||
				//////////////////////
				NumVertices == 275 && primCount == 300  || //Body4
				NumVertices == 394 && primCount == 482  || 
				NumVertices == 673 && primCount == 886  || 
				NumVertices == 1105 && primCount == 1350 || 
				NumVertices == 1315 && primCount == 1742 || 
				NumVertices == 1694 && primCount == 2482 ||
				//////////////////////
				NumVertices == 60 && primCount == 92  || //Hair1
				NumVertices == 104 && primCount == 171  ||
				NumVertices == 229 && primCount == 391  ||
				NumVertices == 152 && primCount == 238 ||
				NumVertices == 187 && primCount == 304 ||
				NumVertices == 492 && primCount == 886 ||
				//////////////////////
				NumVertices == 73 && primCount == 116  || //Hair2
				NumVertices == 128 && primCount == 211  ||
				NumVertices == 291 && primCount == 517  ||
				NumVertices == 139 && primCount == 221 ||
				NumVertices == 199 && primCount == 331 ||
				NumVertices == 582 && primCount == 1056 ||
				//////////////////////
				NumVertices == 44 && primCount == 69  || //Hair3
				NumVertices == 121 && primCount == 202  ||
				NumVertices == 243 && primCount == 424  ||
				NumVertices == 112 && primCount == 172 ||
				NumVertices == 152 && primCount == 244 ||
				NumVertices == 305 && primCount == 523 ||
				//////////////////////
				NumVertices == 65 && primCount == 103  || //Hair4
				NumVertices == 141 && primCount == 219  ||
				NumVertices == 245 && primCount == 396  ||
				NumVertices == 214 && primCount == 346 ||
				NumVertices == 283 && primCount == 477 ||
				NumVertices == 315 && primCount == 534 ||
				//////////////////////
				NumVertices == 351 && primCount == 629  || //Face1
				NumVertices == 440 && primCount == 800  ||
				NumVertices == 499 && primCount == 891  ||
				//////////////////////
				NumVertices == 363 && primCount == 650 || //Face2
				NumVertices == 451 && primCount == 805 ||
				NumVertices == 565 && primCount == 1002)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetPixelShader(PurpleShader);
				//Device->DrawIndexedPrimitive( PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetPixelShader(CyanShader);
			}*/
//-------------------CT CHAMS---------------------------------//
			if (NumVertices == 343 && primCount == 344  || 
				NumVertices == 475 && primCount == 510  || 
				NumVertices == 814 && primCount == 945  || 
				NumVertices == 1424 && primCount == 1858 || 
				NumVertices == 2130 && primCount == 3004 || 
				NumVertices == 3206 && primCount == 4872 ||
				//////////////////////
				NumVertices == 369 && primCount == 297  ||
				NumVertices == 513 && primCount == 451  ||
				NumVertices == 899 && primCount == 910  ||
				NumVertices == 1609 && primCount == 1794 ||
				NumVertices == 2487 && primCount == 3006 ||
				NumVertices == 3887 && primCount == 4974 ||
				//////////////////////
				NumVertices == 324 && primCount == 372  ||
				NumVertices == 466 && primCount == 550  ||
				NumVertices == 811 && primCount == 989  ||
				NumVertices == 1404 && primCount == 1919 ||
				NumVertices == 2196 && primCount == 3061 ||
				NumVertices == 3304 && primCount == 5003 ||
				//////////////////////
				NumVertices == 409 && primCount == 433  ||
				NumVertices == 499 && primCount == 533  ||
				NumVertices == 929 && primCount == 1007  ||
				NumVertices == 1498 && primCount == 1822 ||
				NumVertices == 2245 && primCount == 2998 ||
				NumVertices == 3417 && primCount == 5030)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetPixelShader(GreenShader);
				//Device->DrawIndexedPrimitive( PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetPixelShader(BlueShader);
			}
			/*{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				//pDev->SetTexture(0, red);
				pDev->SetTexture(0, blue);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				//pDev->SetTexture(0, green);
				pDev->SetTexture(0, green);
			}*/

//===================GLOVES================
			//if (NumVertices == 1286 && primCount == 1778 )	
			//{
			//	pDev->SetRenderState(D3DRS_ZENABLE, true);
			//	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			//	//pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			//	//pDev->SetRenderState(D3DRS_ZFUNC,D3DCMP_NEVER);

			//	//pDev->SetTexture(0, hands);
			//	pDev->SetPixelShader(Invis);
			//	oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			//}
//===================GLOVES================
/*
//===================AWP===================
			if (NumVertices == 681 && primCount == 523 )	
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDev->SetTexture(0, white);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}
//===================AWP===================

//===================DEAGLE================
			if (NumVertices == 499 && primCount == 326 )	
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDev->SetTexture(0, white);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}
//===================DEAGLE================

//================CT GLASSES===============
			if (NumVertices == 14 && primCount == 12 ||
				NumVertices == 24 && primCount == 22 ||
				NumVertices == 43 && primCount == 50 ||
				///////////////////////
				NumVertices == 10 && primCount == 8 ||
				NumVertices == 11 && primCount == 9 ||
				NumVertices == 16 && primCount == 14 ||
				NumVertices == 20 && primCount == 18 ||
				NumVertices == 41 && primCount == 50 ||
				///////////////////////
				NumVertices == 12 && primCount == 8 ||
				NumVertices == 16 && primCount == 12 ||
				NumVertices == 24 && primCount == 20)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDev->SetTexture(0, glass);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}
//================CT GLASSES===============

//=================T GLASSES===============
			if (NumVertices == 26 && primCount == 26 ||
				NumVertices == 32 && primCount == 34 ||
				NumVertices == 50 && primCount == 66)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDev->SetTexture(0, glass);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}
//=================T GLASSES===============*/
			
//-------------------CT CHAMS---------------------------------//

//-------------------T CHAMS---------------------------------//
			if (NumVertices == 408 && primCount == 344  || 
				NumVertices == 554 && primCount == 509  || 
				NumVertices == 835 && primCount == 899  || 
				NumVertices == 1488 && primCount == 1819 || 
				NumVertices == 2292 && primCount == 3015 || 
				NumVertices == 3210 && primCount == 4503 ||
				//////////////////////
				NumVertices == 389 && primCount == 386  ||
				NumVertices == 526 && primCount == 594  ||
				NumVertices == 911 && primCount == 996  ||
				NumVertices == 1588 && primCount == 2013 ||
				NumVertices == 2261 && primCount == 3003 ||
				NumVertices == 3561 && primCount == 5066 ||
				//////////////////////
				NumVertices == 368 && primCount == 377  ||
				NumVertices == 563 && primCount == 590  ||
				NumVertices == 873 && primCount == 986  ||
				NumVertices == 1510 && primCount == 1871 ||
				NumVertices == 2274 && primCount == 3070 ||
				NumVertices == 3265 && primCount == 5015 ||
				//////////////////////
				NumVertices == 323 && primCount == 338  ||
				NumVertices == 454 && primCount == 519  ||
				NumVertices == 735 && primCount == 899  ||
				NumVertices == 1311 && primCount == 1812 ||
				NumVertices == 1992 && primCount == 2996 ||
				NumVertices == 3087 && primCount == 4911)
			{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				//pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				pDev->SetPixelShader(YellowShader);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				//pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				pDev->SetPixelShader(RedShader);
			}
			/*else
			{
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}*/
				
			/*{
				pDev->SetRenderState(D3DRS_ZENABLE, false);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				//pDev->SetTexture(0, red);
				pDev->SetTexture(0, red);
				oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
				pDev->SetRenderState(D3DRS_ZENABLE, true);
				pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				//pDev->SetTexture(0, green);
				pDev->SetTexture(0, yellow);
			}*/
//-------------------T CHAMS---------------------------------//
		}
	}
	return oDrawIndexedPrimitive(pDev, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

void DrawCrosshair(void* self)
{
	IDirect3DDevice9* dev = (IDirect3DDevice9*)self;
	D3DVIEWPORT9 viewP;
	// Через функцию GetViewport находим размер игрового экрана
	IDirect3DDevice9_GetViewport(dev, &viewP);
	// И его центр
	DWORD scrCenterX = viewP.Width / 2;
	DWORD scrCenterY = viewP.Height / 2;
	// Задаём размер прицела
	/*D3DRECT rect1 =
	{ scrCenterX - 5, scrCenterY, scrCenterX + 5, scrCenterY + 1 };
	D3DRECT rect2 =
	{ scrCenterX, scrCenterY - 5, scrCenterX + 1, scrCenterY + 5 };*/
	D3DRECT rect3 = 
	{scrCenterX - 10, scrCenterY, scrCenterX + 11, scrCenterY + 1};
	D3DRECT rect4 = 
	{scrCenterX, scrCenterY - 10, scrCenterX + 1, scrCenterY + 11};
	// Задаём цвет (RGB, Red Green Blue, Красный Зелёный Синий, 0-255)
	D3DCOLOR color = D3DCOLOR_XRGB(255, 0, 0);
	// Рисуем
	//IDirect3DDevice9_Clear(dev, 1, &rect1, D3DCLEAR_TARGET, color, 0, 0);
	//IDirect3DDevice9_Clear(dev, 1, &rect2, D3DCLEAR_TARGET, color, 0, 0);
	IDirect3DDevice9_Clear(dev, 1, &rect3, D3DCLEAR_TARGET, color, 0, 0);
	IDirect3DDevice9_Clear(dev, 1, &rect4, D3DCLEAR_TARGET, color, 0, 0);
}

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDev)
{	
	pDev->GetViewport(&vpt);
		//RECT FRect = { 10, 10, vpt.Width, vpt.Height };
		RECT FRect = { 5, 5, vpt.Width, vpt.Height };
		if (blue == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &blue, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTxB, 0) == S_OK)
					if (pTxB->LockRect(0, &d3dlrB, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlrB.pBits)[xy] = 0xFF0000FF;
						pTxB->UnlockRect(0);
						pDev->UpdateTexture(pTxB, blue);
						pTxB->Release();
					}
		if (green == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &green, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTx, 0) == S_OK)
					if (pTx->LockRect(0, &d3dlr, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlr.pBits)[xy] = 0xFF00FF00;
						pTx->UnlockRect(0);
						pDev->UpdateTexture(pTx, green);
						pTx->Release();
					}
		if (yellow == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &yellow, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTxY, 0) == S_OK)
					if (pTxY->LockRect(0, &d3dlrY, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlrY.pBits)[xy] = 0xFFFFFF00;
						pTxY->UnlockRect(0);
						pDev->UpdateTexture(pTxY, yellow);
						pTxY->Release();
					}
		if (red == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &red, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTx1, 0) == S_OK)
					if (pTx1->LockRect(0, &d3dlr1, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlr1.pBits)[xy] = 0xFFFF0000;
						pTx1->UnlockRect(0);
						pDev->UpdateTexture(pTx1, red);
						pTx1->Release();
					}
		if (white == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &white, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTx_white, 0) == S_OK)
					if (pTx_white->LockRect(0, &d3dlr_white, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlr_white.pBits)[xy] = 0xFFFFFFFF;
						pTx_white->UnlockRect(0);
						pDev->UpdateTexture(pTx_white, white);
						pTx_white->Release();
					}
		if (glass == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &glass, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTx_glass, 0) == S_OK)
					if (pTx_glass->LockRect(0, &d3dlr_glass, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlr_glass.pBits)[xy] = 0xFF00F2FF;
						pTx_glass->UnlockRect(0);
						pDev->UpdateTexture(pTx_glass, glass);
						pTx_glass->Release();
					}
		/*if (hands == 0)
			if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &hands, 0) == S_OK)
				if (pDev->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTx_hands, 0) == S_OK)
					if (pTx_hands->LockRect(0, &d3dlr_hands, 0, D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK) 
					{
						for (UINT xy = 0; xy < 8 * 8; xy++)
							((PDWORD)d3dlr_hands.pBits)[xy] = 0x00000000;
						pTx_hands->UnlockRect(0);
						pDev->UpdateTexture(pTx_hands, hands);
						pTx_hands->Release();
					}*/
		if (pFont == 0)
			//D3DXCreateFontA(pDev, 16, 0, 700, 0, 0, 1, 0, 0, DEFAULT_PITCH | FF_DONTCARE, "Terminal", &pFont);
			D3DXCreateFontA(pDev, 16, 0, 700, 0, 0, 1, 0, 0, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);

		if(p_Line == 0)
			D3DXCreateLine(pDev, &p_Line);
		char* wallHackStatus = wallHack ? "on" : "off";
		char* crosshairStatus = crosshairToggle ? "on" : "off";
	if (pFont)
			pFont->DrawTextA(0, strbuff, -1, &FRect, DT_LEFT | DT_NOCLIP, 0xFFFF00FF);

		DrawShadowString("D3D", 5, 5, 255, 255, 255, pFont);

		int x = 1360;
		int y = 768;

		int iSize = 10;

		x /= 2;
		y /= 2;

		if (esp_plus)
		{
			FillRGBA(x - 15, y, 30, 1, CHEAT_RED);
			FillRGBA(x, y - 15, 1, 30, CHEAT_RED);
		}

		if (esp_square)
		{
			DrawLine(x - 15, y - 15, x + 15, y - 15, 255, 0, 0, 255); // Top
			DrawLine(x - 15, y - 15, x - 15, y + 15, 255, 0, 0, 255); // Left
			DrawLine(x + 15, y - 15, x + 15, y + 15, 255, 0, 0, 255); // Right
			DrawLine(x + 15, y + 15, x - 15, y + 15, 255, 0, 0, 255); // Bottom
		}

		if(crosshaire > 0)
		{
			if(crosshaire == 1)
			{
				DrawCrosshair(pDev);
			}
			if(crosshaire == 2)
			{
				DrawRectA(pDev, x, y - 5, 1, 11, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 5, y, 11, 1, D3DCOLOR_RGBA(0, 0, 0, 255) );

				DrawRectA(pDev, x - 1, y  - 6, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  + 6, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  - 8, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  + 8, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 2, y  - 8, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 2, y  + 6, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 2, y  - 8, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 2, y  + 6, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 6, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 8, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  - 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  - 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  + 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  + 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );

				DrawRectA(pDev, x - 7, y  - 1, 1,3, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x + 7, y  - 1, 1,3, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x - 1, y  - 7, 3,1, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x - 1, y  + 7, 3,1, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
			}
			if(crosshaire == 3)
			{
				DrawRectA(pDev, x - iSize, y, iSize * 2 + 1, 1, D3DCOLOR_RGBA(255, 0, 0, 255) );
                DrawRectA(pDev, x, y - iSize, 1, iSize * 2 + 1, D3DCOLOR_RGBA(255, 0, 0, 255) );
 
                /*
                                   2
                                   _
                        1->     |_|_
                                 _| |  <-3
 
                                 4
                */
                DrawRectA(pDev, x - iSize, y - iSize, 1, iSize, D3DCOLOR_RGBA(255, 0, 0, 255) ); // 1
                DrawRectA(pDev, x, y - iSize, iSize + 1, 1, D3DCOLOR_RGBA(255, 0, 0, 255) ); // 2
                DrawRectA(pDev, x + iSize + 1, y, 1, iSize + 1, D3DCOLOR_RGBA(255, 0, 0, 255) ); // 3
                DrawRectA(pDev, x - iSize, y + iSize, iSize + 1, 1, D3DCOLOR_RGBA(255, 0, 0, 255) );
			}
			if(crosshaire == 4)
			{
				DrawLine(x, y - iSize, x, y - 2, 255, 0, 0, 255);
				DrawLine(x + 2, y, x + iSize, y, 255, 0, 0, 255);
				DrawLine(x, y + 2, x, y + iSize, 255, 0, 0, 255);
				DrawLine(x - iSize, y, x - 2, y, 255, 0, 0, 255);
			}
			if(crosshaire == 5)
			{
				DrawLine(x - iSize, y - iSize, x - 1, y - 1, 255, 0, 0, 255); 
				DrawLine(x - iSize, y + iSize, x - 1, y + 1, 255, 0, 0, 255);
				DrawLine(x + iSize, y - iSize, x + 2, y - 2, 255, 0, 0, 255);
				DrawLine(x + iSize, y + iSize, x + 2, y + 2, 255, 0, 0, 255);
			}
			if(crosshaire == 6)
			{
				DrawRectA(pDev, x, y - 5, 1, 11, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 5, y, 11, 1, D3DCOLOR_RGBA(0, 0, 0, 255) );

				DrawRectA(pDev, x - 1, y  - 6, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  + 6, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  - 8, 3,1, D3DCOLOR_RGBA(0, 0 ,0 ,255) );
				DrawRectA(pDev, x - 1, y  + 8, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 2, y  - 8, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 2, y  + 6, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 2, y  - 8, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 2, y  + 6, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 6, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 8, y  - 1, 1,3, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  - 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  - 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x - 8, y  + 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );
				DrawRectA(pDev, x + 6, y  + 2, 3,1, D3DCOLOR_RGBA(0, 0, 0, 255) );

				DrawRectA(pDev, x - 7, y  - 1, 1,3, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x + 7, y  - 1, 1,3, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x - 1, y  - 7, 3,1, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );
				DrawRectA(pDev, x - 1, y  + 7, 3,1, D3DCOLOR_RGBA(rand() % 255 + 0, rand() % 255 + 0, rand() % 255 + 0, 255) );

				DrawLine(x - iSize, y - iSize, x - 1, y - 1, 255, 0, 0, 255);
				DrawLine(x - iSize, y + iSize, x - 1, y + 1, 255, 0, 0, 255);
				DrawLine(x + iSize, y - iSize, x + 2, y - 2, 255, 0, 0, 255);
				DrawLine(x + iSize, y + iSize, x + 2, y + 2, 255, 0, 0, 255);
			}
			if(crosshaire == 7) 
			{
				CECrosshair.GetAreaColor();
				CECrosshair.DrawCrosshair( pDev );
			}
		}

		DoMenu(pDev);

		if(showMenu)
		{
			hkSetCursorPos(false);
		}
		else
		{
			hkSetCursorPos(true);
		}

	return oEndScene(pDev);
}

HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDev,D3DPRESENT_PARAMETERS* PresP)
{
	if (pFont) 
	{ 
		pFont->Release(); 
		pFont = NULL; 
	}
	if (blue) 
	{ 
		blue->Release(); 
		blue = NULL; 
	}
	if (green) 
	{ 
		green->Release(); 
		green = NULL; 
	}
	if (yellow)
	{
		yellow->Release();
		yellow = NULL;
	}
	if (red)
	{
		red->Release();
		red = NULL;
	}
	/*if (hands)
	{
		hands->Release();
		hands = NULL;
	}*/
	return oReset(pDev, PresP);
}

class CFakeQuery : public IDirect3DQuery9
{
public:
    HRESULT WINAPI QueryInterface(REFIID riid, void** ppvObj)
    {
        return D3D_OK;
    }

    ULONG WINAPI AddRef()
    {
        m_ref++;

        return m_ref;
    }

    ULONG WINAPI Release()
    {
        return 1;
    }

    HRESULT WINAPI GetDevice(IDirect3DDevice9 **ppDevice)
    {
        return D3D_OK;
    }

    D3DQUERYTYPE WINAPI GetType()
    {
        return m_type;
    }

    DWORD WINAPI GetDataSize()
    {
        return sizeof(DWORD);
    }

    HRESULT    WINAPI Issue(DWORD dwIssueFlags)
    {
        return D3D_OK;
    }

    HRESULT WINAPI GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags)
    {
        DWORD* pdwData = (DWORD*)pData;

        *pdwData = leet; //500 pixels visible

        return D3D_OK;
    }

    int                m_ref;
    D3DQUERYTYPE    m_type;
};

HRESULT WINAPI hkCreateQuery(LPDIRECT3DDEVICE9 pDevice, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
    if(Type == D3DQUERYTYPE_OCCLUSION)
    {
        *ppQuery = new CFakeQuery;

        ((CFakeQuery*) *ppQuery)->m_type = Type;

        return D3D_OK;
    }

    return oCreateQuery(pDevice, Type, ppQuery);
} 

void GetDevice9Methods()
{
	DWORD dip9 = 0;
	DWORD endScene9 = 0;
	DWORD reset9 = 0;
	DWORD renderState9 = 0;
	DWORD createQuery9 = 0;
	IDirect3D9 *d3d9_ptr;
	IDirect3DDevice9* d3dDevice;
	DWORD* vtablePtr;
	D3DPRESENT_PARAMETERS d3dpp;
	HWND hWnd = CreateWindowExA(0, "STATIC", "dummy", 0, 0, 0, 
								0, 0, 0, 0, 0, 0);
	HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
	if (hD3D9 != 0) 
	{
		d3d9_ptr = Direct3DCreate9(D3D_SDK_VERSION);
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d9_ptr->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice);
		vtablePtr = (PDWORD)(*((PDWORD)d3dDevice));
		createQuery9 = vtablePtr[118] - (DWORD)hD3D9;
		dip9 = vtablePtr[82] - (DWORD)hD3D9;
		renderState9 = vtablePtr[57] - (DWORD)hD3D9;
		endScene9 = vtablePtr[42] - (DWORD)hD3D9;
		reset9 = vtablePtr[16] - (DWORD)hD3D9;
		d3dDevice->Release();
		d3d9_ptr->Release();
	}
	CloseHandle(hWnd);
	oDrawIndexedPrimitive = (pDrawIndexedPrimitive)DetourFunction((PBYTE)((DWORD)hD3D9 + dip9), (PBYTE)hkDrawIndexedPrimitive);
	oEndScene = (pEndScene)DetourFunction((PBYTE)((DWORD)hD3D9 + endScene9), (PBYTE)hkEndScene);
	oReset = (pReset)DetourFunction((PBYTE)((DWORD)hD3D9 + reset9), (PBYTE)hkReset);
	oSetRenderState = (pSetRenderState)DetourFunction((PBYTE)((DWORD)hD3D9 + renderState9), (PBYTE)hkSetRenderState);
	oCreateQuery = (pCreateQuery)DetourFunction((PBYTE)((DWORD)hD3D9 + createQuery9), (PBYTE)hkCreateQuery);
	if (oDrawIndexedPrimitive && oEndScene && oReset && oSetRenderState && oCreateQuery)
		gotMethods = true;
	GetOldBytes();
}

void TF() 
{
	Beep(1000, 500);
	while (1) 
	{		
		if (!gotMethods) GetDevice9Methods();
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			wallHack = !wallHack;
		Sleep(1);
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			crosshairToggle = !crosshairToggle;
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			AsusHack = !AsusHack;
		}
		if(GetAsyncKeyState(VK_ADD)&1)
		{
			SRCBLENDINT++;
		}
		if(GetAsyncKeyState(VK_SUBTRACT)&1)
		{
			DESTBLENDINT++;
		}
		if(GetAsyncKeyState(VK_DIVIDE)&1)
		{
			SRCBLENDINT=0;
			DESTBLENDINT=0;
		}
		
	}	
}

int WINAPI DllMain(HINSTANCE hInst, DWORD ul_reason_for_call, void* lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TF, 0, 0, 0);
	return 1;
}