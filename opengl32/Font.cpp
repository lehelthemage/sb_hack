/*
	Q3 Engine Graphic User Interface :: Font.cpp
	By BiGmAc
*/

/*
	This font class was created by Azorbix for Game-Deception (http://www.game-deception.com).
	Original print style created by Jeff Molofee (http://nehe.gamedev.net)
	
	Credits:
		Azorbix (for making it) [http://zox.game-deception.com]
		Crusader (Help with original glPrint function) [http://crusader.game-deception.com]
		LanceVorgin (Help with character lengths) [http://lvgl.game-deception.com]
		Xnient (Many improvements to fontclass) [http://zox.game-deception.com]

	This code may not be reproduced anywhere without concent from Xnient or Azorbix.
	If you are going to use this code in any program, please credit the names above.
	
	Dont be a RIPPER!

	bigmac: edited some stuff (for neatness)
*/

#undef _UNICODE
#undef UNICODE

#include "Gui.h"
#include "Font.h"
#include "opengl32.h"


float fColorTable[10][3] = 
{
	{ 0, 0, 0 }, // 0 black
	{ 255, 60, 40 }, // 1 red
	{ 40, 255, 60 }, // 2 green
	{ 40, 60, 255 }, // 3 blue
	{ 240, 240, 20 }, // 4 yellow
	{ 240, 40, 240 }, // 5 cyan
	{ 40, 240, 240 }, // 6 magenta
	{ 220, 128, 0 }, // 7 orange
	{ 80, 32, 160 }, // 8 purple
	{ 255, 255, 255 } // 9 white
};

CFont::CFont( void )
{
	strcpy( this->m_szFontName, "Tahoma" );
	this->m_iFontSize = 14;
	this->m_iFontFlag = FW_NORMAL;
}

CFont::CFont( char *szFontName, int iFontSize, int iFontFlag )
{
	strcpy( this->m_szFontName, szFontName );
	this->m_iFontSize = iFontSize;
	this->m_iFontFlag = iFontFlag;
}

CFont::~CFont( void )
{
}

void CFont::InitFont( void )
{
	HFONT hFont;
	HFONT hOldFont;
	HDC hDC;
	int i;

	hDC = orig_wglGetCurrentDC( );

	this->m_iFontID = orig_glGenLists( 256 );

	hFont = CreateFont( this->m_iFontSize, 0, 0, 0, this->m_iFontFlag, 0, 0, 0, ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, this->m_szFontName );

	hOldFont = ( HFONT )SelectObject( hDC, hFont );
	
	orig_wglUseFontBitmapsA( hDC, 0, 255, this->m_iFontID );
	
	for( i = 0; i < 255; i++ )
	{
		SIZE Size = { 0 };

		char szLine[2] = { ( char )i, 0 }; // credits to LanceVorgin

		GetTextExtentPoint( hDC, szLine, 1, &Size );

		this->m_iFontWidth[ i ] = ( short )Size.cx;
		this->m_iFontHeight = ( short )Size.cy;
	}

	SelectObject( hDC, hOldFont );

	DeleteObject( hFont );
}

inline short CFont::GetFontHeight( void )
{
	return( this->m_iFontHeight );
}

inline int CFont::GetStringLength( char *szBuf, bool bColor )
{
	int iDrawLen = 0;
	char *p, *szTemp = { '\0' };

	if( bColor )
		iDrawLen = ParseString( szBuf, szTemp );
	else
	{
		for( p = szBuf; *p; p++ )
			iDrawLen += this->m_iFontWidth[ *p ];
	}

	return( iDrawLen );
}

int CFont::ParseString( char *szIn, char *szOut )
{
	int i = 0;
	int iDrawLen = 0;

	for( ;*szIn; szIn++ )
	{
		if( *szIn == '^' )
		{	
			szIn++;

			if( !*szIn )
				break;

			continue;
		}
		szOut[ i ] = *szIn;
		i++;
		iDrawLen += this->m_iFontWidth[ *szIn ];
	}

	szOut[ i ] = 0;

	return( iDrawLen );
}

