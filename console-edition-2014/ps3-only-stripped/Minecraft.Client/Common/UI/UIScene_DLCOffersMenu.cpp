#include "stdafx.h"
#include "UI.h"
#include "UIScene_DLCOffersMenu.h"
#include "..\..\..\Minecraft.World\StringHelpers.h"
#if defined(__PS3__)
#include "Common\Network\Sony\SonyHttp.h"
#endif


#define PLAYER_ONLINE_TIMER_ID 0
#define PLAYER_ONLINE_TIMER_TIME 100

UIScene_DLCOffersMenu::UIScene_DLCOffersMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	m_bProductInfoShown=false;
	DLCOffersParam *param=(DLCOffersParam *)initData;
	m_iProductInfoIndex=param->iType;
	m_iCurrentDLC=0;
	m_iTotalDLC=0;
#if defined(__PS3__)
	m_pvProductInfo=NULL;
#endif
	m_bAddAllDLCButtons=true;

	// Setup all the Iggy references we need for this scene
	initialiseMovie();
	// Alert the app the we want to be informed of ethernet connections
	app.SetLiveLinkRequired( true );

	m_bIsSD=!RenderManager.IsHiDef() && !RenderManager.IsWidescreen();

	m_labelOffers.init(app.GetString(IDS_DOWNLOADABLE_CONTENT_OFFERS));
	m_buttonListOffers.init(eControl_OffersList);
	m_labelHTMLSellText.init(" ");
	m_labelPriceTag.init(" ");
	TelemetryManager->RecordMenuShown(m_iPad, eUIScene_DLCOffersMenu, 0);

	m_bHasPurchased = false;
	m_bIsSelected = false;

	if(m_loadedResolution == eSceneResolution_1080)
	{
		m_labelXboxStore.init( L"" );
	}



#if defined __PS3__
	addTimer( PLAYER_ONLINE_TIMER_ID, PLAYER_ONLINE_TIMER_TIME );
#endif

}

UIScene_DLCOffersMenu::~UIScene_DLCOffersMenu()
{
	// Alert the app the we no longer want to be informed of ethernet connections
	app.SetLiveLinkRequired( false );
}

void UIScene_DLCOffersMenu::handleTimerComplete(int id)
{
#if defined __PS3__
	switch(id)
	{
	case PLAYER_ONLINE_TIMER_ID:
		if(ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad())==false)
		{
			// check the player hasn't gone offline
			// If they have, bring up the PSN warning and exit from the DLC menu
			unsigned int uiIDA[1];
			uiIDA[0]=IDS_OK;
			C4JStorage::EMessageResult result = ui.RequestMessageBox( IDS_CONNECTION_LOST, g_NetworkManager.CorrectErrorIDS(IDS_CONNECTION_LOST_LIVE_NO_EXIT), uiIDA,1,ProfileManager.GetPrimaryPad(),UIScene_DLCOffersMenu::ExitDLCOffersMenu,this, app.GetStringTable());
		}
		break;
	}
#endif
}

int UIScene_DLCOffersMenu::ExitDLCOffersMenu(void *pParam,int iPad,C4JStorage::EMessageResult result)
{
	UIScene_DLCOffersMenu* pClass = (UIScene_DLCOffersMenu*)pParam;

	ui.NavigateToHomeMenu();//iPad,eUIScene_MainMenu);

	return 0;
}

wstring UIScene_DLCOffersMenu::getMoviePath()
{
	return L"DLCOffersMenu";
}

void UIScene_DLCOffersMenu::updateTooltips()
{
	int iA = -1;
	if(m_bIsSelected)
	{
		if( !m_bHasPurchased )
		{
			iA = IDS_TOOLTIPS_INSTALL;
		}
		else
		{
			iA = IDS_TOOLTIPS_REINSTALL;
		}
	}
	ui.SetTooltips( m_iPad, iA,IDS_TOOLTIPS_BACK);
}

