/*
	Q3 Engine Graphic User Interface :: Gui.cpp
	By BiGmAc
*/

#include "Gui.h"
#include "opengl32.h"

namespace Colors
{
	float fBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float fTransBlack[4] = { 0.0f, 0.0f, 0.0f, 0.7f };
	float fRed[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float fGreen[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float fWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fPanz[4] = { 0.26f, 0.39f, 0.61f, 0.69f };
	float fGrey[4] = { 0.44f, 0.44f, 0.44f, 0.44f };
}

CGui Gui( '*', "Tahoma", 14, Colors::fWhite, Colors::fPanz );

void Fake( CButton *pThis )
{
	// dont do anything in here.
}

CTemplate::CTemplate( void )
{
	memset( this->m_iDimensions, 0, sizeof( this->m_iDimensions ) );
	
	strcpy( this->m_szClassName, "Class_Default" );
}

CTemplate::CTemplate( char *szClassName, int x, int y, int iWidth, int iHeight )
{	
	this->m_iDimensions[0] = x;
	this->m_iDimensions[1] = y;
	this->m_iDimensions[2] = iWidth;
	this->m_iDimensions[3] = iHeight;

	strcpy( this->m_szClassName, szClassName );
}

CTemplate::~CTemplate( void )
{
}

char *CTemplate::GetClassName( void )
{
	return( this->m_szClassName );
}

int *CTemplate::GetDimensions( void )
{
	return( this->m_iDimensions );
}

void CTemplate::SetDimensions( int x, int y, int iWidth, int iHeight )
{
	this->m_iDimensions[0] = x;
	this->m_iDimensions[1] = y;
	this->m_iDimensions[2] = iWidth;
	this->m_iDimensions[3] = iHeight;
}

CObject::CObject( void ) : CTemplate( )
{
	this->m_pParentWindow = 0;
}

CObject::CObject( char *szClassName, int x, int y, int iWidth, int iHeight, CWindow *pParentWindow ) : CTemplate( szClassName, x, y, iWidth, iHeight )
{
	this->m_pParentWindow = pParentWindow;
}

CObject::~CObject( void )
{
}

void CObject::Draw( void )
{
	int *iInheritedDim = this->m_pParentWindow->GetDimensions( );

	this->m_pParentWindow->SetScheme( this->m_pParentWindow->GetScheme( )->GetBorderColor( ), this->m_pParentWindow->GetScheme( )->GetColor( ), S_LINE_LOOP );
	this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0], iInheritedDim[1] + this->m_iDimensions[1], iInheritedDim[2] + this->m_iDimensions[2], iInheritedDim[3] + this->m_iDimensions[3] );
	this->m_pParentWindow->ResetScheme( );
}

inline CWindow *CObject::GetParent( void )
{
	return( this->m_pParentWindow );
}

inline void CObject::SetParent( CWindow *pParentWindow )
{
	this->m_pParentWindow = pParentWindow;
}

CLine::CLine( void )
{
	struct tm *CurTime;
    time_t *pTemp = new time_t;

   	time( pTemp );
	CurTime = localtime( pTemp );

	this->m_iTime[0] = CurTime->tm_hour;
	this->m_iTime[1] = CurTime->tm_min;
	this->m_iTime[2] = CurTime->tm_sec;

	delete pTemp;
}

CLine::CLine( char *szBuffer )
{
	struct tm *CurTime;
    time_t *pTemp = new time_t;

   	time( pTemp );
	CurTime = localtime( pTemp );

	this->m_iTime[0] = CurTime->tm_hour;
	this->m_iTime[1] = CurTime->tm_min;
	this->m_iTime[2] = CurTime->tm_sec;

	strcpy( this->m_szBuffer, szBuffer );

	delete pTemp;
}

CLine::~CLine( void )
{
}

inline char *CLine::GetBuffer( void )
{
	return( this->m_szBuffer );
}

inline int CLine::GetTime( int iType )
{
	return( this->m_iTime[ iType ] );
}

CMemo::CMemo( void ) : CObject( )
{
	this->m_pScrollDown = new CButton;
	this->m_pScrollUp = new CButton;

	this->m_iMaxVisLines = 1;
	this->m_iCurVisibleLines = ( int )this->m_vLines.size( );
}

CMemo::CMemo( char *szClassName, int x, int y, int iWidth, int iHeight, CWindow *pParentWindow ) : CObject( szClassName, x, y, iWidth, iHeight, pParentWindow )
{
	char szBuf[128];
	int *iInheritedDim = pParentWindow->GetDimensions( );

	this->m_iMaxVisLines = 9;//( iInheritedDim[3] + iHeight ) / 6;
	this->m_iCurVisibleLines = ( int )this->m_vLines.size( );

	sprintf( szBuf, "Object_%s_ScrollDown", this->m_szClassName );
	this->m_pScrollDown = new CButton( szBuf, x + iInheritedDim[2] + iWidth - 11, y + iInheritedDim[3] + iHeight - 11, 9 - iInheritedDim[2], 9 - iInheritedDim[3], CMemo::fnScrollDown, "", false, false, pParentWindow );

	sprintf( szBuf, "Object_%s_ScrollUp", this->m_szClassName );
	this->m_pScrollUp = new CButton( szBuf, x + iInheritedDim[2] + iWidth - 11, y + 2, 9 - iInheritedDim[2], 9 - iInheritedDim[3], CMemo::fnScrollUp, "", false, false, pParentWindow );
}

