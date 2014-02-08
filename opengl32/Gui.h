/*
	Q3 Engine Graphic User Interface :: Gui.h
	By BiGmAc
*/

#ifndef GUI_H
#define GUI_H

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <time.h>
#include <vector>

#include "Font.h"
//#include "GL.h"

// dont think i will ever need obj_minimize but there for safe keepin
#define OBJ_TITLEBAR	0
#define OBJ_MINIMIZE	1

#define S_LINE 0
#define S_LINE_LOOP 1
#define S_RECT 2

class CDrawScheme
{
	public:
		CDrawScheme( void );
		CDrawScheme( float *, float *, int );

		void DrawShape( int, int, int, int );

		float *GetBorderColor( void );
		float *GetColor( void );
		int GetType( void );

		void SetBorderColor( float * );
		void SetColor( float * );
		void SetType( int );

		~CDrawScheme( void );		
	private:
		float m_fBorderCol[4];
		float m_fColor[4];
		int m_iType; // like S_RECT / S_LINE_LOOP etc
};

class CTemplate
{
	public:
		CTemplate( void );
		CTemplate( char *, int, int, int, int );

		char *GetClassName( void );
		int *GetDimensions( void );
		void SetDimensions( int, int, int, int );
		
		~CTemplate( void );
	protected:
		int m_iDimensions[4];
		char m_szClassName[64]; // only needed for debugging
};

class CWindow; // define CWindow so we can use it inside CObject

class CObject : public CTemplate // Inherit CTemplate class
{
	public:
		CObject( void );
		CObject( char *, int, int, int, int, CWindow * );

		virtual void Draw( void );
		
		CWindow *GetParent( void );
		void SetParent( CWindow * );

		~CObject( void );
	protected:
		CWindow *m_pParentWindow;
};

class CButton : public CObject // inherit CObject class
{
	public:
		CButton( void );
		CButton( char *, int, int, int, int, void ( * )( CButton * ), char *, bool, bool, CWindow * );

		void Draw( void );

		char *GetCaption( void );
		bool GetClick( void );

		void ( *fnCallback )( CButton * );

		~CButton( void );
	private:
		bool m_bClick, m_bShadow, m_bCenterTitle;
		char m_szCaption[256];
};

class CLine
{
	public:
		CLine( void );
		CLine( char * );

		char *GetBuffer( void );
		int GetTime( int );

		~CLine( void );
	private:
		char m_szBuffer[512];
		int m_iTime[3]; // 0 = hr, 1 = min, 2 = sec
};

class CMemo : public CObject
{
	public:
		CMemo( void );
		CMemo( char *, int, int, int, int, CWindow * );

		static void fnScrollDown( CButton * );
		static void fnScrollUp( CButton * );

		void Draw( void );
		void PushLine( char * );

		~CMemo( void );
	private:
		int m_iMaxVisLines;
		int m_iCurVisibleLines;
		CButton *m_pScrollDown, *m_pScrollUp;
		std::vector<CLine> m_vLines;
};

class CEditBox : public CObject
{
	public:
		CEditBox( void );
		CEditBox( char *, int, int, int, int, void ( * )( CEditBox * ), CWindow * );

		void Draw( void );

		void ( *fnCallback )( CEditBox * );

		char *GetInput( void );
		void DrawListBox( void );
		void PushInput( CButton *, char *, char * );
		void SetEditable( bool );
		void SetMemo( CMemo * );

		~CEditBox( void );
	private:
		std::vector<CButton *> m_vInputList;
		float m_fLongestCaption;
		bool m_bEditable;
		char m_szInputDisplay[512], m_szInput[512]; // changed to 512

		CMemo *m_pMemo;
		CButton *m_pInputTest1, *m_pInputTest2, *m_pInputTest3, *m_pInputTest4;
		// these are for the console "auto-complete" pop-up
};

class CWindow : public CTemplate
{
	public:
		CWindow( void );
		CWindow( char *, int, int, int, int, char *, bool, int );

		void Draw( bool );
		
		static void fnMinimize( CButton * );
		static void fnTitleBar( CButton * );

		bool GetDragging( void );
		bool GetMaximized( void );
		size_t GetObjectListSize( void );
		CObject *GetObjectPtr( int );
		CDrawScheme *GetScheme( void );
		char *GetTitle( void );
		
		void PushObject( CObject *, char *, int, int, int, int );
		void PushObject( CEditBox *, char *, int, int, int, int, void ( * )( CEditBox * ), char *, int, CMemo * );
		void PushObject( CButton *, char *, int, int, int, int, void ( * )( CButton * ), char *, bool, bool );
		void ResetScheme( void );
		void SetDragging( bool );
		void SetProperties( bool );
		void SetScheme( float *, float *, int );
		void SetTitle( char * );	

		~CWindow( void );
	private:
		bool m_bMaximized, m_bDragging;
		int m_iWinNum; // for getting this window off the vector stack inside CGui
		char m_szTitle[64];
		std::vector<CObject *> m_vObjects;

		CDrawScheme *m_pScheme;
		CButton *m_pTitleBar, *m_pMinBox;
};

// neaten up things with a class, cause namespace's are ugly :/

class CGui
{
	public:
		CGui( void );
		CGui( char, char *, int, float *, float * );
		
		void DrawGuiMouse( void );
		void DrawGuiWindows( void );
		virtual void Enable( void ); // use virtual if ur not calling it inside Gui.cpp
		int GetActiveState( void );
		bool GetDrawState( void );
		virtual HWND GetWinHandle( void ); // ^^
		CFont *GetFont( void );
		bool GetKey( int );
		float GetLastMouse( int );
		float GetMouse( int );
		virtual int GetOrigMouse( int ); // ^^
		CDrawScheme *GetScheme( void );
		CWindow *GetWindow( int );
		int GetWindowSize( void );

		int HandleMsg( int, WPARAM, LPARAM );
		
		void PushWindow( CWindow *, char *, int, int, int, int, char *, bool );
		void SetDrawState( bool );
		virtual void SetWinHandle( HWND ); // ^^
		void SetKey( int, bool );
		virtual void SetOrigMouse( int, int ); // ^^
		void SetScheme( float *, float *, int );
		virtual void Viewport( void ); // ^^

		~CGui( void );
	private:
		char m_cActivationKey;
		HWND m_hWnd;
		int m_iActive;
		bool m_bDraw, m_bKeys[258]; // 256 + 257 are lbutton + rbutton
		float m_fMouse[2], m_fLastMouse[2];
		int m_iOrigMouse[2];
		CFont *m_pFont; // could make multiple fonts if u want (CFont TitleBar, EditBox etc)
		CDrawScheme *m_pScheme;
		std::vector<CWindow *> m_vWindows;

		CWindow *m_pInputTest, *m_pBlankTest, *m_pButtonTest; // add windows here.
		CEditBox *m_pInputBox; // add objects here.
		CButton *m_pButton;
		CMemo *m_pMemo;
};

extern CGui Gui;

#endif