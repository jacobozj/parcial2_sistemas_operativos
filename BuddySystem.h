#ifndef BUDDY_SYSTEM_H
#define BUDDY_SYSTEM_H

#include <cstddef>
#include <vector>

class BuddySystem
{
public:
    BuddySystem(size_t totalSize = 8 * 1024 * 1024); // 1 MB por defecto
    ~BuddySystem();

    static void *allocate(size_t size);
    static void free(void *ptr);

    // 🔓 Hacemos pública la estructura para que pueda usarse en ImageProcessor
    struct Block
    {
        size_t size;
        bool free;
        Block *next;
    };

private:
    static BuddySystem *instance;
    void *memoryPool;
    size_t totalSize;
    std::vector<Block *> freeLists;

    void splitBlock(size_t level);
    size_t getLevel(size_t size);
};

#endif