CMemo::~CMemo( void )
{
	delete this->m_pScrollDown;
	delete this->m_pScrollUp;
}

// u have to make 2 sep funcs other wise u will get a problem that i cant be assed to explain
void CMemo::fnScrollDown( CButton *pThis )
{
	static bool bClicked = false;

	int *iArea = pThis->GetDimensions( );

	if( Gui.GetKey( 256 ) )
	{
		if( pThis->GetClick( ) && !bClicked )
		{
			bClicked = true;
			pThis->SetDimensions( iArea[0] + 1, iArea[1] + 1, iArea[2], iArea[3] );
		}
	}
	else if( bClicked )
	{
		bClicked = false;
		pThis->SetDimensions( iArea[0] - 1, iArea[1] - 1, iArea[2], iArea[3] );
	}
}

void CMemo::fnScrollUp( CButton *pThis )
{
	static bool bClicked = false;

	int *iArea = pThis->GetDimensions( );

	if( Gui.GetKey( 256 ) )
	{
		if( pThis->GetClick( ) && !bClicked )
		{
			bClicked = true;
			pThis->SetDimensions( iArea[0] + 1, iArea[1] + 1, iArea[2], iArea[3] );
		}
	}
	else if( bClicked )
	{
		bClicked = false;
		pThis->SetDimensions( iArea[0] - 1, iArea[1] - 1, iArea[2], iArea[3] );
	}
}

void CMemo::Draw( void )
{
	int *iInheritedDim = this->m_pParentWindow->GetDimensions( ), i;
	int *iScrollDownDim = this->m_pScrollDown->GetDimensions( );
	int *iScrollUpDim = this->m_pScrollUp->GetDimensions( );

	this->m_pParentWindow->SetScheme( this->m_pParentWindow->GetScheme( )->GetBorderColor( ), this->m_pParentWindow->GetScheme( )->GetColor( ), S_LINE_LOOP );
	this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0], iInheritedDim[1] + this->m_iDimensions[1], iInheritedDim[2] + this->m_iDimensions[2], iInheritedDim[3] + this->m_iDimensions[3] );
	this->m_pParentWindow->ResetScheme( );

	this->m_pScrollDown->Draw( );
	this->m_pScrollUp->Draw( );

	if( this->m_iCurVisibleLines < this->m_iMaxVisLines )
		this->m_iCurVisibleLines = ( int )this->m_vLines.size( ) - 1;

	if( Gui.GetKey( 256 ) && Gui.GetMouse( 0 ) >= iInheritedDim[0] + iScrollDownDim[0] && Gui.GetMouse( 0 ) <= iInheritedDim[0] + iScrollDownDim[0] + iInheritedDim[2] + iScrollDownDim[2] && Gui.GetMouse( 1 ) >= iInheritedDim[1] + iScrollDownDim[1] && Gui.GetMouse( 1 ) <= iInheritedDim[1] + iScrollDownDim[1] + iInheritedDim[3] + iScrollDownDim[3] )
	{
		if( this->m_iCurVisibleLines < ( int )this->m_vLines.size( ) - 1 )
			this->m_iCurVisibleLines++;
	}

	if( Gui.GetKey( 256 ) && Gui.GetMouse( 0 ) >= iInheritedDim[0] + iScrollUpDim[0] && Gui.GetMouse( 0 ) <= iInheritedDim[0] + iScrollUpDim[0] + iInheritedDim[2] + iScrollUpDim[2] && Gui.GetMouse( 1 ) >= iInheritedDim[1] + iScrollUpDim[1] && Gui.GetMouse( 1 ) <= iInheritedDim[1] + iScrollUpDim[1] + iInheritedDim[3] + iScrollUpDim[3] )
	{
		if( this->m_iCurVisibleLines > this->m_iMaxVisLines )
			this->m_iCurVisibleLines--;
	}

	for( i = this->m_iCurVisibleLines; i >= ( this->m_iCurVisibleLines - this->m_iMaxVisLines < 0 ? 0 : this->m_iCurVisibleLines - this->m_iMaxVisLines ); i-- )
		Gui.GetFont( )->DrawString( iInheritedDim[0] + this->m_iDimensions[0] + 2, iInheritedDim[1] + this->m_iDimensions[1] + iInheritedDim[3] + this->m_iDimensions[3] - ( ( this->m_iCurVisibleLines - i ) * ( Gui.GetFont( )->GetFontHeight( ) - 2 ) ) - 4, Colors::fWhite, FL_NONE, this->m_vLines[ i ].GetBuffer( ) );
}

void CMemo::PushLine( char *szBuf )
{
	CLine *pTemp = new CLine( szBuf );

	this->m_vLines.push_back( *pTemp );

	this->m_iCurVisibleLines++;

	delete pTemp;
}

