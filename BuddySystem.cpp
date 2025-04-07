#include "BuddySystem.h"
#include <cstring>
#include <cmath>

BuddySystem *BuddySystem::instance = nullptr;

BuddySystem::BuddySystem(size_t totalSize) : totalSize(totalSize)
{
    memoryPool = malloc(totalSize);
    freeLists.resize(ceil(log2(totalSize)) + 1, nullptr);

    Block *initialBlock = static_cast<Block *>(memoryPool);
    initialBlock->size = totalSize;
    initialBlock->free = true;
    initialBlock->next = nullptr;
    freeLists[getLevel(totalSize)] = initialBlock;
}

BuddySystem::~BuddySystem()
{
    free(memoryPool);
    delete instance;
    instance = nullptr;
}

size_t BuddySystem::getLevel(size_t size)
{
    return ceil(log2(size));
}

void BuddySystem::splitBlock(size_t level)
{
    if (level == 0 || !freeLists[level])
        return;

    Block *block = freeLists[level];
    freeLists[level] = block->next;

    size_t newSize = block->size / 2;
    Block *buddy = reinterpret_cast<Block *>(reinterpret_cast<char *>(block) + newSize);

    block->size = newSize;
    block->free = true;
    buddy->size = newSize;
    buddy->free = true;

    buddy->next = freeLists[level - 1];
    freeLists[level - 1] = block;
    freeLists[level - 1]->next = buddy;
}

void *BuddySystem::allocate(size_t size)
{
    if (!instance)
        instance = new BuddySystem(8 * 1024 * 1024); // mismo tamaño que definiste

    size_t allocSize = pow(2, ceil(log2(size)));
    size_t level = instance->getLevel(allocSize);

    if (level >= instance->freeLists.size())
        return nullptr;

    while (!instance->freeLists[level])
    {
        size_t nextLevel = level + 1;
        while (nextLevel < instance->freeLists.size() && !instance->freeLists[nextLevel])
            ++nextLevel;
        if (nextLevel >= instance->freeLists.size())
            return nullptr;
        instance->splitBlock(nextLevel);
    }

    Block *block = instance->freeLists[level];
    instance->freeLists[level] = block->next;
    block->free = false;
    return block;
}

void BuddySystem::free(void *ptr)
{
    if (!instance || !ptr)
        return;

    Block *block = static_cast<Block *>(ptr);
    block->free = true;

    size_t level = instance->getLevel(block->size);
    char *poolStart = static_cast<char *>(instance->memoryPool);
    char *blockAddr = reinterpret_cast<char *>(block);
    size_t offset = blockAddr - poolStart;
    Block *buddy = reinterpret_cast<Block *>(poolStart + (offset ^ block->size));
    while (level < instance->freeLists.size() - 1 && buddy->free && buddy->size == block->size)
    {
        if (buddy < block)
            std::swap(block, buddy);
        block->size *= 2;
        level++;

        buddy = reinterpret_cast<Block *>(poolStart + ((blockAddr - poolStart) ^ block->size));
    }

    block->next = instance->freeLists[level];
    instance->freeLists[level] = block;
}