void UIScene_DLCOffersMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			navigateBack();
		}
		break;
	case ACTION_MENU_OK:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	case ACTION_MENU_UP:
		if(pressed)
		{
			// 4J - TomK don't proceed if there is no DLC to navigate through
			if(m_iTotalDLC > 0)
			{
				if(m_iCurrentDLC > 0)
					m_iCurrentDLC--;

				m_bProductInfoShown = false;
			}
		}
		sendInputToMovie(key, repeat, pressed, released);
		break;

	case ACTION_MENU_DOWN:
		if(pressed)
		{
			// 4J - TomK don't proceed if there is no DLC to navigate through
			if(m_iTotalDLC > 0)
			{
				if(m_iCurrentDLC < (m_iTotalDLC - 1))
					m_iCurrentDLC++;

				m_bProductInfoShown = false;
			}
		}
		sendInputToMovie(key, repeat, pressed, released);
		break;

	case ACTION_MENU_LEFT:
		/*
#ifdef _DEBUG
	static int iTextC=0;
	switch(iTextC)
	{
	case 0:
		m_labelHTMLSellText.init("Voici un fantastique mini-pack de 24 apparences pour personnaliser votre personnage Minecraft et vous mettre dans l'ambiance des f�tes de fin d'ann�e.<br><br>1-4 joueurs<br>2-8 joueurs en r�seau<br><br>  Cet article fait l�objet d�une licence ou d�une sous-licence de Sony Computer Entertainment America, et est soumis aux conditions g�n�rales du service du r�seau, au contrat d�utilisateur, aux restrictions d�utilisation de cet article et aux autres conditions applicables, disponibles sur le site www.us.playstation.com/support/useragreements. Si vous ne souhaitez pas accepter ces conditions, ne t�l�chargez pas ce produit. Cet article peut �tre utilis� avec un maximum de deux syst�mes PlayStation�3 activ�s associ�s � ce compte Sony Entertainment Network.�<br><br>'Minecraft' est une marque commerciale de Notch Development AB.");
		break;
	case 1:
		m_labelHTMLSellText.init("Un fabuloso minipack de 24 aspectos para personalizar tu personaje de Minecraft y ponerte a tono con las fiestas.<br><br>1-4 jugadores<br>2-8 jugadores en red<br><br>  Sony Computer Entertainment America le concede la licencia o sublicencia de este art�culo, que est� sujeto a los t�rminos de servicio y al acuerdo de usuario de la red. Las restricciones de uso de este art�culo, as� como otros t�rminos aplicables, se encuentran en www.us.playstation.com/support/useragreements. Si no desea aceptar todos estos t�rminos, no descargue este art�culo. Este art�culo puede usarse en hasta dos sistemas PlayStation�3 activados asociados con esta cuenta de Sony Entertainment Network.�<br><br>'Minecraft' es una marca comercial de Notch Development AB.");
		break;
	case 2:
		m_labelHTMLSellText.init("Este � um incr�vel pacote com 24 capas para personalizar seu personagem no Minecraft e entrar no clima de final de ano.<br><br>1-4 Jogadores<br>Jogadores em rede 2-8<br><br>  Este item est� sendo licenciado ou sublicenciado para voc� pela Sony Computer Entertainment America e est� sujeito aos Termos de Servi�o da Rede e Acordo do Usu�rio, as restri��es de uso deste item e outros termos aplic�veis est�o localizados em www.us.playstation.com/support/useragreements. Caso n�o queira aceitar todos esses termos, n�o baixe este item. Este item pode ser usado com at� 2 sistemas PlayStation�3 ativados associados a esta Conta de Rede Sony Entertainment.�<br><br>'Minecraft' � uma marca registrada da Notch Development AB");
		break;
	}
	iTextC++;
	if(iTextC>2) iTextC=0;
#endif
	*/
	case ACTION_MENU_RIGHT:
	case ACTION_MENU_OTHER_STICK_DOWN:
	case ACTION_MENU_OTHER_STICK_UP:
	// don't pass down PageUp or PageDown because this will cause conflicts between the buttonlist and scrollable html text component
	//case ACTION_MENU_PAGEUP:	
	//case ACTION_MENU_PAGEDOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_DLCOffersMenu::handlePress(F64 controlId, F64 childId)
{
	switch((int)controlId)
	{
	case eControl_OffersList:
		{
#if defined(__PS3__)
			// buy the DLC

			vector<SonyCommerce::ProductInfo >::iterator it = m_pvProductInfo->begin();
			string teststring;
			for(int i=0;i<childId;i++)
			{
				it++;
			}

			SonyCommerce::ProductInfo info = *it;

#ifdef __PS3__
			// is the item purchasable?
			if(info.purchasabilityFlag==1) 
			{
				// can be bought
				app.Checkout(info.skuId);
			}
			else
			{
				if((info.annotation & (SCE_NP_COMMERCE2_SKU_ANN_PURCHASED_CANNOT_PURCHASE_AGAIN | SCE_NP_COMMERCE2_SKU_ANN_PURCHASED_CAN_PURCHASE_AGAIN))!=0)
				{
 					app.DownloadAlreadyPurchased(info.skuId);
				}
			}
#else // __ORBIS__
			// is the item purchasable?
			if(info.purchasabilityFlag==SCE_TOOLKIT_NP_COMMERCE_NOT_PURCHASED) 
			{
				// can be bought
				app.Checkout(info.skuId);
			}
			else
			{
				app.DownloadAlreadyPurchased(info.skuId);
			}
#endif // __PS3__
#else
			int iIndex = (int)childId;

			ULONGLONG ullIndexA[1];
			ullIndexA[0]=StorageManager.GetOffer(iIndex).qwOfferID;
			StorageManager.InstallOffer(1,ullIndexA,NULL,NULL);
#endif
		}
		break;
	}
}