CEditBox::CEditBox( void ) : CObject( )
{
	this->m_fLongestCaption = 1;
	this->m_bEditable = false;
	this->fnCallback = 0;

	memset( this->m_szInput, '\0', sizeof( this->m_szInput ) );
	memset( this->m_szInputDisplay, '\0', sizeof( this->m_szInput ) );

	this->m_pInputTest1 = new CButton;
	this->m_pInputTest2 = new CButton;
	this->m_pInputTest3 = new CButton;
	this->m_pInputTest4 = new CButton;
}

CEditBox::CEditBox( char *szClassName, int x, int y, int iWidth, int iHeight, void ( *pFunction )( CEditBox * ), CWindow *pParentWindow ) : CObject( szClassName, x, y, iWidth, iHeight, pParentWindow )
{
	this->m_fLongestCaption = 1;
	this->m_bEditable = false;
	this->fnCallback = pFunction;

	memset( this->m_szInput, '\0', sizeof( this->m_szInput ) );
	memset( this->m_szInputDisplay, '\0', sizeof( this->m_szInput ) );

	this->PushInput( this->m_pInputTest1, "Input_Test_1", "cvar_dummy_wall" );
	this->PushInput( this->m_pInputTest2, "Input_Test_2", "cvar_dummy_aimbot" );
	this->PushInput( this->m_pInputTest3, "Input_Test_3", "cvar_dummy_radar" );
	this->PushInput( this->m_pInputTest4, "Input_Test_4", "cvar_dummy_enemy" );
}

CEditBox::~CEditBox( void )
{
}

void CEditBox::Draw( void )
{
	bool bLockEdit = false, bTruncated = false;
	int *iInheritedDim = this->m_pParentWindow->GetDimensions( ), i, iLen;

	if( Gui.GetMouse( 0 ) >= iInheritedDim[0] + this->m_iDimensions[0] && Gui.GetMouse( 0 ) <= iInheritedDim[0] + this->m_iDimensions[0] + iInheritedDim[2] + this->m_iDimensions[2] && Gui.GetMouse( 1 ) >= iInheritedDim[1] + this->m_iDimensions[1] && Gui.GetMouse( 1 ) <= iInheritedDim[1] + this->m_iDimensions[1] + iInheritedDim[3] + this->m_iDimensions[3] )
		this->fnCallback( this );
	else if( Gui.GetKey( 256 ) )
		bLockEdit = true;

	this->m_pParentWindow->SetScheme( this->m_pParentWindow->GetScheme( )->GetBorderColor( ), this->m_pParentWindow->GetScheme( )->GetColor( ), S_LINE_LOOP );
	this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0], iInheritedDim[1] + this->m_iDimensions[1], iInheritedDim[2] + this->m_iDimensions[2], iInheritedDim[3] + this->m_iDimensions[3] );
	this->m_pParentWindow->ResetScheme( );

	this->m_pParentWindow->SetScheme( this->m_pParentWindow->GetScheme( )->GetBorderColor( ), Colors::fGrey, S_RECT );
	this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0], iInheritedDim[1] + this->m_iDimensions[1], iInheritedDim[2] + this->m_iDimensions[2], iInheritedDim[3] + this->m_iDimensions[3] );
	this->m_pParentWindow->ResetScheme( );

	if( this->m_bEditable && Gui.GetActiveState( ) == 1 )
	{
		for( i = 0; i < 255; i++ )
		{
			if( !Gui.GetKey( i ) )
				continue;

			switch( i )
			{
				case '\r':
				case '\n':
					if( this->m_szInput[0] )
					{
						while( Gui.GetFont( )->GetStringLength( this->m_szInput, false ) + 2 >= iInheritedDim[2] + this->m_iDimensions[2] - 13 )
						{
							iLen = ( int )strlen( this->m_szInput );
							bTruncated = true;
							this->m_szInput[ strlen( this->m_szInput ) - 1 ] = '\0';
						}

						if( bTruncated )
							strcat( this->m_szInput, ".." );

						this->m_pMemo->PushLine( this->m_szInput );
					}

					memset( this->m_szInput, '\0', sizeof( this->m_szInput ) );
					break;
						
				case '\b':
					if( this->m_szInput[0] )
						this->m_szInput[ strlen( this->m_szInput ) - 1 ] = '\0';
					break;

				default:
					if( isprint( i ) && strlen( this->m_szInput ) < 511 )
						this->m_szInput[ strlen( this->m_szInput ) ] = i;
					break;
			}

			Gui.SetKey( i, false );
		}

		memcpy( this->m_szInputDisplay, this->m_szInput, sizeof( this->m_szInputDisplay ) );

		while( Gui.GetFont( )->GetStringLength( this->m_szInputDisplay, false ) + 1 >= iInheritedDim[2] + this->m_iDimensions[2] - 4 /*-4 to be safe*/ )
			memcpy( this->m_szInputDisplay, this->m_szInputDisplay + 1, sizeof( this->m_szInputDisplay ) - 1 );

		Gui.GetFont( )->DrawString( iInheritedDim[0] + this->m_iDimensions[0] + 2, iInheritedDim[1] + this->m_iDimensions[1] + Gui.GetFont( )->GetFontHeight( ) - 3, this->m_pParentWindow->GetScheme( )->GetBorderColor( ), FL_NONE, "%s_", this->m_szInputDisplay );

		if( strlen( this->m_szInputDisplay ) )
			this->DrawListBox( );
	}
	else
		Gui.GetFont( )->DrawString( iInheritedDim[0] + this->m_iDimensions[0] + 2, iInheritedDim[1] + this->m_iDimensions[1] + Gui.GetFont( )->GetFontHeight( ) - 3, Colors::fTransBlack, FL_NONE, ( !this->m_szInputDisplay[0] ? this->m_szInput : this->m_szInputDisplay ) );

	if( bLockEdit )
		this->m_bEditable = false;
}

