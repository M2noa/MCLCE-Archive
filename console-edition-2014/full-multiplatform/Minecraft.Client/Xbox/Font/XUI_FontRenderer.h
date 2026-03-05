#pragma once
using namespace std;
class XUI_FontData;
class XUI_Font;

// Define this to use this class as the XUI font renderer
#define OVERRIDE_XUI_FONT_RENDERER

//#define USE_SCALING_FONT

class XUI_FontRenderer : public IXuiFontRenderer
{
protected:
	enum eFontData
	{
		eFontData_MIN = 0,
		eFontData_Mojangles_7 = 0,
		eFontData_Mojangles_11,
		eFontData_MAX
	};

	// The font data is the image and size/coords data
	XUI_FontData *m_loadedFontData[eFontData_MAX];

	// The XUI_Font is a temporary instance that is around as long as XUI needs it, but does the actual rendering
	// These can be chained
	unordered_map<float, XUI_Font *> m_loadedFonts[eFontData_MAX];

public:
    XUI_FontRenderer();

	// 4J - IXuiFontRenderer interface 
    virtual HRESULT STDMETHODCALLTYPE Init( float fDpi );
    virtual VOID STDMETHODCALLTYPE Term();
    virtual HRESULT STDMETHODCALLTYPE GetCaps( DWORD * pdwCaps );
    virtual HRESULT STDMETHODCALLTYPE CreateFont( const TypefaceDescriptor * pTypefaceDescriptor, 
        float fPointSize, DWORD dwStyle, DWORD dwReserved, HFONTOBJ * phFont );
    virtual VOID STDMETHODCALLTYPE ReleaseFont( HFONTOBJ hFont );
    virtual HRESULT STDMETHODCALLTYPE GetFontMetrics( HFONTOBJ hFont, XUIFontMetrics *pFontMetrics );
    virtual HRESULT STDMETHODCALLTYPE GetCharMetrics( HFONTOBJ hFont, WCHAR wch, 
        XUICharMetrics *pCharMetrics );
    virtual HRESULT STDMETHODCALLTYPE DrawCharToTexture( HFONTOBJ hFont, WCHAR wch, HXUIDC hDC,
        IXuiTexture * pTexture, UINT x, UINT y, UINT width, UINT height, 
        UINT insetX, UINT insetY );
    virtual HRESULT STDMETHODCALLTYPE DrawCharsToDevice( HFONTOBJ hFont, CharData * pCharData, 
        DWORD dwCount, RECT *pClipRect, HXUIDC hDC, 
        D3DXMATRIX * pWorldViewProj );

};