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

    akinator->root_node = NULL;
    akinator->stack1 = NULL;
    akinator->stack2 = NULL;
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
    CustomWarning (akinator != NULL, NULL_PTR);
    CustomWarning (database != NULL, NULL_PTR);

    size_t database_size = FileSizeFinder (database);
    CustomWarning (database_size != 0, EMPTY_DATABASE);

    akinator->database_buffer = (char*) calloc (database_size + 1, sizeof (char));
    CustomWarning (akinator->database_buffer != NULL, ALLOCATION_ERROR);

    size_t n_readen = fread (akinator->database_buffer, sizeof (char), database_size, database);
    CustomWarning (n_readen == database_size, ALLOCATION_ERROR);
    
    char* shift = akinator->database_buffer;

    stack_t* stack_database = (stack_t*) calloc (1, sizeof (stack_t));
    CustomWarning (stack_database != NULL, ALLOCATION_ERROR);
    stack_error_t err = StackInit (stack_database);
    CustomWarning (err == STACK_OK, STACK_ERROR);
    akinator->stack1 = stack_database;

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
            err = StackPush (akinator->stack1, node);
            CustomWarning (err == STACK_OK, STACK_ERROR);

            // сдвигаем сдвиг до закрывающей " и заменяем её на \0
            shift = strchr (shift, '"');
            CustomWarning (shift != NULL, INVALID_DATABASE_FORMAT);

            *shift = '\0';
            shift += 1;
        }

        else if (next_step_buf == '}')
        {
            if (akinator->stack1->size > 1)
            {   
                err = STACK_OK;

                tree_node_t* closed_node = NULL;
                err = StackPop (akinator->stack1, &closed_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                tree_node_t* parent_node = NULL;
                err = StackPop (akinator->stack1, &parent_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                if (parent_node->right != NULL && parent_node->left != NULL)
                {
                    err = StackPush (akinator->stack1, parent_node);
                    CustomWarning (err == STACK_OK, STACK_ERROR);
                    CustomWarning (!"Too many nodes for the parent", INVALID_DATABASE_FORMAT);
                }

                if      (parent_node->left == NULL)
                    NodeLink (closed_node, parent_node, &parent_node->left);

                else if (parent_node->right == NULL)
                    NodeLink (closed_node, parent_node, &parent_node->right);

                err = StackPush (akinator->stack1, parent_node);
                CustomWarning (err == STACK_OK, STACK_ERROR);
                
                shift += 1;
            }

            else
            {
                CustomWarning (akinator->stack1->size == 1, INVALID_DATABASE_FORMAT);

                err = STACK_OK;

                tree_node_t* root = NULL;
                err = StackPop (akinator->stack1, &root);
                CustomWarning (err == STACK_OK, STACK_ERROR);

                akinator->root_node = root;
                break;
            }
        }
        else
            CustomWarning (!"Expected Opening or Closing Bracket", INVALID_DATABASE_FORMAT);

        if (shift >= akinator->database_buffer + database_size && akinator->stack1->size != 0)
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
            "  Загрузить базу       - нажми 5\n"   \
            "  Выйти без сохранения - нажми 6\n"   \
            "  Выйти с сохранением  - нажми 7\n"   \
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
            case 5: return DATABASEUPLOAD_MODE;
            case 6: return EXITNOSAVING_MODE;
            case 7: return EXITSAVING_MODE;
            default: 
                printf ("\nТы выбрал секс мод......................\n" \
                        "Извини, пирожочек, давай в другой раз :(\n" \
                        "Выбери что-нибудь другое сегодня........\n\n");
        }
    }
}

//------------------------------------------------------

static int strcmpno (char* buf)
{
    return (strcmp (buf, "Нет") && strcmp (buf, "нет") && 
            strcmp (buf, "No")  && strcmp (buf, "no")  && 
            strcmp (buf, "N")   && strcmp (buf, "n")   && 
            strcmp (buf, "Н")   && strcmp (buf, "н")   );
}

static int strcmpyes (char* buf)
{
    return (strcmp (buf, "Да")  && strcmp (buf, "да")  && 
            strcmp (buf, "Yes") && strcmp (buf, "yes") && 
            strcmp (buf, "Y")   && strcmp (buf, "y")   && 
            strcmp (buf, "Д")   && strcmp (buf, "д")   );
}