inline char *CEditBox::GetInput( void )
{
	return( this->m_szInput );
}

void CEditBox::DrawListBox( void )
{
	int iCounter = 0;
	int *iInheritedDim = this->m_pParentWindow->GetDimensions( ), *iInputDim, i;

	for( i = 0; i < ( int )this->m_vInputList.size( ); i++ )
	{
		if( strncmp( this->m_vInputList[ i ]->GetCaption( ), this->m_szInput, strlen( this->m_szInput ) ) )
			continue;

		this->m_vInputList[ i ]->SetDimensions( 0, iInheritedDim[3] + 2 + ( iCounter * 15 ), ( int )this->m_fLongestCaption + 4 - iInheritedDim[2], 13 - iInheritedDim[3] );

		iCounter++;

		iInputDim = this->m_vInputList[ i ]->GetDimensions( );

		if( Gui.GetMouse( 0 ) >= iInheritedDim[0] + iInputDim[0] && Gui.GetMouse( 0 ) <= iInheritedDim[0] + iInputDim[0] + iInheritedDim[2] + iInputDim[2] && Gui.GetMouse( 1 ) >= iInheritedDim[1] + iInputDim[1] && Gui.GetMouse( 1 ) <= iInheritedDim[1] + iInputDim[1] + iInheritedDim[3] + iInputDim[3] )
		{
			if( Gui.GetKey( 256 ) )
			{
				strcpy( this->m_szInput, this->m_vInputList[ i ]->GetCaption( ) );
				return;
			}
		}

		this->m_vInputList[ i ]->Draw( );
	}
}

void CEditBox::PushInput( CButton *pObject, char *szClassName, char *szCaption )
{
	pObject = new CButton( szClassName, 0, 0, 0, 0, &Fake, szCaption, false, false, this->m_pParentWindow );

	if( strlen( szCaption ) * 7.5f > this->m_fLongestCaption )
		this->m_fLongestCaption = strlen( szCaption ) * 7.5f;

	this->m_vInputList.push_back( pObject );
}

inline void CEditBox::SetEditable( bool bState )
{
	this->m_bEditable = bState;
}

inline void CEditBox::SetMemo( CMemo *pMemo )
{
	this->m_pMemo = pMemo;
}

CButton::CButton( void ) : CObject( )
{
	this->m_bCenterTitle = false;
	this->m_bShadow = false;
	this->fnCallback = 0;
	strcpy( this->m_szCaption, "" );
}

CButton::CButton( char *szClassName, int x, int y, int iWidth, int iHeight, void ( *pFunction )( CButton * ), char *szCaption, bool bShadow, bool bCenterTitle, CWindow *pParentWindow ) : CObject( szClassName, x, y, iWidth, iHeight, pParentWindow )
{
	this->m_bCenterTitle = bCenterTitle;
	this->m_bShadow = bShadow;
	this->fnCallback = pFunction;
	strcpy( this->m_szCaption, szCaption );
}

CButton::~CButton( void )
{
}

void CButton::Draw( void )
{
	int *iInheritedDim = this->m_pParentWindow->GetDimensions( );

	// check if this is a titlebar, so we can enable dragging.
	if( ( this == this->m_pParentWindow->GetObjectPtr( OBJ_TITLEBAR ) && this->m_pParentWindow->GetDragging( ) ) 
	|| Gui.GetMouse( 0 ) >= iInheritedDim[0] + this->m_iDimensions[0] && Gui.GetMouse( 0 ) <= iInheritedDim[0] + this->m_iDimensions[0] + iInheritedDim[2] + this->m_iDimensions[2] && Gui.GetMouse( 1 ) >= iInheritedDim[1] + this->m_iDimensions[1] && Gui.GetMouse( 1 ) <= iInheritedDim[1] + this->m_iDimensions[1] + iInheritedDim[3] + this->m_iDimensions[3] )
		this->fnCallback( this );

	this->m_pParentWindow->SetScheme( this->m_pParentWindow->GetScheme( )->GetBorderColor( ), this->m_pParentWindow->GetScheme( )->GetColor( ), S_LINE_LOOP );
	this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0], iInheritedDim[1] + this->m_iDimensions[1], iInheritedDim[2] + this->m_iDimensions[2], iInheritedDim[3] + this->m_iDimensions[3] );

	if( this->m_bShadow )
	{
		// add shadow.
		this->m_pParentWindow->SetScheme( Colors::fBlack, this->m_pParentWindow->GetScheme( )->GetColor( ), S_LINE );
		this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0] + iInheritedDim[2] + this->m_iDimensions[2] + 2, iInheritedDim[1] + this->m_iDimensions[1], 1, iInheritedDim[3] + this->m_iDimensions[3] + 1 );
		this->m_pParentWindow->GetScheme( )->DrawShape( iInheritedDim[0] + this->m_iDimensions[0] + 1, iInheritedDim[1] + this->m_iDimensions[1] + iInheritedDim[3] + this->m_iDimensions[3] + 1, iInheritedDim[2] + this->m_iDimensions[2] + 1, 1 );
	}
    
	this->m_pParentWindow->ResetScheme( );

	Gui.GetFont( )->DrawString( ( this->m_bCenterTitle ? iInheritedDim[0] + this->m_iDimensions[0] + ( ( iInheritedDim[2] + this->m_iDimensions[2] ) / 2 ) : iInheritedDim[0] + this->m_iDimensions[0] + 2 ), ( this->m_bCenterTitle ? iInheritedDim[1] + this->m_iDimensions[1] + ( ( iInheritedDim[3] + this->m_iDimensions[3] ) / 2 ) : iInheritedDim[1] + this->m_iDimensions[1] + Gui.GetFont( )->GetFontHeight( ) - 3 ), this->m_pParentWindow->GetScheme( )->GetBorderColor( ), ( this->m_bCenterTitle ? FL_CENTERX | FL_CENTERY : FL_NONE ), this->m_szCaption );
}

