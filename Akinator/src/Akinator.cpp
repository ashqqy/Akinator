#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Akinator.h"
#include "Tree.h"
#include "Common.h"
#include "Stack.h"
#include "CustomWarning.h"

//------------------------------------------------------

akinator_error_t AkinatorInit (akinator_t* akinator)
{
    CustomWarning (akinator != NULL, NULL_PTR);

    akinator->stack = NULL;
    akinator->root_node = NULL;
    akinator->buffer = NULL;
    akinator->buffer_shift = 0;
    akinator->buffer_size = BUFFER_SIZE;
    akinator->database_buffer = NULL;

    return AKINATOR_OK;
}

//------------------------------------------------------

/// @brief Создаёт дерево на основе базы данных
/// @param[in] database Файл с базой данных
/// @param[out] database_buffer Массив, в который прочитается база данных
/// @param[out] tree В структуре появится указатель на корень созданного дерева и на стек, использовавшийся для чтения базы
/// @return Ошибка при чтении базы
akinator_error_t ReadDataBase (FILE* database, akinator_t* akinator)
{
    CustomWarning (akinator                  != NULL, NULL_PTR);
    CustomWarning (database                  != NULL, NULL_PTR);

    size_t database_size = FileSizeFinder (database);
    CustomWarning (database_size != 0, EMPTY_DATABASE);

    akinator->database_buffer = (char*) calloc (database_size + 1, sizeof (char));
    CustomWarning (akinator->database_buffer != NULL, ALLOCATION_ERROR);

    size_t n_readen = fread (akinator->database_buffer, sizeof (char), database_size, database);
    CustomWarning (n_readen == database_size, ALLOCATION_ERROR);
    
    char* shift = akinator->database_buffer;

    stack_t* stack_database = (stack_t*) calloc (1, sizeof (stack_t));
    stack_error_t err = StackInit (stack_database);
    CustomWarning (err == STACK_OK, STACK_ERROR);
    akinator->stack = stack_database;

    while (shift < akinator->database_buffer + database_size)
    {   
        char next_step_buf = {};

        // читаем пропуская пробелы и все остальное (ищем '{' или '}')
        int n_skipped_spaces = 0;
        sscanf (shift, "%*[ \n\r]%n", &n_skipped_spaces);
        shift += n_skipped_spaces;
        next_step_buf = *shift;

        if (next_step_buf == '{')
        {   
            // сдвигаем сдвиг до открывающей "
            shift = strchr (shift, '"');
            CustomWarning (shift != NULL, INVALID_DATABASE_FORMAT);
            shift += 1;

            tree_node_t* node = NodeCreate (shift);
            err = StackPush (akinator->stack, node);
            CustomWarning (err == STACK_OK, STACK_ERROR);

            // сдвигаем сдвиг до закрывающей " и заменяем её на \0
            shift = strchr (shift, '"');
            CustomWarning (shift != NULL, INVALID_DATABASE_FORMAT);

            *shift = '\0';
            shift += 1;
        }

        else if (next_step_buf == '}')
        {
            if (akinator->stack->size > 1)
            {   
                err = STACK_OK;

                tree_node_t* closed_node = NULL;
                err = StackPop (akinator->stack, &closed_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                tree_node_t* parent_node = NULL;
                err = StackPop (akinator->stack, &parent_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                if (parent_node->right != NULL && parent_node->left != NULL)
                {
                    err = StackPush (akinator->stack, parent_node);
                    CustomWarning (err == STACK_OK, STACK_ERROR);
                    CustomWarning (!"Too many nodes for the parent", INVALID_DATABASE_FORMAT);
                }

                if      (parent_node->left == NULL)
                    NodeLink (closed_node, parent_node, &parent_node->left);

                else if (parent_node->right == NULL)
                    NodeLink (closed_node, parent_node, &parent_node->right);

                err = StackPush (akinator->stack, parent_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);
                
                shift += 1;
            }

            else
            {
                CustomWarning (akinator->stack->size == 1, INVALID_DATABASE_FORMAT);

                err = STACK_OK;

                tree_node_t* root = NULL;
                err = StackPop (akinator->stack, &root);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                akinator->root_node = root;
                break;
            }
        }
        else
            CustomWarning (!"Expected Opening or Closing Bracket", INVALID_DATABASE_FORMAT);

        if (shift >= akinator->database_buffer + database_size && akinator->stack->size != 0)
            CustomWarning (!"Expected Opening or Closing Bracket", INVALID_DATABASE_FORMAT);
    }
    return AKINATOR_OK;
}

//------------------------------------------------------

akinator_mode_t StartMenuMode ()
{
    while (1)
    {
        printf ("\nЧто ты хочешь?\n" \
            "  Угадать персонажа    - нажми 1\n"   \
            "  Определить персонажа - нажми 2\n"   \
            "  Сравнить персонажей  - нажми 3\n"   \
            "  Показать базу        - нажми 4\n"   \
            "  Выйти без сохранения - нажми 5\n"   \
            "  Выйти с сохранением  - нажми 6\n"   \
            "  ???                  - нажми  \n\n" \
            "Твой выбор: ");

        int answer = 0; 
        scanf ("%d", &answer); 
        CleanBuffer ();

        switch (answer)
        {
            case 1: return GUESSING_MODE;
            case 2: return DEFINITION_MODE;
            case 3: return COMPARATION_MODE;
            case 4: return DATABASESHOW_MODE;
            case 5: return EXITNOSAVING_MODE;
            case 6: return EXITSAVING_MODE;
            default: 
                printf ("\nТы выбрал секс мод......................\n" \
                        "Извини, пирожочек, давай в другой раз :(\n" \
                        "Выбери что-нибудь другое сегодня........\n\n");
        }
    }
}

//------------------------------------------------------

akinator_error_t GuessingMode (akinator_t* akinator)
{
    #define strcmpno_  strcmp (temp_buf, "Нет") == 0  || strcmp (temp_buf, "нет") == 0 || \
                       strcmp (temp_buf, "No")  == 0  || strcmp (temp_buf, "no")  == 0 || \
                       strcmp (temp_buf, "N")  == 0   || strcmp (temp_buf, "n")   == 0 || \
                       strcmp (temp_buf, "Н")  == 0   || strcmp (temp_buf, "н")   == 0
    #define strcmpyes_ strcmp (temp_buf, "Да")  == 0  || strcmp (temp_buf, "да")  == 0 || \
                       strcmp (temp_buf, "Yes") == 0  || strcmp (temp_buf, "yes") == 0 || \
                       strcmp (temp_buf, "Y")  == 0   || strcmp (temp_buf, "y")   == 0 || \
                       strcmp (temp_buf, "Д")  == 0   || strcmp (temp_buf, "д")   == 0

    tree_node_t* current_node = akinator->root_node;
    if (akinator->buffer == NULL)
        akinator->buffer = (char*) calloc (BUFFER_SIZE, sizeof(char));

    // игра в угадайку
    while (1)
    {
        char temp_buf[BUFFER_SIZE] = {};
        int n_readen = 0;

        // если сейчас не лист дерева
        if (current_node->left != NULL || current_node->right != NULL)   
        {
            printf ("Твой персонаж %s? Ответ: ", current_node->data);
            scanf ("%[^\n]%n", temp_buf, &n_readen); CleanBuffer ();
            CustomWarning ((size_t) n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

            if      (strcmpno_)
                current_node = current_node->right;
            else if (strcmpyes_)
                current_node = current_node->left;
            else 
                printf ("Введи `да' или `нет'\n");
        }

        // если сейчас лист дерева
        if (current_node->left == NULL && current_node->right == NULL)
        {
            printf ("Это %s? Ответ: ", current_node->data);
            scanf ("%[^\n]%n", temp_buf, &n_readen); CleanBuffer (); 
            CustomWarning ((size_t) n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

            if (strcmpno_)
            {
                printf ("А кто это? Ответ: ");
                scanf ("%[^\n]%n", temp_buf, &n_readen); CleanBuffer ();
                CustomWarning ((size_t) n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

                // если буфера не хватает, то увеличиваем его
                if ((size_t) n_readen >= akinator->buffer_size - (akinator->buffer_shift + 100))
                    akinator->buffer = BufferResize (akinator->buffer, &akinator->buffer_size, akinator->buffer_size * 2);

                // запоминаем ввод пользователя в буфер
                memcpy (akinator->buffer + akinator->buffer_shift, temp_buf, (size_t) n_readen + 1);  
                                
                // создаем слева узел с новым персонажем
                NodeLink (NodeCreate (akinator->buffer + akinator->buffer_shift), current_node, &current_node->left);
                // создаем справа узел со старым персонажем
                NodeLink (NodeCreate (current_node->data), current_node, &current_node->right);

                // увеличиваем сдвиг в буфере
                akinator->buffer_shift += (size_t) n_readen + 1;

                printf ("Чем `%s' отличается от `%s'?\n", temp_buf, current_node->data);
                scanf ("%[^\n]%n", temp_buf, &n_readen); CleanBuffer ();
                CustomWarning ((size_t) n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

                // если буфера не хватает, то увеличиваем его
                if ((size_t) n_readen >= akinator->buffer_size - (akinator->buffer_shift + 100))
                    akinator->buffer = BufferResize (akinator->buffer, &akinator->buffer_size, akinator->buffer_size * 2);

                // запоминаем ввод пользователя в буфер
                memcpy (akinator->buffer + akinator->buffer_shift, temp_buf, (size_t) n_readen + 1);  

                // Заменяем предыдущий лист на новый вопрос
                NodeEditData (current_node, akinator->buffer + akinator->buffer_shift);
                akinator->buffer_shift += (size_t) n_readen + 1;

                printf ("Персонаж добавлен!\n");
                break;
            }

            else if (strcmpyes_)
            {
                printf ("Угадал\n");
                break;
            }

            else 
            {
                printf ("Введи `да' или `нет'\n");
            }
        }
    }

    #undef strcmpno_
    #undef strcmpyes_

    return AKINATOR_OK;
}

//------------------------------------------------------

void DefinitionMode ()
{
}

//------------------------------------------------------

void ComparationMode ()
{
}

//------------------------------------------------------

void DatabaseShowMode ()
{
}


//------------------------------------------------------

akinator_error_t DatabaseWrite (tree_node_t* node)
{
    if (node == NULL)
        return AKINATOR_OK;
    
    printf ("{");
    printf ("\"");
    CustomWarning (node->data != NULL, NULL_PTR);
    printf ("%s", node->data);
    printf ("\"");

    DatabaseWrite (node->left);
    DatabaseWrite (node->right);

    printf ("}");
    
    return AKINATOR_OK;
}

//------------------------------------------------------

char* BufferResize (char* buffer, size_t* buffer_size, size_t new_buffer_size)
{
    CustomWarning (buffer != NULL, NULL);

    buffer = (char*) realloc (buffer, new_buffer_size);
    CustomWarning (buffer != NULL, NULL);

    *buffer_size = new_buffer_size;

    return buffer;
}

//------------------------------------------------------