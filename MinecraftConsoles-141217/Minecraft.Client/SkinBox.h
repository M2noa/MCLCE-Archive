#pragma once

enum eBodyPart
{
	eBodyPart_Unknown=0,
	eBodyPart_Head,
	eBodyPart_Body,
	eBodyPart_Arm0,
	eBodyPart_Arm1,
	eBodyPart_Leg0,
	eBodyPart_Leg1,
};

typedef struct 
{
	eBodyPart ePart;
	float fX,fY,fZ,fW,fH,fD,fU,fV;
}
SKIN_BOX;
