#ifndef CORE
#define CORE 1

#include <stdint.h>
#include <stdbool.h>

#pragma region types
#define bool32 int
#define uchar unsigned char
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t
#define f32 float
#define f64 double
#pragma endregion

#pragma region macros
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
// long long (LL) == int64_t are 64 bits 
#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)
#if DEBUG_BUILD
    void _console_write_error(char *message)
    {
        int char_written_count = 0;
        char sentence_buffer[8192] = {0};
        char_written_count = sprintf_s(sentence_buffer, sizeof(sentence_buffer), "ERROR: %s \033[0m", message);
        // Write to terminal
        if (char_written_count < 0) {
            puts("Failed to format the error message.");
            return;
        }
        fprintf(stderr, "%s\n", sentence_buffer);
    }

    #define DEBUG(message) printf(message); fflush(stdout);
    #define DEBUG_BREAK() __debugbreak()


    // Write to null and crash the program *(int *)0 = 0;                 
    // if the input expression is false, write to null pointer causing program to crash.
    #define ASSERT(expression, message)    \
    {                                      \
        if(!(expression))                  \
        {                                  \
            _console_write_error(message); \
            DEBUG_BREAK();                 \
        }                                  \
    }                                      
#else
    // If not debugging, this expands to nothing, so there is no cost of leaving this in your production build
    #define ASSERT(expression, message)
    #define DEBUG(message)
#endif
#pragma endregion




/// @brief Check if input x is a power of 2 (1, 2, 4, 8, 16, ...).
/// @param x 64 bit usigned integer.
/// @return 
int is_power_of_two(u64 x)
{
	u64 y = (x-1);
    int answer = (x & y) == 0;
    return answer;
}

#pragma region arenas

/// @brief An arena is a memory management data structure. It is a tool for working on a block of memory.
typedef struct arena arena;
struct arena 
{
    void *buffer;
    size_t length;
    size_t last_offset;
    size_t current_offset;
};

void arena_init(arena *new, void *buffer, size_t size)
{
    new->buffer = buffer;
    new->length = size;
    new->last_offset = 0;
    new->current_offset = 0;
}


uintptr_t pointer_align_forward(uintptr_t pointer, size_t alignment)
{
    ASSERT(is_power_of_two(alignment), "Arena pointer is not a power of 2.");
    uintptr_t  p, a, modulo;
    p = pointer;
    a = (uintptr_t) alignment;
    modulo = p & (a-1);
    if (modulo) 
    {
        p += a - modulo;
    }
    return p;
}


void *arena_alloc_align(arena *arena, size_t size, size_t align)
{
    // Align 'current_offset' forward to the specified alignment
	uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->current_offset;
	uintptr_t offset = pointer_align_forward(curr_ptr, align);
	offset -= (uintptr_t)arena->buffer; // Change to relative offset
	// Check to see if the backing memory has space left
	if (offset+size > arena->length) 
    {
        // Return NULL if the arena is out of memory (or handle differently)
        return 0;
	}
    void *ptr = (u8 *)arena->buffer + offset;
    arena->last_offset = offset;
    arena->current_offset = offset+size;
    // Zero new memory by default
    memset((u8*)ptr, 0, size);
    return ptr;
}


#define arena_alloc_array(arena, count, type) (type *) arena_alloc_align(arena, count*sizeof(type), sizeof(type))
void *arena_alloc(arena *arena, size_t size)
{
    void *allocation = arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
    return allocation;
}


void arena_free_all(arena *a)
{
	a->current_offset = 0;
	a->last_offset = 0;
}
#pragma endregion


char * read_file(const char *file, arena *scratch)
{
    FILE *stream;
    char *contents = 0;
    fopen_s(&stream, file, "rb");
    ASSERT(stream, "Failed to read file.");
    fseek(stream, 0, SEEK_END);
    size_t file_size = ftell(stream);
    contents = (char*) arena_alloc(scratch, (file_size + 1)); 
    fseek(stream, 0, SEEK_SET);
    size_t bytes_read = fread(contents, 1, file_size, stream);
    bool success = bytes_read == file_size;
    ASSERT(success, "Read incorrect number of bytes from file.");
    contents[file_size] = '\0';
    fclose(stream);
    return contents;
}

#endif
