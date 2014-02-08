/*
	Q3 Engine Graphic User Interface :: Font.h
	By BiGmAc
*/

#ifndef FONT_H
#define FONT_H

#define FL_NONE 0
#define FL_SHADOW 1
#define FL_UNDERLINE 2
#define FL_CENTERX 4
#define FL_CENTERY 8
#define FL_COLTAGS 16
#define FL_ALL 255

class CFont
{
	public:
		CFont( void );
		CFont( char *, int, int );

		void DrawString( int, int, float *, unsigned char, char *, ... );
		void InitFont( void );
		virtual short GetFontHeight( void );
		virtual int GetStringLength( char *, bool );

		~CFont( void );
	private:
		int	ParseString( char *, char * );
		void Render( int, int, float *, char * );
		void RenderC( int, int, float *, char * );

		char m_szFontName[32];
		int m_iFontSize, m_iFontFlag;
		short m_iFontHeight, m_iFontWidth[255];
		u_int m_iFontID;
};

#endif