inline char *CButton::GetCaption( void )
{
	return( this->m_szCaption );
}

inline bool CButton::GetClick( void )
{
	return( this->m_bClick );
}

CWindow::CWindow( void ) : CTemplate( )
{
	this->m_iWinNum = 0;
	this->m_bMaximized = false;
	this->m_bDragging = false;

	this->m_pScheme = new CDrawScheme;
	this->m_pTitleBar = new CButton;
	this->m_pMinBox = new CButton;

	strcpy( this->m_szTitle, "Window_Default" );
}

CWindow::CWindow( char *szClassName, int x, int y, int iWidth, int iHeight, char *szTitle, bool bMaximized, int iWinNum ) : CTemplate( szClassName, x, y, iWidth, iHeight )
{
	char szBuf[128];

	this->m_iWinNum = iWinNum;
	this->m_bMaximized = bMaximized;
	this->m_bDragging = false;

	this->m_pScheme = new CDrawScheme( Gui.GetScheme( )->GetBorderColor( ), Gui.GetScheme( )->GetColor( ), S_RECT );

	strcpy( this->m_szTitle, szTitle );

	sprintf( szBuf, "Object_%s_TitleBar", this->m_szClassName );
	this->PushObject( this->m_pTitleBar, szBuf, 0, 0, 0, 13 - iHeight, CWindow::fnTitleBar, this->m_szTitle, false, false );

	sprintf( szBuf, "Object_%s_MinBox", this->m_szClassName );
	this->PushObject( this->m_pMinBox, szBuf, iWidth - 11, 3, 9 - iWidth, 8 - iHeight, CWindow::fnMinimize, "", false, false );
	/*all object dimensions are inherited from their parent: x +, y +, iWidth +, iHeight +*/
}

CWindow::~CWindow( void )
{
}

// dunno why i pass maximized in here but cant be assed to change it.
void CWindow::Draw( bool bMaximized )
{
	int i;

	if( !bMaximized )
	{
		this->m_pTitleBar->Draw( );
		this->m_pMinBox->Draw( );
		return;
	}

	//if( Gui.GetKey( 257 ) && Gui.GetMouse( 0 ) >= this->m_iDimensions[0] && Gui.GetMouse( 0 ) <= this->m_iDimensions[0] + this->m_iDimensions[2] && Gui.GetMouse( 1 ) >= this->m_iDimensions[1] && Gui.GetMouse( 1 ) <= this->m_iDimensions[1] + this->m_iDimensions[3] )
		//TODO: right click menu to change scheme etc

	this->m_pScheme->DrawShape( this->m_iDimensions[0], this->m_iDimensions[1], this->m_iDimensions[2], this->m_iDimensions[3] );

	for( i = 0; i < ( int )this->m_vObjects.size( ); i++ )
		this->m_vObjects[ i ]->Draw( );
}

void CWindow::fnMinimize( CButton *pThis )
{
	int *iObjDim = pThis->GetDimensions( );

	if( Gui.GetKey( 256 ) )
	{
		pThis->GetParent( )->SetProperties( !pThis->GetParent( )->GetMaximized( ) );
		Gui.SetKey( 256, false );
	}
}

void CWindow::fnTitleBar( CButton *pThis )
{
	int i;
	int *iObjDim = pThis->GetDimensions( );
	int *iParentDim = pThis->GetParent( )->GetDimensions( );

	if( Gui.GetKey( 256 ) )
	{
		for( i = 0; i < Gui.GetWindowSize( ); i++ )
		{
			if( Gui.GetWindow( i )->GetDragging( ) && Gui.GetWindow( i ) != pThis->GetParent( ) )
				return;
		}

		pThis->GetParent( )->SetDragging( true );
		pThis->GetParent( )->SetDimensions( iParentDim[0] + ( int )Gui.GetMouse( 0 ) - ( int )Gui.GetLastMouse( 0 ), iParentDim[1] + ( int )Gui.GetMouse( 1 ) - ( int )Gui.GetLastMouse( 1 ), iParentDim[2], iParentDim[3] );
	}
	else
		pThis->GetParent( )->SetDragging( false );
}

