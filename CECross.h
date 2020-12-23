#ifndef ENV_CROSS
#define ENV_CROSS
//----------------------------------//
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
#include <d3d9.h>
#include <d3dx9core.h>
#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")
//----------------------------------//
struct PRGBA
{
        BYTE B, G, R, A; // Structure for accessing D3DCOLOR
};
//----------------------------------//
 
//----------------------------------//
class C_Environmental_Crosshair
{
        public:
                C_Environmental_Crosshair(); // Prototype
                C_Environmental_Crosshair( D3DCOLOR clrCrosshair, DWORD dwWidth, DWORD dwHeight ); // Prototype
                ~C_Environmental_Crosshair(); // Prototype
               
                void GetAreaColor( void ); // Prototype
                bool GetWindowHandles( void ); // Prototype
                //void DrawCrosshair( void ); // Prototype
				void DrawCrosshair( IDirect3DDevice9* device ); // Prototype
        private:
                POINT   m_ptSize, // Size of crosshair
                        m_ptCenter; // Center of screen
                HWND    m_hWnd; // Window handle
                HDC     m_hDC; // draw context handle
                POINT   m_pt[ 4 ]; // four points surrounding crosshair
                RECT    m_rtClient; // window client area
                D3DCOLOR m_clrCrosshair, // original crosshair color
                         m_clrTargetCrosshair; // target crosshair color
                DWORD   m_dwSleep; // How much to cool down when changing crosshair color
}CECrosshair( D3DCOLOR_ARGB( 255, 255, 0, 0 ), 25, 25 );
//----------------------------------//
 
//----------------------------------//
C_Environmental_Crosshair::C_Environmental_Crosshair()
{
       
}
//----------------------------------//
C_Environmental_Crosshair::C_Environmental_Crosshair( D3DCOLOR clrCrosshair, DWORD dwWidth, DWORD dwHeight )
{
        m_ptSize.x              = dwWidth;
        m_ptSize.y              = dwHeight;
        m_clrCrosshair          = clrCrosshair;
        m_clrTargetCrosshair    = m_clrCrosshair;
        m_dwSleep               = 75;
       
        m_hWnd  = NULL;
        m_hDC   = NULL;
        ZeroMemory( &m_rtClient, sizeof( m_rtClient ) );
}
//----------------------------------//
C_Environmental_Crosshair::~C_Environmental_Crosshair()
{
       
}
//----------------------------------//
 
//----------------------------------//
bool C_Environmental_Crosshair::GetWindowHandles( void )
{
        if( !m_hWnd )
                m_hWnd = GetFocus();
               
        m_hDC = GetDC( m_hWnd );
       
        GetClientRect( m_hWnd, &m_rtClient );
       
        m_ptCenter.x = m_rtClient.right/2;
        m_ptCenter.y = m_rtClient.bottom/2;
       
        if( !m_hWnd || !m_hDC || !m_rtClient.right )
                return false;
        return true;
}
//----------------------------------//
void C_Environmental_Crosshair::GetAreaColor( void )
{
        static DWORD dwTickCount = GetTickCount();
       
        if( GetTickCount() > dwTickCount + m_dwSleep )
                if( GetWindowHandles() )
                {
                        COLORREF        crColor;
                        DWORD           dwLuminosities = NULL;
                        WORD            H, L, S;
                       
                        m_pt[ 0 ].x = m_ptCenter.x - 3;
                        m_pt[ 0 ].y = m_ptCenter.y - 3;
                       
                        m_pt[ 1 ].x = m_ptCenter.x + 3;
                        m_pt[ 1 ].y = m_ptCenter.y - 3;
                       
                        m_pt[ 2 ].x = m_ptCenter.x - 3;
                        m_pt[ 2 ].y = m_ptCenter.y + 3;
                       
                        m_pt[ 3 ].x = m_ptCenter.x + 3;
                        m_pt[ 3 ].y = m_ptCenter.y + 3;
       
                        for( int i = 0; i < 4; i++ )
                        {
                                crColor = GetPixel( m_hDC, m_pt[ i ].x, m_pt[ i ].y );
                                ColorRGBToHLS( crColor, &H, &L, &S );
                                dwLuminosities += L; // Add together all luminosities
                        }
                       
                        if( dwLuminosities )
                                dwLuminosities /= 5; // Get average
                               
                        PRGBA* pColor = ( PRGBA* )&m_clrCrosshair; // access D3DCOLOR
 
                        COLORREF Original = RGB( pColor->R, pColor->G, pColor->B );
                        ColorRGBToHLS( Original, &H, &L, &S ); // extract hue and saturation
                       
                        COLORREF Ret = ColorHLSToRGB( H, 240 - dwLuminosities, S ); // replace with new luminosity
                        m_clrTargetCrosshair = D3DCOLOR_XRGB( GetRValue( Ret ), GetGValue( Ret ), GetBValue( Ret ) );
                       
                        dwTickCount = GetTickCount();
                }
       
}
//----------------------------------//

//----------------------------------//
void Rect(IDirect3DDevice9* Unidade, int baseX, int baseY, int baseW, int baseH, D3DCOLOR Cor)
{
	D3DRECT BarRect = { baseX, baseY, baseX + baseW, baseY + baseH }; 
	Unidade->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Cor, 0,  0); 
}
//----------------------------------//
 
//----------------------------------//
void C_Environmental_Crosshair::DrawCrosshair( IDirect3DDevice9* device )
{
        Rect(device, m_ptCenter.x - m_ptSize.x/2, m_ptCenter.y, m_ptSize.x, 1, m_clrTargetCrosshair );
        Rect(device, m_ptCenter.x, m_ptCenter.y - m_ptSize.y/2, 1, m_ptSize.y, m_clrTargetCrosshair );
}
//----------------------------------//
#endif