#include "hDrawings.h"
#include "stdafx.h"

float sliderx = 0; 
float SliderValue = 0; 
char SliderValue2[101]; 
bool clicking;
bool holdclicking;
int cursorx, cursory;

//D3DPRESENT_PARAMETERS p_Params;	

int DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT ShadowPos;
	ShadowPos.left = x + 1;
	ShadowPos.top = y + 1;
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	ifont->DrawTextA(0, String, strlen(String), &ShadowPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
	ifont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
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
//=====================================================================================================
int DrawShadowString2(LPCSTR String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
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
//=====================================================================================================

void GradientFunc(int x, int y, int w, int h,int r, int g, int b, int a)
{
	int iColorr,iColorg,iColorb;
	for(int i = 1; i < h; i++)
	{
		iColorr = (int)((float)i/h*r);
		iColorg = (int)((float)i/h*g);
		iColorb = (int)((float)i/h*b);
		FillRGB(x ,y+ i,w,1,r-iColorr,g-iColorg,b-iColorb,a);
	}
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
void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa) 
{ 
    DrawBox(x,y,w,h, 1,r ,g ,b ,a);    
    FillRGB(x+1,y+1,w-1,h-1,rr,gg,bb,aa);   
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
void DrawHealthBar(float x, float y, float w, float h, int r, int g) 
{     
    FillRGB(x,y,w,h,r,g,0,255);      
} 
void DrawHealthBarBack(float x, float y, float w, float h) 
{     
    FillRGB(x,y,w,h,0,0,0,255);      
}
/*
void DrawCenterLine(float x, float y, int width, int r, int g, int b)
{
	//p_Params.BackBufferWidth = Width;    
   // p_Params.BackBufferHeight = Height;

	D3DXVECTOR2 dPoints[2];
	dPoints[0] = D3DXVECTOR2(x, y);
	dPoints[1] = D3DXVECTOR2(Width / 2, Height);
	p_Line->SetWidth(width);
	p_Line->Draw(dPoints, 2, D3DCOLOR_RGBA(r, g, b, 255));
}*/ 
//=================================================================================
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

//We receive the 2-D Coordinates the colour and the device we want to use to draw those colours with 
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev) 
{ 
    //We create our rectangle to draw on screen 
    D3DRECT BarRect = { x, y, x + w, y + h };  
    //We clear that portion of the screen and display our rectangle 
    dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0); 
} 
void Draw_Text(LPCSTR TextToDraw, int x, int y, D3DCOLOR Colour, LPD3DXFONT m_font) 
{ 
     
    // Create a rectangle to indicate where on the screen it should be drawn     
    RECT rct = {x- 120, y, x+ 120, y + 15}; 

    // Draw some text  
    pFontSmall->DrawTextA(NULL, TextToDraw, -1, &rct, DT_NOCLIP, Colour );      
} 

void Draw_TextOutlined(LPCSTR fmt, int xx, int yy, D3DCOLOR Colour) 
{ 
    D3DCOLOR black = D3DCOLOR_ARGB(255, 0, 0, 0);  
    //LPD3DXFONT m_font; 
    if( Colour != black ) 
    { 
        Draw_Text(fmt, xx - 1, yy - 1, black, pFontSmall); 
        Draw_Text(fmt, xx + 0, yy - 1, black, pFontSmall); 
        Draw_Text(fmt, xx + 1, yy - 1, black, pFontSmall); 
        Draw_Text(fmt, xx - 1, yy + 0, black, pFontSmall); 
        Draw_Text(fmt, xx + 1, yy + 0, black, pFontSmall); 
        Draw_Text(fmt, xx - 1, yy + 1, black, pFontSmall); 
        Draw_Text(fmt, xx + 0, yy + 1, black, pFontSmall); 
    } 
    Draw_Text(fmt, xx + 0, yy + 0, Colour, pFontSmall); 
    
} 

void RenderString(LPDIRECT3DDEVICE9 pDev, int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)  
{ 
    RECT FontPos = { x, y, x + 50, y + 50}; 
    char buf[1024] = {'\0'}; 
    va_list va_alist; 

    va_start(va_alist, fmt); 
    vsprintf_s(buf, fmt, va_alist); 
    va_end(va_alist); 

    //pDev->SetRenderState( D3DRS_ZENABLE,false ); 
    //pDev->SetRenderState( D3DRS_FILLMODE,D3DFILL_SOLID ); 
    g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color); 
    //pDev->SetRenderState( D3DRS_ZENABLE, true ); 
    //pDev->SetRenderState( D3DRS_FILLMODE,D3DFILL_SOLID ); 
} 
void DrawEspText(LPDIRECT3DDEVICE9 pDevice, int X, int Y, D3DCOLOR color, const char *texttodraw) 
{ 
     
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE); 
    D3DCOLOR black = D3DCOLOR_ARGB(255, 0, 0, 0);  
    RenderString(pDevice, X - 1, Y - 1, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X, Y - 1, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X + 1, Y - 1, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X - 1, Y, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X + 1, Y, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X - 1, Y + 1, black, pFontSmall, texttodraw); 
    RenderString(pDevice, X, Y + 1, black, pFontSmall, texttodraw); 

    RenderString(pDevice, X, Y, color, pFontSmall, texttodraw); 
    pDevice->SetRenderState(D3DRS_ZENABLE, TRUE); 
}  