inline void CWindow::ResetScheme( void )
{
	this->SetScheme( Gui.GetScheme( )->GetBorderColor( ), Gui.GetScheme( )->GetColor( ), S_RECT );
}

inline void CWindow::SetDragging( bool bState )
{
	this->m_bDragging = bState;
}

inline void CWindow::SetProperties( bool bMaximized )
{
	this->m_bMaximized = bMaximized;
}

inline void CWindow::SetScheme( float *fBorderCol, float *fColor, int iType )
{
	this->m_pScheme->SetBorderColor( fBorderCol );
	this->m_pScheme->SetColor( fColor );
	this->m_pScheme->SetType( iType );
}

inline void CWindow::SetTitle( char *szTitle )
{
	strcpy( this->m_szTitle, szTitle );
}

void CWindow::PushObject( CObject *pObject, char *szClassName, int x, int y, int iWidth, int iHeight )
{
	pObject = new CObject( szClassName, x, y, iWidth, iHeight, Gui.GetWindow( this->m_iWinNum ) );

	this->m_vObjects.push_back( pObject );
}

void CWindow::PushObject( CButton *pObject, char *szClassName, int x, int y, int iWidth, int iHeight, void ( *pFunction )( CButton * ), char *szCaption, bool bShadow, bool bCenterTitle )
{
	pObject = new CButton( szClassName, x, y, iWidth, iHeight, pFunction, szCaption, bShadow, bCenterTitle, Gui.GetWindow( this->m_iWinNum ) );

	switch( this->m_vObjects.size( ) )
	{
		case OBJ_TITLEBAR:
			this->m_pTitleBar = pObject;
			break;

		case OBJ_MINIMIZE:
			this->m_pMinBox = pObject;
			break;
	}
	
	this->m_vObjects.push_back( pObject );
}

void CWindow::PushObject( CEditBox *pObject, char *szClassName, int x, int y, int iWidth, int iHeight, void ( *pFunction )( CEditBox * ), char *szMemoName, int iMemoHeight, CMemo *pMemo )
{
	pObject = new CEditBox( szClassName, x, y, iWidth, iHeight, pFunction, Gui.GetWindow( this->m_iWinNum ) );

	this->m_vObjects.push_back( pObject );

	pMemo = new CMemo( szClassName, x, y - ( this->m_iDimensions[3] + iMemoHeight ) - 2, iWidth, iMemoHeight, Gui.GetWindow( this->m_iWinNum ) );
    
	this->m_vObjects.push_back( pMemo );

	pObject->SetMemo( pMemo );
}

inline bool CWindow::GetDragging( void )
{
	return( this->m_bDragging );
}

inline bool CWindow::GetMaximized( void )
{
	return( this->m_bMaximized );
}

inline CDrawScheme *CWindow::GetScheme( void )
{
	return( this->m_pScheme );
}

inline char *CWindow::GetTitle( void )
{
	return( this->m_szTitle );
}

inline CObject *CWindow::GetObjectPtr( int iNum )
{
	switch( iNum )
	{
		case OBJ_TITLEBAR:
			return( this->m_pTitleBar );
			
		case OBJ_MINIMIZE:
			return( this->m_pMinBox );

		default:
			break;
	}

	return( this->m_vObjects[ iNum ] );
}

inline size_t CWindow::GetObjectListSize( void )
{
	return( this->m_vObjects.size( ) );
}

CDrawScheme::CDrawScheme( void )
{
	memset( this->m_fBorderCol, 0, sizeof( this->m_fBorderCol ) );
	memset( this->m_fColor, 0, sizeof( this->m_fColor ) );

	this->m_iType = 0;
}

CDrawScheme::CDrawScheme( float *fBorderCol, float *fColor, int iType )
{
	memcpy( this->m_fBorderCol, fBorderCol, sizeof( this->m_fBorderCol ) );
	memcpy( this->m_fColor, fColor, sizeof( this->m_fColor ) );

	this->m_iType = iType;
}

CDrawScheme::~CDrawScheme( void )
{
}

void CDrawScheme::DrawShape( int x, int y, int iWidth, int iHeight )
{
	orig_glEnable( GL_BLEND );
	orig_glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	orig_glDisable( GL_TEXTURE_2D );
	orig_glDisable( GL_DEPTH_TEST );

	switch( this->m_iType )
	{
		case S_LINE:
			orig_glColor4fv( this->m_fBorderCol );
			orig_glBegin( GL_LINES );
			orig_glVertex2i( x, y );
			orig_glVertex2i( x + iWidth, y + iHeight );
			orig_glEnd( );
            break;

		case S_LINE_LOOP:
			orig_glColor4fv( this->m_fBorderCol );
			orig_glBegin( GL_LINE_LOOP );
			orig_glVertex2i( x, y );
			orig_glVertex2i( x + iWidth, y );
			orig_glVertex2i( x + iWidth, y + iHeight );
			orig_glVertex2i( x, y + iHeight );
			orig_glEnd( );
			break;
			
		case S_RECT:
			orig_glColor4fv( this->m_fColor );
			orig_glBegin( GL_QUADS );
			orig_glVertex2i( x, y );
			orig_glVertex2i( x + iWidth, y );
			orig_glVertex2i( x + iWidth, y + iHeight );
			orig_glVertex2i( x, y + iHeight );
			orig_glEnd( );

			orig_glColor4fv( this->m_fBorderCol );
			orig_glBegin( GL_LINE_LOOP );
			orig_glVertex2i( x, y );
			orig_glVertex2i( x + iWidth, y );
			orig_glVertex2i( x + iWidth, y + iHeight );
			orig_glVertex2i( x, y + iHeight );
			orig_glEnd( );
			break;
	}

	orig_glEnable( GL_DEPTH_TEST );
	orig_glEnable( GL_TEXTURE_2D );
}

