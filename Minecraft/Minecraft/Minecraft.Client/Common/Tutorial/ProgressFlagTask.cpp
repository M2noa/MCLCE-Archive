#include "stdafx.h"
#include "ProgressFlagTask.h"

bool ProgressFlagTask::isCompleted()
{
	switch( m_type )
	{
	case e_Progress_Set_Flag:
		(*flags) |= m_mask;
		bIsCompleted = true;
		break;
	case e_Progress_Flag_On:
		bIsCompleted = ((*flags) & m_mask) == m_mask;
		break;
	}
	return bIsCompleted;
}