akinator_error_t GuessingAddNewCharacter (akinator_t* akinator, char* temp_buf, int* n_readen, tree_node_t* current_node)
{
    printf ("А кто это? Ответ: ");
    scanf ("%[^\n]%n", temp_buf, n_readen); CleanBuffer ();
    CustomWarning ((size_t) *n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

    // если буфера не хватает, то увеличиваем его
    if ((size_t) *n_readen >= akinator->buffer_size - (akinator->buffer_shift + 100))
    {
        akinator->buffer = BufferResize (akinator->buffer, &akinator->buffer_size, akinator->buffer_size * 2);
        CustomWarning (akinator->buffer != NULL, ALLOCATION_ERROR);
    }

    // запоминаем ввод пользователя в буфер
    memcpy (akinator->buffer + akinator->buffer_shift, temp_buf, (size_t) *n_readen + 1);  
                                
    // создаем слева узел с новым персонажем
    NodeLink (NodeCreate (akinator->buffer + akinator->buffer_shift), current_node, &current_node->left);
    // создаем справа узел со старым персонажем
    NodeLink (NodeCreate (current_node->data), current_node, &current_node->right);

    // увеличиваем сдвиг в буфере
    akinator->buffer_shift += (size_t) *n_readen + 1;

    printf ("Чем `%s' отличается от `%s'?\n", temp_buf, current_node->data);
    scanf ("%[^\n]%n", temp_buf, n_readen); CleanBuffer ();
    CustomWarning ((size_t) *n_readen < BUFFER_SIZE, BUFFER_OVERFLOW);

    // если буфера не хватает, то увеличиваем его
    if ((size_t) *n_readen >= akinator->buffer_size - (akinator->buffer_shift + 100))
        akinator->buffer = BufferResize (akinator->buffer, &akinator->buffer_size, akinator->buffer_size * 2);

    // запоминаем ввод пользователя в буфер
    memcpy (akinator->buffer + akinator->buffer_shift, temp_buf, (size_t) *n_readen + 1);  

    // Заменяем предыдущий лист на новый вопрос
    NodeEditData (current_node, akinator->buffer + akinator->buffer_shift);
    akinator->buffer_shift += (size_t) *n_readen + 1;

    printf ("Персонаж добавлен!\n");

    return AKINATOR_OK;
}

akinator_error_t GuessingMode (akinator_t* akinator)
{
    CustomWarning (akinator != NULL, NULL_PTR);

    tree_node_t* current_node = akinator->root_node;
    if (akinator->buffer == NULL)
    {
        akinator->buffer = (char*) calloc (BUFFER_SIZE, sizeof(char));
        CustomWarning (akinator->buffer != NULL, ALLOCATION_ERROR);
    }

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

            if      (strcmpno (temp_buf) == 0)
                current_node = current_node->right;
            else if (strcmpyes (temp_buf) == 0)
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

            if (strcmpno (temp_buf) == 0)
            {
                akinator_error_t err = GuessingAddNewCharacter (akinator, temp_buf, &n_readen, current_node);
                if (err != AKINATOR_OK)
                    return err;
                break;
            }

            else if (strcmpyes (temp_buf) == 0)
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

    return AKINATOR_OK;
}

//------------------------------------------------------

void AkinatorNodeSearch (akinator_t* akinator, char* temp_buf, tree_node_t** founded_node, const char* phrase)
{
    while (1)
    {
        printf ("%s", phrase);
        scanf ("%[^\n]", temp_buf); CleanBuffer ();

        *founded_node = NodeSearch (akinator->root_node, temp_buf);

        if (*temp_buf == '0')
            return;

        else if (*founded_node == NULL)
            printf ("Я не нашел персонажа с таким именем. Попробуй снова.\n"
                    "(Или введи '0', чтобы вернуться назад)\n");
        else
            break;
    }
}

//------------------------------------------------------

akinator_error_t AkinatorNodePathFill (stack_t* stack, tree_node_t* node)
{
    CustomWarning (stack != NULL, NULL_PTR);
    CustomWarning (node  != NULL, NULL_PTR);
    tree_node_t* temp_node = node;

    while (temp_node != NULL)
    {
        CustomWarning (StackPush (stack, temp_node) == STACK_OK, STACK_ERROR);
        temp_node = temp_node->parent;
    }

    return AKINATOR_OK;
}

//------------------------------------------------------

akinator_error_t AkinatorDefinitionSmall (stack_t* stack)
{
    tree_node_t* current_node = NULL;

    while (stack->size != 0)
    {
        CustomWarning (StackPop (stack, &current_node) == STACK_OK, STACK_ERROR);

        if (current_node->parent != NULL)
        {
            if (current_node == current_node->parent->left)
            {
                if (stack->size != 0)
                    printf ("%s, ", current_node->parent->data);
                else
                    printf ("%s", current_node->parent->data);
            }
            if (current_node == current_node->parent->right)
            {
                if (stack->size != 0)
                    printf ("не %s, ", current_node->parent->data);
                else
                    printf ("не %s", current_node->parent->data);
            } 
        }
    }

    printf ("\n");

    return AKINATOR_OK;
}

//------------------------------------------------------

akinator_error_t DescriptionMode (akinator_t* akinator)
{
    CustomWarning (akinator != NULL, NULL_PTR);

    akinator->stack1 = (stack_t*) calloc (1, sizeof(stack_t));
    StackInit (akinator->stack1);
    char temp_buf[BUFFER_SIZE] = {};
    tree_node_t* founded_node = NULL;

    // находим запрошенного персонажа
    AkinatorNodeSearch (akinator, temp_buf, &founded_node, "Введи имя персонажа. Ответ: ");
    if (founded_node == NULL)
        return AKINATOR_OK;

    // заполняем стек путём до найденного узла
    CustomWarning (AkinatorNodePathFill (akinator->stack1, founded_node) == AKINATOR_OK, STACK_ERROR);

    printf ("`%s': ", founded_node->data);

    // печатаем описание
    CustomWarning (AkinatorDefinitionSmall (akinator->stack1) == AKINATOR_OK, STACK_ERROR);

    return AKINATOR_OK;
}

//------------------------------------------------------

akinator_error_t ComparationMode (akinator_t* akinator)
{
    CustomWarning (akinator != NULL, NULL_PTR);

    akinator->stack1 = (stack_t*) calloc (1, sizeof(stack_t));
    StackInit (akinator->stack1);
    akinator->stack2 = (stack_t*) calloc (1, sizeof(stack_t));
    StackInit (akinator->stack2);

    char temp_buf[BUFFER_SIZE] = {};
    tree_node_t* founded_node1 = NULL;
    tree_node_t* founded_node2 = NULL;

    // находим первого персонажа
    AkinatorNodeSearch (akinator, temp_buf, &founded_node1, "Введи имя первого персонажа. Ответ: ");
    if (founded_node1 == NULL)
        return AKINATOR_OK;
    // находим второго персонажа
    AkinatorNodeSearch (akinator, temp_buf, &founded_node2, "Введи имя второго персонажа. Ответ: ");
    if (founded_node2 == NULL)
        return AKINATOR_OK;

    // заполняем стеки путями до найденных узлов
    CustomWarning (AkinatorNodePathFill (akinator->stack1, founded_node1) == AKINATOR_OK, STACK_ERROR);
    CustomWarning (AkinatorNodePathFill (akinator->stack2, founded_node2) == AKINATOR_OK, STACK_ERROR);

    printf ("\nОбщие     признаки `%s' и `%s': ", founded_node1->data, founded_node2->data);

    tree_node_t* current_node1 = NULL;
    tree_node_t* current_node2 = NULL;
    CustomWarning (StackPop (akinator->stack1, &current_node1) == STACK_OK, STACK_ERROR);
    CustomWarning (StackPop (akinator->stack2, &current_node2) == STACK_OK, STACK_ERROR);

    while (current_node1 == current_node2)
    {
        CustomWarning (StackPop (akinator->stack1, &current_node1) == STACK_OK, STACK_ERROR);
        CustomWarning (StackPop (akinator->stack2, &current_node2) == STACK_OK, STACK_ERROR);

        if (current_node1->parent != NULL && current_node2->parent != NULL)
        {
            if (current_node1->parent == current_node2->parent && current_node1 == current_node2)
            {
                if (current_node1 == current_node1->parent->left)
                {
                    printf ("%s, ", current_node1->parent->data);
                }
                if (current_node1 == current_node1->parent->right)
                {
                    printf ("не %s, ", current_node1->parent->data);
                }
            }
        }
    }
    // стираем последнюю запятую
    printf ("\b\b  \n");

    CustomWarning (StackPush (akinator->stack1, current_node1) == STACK_OK, STACK_ERROR);
    CustomWarning (StackPush (akinator->stack2, current_node2) == STACK_OK, STACK_ERROR);

    printf ("Различные признаки `%s' и `%s':\n", founded_node1->data, founded_node2->data);
    printf ("`%s': ", founded_node1->data);
    CustomWarning (AkinatorDefinitionSmall (akinator->stack1) == AKINATOR_OK, STACK_ERROR);
    printf ("`%s': ", founded_node2->data);
    CustomWarning (AkinatorDefinitionSmall (akinator->stack2) == AKINATOR_OK, STACK_ERROR);

    return AKINATOR_OK;
}

//------------------------------------------------------

akinator_error_t DatabaseWrite (tree_node_t* node, FILE* database_out)
{
    if (node == NULL)
        return AKINATOR_OK;
    
    fprintf (database_out, "{");
    fprintf (database_out, "\"");
    CustomWarning (node->data != NULL, NULL_PTR);
    fprintf (database_out, "%s", node->data);
    fprintf (database_out, "\"");

    DatabaseWrite (node->left, database_out);
    DatabaseWrite (node->right, database_out);

    fprintf (database_out, "}");

    return AKINATOR_OK;
}

//------------------------------------------------------

char* BufferResize (char* buffer, size_t* buffer_size, size_t new_buffer_size)
{
    CustomWarning (buffer != NULL, NULL);

    buffer = (char*) realloc (buffer, new_buffer_size); // TODO use MyRecalloc
    CustomWarning (buffer != NULL, NULL);

    *buffer_size = new_buffer_size;

    return buffer;
}

//------------------------------------------------------