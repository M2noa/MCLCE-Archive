#pragma once

#include <unordered_map>

using namespace std;

#define _DEBUG_BLOCK_CHARS 0

// For hardcoded font data.
struct SFontData
{
public:
	static const unsigned short FONTCOLS = 23;
	static const unsigned short FONTROWS = 20;

	static const unsigned short FONTSIZE = FONTCOLS * FONTROWS;

public:
	// Font name.
	string				m_strFontName;

	// Filename of the glyph archive.
	wstring				m_wstrFilename;

	// Number of glyphs in the archive.
	unsigned int		m_uiGlyphCount;

	// Unicode values of each glyph.
	unsigned short		*m_arrCodepoints;

	// X resolution of glyph archive.
	unsigned int		m_uiGlyphMapX;

	// Y resolution of glyph archive.
	unsigned int		m_uiGlyphMapY;

	// Number of columns in the glyph archive.
	unsigned int		m_uiGlyphMapCols;

	// Number of rows in the glyph archive.
	unsigned int		m_uiGlyphMapRows;

	// Width of each glyph.
	unsigned int		m_uiGlyphWidth;

	// Height of each glyph.
	unsigned int		m_uiGlyphHeight;

	// Ascent of each glyph above the baseline (units?).
	float				m_fAscent;

	// Descent of each glyph below the baseline (units?).
	float				m_fDescent;

	// How much to advance for each pixel wide the glyph is.
	float				m_fAdvPerPixel;

	// How many pixels wide any whitespace characters are.
	unsigned int		m_uiWhitespaceWidth;

public:
	static unsigned short	Codepoints[FONTSIZE];
	static SFontData		Mojangles_7;
	static SFontData		Mojangles_11;
};

// Provides a common interface for dealing with font data.
class CFontData
{
public:
	CFontData();

	// pbRawImage consumed by constructor.
	CFontData(SFontData &sFontData, int *pbRawImage);

	// Release memory.
	void				release();

protected:

	// Hardcoded font data.
	SFontData			*m_sFontData;

	// Map Unicodepoints to glyph ids.
	unordered_map<unsigned int, unsigned short> m_unicodeMap;

	// Kerning value for each glyph.
	unsigned short		*m_kerningTable;

	// Binary blob of the archive image.
	unsigned char		*m_pbRawImage;

	// Total advance of each character.
	float				*m_pfAdvanceTable;

public:

	// Accessor for the font name in the internal SFontData.
	const string		getFontName();

	// Accessor for the hardcoded internal font data.
	SFontData			*getFontData();

	// Get the glyph id corresponding to a unicode point.
	unsigned short		getGlyphId(unsigned int unicodepoint);

	// Get the unicodepoint corresponding to a glyph id.
	unsigned int		getUnicode(unsigned short glyphId);

	// Get a pointer to the top left pixel of a row/column in the raw image.
	unsigned char		*topLeftPixel(int row, int col);

	// Get the row and column where a glyph appears in the archive.
	void				getPos(unsigned short gyphId, int &row, int &col);

	// Get the advance of this character (units?).
	float				getAdvance(unsigned short glyphId);

	// Get the width (in pixels) of a given character.
	int					getWidth(unsigned short glyphId);

	// Returns true if this glyph is whitespace.
	bool				glyphIsWhitespace(unsigned short glyphId);
	
	// Returns true if this unicodepoint is whitespace
	bool				unicodeIsWhitespace(unsigned int unicodepoint);

private:

	// Move a pointer in an image dx pixels right and dy pixels down, wrap around in either dimension leads to unknown behaviour.
	void				moveCursor(unsigned char *&cursor, unsigned int dx, unsigned int dy);
};