inline float *CDrawScheme::GetBorderColor( void )
{
	return( this->m_fBorderCol );
}

inline float *CDrawScheme::GetColor( void )
{
	return( this->m_fColor );
}

inline int CDrawScheme::GetType( void )
{
	return( this->m_iType );
}

inline void CDrawScheme::SetBorderColor( float *fBorderCol )
{
	this->m_fBorderCol[0] = fBorderCol[0];
	this->m_fBorderCol[1] = fBorderCol[1];
	this->m_fBorderCol[2] = fBorderCol[2];
	this->m_fBorderCol[3] = fBorderCol[3];
}

inline void CDrawScheme::SetColor( float *fColor )
{
	this->m_fColor[0] = fColor[0];
	this->m_fColor[1] = fColor[1];
	this->m_fColor[2] = fColor[2];
	this->m_fColor[3] = fColor[3];
}

inline void CDrawScheme::SetType( int iType )
{
	this->m_iType = iType;
}

void ButtonTest( CButton *pThis )
{
	static bool bClicked = false;

	int *iArea = pThis->GetDimensions( );

	if( Gui.GetKey( 256 ) )
	{
		if( pThis->GetClick( ) && !bClicked )
		{
			bClicked = true;
			pThis->SetDimensions( iArea[0] + 1, iArea[1] + 1, iArea[2], iArea[3] );
		}
	}
	else if( bClicked )
	{
		bClicked = false;
		pThis->SetDimensions( iArea[0] - 1, iArea[1] - 1, iArea[2], iArea[3] );
	}
}

void InputTest( CEditBox *pThis )
{
	if( Gui.GetKey( 256 ) )
		pThis->SetEditable( true );
}

CGui::CGui( void )
{
	this->m_pFont = new CFont;
	this->m_pScheme = new CDrawScheme;

	this->m_pBlankTest = new CWindow;
	this->m_pButtonTest = new CWindow;
	this->m_pInputTest = new CWindow;

	this->m_iActive = 0;
	this->m_bDraw = false;
	this->m_hWnd = 0;
	this->m_cActivationKey = '*';
}

// if u want, pass in pointers to classes that can be instead of m_pConsole or w/e
CGui::CGui( char cActivationKey, char *szFontName, int iFontSize, float *fBorderCol, float *fColor )
{
	this->m_pFont = new CFont( szFontName, iFontSize, FW_BOLD );

	// cant be assed to add a param for iType, so default it to S_RECT
	this->m_pScheme = new CDrawScheme( fBorderCol, fColor, S_RECT );

	this->m_pBlankTest = new CWindow;
	this->m_pButtonTest = new CWindow;
	this->m_pInputTest = new CWindow;

	this->m_iActive = 0;
	this->m_bDraw = false;
	this->m_hWnd = 0;
	this->m_cActivationKey = cActivationKey;
}

CGui::~CGui( void )
{
	delete this->m_pFont;
	delete this->m_pScheme;
	delete this->m_pBlankTest;
	delete this->m_pButtonTest;
	delete this->m_pInputTest;
	delete this->m_pInputBox;
	delete this->m_pButton;
	delete this->m_pMemo;
}

inline void CGui::DrawGuiMouse( void )
{
	if( this->m_iActive != 1 )
		return;

	this->SetScheme( this->m_pScheme->GetBorderColor( ), this->m_pScheme->GetColor( ), S_LINE );

	this->m_pScheme->DrawShape( ( int )this->m_fMouse[0], ( int )this->m_fMouse[1], 0, 11 );
	this->m_pScheme->DrawShape( ( int )this->m_fMouse[0], ( int )this->m_fMouse[1], 8, 8 );
	this->m_pScheme->DrawShape( ( int )this->m_fMouse[0], ( int )this->m_fMouse[1] + 11, 8, -3 );

	this->SetScheme( this->m_pScheme->GetBorderColor( ), Colors::fPanz, S_RECT );
}

inline void CGui::DrawGuiWindows( void )
{
	int i;

	for( i = 0; i < ( int )this->m_vWindows.size( ); i++ )
		this->m_vWindows[ i ]->Draw( this->m_vWindows[ i ]->GetMaximized( ) );
}

