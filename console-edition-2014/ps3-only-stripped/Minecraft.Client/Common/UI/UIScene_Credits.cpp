#include "stdafx.h"
#include "UI.h"
#include "UIScene_Credits.h"

#define CREDIT_ICON -2

SCreditTextItemDef UIScene_Credits::gs_aCreditDefs[MAX_CREDIT_STRINGS] = 
{
	{ L"MOJANG",										NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eExtraLargeText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"%ls",											IDS_CREDITS_ORIGINALDESIGN,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Markus Persson",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"%ls",											IDS_CREDITS_PMPROD,				NO_TRANSLATED_STRING,eLargeText },
	{ L"Daniel Kaplan",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"%ls",											IDS_CREDITS_RESTOFMOJANG,		NO_TRANSLATED_STRING,eMediumText },																						
	{ L"%ls",											IDS_CREDITS_LEADPC,				NO_TRANSLATED_STRING,eLargeText },
	{ L"Jens Bergensten",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_JON_KAGSTROM,		NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_CEO,				NO_TRANSLATED_STRING,eLargeText },
	{ L"Carl Manneh",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_DOF,				NO_TRANSLATED_STRING,eLargeText },
	{ L"Lydia Winters",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_WCW,				NO_TRANSLATED_STRING,eLargeText },
	{ L"Karin Severinsson",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_CUSTOMERSUPPORT,	NO_TRANSLATED_STRING,eLargeText },
	{ L"Marc Watson",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line																					
	{ L"%ls",											IDS_CREDITS_DESPROG,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Aron Nieminen",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line																					
	{ L"%ls",											IDS_CREDITS_CHIEFARCHITECT,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Daniel Frisk",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_CODENINJA,			NO_TRANSLATED_STRING,eLargeText },
	{ L"%ls",											IDS_CREDITS_TOBIAS_MOLLSTAM,	NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_OFFICEDJ,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Kristoffer Jelbring",							NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_DEVELOPER,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Leonard Axelsson",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																						
	{ L"%ls",											IDS_CREDITS_BULLYCOORD,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Jakob Porser",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_ARTDEVELOPER,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Junkboy",										NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_EXPLODANIM,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Mattis Grahm",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"%ls",											IDS_CREDITS_CONCEPTART,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Henrik Petterson",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																																												
	{ L"%ls",											IDS_CREDITS_CRUNCHER,			NO_TRANSLATED_STRING,eLargeText },
	{ L"Patrick Geuder",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																						
	{ L"%ls",											IDS_CREDITS_MUSICANDSOUNDS,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Daniel Rosenfeld (C418)",						NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line																						
	{ L"4J Studios",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eExtraLargeText },
	{ L"%ls",											IDS_CREDITS_PROGRAMMING,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Paddy Burns",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Richard Reavy",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Stuart Ross",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"James Vaughan",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Mark Hughes",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																					
	{ L"Harry Gordon",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																					
	{ L"Thomas Kronberg",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	


	{ L"%ls",											IDS_CREDITS_ART,				NO_TRANSLATED_STRING,eLargeText },
	{ L"David Keningale",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Alan Redmond",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Chris Reeves",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Kate Wright",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Michael Hansen",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Donald Robertson",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },																						
	{ L"Jamie Keddie",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Thomas Naylor",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Brian Lindsay",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Hannah Watts",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Rebecca O'Neil",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },

	{ L"%ls",											IDS_CREDITS_QA,					NO_TRANSLATED_STRING,eLargeText },
	{ L"Steven Gary Woodward",							NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"George Vaughan",								NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"%ls",											IDS_CREDITS_SPECIALTHANKS,		NO_TRANSLATED_STRING,eLargeText },
	{ L"Chris van der Kuyl",							NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Roni Percy",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Anne Clarke",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
	{ L"Anthony Kent",									NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },
#if defined(__PS3__)
// font credits
	{ L"",												NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"%ls",											IDS_DYNAFONT,					NO_TRANSLATED_STRING,eLargeText },

#elif defined(_WIN64)
#endif

// Miles & Iggy credits
	{ L"",													NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"",													CREDIT_ICON,					eCreditIcon_Iggy,eSmallText },	// extra blank line
	{ L"Uses Iggy.",										NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#ifdef __PS3__
	{ L"Copyright (C) 2009-2013 by RAD Game Tools, Inc.",	NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#else
	{ L"Copyright (C) 2009-2014 by RAD Game Tools, Inc.",	NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#endif
	{ L"",													NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"",													CREDIT_ICON,					eCreditIcon_Miles,eSmallText },	// extra blank line
	{ L"Uses Miles Sound System.",							NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#ifdef __PS3__
	{ L"Copyright (C) 1991-2013 by RAD Game Tools, Inc.",	NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#else
	{ L"Copyright (C) 1991-2014 by RAD Game Tools, Inc.",	NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#endif
#ifdef __PS3__
	{ L"",													NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"",													CREDIT_ICON,					eCreditIcon_Dolby,eSmallText },	// extra blank line
	{ L"Dolby and the double-D symbol",						NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
	{ L"are trademarks of Dolby Laboratories.",				NO_TRANSLATED_STRING,			NO_TRANSLATED_STRING,eSmallText },	// extra blank line
#endif
};

UIScene_Credits::UIScene_Credits(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bAddNextLabel = false;

	// How many lines of text are in the credits?
	m_iNumTextDefs = MAX_CREDIT_STRINGS;

	// Are there any additional lines needed for the DLC credits?
	m_iNumTextDefs+=app.GetDLCCreditsCount();

	m_iCurrDefIndex = -1;

	// Add the first 20 Flash can cope with
	for(unsigned int i = 0; i < 20; ++i)
	{
		++m_iCurrDefIndex;

		// Set up the new text element.
		if ( gs_aCreditDefs[i].m_iStringID[0] == NO_TRANSLATED_STRING )
		{
			setNextLabel(gs_aCreditDefs[i].m_Text,gs_aCreditDefs[i].m_eType);
		}
		else // using additional translated string.
		{
			LPWSTR creditsString = new wchar_t[ 128 ];
			if(gs_aCreditDefs[i].m_iStringID[1]!=NO_TRANSLATED_STRING)
			{
				swprintf( creditsString, 128, gs_aCreditDefs[i].m_Text, app.GetString( gs_aCreditDefs[i].m_iStringID[0] ),  app.GetString( gs_aCreditDefs[i].m_iStringID[1] ) );	
			}
			else
			{
				swprintf( creditsString, 128, gs_aCreditDefs[i].m_Text, app.GetString( gs_aCreditDefs[i].m_iStringID[0] ) );	
			}
			setNextLabel(creditsString,gs_aCreditDefs[i].m_eType);
			delete [] creditsString;
		}
	}
}

wstring UIScene_Credits::getMoviePath()
{
	return L"Credits";
}

void UIScene_Credits::updateTooltips()
{
	ui.SetTooltips( m_iPad, -1, IDS_TOOLTIPS_BACK);
}

void UIScene_Credits::updateComponents()
{
	m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
}

void UIScene_Credits::handleReload()
{
	// We don't allow this in splitscreen, so just go back
	navigateBack();
}

void UIScene_Credits::tick()
{
	UIScene::tick();

	if(m_bAddNextLabel)
	{
		m_bAddNextLabel = false;

		const SCreditTextItemDef* pDef;

		// Time to create next text item.
		++m_iCurrDefIndex;

		// Wrap back to start.
		if ( m_iCurrDefIndex >= m_iNumTextDefs )
		{
			m_iCurrDefIndex = 0;
		}

		if(m_iCurrDefIndex >= MAX_CREDIT_STRINGS)
		{
			app.DebugPrintf("DLC credit %d\n",m_iCurrDefIndex-MAX_CREDIT_STRINGS);
			// DLC credit
			pDef = app.GetDLCCredits(m_iCurrDefIndex-MAX_CREDIT_STRINGS);
		}
		else
		{
			// Get text def for this item.
			pDef = &( gs_aCreditDefs[ m_iCurrDefIndex ] );
		}

		// Set up the new text element.
		if(pDef->m_Text!=NULL) // 4J-PB - think the RAD logo ones aren't set up yet and are coming is as null
		{	

			if ( pDef->m_iStringID[0] == CREDIT_ICON )
			{
				addImage((ECreditIcons)pDef->m_iStringID[1]);
			}
			else if ( pDef->m_iStringID[0] == NO_TRANSLATED_STRING )
			{
				setNextLabel(pDef->m_Text,pDef->m_eType);
			}
			else // using additional translated string.
			{
				LPWSTR creditsString = new wchar_t[ 128 ];
				if(pDef->m_iStringID[1]!=NO_TRANSLATED_STRING)
				{
					swprintf( creditsString, 128, pDef->m_Text, app.GetString( pDef->m_iStringID[0] ),  app.GetString( pDef->m_iStringID[1] ) );	
				}
				else
				{
					swprintf( creditsString, 128, pDef->m_Text, app.GetString( pDef->m_iStringID[0] ) );	
				}
				setNextLabel(creditsString,pDef->m_eType);
				delete [] creditsString;
			}
		}
	}
}

void UIScene_Credits::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %ls, pressed- %ls, released- %ls\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");

	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed && !repeat)
		{
			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_Credits::setNextLabel(const wstring &label, ECreditTextTypes size)
{
	IggyDataValue result;
	IggyDataValue value[3];

	IggyStringUTF16 stringVal;
	stringVal.string = (IggyUTF16*)label.c_str();
	stringVal.length = label.length();
	value[0].type = IGGY_DATATYPE_string_UTF16;
	value[0].string16 = stringVal;

	value[1].type = IGGY_DATATYPE_number;
	value[1].number = (int)size;

	value[2].type = IGGY_DATATYPE_boolean;
	value[2].boolval = (m_iCurrDefIndex == (m_iNumTextDefs - 1));

	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcSetNextLabel , 3 , value );
}

void UIScene_Credits::addImage(ECreditIcons icon)
{
	IggyDataValue result;
	IggyDataValue value[2];

	value[0].type = IGGY_DATATYPE_number;
	value[0].number = (int)icon;

	value[1].type = IGGY_DATATYPE_boolean;
	value[1].boolval = (m_iCurrDefIndex == (m_iNumTextDefs - 1));

	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcAddImage , 2 , value );	
}

void UIScene_Credits::handleRequestMoreData(F64 startIndex, bool up)
{
	m_bAddNextLabel = true;
}
