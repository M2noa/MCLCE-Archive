#include "stdafx.h"
#include "Tutorial.h"
#include "LookAtEntityHint.h"


LookAtEntityHint::LookAtEntityHint(eTutorial_Hint id, Tutorial *tutorial, int descriptionId, int titleId, eINSTANCEOF type)
	: TutorialHint(id, tutorial, descriptionId, e_Hint_LookAtEntity)
{
	m_type = type;
	m_titleId = titleId;
}

bool LookAtEntityHint::onLookAtEntity(eINSTANCEOF type)
{
	if(m_type == type)
	{
		// Display hint
		Tutorial::PopupMessageDetails *message = new Tutorial::PopupMessageDetails();
		message->m_messageId = m_descriptionId;
		message->m_titleId = m_titleId;
		message->m_delay = true;
		return m_tutorial->setMessage(this, message);
	}
	return false;
}