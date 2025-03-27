#ifndef BUDDY_SYSTEM_H
#define BUDDY_SYSTEM_H

#include <cstddef>
#include <vector>

class BuddySystem
{
public:
    BuddySystem(size_t totalSize = 1024 * 1024); // 1MB por defecto
    ~BuddySystem();

    static void *allocate(size_t size);
    static void free(void *ptr);

private:
    struct Block
    {
        size_t size;
        bool free;
        Block *next;
    };

    static BuddySystem *instance;
    void *memoryPool;
    size_t totalSize;
    std::vector<Block *> freeLists;

    void splitBlock(size_t level);
    size_t getLevel(size_t size);
};

#endif