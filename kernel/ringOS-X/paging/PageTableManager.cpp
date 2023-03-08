#include "PageTableManager.h"
#include "../kernel.h"

PageTableManager::PageTableManager(PageTable *PML4Address)
{
    this->PML4 = PML4Address;
}

void PageTableManager::MapMemory(void *virtualMemory, void *physicalMemory, bool allowCache)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable *PDP;
    
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PDP = (PageTable *)GlobalAllocator->RequestPage();
        memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::CacheDisabled, !allowCache);
        PML4->entries[indexer.PDP_i] = PDE;
    }

    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PDP->entries[indexer.PD_i];
    PageTable *PD;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PD = (PageTable *)GlobalAllocator->RequestPage();
        memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::CacheDisabled, !allowCache);
        PDP->entries[indexer.PD_i] = PDE;
    }

    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    PageTable *PT;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PT = (PageTable *)GlobalAllocator->RequestPage();
        memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::CacheDisabled, !allowCache);

        PD->entries[indexer.PT_i] = PDE;
    }

    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PDE.SetFlag(PT_Flag::CacheDisabled, !allowCache);
    PT->entries[indexer.P_i] = PDE;

    return;
}

void PageTableManager::MapFramebufferMemory(void *virtualMemory, void *physicalMemory)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable *PDP;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PDP = (PageTable *)GlobalAllocator->RequestPage();
        memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::NX, false);

        PDE.SetFlag(PT_Flag::PAT, true); // PAT5
        PDE.SetFlag(PT_Flag::CacheDisabled, false);
        PDE.SetFlag(PT_Flag::WriteThrough, true);

        PML4->entries[indexer.PDP_i] = PDE;
    }

    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PDP->entries[indexer.PD_i];
    PageTable *PD;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PD = (PageTable *)GlobalAllocator->RequestPage();
        memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::NX, false);

        PDE.SetFlag(PT_Flag::PAT, true);
        PDE.SetFlag(PT_Flag::CacheDisabled, false);
        PDE.SetFlag(PT_Flag::WriteThrough, true);

        PDP->entries[indexer.PD_i] = PDE;
    }

    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    PageTable *PT;

    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PT = (PageTable *)GlobalAllocator->RequestPage();
        memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDE.SetFlag(PT_Flag::NX, false);

        PDE.SetFlag(PT_Flag::PAT, true);
        PDE.SetFlag(PT_Flag::CacheDisabled, false);
        PDE.SetFlag(PT_Flag::WriteThrough, true);

        PD->entries[indexer.PT_i] = PDE;
    }

    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PDE.SetFlag(PT_Flag::WriteThrough, true);
    PDE.SetFlag(PT_Flag::NX, false);

    PDE.SetFlag(PT_Flag::PAT, true); // PAT5
    PDE.SetFlag(PT_Flag::CacheDisabled, false);
    PDE.SetFlag(PT_Flag::WriteThrough, true);

    PT->entries[indexer.P_i] = PDE;

    return;
}

PageTableManager GlobalPageTableManager = NULL;
