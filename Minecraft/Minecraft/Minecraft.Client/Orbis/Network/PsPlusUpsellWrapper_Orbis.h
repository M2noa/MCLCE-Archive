#pragma once


// 4J-JEV: To help handle PsPlus upsells.
class PsPlusUpsellWrapper
{
private:
	bool m_bHasResponse;

	SceNpCommerceDialogResult m_result;

public:
	const int m_userIndex;

	PsPlusUpsellWrapper(int userIndex);

	bool displayUpsell();

	bool hasResponse();
};