void CFont::DrawString( int x, int y, float *fColor, unsigned char cFlags, char *szString, ... )
{
	GLboolean bTex2D;

	orig_glGetBooleanv( GL_TEXTURE_2D, &bTex2D );

	if( bTex2D )
		orig_glDisable( GL_TEXTURE_2D );
 
	int iDrawLen = 0;
	char szText[512];
	char szPrintText[512];
	char *p;
	va_list argPtr;

	va_start( argPtr, szString );				
	vsprintf( szText, szString, argPtr );			
	va_end( argPtr );

	if( cFlags & FL_COLTAGS )
		iDrawLen = ParseString( szText, szPrintText );
	else
	{
		for( p = szText; *p; p++ )
			iDrawLen += this->m_iFontWidth[ *p ];

		strcpy( szPrintText, szText );
	}
		
	if( cFlags & FL_CENTERX )
		x -= ( iDrawLen / 2 );

	if( cFlags & FL_CENTERY )
		y += ( this->m_iFontHeight - 3 ) / 2;

	if( cFlags & FL_SHADOW )
	{
		Render( x + 2, y + 2, fColorTable[0], szPrintText );

		if( cFlags & FL_UNDERLINE )
		{
			orig_glColor3f( 0.0, 0.0, 0.0 );
			orig_glLineWidth( 1.0f );

			orig_glBegin( GL_LINES );
			orig_glVertex2i( x + 2, y + 5 );
			orig_glVertex2i( x + iDrawLen + 2, y + 5 );
			orig_glEnd( );
		}
	}

	if( cFlags & FL_UNDERLINE )
	{
		orig_glColor4fv( fColor );
		orig_glLineWidth( 1.0f );

		orig_glBegin( GL_LINES );
		orig_glVertex2i( x, y + 3 );
		orig_glVertex2i( x + iDrawLen, y + 3 );
		orig_glEnd( );
	}

	// draw the original text
	( cFlags & FL_COLTAGS ? RenderC( x, y, fColor, szText ) : Render( x, y, fColor, szText ) );

	if( bTex2D )
		orig_glEnable( GL_TEXTURE_2D );
}

void CFont::RenderC( int x, int y, float *fColor, char *szString )
{
	int iWidth = 0;
	int i = 0;
	float fNewColor[4];
	//char *p;
	char szBuf[512];

	fNewColor[3] = fColor[3];

	for( char *p = szString; *p; p++ )
	{
		if( *p == '^' )
		{
			if( i )
				Render( x, y, fColor, szBuf );

			p++;

			if( !*p )
				break;

			int c = *p - 48;

			if( c > 9 )	c = 9;
			if( c < 0 )	c = 0;

			fNewColor[0] = ::fColorTable[ c ][0];
			fNewColor[1] = ::fColorTable[ c ][1];
			fNewColor[2] = ::fColorTable[ c ][2];

			x += iWidth;
			i = iWidth = 0;
		}
		else
		{
			szBuf[ i ] = *p;
			szBuf[ i + 1 ] = 0;

			i++;

			iWidth += this->m_iFontWidth[ *p ];
		}
	}

	if( i )
		//render whatever is left
		Render( x, y, fNewColor, szBuf );
}

void CFont::Render( int x, int y, float *fColor, char *szString )
{
	int i = 0;

	while( x < 0 ) // prevent rendering from failing
	{
		x += this->m_iFontWidth[ szString[ i ] ];
		i++;

		if( !szString[ i ] )
			return;
	}

	orig_glColor4fv( fColor );
	orig_glRasterPos2i( x, y );

	orig_glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	orig_glPushAttrib( GL_LIST_BIT );							
	orig_glListBase( this->m_iFontID );						
	orig_glCallLists( ( int )strlen( szString ) - i, GL_UNSIGNED_BYTE, szString + i );
	orig_glPopAttrib( );
}