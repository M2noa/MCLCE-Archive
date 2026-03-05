#pragma once
#include "Screen.h"
#include "ScrolledSelectionList.h"
class ItemStat;

class StatsScreen : public Screen
{
private:
	static ItemRenderer *itemRenderer;

protected:
	static const int BUTTON_CANCEL_ID = 0;
    static const int BUTTON_STATS_ID = 1;
    static const int BUTTON_BLOCKITEMSTATS_ID = 2;
    static const int BUTTON_ITEMSTATS_ID = 3;

    Screen *lastScreen;
    wstring title;
public:
	class GeneralStatisticsList;
	class ItemStatisticsList;
	class BlockStatisticsList;
private:
	GeneralStatisticsList *statsList;
    ItemStatisticsList *itemStatsList;
    BlockStatisticsList *blockStatsList;
    StatsCounter *stats;

    ScrolledSelectionList *activeList;

public:
	StatsScreen(Screen *lastScreen, StatsCounter *stats);
    virtual void init();
    virtual void postInit();
protected:
	virtual void buttonClicked(Button *button);
public:
	virtual void render(int xm, int ym, float a);

	class GeneralStatisticsList : public ScrolledSelectionList
	{
	protected:
		StatsScreen *parent;
	public:
		GeneralStatisticsList(StatsScreen *ss);	// 4J - added parameter so we can access parent
	    virtual int getNumberOfItems();
        virtual void selectItem(int item, bool doubleClick);
        virtual bool isSelectedItem(int item);
        virtual int getMaxPosition();
        virtual void renderBackground();
        virtual void renderItem(int i, int x, int y, int h, Tesselator *t);
    };
private:

    static const float SLOT_TEX_SIZE;
    static const int SLOT_BG_SIZE = 18;
    static const int SLOT_STAT_HEIGHT = SLOT_BG_SIZE + 2;
    static const int SLOT_BG_X = 1;
    static const int SLOT_BG_Y = 1;
    static const int SLOT_FG_X = 2;
    static const int SLOT_FG_Y = 2;
    static const int SLOT_LEFT_INSERT = 40;
    static const int ROW_COL_1 = 2 + 113;
    static const int ROW_COL_2 = 2 + 163;
    static const int ROW_COL_3 = 2 + 213;
    static const int SLOT_TEXT_OFFSET = 5;
    static const int SORT_NONE = 0;
    static const int SORT_DOWN = -1;
    static const int SORT_UP = 1;

    void blitSlot(int x, int y, int item);
    void blitSlotBg(int x, int y);
    void blitSlotIcon(int x, int y, int sx, int sy);

	class StatisticsList : public ScrolledSelectionList
	{
	public:
		StatsScreen *parent;
	protected:
		int headerPressed;
        vector<ItemStat *> statItemList;
//        Comparator<ItemStat> itemStatSorter;

        int sortColumn;
        int sortOrder;
	public:
        StatisticsList(StatsScreen *ss);	// 4J - added parameter so we can access parent
        virtual void selectItem(int item, bool doubleClick);
        virtual bool isSelectedItem(int item);
        virtual void renderBackground();
        virtual void renderHeader(int x, int y, Tesselator *t);
        virtual void clickedHeader(int headerMouseX, int headerMouseY);
        virtual int getNumberOfItems();
        ItemStat *getSlotStat(int slot);
        virtual wstring getHeaderDescriptionId(int column) = 0;
        virtual void renderStat(ItemStat *stat, int x, int y, bool shaded);
        virtual void renderDecorations(int mouseX, int mouseY);
        virtual void renderMousehoverTooltip(ItemStat *stat, int x, int y);
        virtual void sortByColumn(int column);
    };

public:
    class ItemStatisticsList : public StatisticsList
	{

	private:
		static const int COLUMN_DEPLETED = 0;
        static const int COLUMN_CRAFTED = 1;
        static const int COLUMN_USED = 2;

	public:
		ItemStatisticsList(StatsScreen *ss);	// 4J - added parameter so we can access parent
		virtual void renderHeader(int x, int y, Tesselator *t);
        virtual void renderItem(int i, int x, int y, int h, Tesselator *t);
        virtual wstring getHeaderDescriptionId(int column);
	};

	class BlockStatisticsList : public StatisticsList
	{

	private:
		static const int COLUMN_CRAFTED = 0;
        static const int COLUMN_USED = 1;
        static const int COLUMN_MINED = 2;

	public:
		BlockStatisticsList(StatsScreen *ss);	// 4J - added parameter so we can access parent
		virtual void renderHeader(int x, int y, Tesselator *t);
        virtual void renderItem(int i, int x, int y, int h, Tesselator *t);
        virtual wstring getHeaderDescriptionId(int column);
	};
 
};