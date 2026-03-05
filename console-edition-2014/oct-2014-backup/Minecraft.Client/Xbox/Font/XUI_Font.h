#pragma once

class XUI_FontData;

// 4J This class is partially based of the ATG font implementation, modified to suit our uses for XUI

//--------------------------------------------------------------------------------------
// Flags for the Font::DrawText() function (Or them together to combine features)
//--------------------------------------------------------------------------------------
#define ATGFONT_LEFT       0x00000000
#define ATGFONT_RIGHT      0x00000001
#define ATGFONT_CENTER_X   0x00000002
#define ATGFONT_CENTER_Y   0x00000004
#define ATGFONT_TRUNCATED  0x00000008 

class XUI_Font
{
public:
	XUI_FontData *m_fontData;

public:
	const int m_iFontData;
	const float m_fScaleFactor;

    FLOAT m_fXScaleFactor;      // Scaling constants
    FLOAT m_fYScaleFactor;
    FLOAT m_fSlantFactor;       // For italics
    DOUBLE m_dRotCos;           // Precalculated sine and cosine for italic like rotation
    DOUBLE m_dRotSin;

    D3DRECT m_rcWindow;         // Bounds rect if the text window, modify via accessors only!
    FLOAT m_fCursorX;           // Current text cursor
    FLOAT m_fCursorY;

    BOOL m_bRotate;

	DWORD m_dwNestedBeginCount;

	wstring m_fontName;
	wstring m_fallbackFont;
	DWORD refCount;
public:
	float getScaleFactor() { return m_fScaleFactor; }
	void GetScaleFactors(FLOAT *pfXScaleFactor, FLOAT *pfYScaleFactor) { *pfXScaleFactor = m_fScaleFactor;  *pfYScaleFactor = m_fScaleFactor; }
    // Accessor functions
    inline VOID SetSlantFactor( FLOAT fSlantFactor )
    {
        m_fSlantFactor = fSlantFactor;
    }

    inline VOID SetScaleFactors( FLOAT fXScaleFactor, FLOAT fYScaleFactor )
    {
      //  m_fXScaleFactor = m_fYScaleFactor = m_fScaleFactor;
    }

	void SetFallbackFont(const wstring &fallbackFont) { m_fallbackFont = fallbackFont; }
	void IncRefCount() { ++refCount; }
	void DecRefCount() { --refCount; }

public:
    XUI_Font(int iFontData, float scaleFactor, XUI_FontData *fontData);
    ~XUI_Font();

    // Returns the dimensions of a text string
    VOID    GetTextExtent( const WCHAR* strText, FLOAT* pWidth,
                           FLOAT* pHeight, BOOL bFirstLineOnly=FALSE ) const;
    FLOAT   GetTextWidth( const WCHAR* strText ) const;
    FLOAT   GetCharAdvance( const WCHAR* strChar ) const;

    VOID    SetWindow(const D3DRECT &rcWindow );
    VOID    SetWindow( LONG x1, LONG y1, LONG x2, LONG y2 );
    VOID    GetWindow(D3DRECT &rcWindow) const;
    VOID    SetCursorPosition( FLOAT fCursorX, FLOAT fCursorY );
    VOID    SetRotationFactor( FLOAT fRotationFactor );

    // Function to create a texture containing rendered text
    D3DTexture* CreateTexture( const WCHAR* strText,
                               D3DCOLOR dwBackgroundColor = 0x00000000,
                               D3DCOLOR dwTextColor = 0xffffffff,
                               D3DFORMAT d3dFormat = D3DFMT_A8R8G8B8 );

    // Public calls to render text. Callers can simply call DrawText(), but for
    // performance, they should batch multiple calls together, bracketed by calls to
    // Begin() and End().
    VOID    Begin();
    VOID    DrawText( DWORD dwColor, const WCHAR* strText, DWORD dwFlags=0L,
                      FLOAT fMaxPixelWidth = 0.0f );
    VOID    DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, const WCHAR* strText,
                      DWORD dwFlags=0L, FLOAT fMaxPixelWidth = 0.0f, bool darken = false );
    VOID    DrawShadowText( FLOAT sx, FLOAT sy, DWORD dwColor, DWORD dwShadowColor, const WCHAR* strText,
                      DWORD dwFlags=0L, FLOAT fMaxPixelWidth = 0.0f );
    VOID    End();
};