void MouseInput(LPDIRECT3DDEVICE9 pDevice) 

{ 
    //MOUSE STUFF 
    POINT Pos; 
    RECT rMyRect; 
     
    HWND hWnd = ::GetActiveWindow(); 

        GetWindowRect(hWnd, (LPRECT)&rMyRect); 
        GetCursorPos(&Pos); 
        cursorx = Pos.x - 05 - rMyRect.left; 
        cursory = Pos.y - 30 - rMyRect.top; 

    //DRAW MOUSE 
    //DrawEspText(pDevice, cursorx, cursory, fontWhite, "<--");  
        //Use your own drawing functions to draw a "x" or something ^^ 


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
} 
/*
POINT Clicking() 
{ 
	int cursorx, cursory; //dont need to be global
    POINT Pos;
    //HWND hWnd = GetActiveWindow(); //use it in a class, so you dont need to get it always
	
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
}*/

void DrawButton(IDirect3DDevice9 *d3dDevice, int x, int y, int w, int h, LPCSTR text, int action) 
{ 
    //MOUSE SHIT 
    HWND hWnd = ::GetActiveWindow(); 
    POINT Pos; 
    RECT wRect; 
    int cursorx, cursory; 
    GetCursorPos(&Pos); 
    RECT rMyRect; 
        GetWindowRect(hWnd, (LPRECT)&rMyRect);  
        GetCursorPos(&Pos); 
        cursorx = Pos.x - 05 - rMyRect.left; 
        cursory = Pos.y - 30 - rMyRect.top; 
     
    bool isclicked; 
    DrawFilledRect( x, y, w, h, COLOR_GRAY, d3dDevice ); 
    DrawBorderBox(  x, y, w, h, 1, COLOR_BLACK, d3dDevice ); 
    //Draw_Text(text, x + 124, y+5, COLOR_WHITE, pFontSmall);
	DrawShadowString2(text, x + 24, y+4, 255, 255, 255, pFontSmall);
    if(cursorx > x && cursory > y  && cursorx < x+w && cursory < y+h) 
            { 
                DrawBorderBox(  x, y , w, h, 1, COLOR_WHITE, d3dDevice ); 
                if(clicking) 
                { 
                    //Draw_Text(text,  x + 124 + 1, y+5, COLOR_WHITE, pFontSmall);
					DrawShadowString2(text, x + 24, y+4, 255, 255, 255, pFontSmall);
                                    //Just Define your actions and call them like you want, or use your menu 
                                        //index and do whatever you want 
                   /*if(action == SAVE) 
                    { 
                        //DO shit here 
                    } 
                    else if(action == LOAD) 
                    { 
                        //Kill Justin Bieber 
                    }*/
                     
                } 
            } 
}

void DrawCheckBox(IDirect3DDevice9 *d3dDevice, int x, int y, LPCSTR text) 
{ 
    DrawFilledRect( x, y, 15, 15, COLOR_GRAY, d3dDevice ); 
    DrawBorderBox(x, y, 15, 15, 1, COLOR_BLACK, d3dDevice ); 
    Draw_Text(text, x+140, y+2, COLOR_WHITE, pFontSmall);     
} 

void DrawSlider(IDirect3DDevice9 *d3dDevice, float x, float y, float w, int min, int max ,int rgb) 
{ 
	float sliderw = 10.f; 
	float sliderh = 15.f; 
     
     

    if (rgb == 1) 
    { 
        float clickx = sliderx - cursorx; 
        float moveto = sliderx - clickx - sliderw/2;     
        if(cursorx >= x- sliderw/2 && cursory >= y-2 && cursorx <= x+w+ sliderw/2 && cursory <= y+sliderh+2) 
        { 
            if(GetAsyncKeyState(0x1)) 
            { 
                sliderx = moveto; 
            }             
        } 
        if (sliderx < x - sliderw/2){sliderx= x - sliderw/2;} 
        if (sliderx > x+w - sliderw/2){sliderx=x+w - sliderw/2;} 
        //Time To Draw 
        DrawFilledRect( x, y, w, sliderh , COLOR_GRAY, d3dDevice ); 
            DrawFilledRect( x, y, sliderx - x , sliderh , COLOR_RED, d3dDevice ); 
        DrawBorderBox(x, y, w, sliderh, 1, COLOR_BLACK, d3dDevice ); 
        DrawButton(d3dDevice, sliderx, y, sliderw, sliderh, "", NULL); 
        SliderValue = ((max-min)/(w))*(sliderx-x+sliderw/2);
        sprintf(SliderValue2, "  [%1.0f]",SliderValue); 
        Draw_Text(SliderValue2, x+w+120 , y+1 , COLOR_RED, pFontSmall); 
    } 
}