#ifndef HDRAWFUNC_H
#define HDRAWFUNC_H

//#include "hDirectX.h"

#pragma warning( disable : 4244 ) //remove the incoming warns
#pragma warning( disable : 4996 ) //remove the incoming warns

#include <d3dx9.h>
#include <d3d9.h>
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define COLOR_WHITE             D3DCOLOR_RGBA( 255, 255, 255, 255 )
#define COLOR_RED               D3DCOLOR_RGBA( 255, 0, 0, 255 )
#define COLOR_GREEN             D3DCOLOR_RGBA( 0, 255, 0, 255 )
#define COLOR_BLUE              D3DCOLOR_RGBA( 0, 0, 255, 255 )
#define COLOR_GRAY              D3DCOLOR_RGBA( 100, 100, 100, 255 )
#define COLOR_BLACK             D3DCOLOR_RGBA( 0, 0, 0, 255 )
#define COLOR_DARK_GRAY D3DCOLOR_RGBA( 50, 50, 50, 255 )
#define COLOR_CYAN              D3DCOLOR_RGBA( 0, 150, 255, 255 )
#define COLORCODE( r, g, b, a ) D3DCOLOR_ARGB( a, r, g, b )

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;
extern ID3DXLine* p_Line;
extern ID3DXFont* pFontSmall;

void GradientFunc(int x, int y, int w, int h,int r, int g, int b, int a);
void DrawCenterLine(float x, float y, int r, int g, int b, int a);
void DrawLine(float x, float y, float xx, float yy, int r, int g, int b, int a);
void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a); 
void DrawBox(float x, float y, float width, float height, float px, int r, int g, int b, int a); 
void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa);
void DrawRadarBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa);
void DrawHealthBar(float x, float y, float w, float h, int r, int g);
void DrawHealthBarBack(float x, float y, float w, float h); 

int DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);
int DrawShadowString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont);

void DrawBorderBox( int x, int y, int w, int h, int thickness, D3DCOLOR Colour, IDirect3DDevice9 *pDevice); 
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev);
void Draw_Text(LPCSTR TextToDraw, int x, int y, D3DCOLOR Colour, LPD3DXFONT m_font);
void Draw_TextOutlined(LPCSTR fmt, int xx, int yy, D3DCOLOR Colour);
void RenderString(LPDIRECT3DDEVICE9 pDev, int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawEspText(LPDIRECT3DDEVICE9 pDevice, int X, int Y, D3DCOLOR color, const char *texttodraw);
void MouseInput(LPDIRECT3DDEVICE9 pDevice);
void DrawButton(IDirect3DDevice9 *d3dDevice, int x, int y, int w, int h, LPCSTR text, int action);
void DrawCheckBox(IDirect3DDevice9 *d3dDevice, int x, int y, LPCSTR text);
void DrawSlider(IDirect3DDevice9 *d3dDevice, float x, float y, float w, int min, int max ,int rgb);

#endif