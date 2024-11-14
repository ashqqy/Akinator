#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Akinator.h"
#include "Common.h"

//------------------------------------------------------

tree_error_t TreeInit (tree_t* tree)
{
    tree->root_node = NULL;
    tree->n_nodes = 0;

    return CHIKI_PUKI;
}

//------------------------------------------------------

tree_error_t CreateAndLinkNode (tree_t* tree, tree_elem_t data, tree_node_t** node_to_link_to)
{
    tree_node_t* node = CreateNode (tree, data);
    if (node == NULL)
        return ALLOCATION_ERROR;
    
    return LinkNode (node, node_to_link_to);
}

//------------------------------------------------------

tree_node_t* CreateNode (tree_t* tree, tree_elem_t data)
{
    assert (tree != NULL);

    tree_node_t* node = (tree_node_t*) calloc (1, sizeof (tree_node_t));
    if (node == NULL)
        return NULL;

    node->data  = data;
    node->left  = NULL;
    node->right = NULL;

    return node;
}
//------------------------------------------------------

tree_error_t LinkNode (tree_node_t* node, tree_node_t** node_to_link_to)
{
    assert (node            != NULL);
    assert (node_to_link_to != NULL);

    *node_to_link_to = node;

    return CHIKI_PUKI;
}

//------------------------------------------------------

tree_error_t EditNodeData (tree_node_t* node, tree_elem_t new_data)
{
    assert (node != NULL);

    node->data = new_data;

    return CHIKI_PUKI;
}

//------------------------------------------------------

MODE StartMenuMode ()
{
    printf ("\nЧто ты хочешь?\n" \
            "   Угадать персонажа    - нажми 1\n" \
            "   Определить персонажа - нажми 2\n" \
            "   Сравнить персонажей  - нажми 3\n" \
            "   Показать базу        - нажми 4\n" \
            "   Выйти без сохранения - нажми 5\n" \
            "   Выйти с сохранением  - нажми 6\n" \
            "   Секса                - нажми  \n");

    while (1)
    {
        char answer = {}; 
        scanf ("%c", &answer);
        CleanBufer ();
        if (answer == '1')
            return GUESSING_MODE;
        if (answer == '2')
            return DEFINITION_MODE;
        if (answer == '3')
            return COMPARATION_MODE;
        if (answer == '4')
            return DATABASESHOW_MODE;
        if (answer == '5')
            return EXITNOSAVING_MODE;
        if (answer == '6')
            return EXITSAVING_MODE;
        else
            printf ("Ты выбрал секс мод......................\n" \
                    "Извини, пирожочек, давай в другой раз :(\n" \
                    "Выбери что-нибудь другое сегодня........\n\n");
    }
}

//------------------------------------------------------

void GuessingMode (tree_t* tree)
{
    tree_node_t* current_node = tree->root_node;

    // игра в угадайку
    while (1)
    {
        char user_ans_buf[1024] = {};

        // если сейчас лист дерева
        if (current_node->left == NULL && current_node->right == NULL)
        {
            printf ("Вы загадали %s?\n", current_node->data);
            scanf ("%[^\n]", user_ans_buf);
            CleanBufer ();

            if (strcmp (user_ans_buf, "Нет") == 0 || strcmp (user_ans_buf, "нет") == 0)
            {
                printf ("Кого вы загадали?\n");
                scanf ("%[^\n]", user_ans_buf);
                CleanBufer ();
                
                // создаем слева узел с новым персонажем
                CreateAndLinkNode (tree, user_ans_buf,       &current_node->left);
                // создаем справа узел со старым персонажем
                CreateAndLinkNode (tree, current_node->data, &current_node->right);

                printf ("Чем %s отличается от %s?\n", user_ans_buf, current_node->data);
                scanf ("%[^\n]", user_ans_buf);
                CleanBufer ();

                // Заменяем предыдущий лист на новый вопрос
                EditNodeData (current_node, user_ans_buf);

                printf ("Ваш персонаж добавлен!\n");
                break;
            }
            else if (strcmp (user_ans_buf, "Да") == 0 || strcmp (user_ans_buf, "да") == 0)
            {
                printf ("Сосать угадал\n");
                break;
            }
            else printf ("Не очепятывайся, последняя попытка тебе исправиться\n");
        }
        // если сейчас не лист дерева
        else    
        {
            printf ("Ваш персонаж обладает таким признаком: %s?\n", current_node->data);
            scanf ("%[^\n]", user_ans_buf);
            CleanBufer ();
            if (strcmp(user_ans_buf, "Нет") == 0 || strcmp (user_ans_buf, "нет") == 0)
                current_node = current_node->right;
            else if (strcmp (user_ans_buf, "Да") == 0 || strcmp (user_ans_buf, "да") == 0)
                current_node = current_node->left;
            else printf ("Не очепятывайся, убью\n");
        }
    }
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

void ExitNoSavingMode ()
{
}

//------------------------------------------------------

void ExitSavingMode ()
{
}

//------------------------------------------------------

void TreeDump (tree_t* tree)
{
    assert (tree != NULL);

    FILE* dump_file = fopen ("./dump/dump.dot", "w");

    fprintf (dump_file, "digraph G\n");
    fprintf (dump_file, "{\n");
    fprintf (dump_file, "node[shape=\"record\", style=\"rounded, filled\"];\n\n");

    // определяем узлы 
    fprintf (dump_file, "p0x%p[label = \"{ <p>ptr = 0x%p | <d> %s| { <l>left|<r>right } }\"];\n", 
                                        tree->root_node, tree->root_node, tree->root_node->data);
    TreeNodeDescrDump (dump_file, tree->root_node->left);
    TreeNodeDescrDump (dump_file, tree->root_node->right);
    fprintf (dump_file, "\n");

    // соединяем узлы
    TreeNodeLinkDump (dump_file, tree->root_node);

    fprintf (dump_file, "}\n");

    fclose (dump_file);

    const char command[81] = "\"C:/Program Files/Graphviz/bin/dot.exe\" ./dump/dump.dot -Tpng -o ./dump/dump.png";
    system(command);
}

void TreeNodeDescrDump (FILE* dump_file, tree_node_t* node)
{
    assert (dump_file != NULL);

    if (node == NULL)
        return;

    fprintf (dump_file, "p0x%p[label = \"{ <p>ptr = 0x%p| <d> = %s| { <l>left|<r>right } }\"];\n", 
                                                                          node, node, node->data);
    
    if (node->left  != NULL) TreeNodeDescrDump (dump_file, node->left);
    if (node->right != NULL) TreeNodeDescrDump (dump_file, node->right);
}

void TreeNodeLinkDump (FILE* dump_file, tree_node_t* node)
{   
    assert (dump_file != NULL);

    if (node->left  != NULL) 
    {
        fprintf (dump_file, "p0x%p:<l> -> p0x%p\n", node, node->left);
        TreeNodeLinkDump (dump_file, node->left);
    }
    if (node->right != NULL) 
    {
        fprintf (dump_file, "p0x%p:<r> -> p0x%p\n", node, node->right);
        TreeNodeLinkDump (dump_file, node->right);
    }
}

//------------------------------------------------------