void UIScene_DLCOffersMenu::handleSelectionChanged(F64 selectedId)
{

}

void UIScene_DLCOffersMenu::handleFocusChange(F64 controlId, F64 childId)
{	
	app.DebugPrintf("UIScene_DLCOffersMenu::handleFocusChange\n");



}

void UIScene_DLCOffersMenu::tick()
{
	UIScene::tick();

#if defined(__PS3__)

	if(m_bAddAllDLCButtons)
	{
		// need to fill out all the dlc buttons

		if((m_bProductInfoShown==false) && app.GetCommerceProductListRetrieved() && app.GetCommerceProductListInfoRetrieved())
		{
			m_bAddAllDLCButtons=false;
			// add the categories to the list box
			if(m_pvProductInfo==NULL)
			{
				m_pvProductInfo=app.GetProductList(m_iProductInfoIndex);
				if(m_pvProductInfo==NULL)
				{
					m_iTotalDLC=0;
					// need to display text to say no downloadable content available yet
					m_labelOffers.setLabel(app.GetString(IDS_NO_DLCCATEGORIES));

					m_bProductInfoShown=true;
					return;
				}
				else m_iTotalDLC=m_pvProductInfo->size();
			}

			vector<SonyCommerce::ProductInfo >::iterator it = m_pvProductInfo->begin();
			string teststring;
			bool bFirstItemSet=false;
			for(int i=0;i<m_iTotalDLC;i++)
			{
				SonyCommerce::ProductInfo info = *it;

				if(strncmp(info.productName,"Minecraft ",10)==0)
				{
					teststring=&info.productName[10];

				}
				else
				{
					teststring=info.productName;
				}

				bool bDLCIsAvailable=false;

#ifdef __PS3__
				// is the item purchasable?
				if(info.purchasabilityFlag==1) 
				{
					// can be bought
					app.DebugPrintf("Adding DLC (%s) - not bought\n",teststring.c_str());
					m_buttonListOffers.addItem(teststring,false,i);
					bDLCIsAvailable=true;
				}
				else
				{
					if((info.annotation & (SCE_NP_COMMERCE2_SKU_ANN_PURCHASED_CANNOT_PURCHASE_AGAIN | SCE_NP_COMMERCE2_SKU_ANN_PURCHASED_CAN_PURCHASE_AGAIN))!=0)
					{
						app.DebugPrintf("Adding DLC (%s) - bought\n",teststring.c_str());
						m_buttonListOffers.addItem(teststring,true,i);
						bDLCIsAvailable=true;
					}
				}
#else // __ORBIS__
				// is the item purchasable?
				if(info.purchasabilityFlag==SCE_TOOLKIT_NP_COMMERCE_NOT_PURCHASED) 
				{
					// can be bought
					m_buttonListOffers.addItem(teststring,false,i);
					bDLCIsAvailable=true;
				}
				else
				{
					m_buttonListOffers.addItem(teststring,true,i);
					bDLCIsAvailable=true;
				}
#endif // __PS3__

				// set the other details for the first item
				if(bDLCIsAvailable && (bFirstItemSet==false))
				{
					bFirstItemSet=true;

					// 4J-PB - info.longDescription isn't null terminated
					char chLongDescription[SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN+1];
					memcpy(chLongDescription,info.longDescription,SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN);
					chLongDescription[SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN]=0;
					m_labelHTMLSellText.setLabel(chLongDescription);

					if(info.ui32Price==0)
					{
						m_labelPriceTag.setLabel(app.GetString(IDS_DLC_PRICE_FREE));
					}
					else
					{
						teststring=info.price;
						m_labelPriceTag.setLabel(teststring);
					}

					// get the image - if we haven't already
					wstring textureName = filenametowstring(info.imageUrl);

					if(hasRegisteredSubstitutionTexture(textureName)==false)
					{
						PBYTE pbImageData;
						int iImageDataBytes=0;
						bool bDeleteData;
						if(info.imageUrl[0]!=0)
						{
							SonyHttp::getDataFromURL(info.imageUrl,(void **)&pbImageData,&iImageDataBytes);
							bDeleteData=true;
						}

						if(iImageDataBytes!=0)
						{
							// set the image	
							registerSubstitutionTexture(textureName,pbImageData,iImageDataBytes,bDeleteData);
							m_bitmapIconOfferImage.setTextureName(textureName);
							// 4J Stu - Don't delete this
							//delete [] pbImageData;
						}
						else
						{
							m_bitmapIconOfferImage.setTextureName(L"");
						}
					}
					else
					{
						m_bitmapIconOfferImage.setTextureName(textureName);
					}
				}
				it++;
			}

			if(bFirstItemSet==false)
			{
				// we were not able to add any items to the list
				m_labelOffers.setLabel(app.GetString(IDS_NO_DLCCATEGORIES));
			}
			else
			{
				// set the focus to the first thing in the categories if there are any
				if(m_pvProductInfo->size()>0)
				{
					m_buttonListOffers.setFocus(true);
				}
				else
				{
					// need to display text to say no downloadable content available yet
					m_labelOffers.setLabel(app.GetString(IDS_NO_DLCCATEGORIES));
				}
			}
			
			m_Timer.setVisible(false);
			m_bProductInfoShown=true;
		}
	}
	else
	{


		// just update the details based on what the current selection is / TomK-4J - don't proceed if total DLC is 0 (bug 4757)
		if((m_bProductInfoShown==false) && app.GetCommerceProductListRetrieved()&& app.GetCommerceProductListInfoRetrieved() && m_iTotalDLC > 0)
		{


			vector<SonyCommerce::ProductInfo >::iterator it = m_pvProductInfo->begin();
			string teststring;
			for(int i=0;i<m_iCurrentDLC;i++)
			{
				it++;
			}

			SonyCommerce::ProductInfo info = *it;

			// 4J-PB - info.longDescription isn't null terminated
			char chLongDescription[SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN+1];
			memcpy(chLongDescription,info.longDescription,SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN);
			chLongDescription[SCE_NP_COMMERCE2_PRODUCT_LONG_DESCRIPTION_LEN]=0;
			m_labelHTMLSellText.setLabel(chLongDescription);

			if(info.ui32Price==0)
			{
				m_labelPriceTag.setLabel(app.GetString(IDS_DLC_PRICE_FREE));
			}
			else
			{
				teststring=info.price;
				m_labelPriceTag.setLabel(teststring);
			}

			// get the image

			// then retrieve from the web
			wstring textureName = filenametowstring(info.imageUrl);

			if(hasRegisteredSubstitutionTexture(textureName)==false)
			{
				PBYTE pbImageData;
				int iImageDataBytes=0;
				bool bDeleteData;
				{
					SonyHttp::getDataFromURL(info.imageUrl,(void **)&pbImageData,&iImageDataBytes);
					bDeleteData=true;
				}

				if(iImageDataBytes!=0)
				{
					// set the image
					registerSubstitutionTexture(textureName,pbImageData,iImageDataBytes, bDeleteData);
					m_bitmapIconOfferImage.setTextureName(textureName);

					// 4J Stu - Don't delete this
					//delete [] pbImageData;
				}
				else
				{
					m_bitmapIconOfferImage.setTextureName(L"");
				}			
			}
			else
			{
				m_bitmapIconOfferImage.setTextureName(textureName);
			}
			m_bProductInfoShown=true;
			m_Timer.setVisible(false);
		}

	}
#endif
}



#ifdef __PS3__
void UIScene_DLCOffersMenu::HandleDLCInstalled()
{
	app.DebugPrintf(4,"UIScene_DLCOffersMenu::HandleDLCInstalled\n");

// 	m_buttonListOffers.clearList();
// 	m_bAddAllDLCButtons=true;
// 	m_bProductInfoShown=false;
}

// void UIScene_DLCOffersMenu::HandleDLCMountingComplete()
// {	
//	app.DebugPrintf(4,"UIScene_SkinSelectMenu::HandleDLCMountingComplete\n");
//}


#endif