inline void CGui::Enable( void )
{
	// lazy
	static bool bInited = false;
	
	if( !bInited )
	{
		this->PushWindow( this->m_pBlankTest, "Window_Blank", 50, 100, 130, 135, "Blank Test", true );
		this->PushWindow( this->m_pButtonTest, "Window_Button", 130, 190, 200, 135, "Button Test", true );
		this->PushWindow( this->m_pInputTest, "Window_Input", 20, 20, 250, 175, "Input Test", true );

		this->m_pInputTest->PushObject( this->m_pInputBox, "InputBox", + 3, 175 - 18, - 6, - 160, &InputTest, "Memo", 80 - 135, this->m_pMemo );
		this->m_pButtonTest->PushObject( this->m_pButton, "Button", 10, 30, 75-200, 35-135, &ButtonTest, "Click Me!", true, true );
		bInited = true;
	}

	if( !this->m_bDraw && bInited && Gui.GetActiveState( ) )
	{
		this->DrawGuiWindows( );
		this->DrawGuiMouse( );
		this->m_bDraw = true;
	}

	this->m_fLastMouse[0] = this->m_fMouse[0];
	this->m_fLastMouse[1] = this->m_fMouse[1];
}

inline int CGui::GetActiveState( void )
{
	return( true );
}

inline bool CGui::GetDrawState( void )
{
	return( this->m_bDraw );
}

inline HWND CGui::GetWinHandle( void )
{
	return( this->m_hWnd );
}

inline CFont *CGui::GetFont( void )
{
	return( this->m_pFont );
}

inline bool CGui::GetKey( int iKey )
{
	return( this->m_bKeys[ iKey ] );
}

inline float CGui::GetLastMouse( int iMouse )
{
	return( this->m_fLastMouse[ iMouse ] );
}

inline float CGui::GetMouse( int iMouse )
{
	return( this->m_fMouse[ iMouse ] );
}

inline int CGui::GetOrigMouse( int iMouse )
{
	return( this->m_iOrigMouse[ iMouse ] );
}

inline CDrawScheme *CGui::GetScheme( void )
{
	return( this->m_pScheme );
}

inline CWindow *CGui::GetWindow( int iNumber )
{
	return( this->m_vWindows[ iNumber ] );
}

inline int CGui::GetWindowSize( void )
{
	return( ( int )this->m_vWindows.size( ) );
}

int CGui::HandleMsg( int iMsg, WPARAM wParam, LPARAM lParam )
{
	if( iMsg < WM_MOUSEMOVE )
	{
		if( wParam == ( unsigned char )this->m_cActivationKey )
		{
			this->m_pFont->InitFont( );

			( this->m_iActive < 2 ? this->m_iActive++ : this->m_iActive = 0 );

			this->m_bKeys[ wParam ] = false;
			return( 0 );
		}
	}

	if( this->m_iActive == 1 )
	{
		if( wParam >= 0 && wParam <= 255 )
		{
			switch( iMsg )
			{
				case WM_CHAR:
					this->m_bKeys[ wParam ] = true;
					return( 0 );

				case WM_KEYUP:
					this->m_bKeys[ toupper( ( int )wParam ) ] = this->m_bKeys[ tolower( ( int )wParam ) ] = false;
					return( 0 );
			}
		}

		switch( iMsg )
		{
			case WM_MOUSEMOVE:
				this->m_fLastMouse[0] = this->m_fMouse[0];
				this->m_fLastMouse[1] = this->m_fMouse[1];

				this->m_fMouse[0] = MAKEPOINTS( lParam ).x;
				this->m_fMouse[1] = MAKEPOINTS( lParam ).y;
				return( 0 );

			case WM_LBUTTONDOWN:
				this->m_bKeys[256] = true;
				return( 0 );

			case WM_LBUTTONUP:
				this->m_bKeys[256] = false;
				return( 0 );

			case WM_RBUTTONDOWN:
				this->m_bKeys[257] = true;
				return( 0 );

			case WM_RBUTTONUP:
				this->m_bKeys[257] = false;
				return( 0 );

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCHAR:
				return( 0 );
		}
	}

	return( 1 );
}

void CGui::PushWindow( CWindow *pWindow, char *szClassName, int x, int y, int iWidth, int iHeight, char *szWinName, bool bMaximized )
{
	this->m_vWindows.push_back( pWindow );

	*pWindow = CWindow( szClassName, x, y, iWidth, iHeight, szWinName, bMaximized, ( int )this->m_vWindows.size( ) - 1 );
}

inline void CGui::SetDrawState( bool bState )
{
	this->m_bDraw = bState;
}

inline void CGui::SetWinHandle( HWND hWnd )
{
	this->m_hWnd = hWnd;
}

inline void CGui::SetKey( int iKey, bool bState )
{
	this->m_bKeys[ iKey ] = bState;
}

inline void CGui::SetOrigMouse( int x, int y )
{
	this->m_iOrigMouse[0] = x;
	this->m_iOrigMouse[1] = y;
}

inline void CGui::SetScheme( float *fBorderCol, float *fColor, int iType )
{
	this->m_pScheme->SetBorderColor( fBorderCol );
	this->m_pScheme->SetColor( fColor );
	this->m_pScheme->SetType( iType );
}

inline void CGui::Viewport( void )
{
	this->m_bDraw = false;
}