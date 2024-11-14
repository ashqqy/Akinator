#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Common.h"

//-------------------------------------------------------

/// @brief Функция выделения блока памяти
/// @param n_elems Новое количество элементов для выделения
/// @param size_elems Размер элементов для выделения
/// @param poison Указатель на ядовитое значение, которым заполнятся выделенные ячейки
/// @return Указатель на выделенный блок памяти

void* MyCalloc (size_t n_elems, size_t size_elems, const void* poison)
{
    void* mem_ptr = calloc (n_elems, size_elems);
    if (mem_ptr == NULL)
        return NULL;
    for (size_t i = 0; i < n_elems; i++)
        memcpy ((char*) mem_ptr + i * size_elems, poison, size_elems);
    return mem_ptr;
}

//-------------------------------------------------------

/// @brief Функция изменения размера уже выделенного блока памяти
/// @param[in] memory Указатель на выделенный блок памяти
/// @param[in] n_elements Новое количество элементов для выделения
/// @param[in] size_elements Размер элементов для выделения
/// @param[in] previous_n_elements Количество элементов в блоке до вызова функции
/// @param[in] poison Указатель на ядовитое значение, которым заполнятся добавленные ячейки
/// @return Указатель на перевыделенный блок памяти

void* MyRecalloc (void* memory, size_t n_elements, size_t size_elements, size_t previous_n_elements, const void* poison)
{
    void* save_memory = memory;
    memory = realloc (memory, n_elements * size_elements);
    /// Если память не удалось перевыделить, то освобождаем старый указатель и возвращаем 0
    if (memory == NULL)
    {
        free (save_memory); save_memory = NULL;
        return NULL;
    }

    /// Если увеличиваем блок памяти, то новые ячейки заполняем ядовитым значением
    if (previous_n_elements < n_elements)
        for (size_t i = 0; i < n_elements - previous_n_elements; i++)
            memcpy(((char*) memory + (previous_n_elements + i) * size_elements), poison, size_elements);

    /// Если уменьшаем блок памяти, то удаленные ячейки обнуляем
    if (previous_n_elements > n_elements)
        for (size_t i = 0; i < previous_n_elements - n_elements; i++)
            memset ((char*) memory + n_elements * size_elements, 0, size_elements);

    return memory;
}

//--------------------------------------------------------------

size_t FileSizeFinder (FILE* file_input)
    {
    fseek (file_input, 0L, SEEK_END); //лучше fstat
    size_t size_file_input = ftell(file_input);
    fseek(file_input, 0L, SEEK_SET);

    return size_file_input;
    }

//--------------------------------------------------------------

void CleanBufer ()
    {
    int last_symb = 0;
    while ((last_symb = getchar ()) != '\n' && last_symb != EOF)
        {
        }
    }
    
//--------------------------------------------------------------

/// @brief НЕ РАБОТАЕТ В ГРАНИЧНОМ СЛУЧАЕ
/// @return 
char* ScanfAndAllocateBuffer ()
{
    int user_ans_len = 11;
    int n_readed = 0;
    char* user_ans = (char*) calloc (user_ans_len, sizeof(char));
    if (user_ans == NULL)
        return NULL;
    *(user_ans + user_ans_len - 1) = '\n';

    while (1)
    {
        scanf ("%10s", user_ans + n_readed);
        n_readed += 10;
        for (int i = 0; i < user_ans_len; ++i)
            printf ("[%d]", *(user_ans + i));
        printf ("\n\n");

        if (*(user_ans + user_ans_len - 1) != '\n')
            {
            user_ans = (char*) MyRecalloc (user_ans, user_ans_len * 2 - 1, sizeof (char), user_ans_len, &POISON);
            if (user_ans == NULL)
                return NULL;
            user_ans_len = user_ans_len * 2 - 1;
            *(user_ans + user_ans_len - 1) = '\n';
            }
        else
            {
            break;
            }
    }

    return user_ans;
}