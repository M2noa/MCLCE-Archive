#include "stdafx.h"
#include "net.minecraft.stats.h"
#include "GeneralStat.h"

GeneralStat::GeneralStat(int id, const wstring& name, StatFormatter *formatter) : Stat(id, name, formatter)
{
}

GeneralStat::GeneralStat(int id, const wstring& name) : Stat(id, name)
{
}

Stat *GeneralStat::postConstruct() 
{
    Stat::postConstruct();
    Stats::generalStats->push_back(this);
    return this;
}