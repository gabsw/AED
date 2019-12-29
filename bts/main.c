
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
//
// Custom ordered binary tree implementation
//

typedef struct file_data //estrutura auxiliar para ler ficheiro
{
    // public data
    long word_pos; // zero-based (posição da palavra no texto)
    long word_num; // zero-based  (numero da palavra ou palavras ja inseridas)
    char word[64];
    // private data
    FILE *fp;
    long current_pos; // zero-based
} file_data_t;

typedef struct node /* no da arvore */
{
    char *word;         // ponteiro para a palavra
    long count;         // numero de vezes que a palavra já apareceu
    long first;         // primeira ocorrencia
    long last;          // ultima occrrencia
    long smallest;      // menor distancia entre 2 apariçoes consecutivas
    long largest;       // maior distancia
    struct node *left;  // ponterio para o no da esquerda
    struct node *right; // ponteriro para o no da direita
} node;

typedef struct hashtable
{
    int size;     // tamanho da hashtable
    node **table; // hash table
    int cont;     // numero de elementos na hash table (ou tamanho atual)
    long WordsCount;
    long DifferentWords;
} hashtable;

static void initHashTable(hashtable *h, int size) // inicializar hash table
{

    h->size = size;
    h->cont = 0;
    node **table = calloc(size, sizeof(node));
    h->table = table;
    h->WordsCount=0;
    h->DifferentWords=0;
}

// hash function
static unsigned int hash_function(const char *str, unsigned int s) // hash function
{
    static unsigned int table[256];
    unsigned int crc, i, j;
    if (table[1] == 0u)
    {
        for (i = 0u; i < 256u; i++)
            for (table[i] = i, j = 0u; j < 8u; j++)
                if (table[i] & 1u)
                {
                    table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
                }
                else
                {
                    table[i] >>= 1;
                }
    }
    crc = 0xAED02019u; // initial value (chosen arbitrarily)
    while (*str != '\0')
        crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
    return crc % s;
}

static node *new_node(file_data_t *f) // criar um novo no
{

    node *n = (node *)malloc(sizeof(node));
    n->word = strdup(f->word); // duplica a string e faz implicitamente o malloc
    n->count = 1;
    n->left = NULL;
    n->right = NULL;
    n->first = n->last = f->word_pos;
    n->largest = (n->last) - (n->first);
    n->smallest = (n->last) - (n->first);

    return n;
}

int open_text_file(char *file_name, file_data_t *fd) // abrir o ficheiro
{
    fd->fp = fopen(file_name, "rb");
    if (fd->fp == NULL)
        return -1;
    fd->word_pos = -1;
    fd->word_num =0;
    fd->word[0] = '\0';
    fd->current_pos = 0;
    return 0;
}
void close_text_file(file_data_t *fd) // fechar
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(file_data_t *fd) // ler ficheiro
{
    int i, c;
    // skip white spaces
    do
    {
        c = fgetc(fd->fp);

        if (c == EOF)
            return -1;

    } while (c <= 32);
    // record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = (char)c;
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++)
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            break; // end of file
        fd->current_pos++;
        if (c <= 32)
            break; // terminate word
        fd->word[i] = (char)c;
    }
    fd->word[i] = '\0';
    // printf("%s\n", fd->word);
    return 0;
}

static node *new_node_resized(node *root) // criar no para a nova hashtable
{

    node *n = (node *)malloc(sizeof(node));
    n->word = root->word; // duplica a string e faz implicitamente o malloc
    n->count = root->count;
    n->first = root->first;
    n->last = root->last;
    n->largest = root->largest;
    n->smallest = root->smallest;
    n->left = NULL;
    n->right = NULL;
    free(root);

    return n;
}

static void insert_resized(hashtable *ht, node *root, int index)
{

    if (ht->table[index] == NULL)
    { // se o nó ainda não existir ,é criado e a função termina

        ht->table[index] = new_node_resized(root);
        ht->cont++;

        return;
    }

    node *n = ht->table[index];

    while (1)
    {

        int c = strcmp(root->word, n->word);

        if (c < 0)
        {
            if (n->left == NULL)
            {

                n->left = new_node_resized(root);
                return;
            }
            else
                n = n->left;
        }
        else if (c == 0)
        {
        }
        else
        {
            if (n->right == NULL)
            {
                n->right = new_node_resized(root);
                return;
            }
            else
                n = n->right;
        }
        // }
    }
}

static void insert(hashtable *h, file_data_t *f, int pos) // inserir palavra
{
    // printf("pos:%d\n",pos);
    if (h->table[pos] == NULL)
    { // se o nó ainda não existir ,é criado e a função termina
        h->table[pos] = new_node(f);
        h->cont++;
        // printf("#####%d\n",h->cont);
        h->DifferentWords++;
        h->WordsCount++;
        return;
    }

    node *n = h->table[pos];
    while (1)
    {
        int c = strcmp(f->word, n->word); // compara a palavra a inserir com as ja existentes na arvore
        if (c < 0)
        {
            if (n->left == NULL)
            {
                n->left = new_node(f);
                h->WordsCount++;
                h->DifferentWords++;

                return;
            }
            else
                n = n->left;
        }
        else if (c == 0)
        {

            n->count++;
            if (n->count == 2)
            {
                n->smallest = f->word_pos - n->last;
            }
            if (f->word_pos - n->last > n->largest)
                n->largest = f->word_pos - n->last;
            if (f->word_pos - n->last < n->smallest)
                n->smallest = f->word_pos - n->last;

            n->last = f->word_pos;
            h->WordsCount++;
            return;
        }
        else
        {
            if (n->right == NULL)
            {
                n->right = new_node(f);
                h->WordsCount++;
                h->DifferentWords++;

                return;
            }
            else
                n = n->right;
        }
    }
}

static void free_tree(node *n) // liberta a memoria alocada da arvore
{
    if (n != NULL)
    {
        free_tree(n->left);
        free_tree(n->right);
        free(n->word);
        free(n);
    }
}

//
// Ordered binary tree query (recursive) functions
//
static void remove_non_alpha(file_data_t *f) // "limpa" a palavra
{
    int i, j;
    i = j = 0;

    while (f->word[i] != '\0')
    {
        if (isalpha(f->word[i]) != 0 || ((f->word[i] == '-' || f->word[i] == 0x27) && isalpha(f->word[i - 1]) != 0 && isalpha(f->word[i + 1]) != 0))
        {

            f->word[j++] = tolower(f->word[i]);
        }
        i++;
    }
    f->word[j] = '\0';
}

// static long count_all_words(node *n) // contar todas as palavras da hashtable
// {

//     return (n == NULL) ? 0 : count_all_words(n->left) + n->count + count_all_words(n->right);
// }

// static long count_different_words(node *n) // conta o numero de palavras diferentes
// {
//     return (n == NULL) ? 0 : count_different_words(n->left) + 1 + count_different_words(n->right);
// }

static void list_words(node *n) // imprime a lista de todas as palavras
{

    if (n != NULL)
    { // verificar se o ponteiro é nulo
    
        list_words(n->left);
        printf("%6ld %s\n", n->count, n->word);
        list_words(n->right);
        
    }
}

static void depth(node *root, hashtable *ht, int size)
{

    if (root != NULL)
    {
        int index;
        depth(root->left, ht, size);
        depth(root->right, ht, size);
        index = hash_function(root->word, size);
        insert_resized(ht, root, index);
    }
    return;
}

static int searchWord(node *n, node *root, char *word) // da informaçõa da palavra introduzida pelo utilizador
{

    if (root != NULL)
    {

        int c = strcmp(word, root->word);
        if (c < 0)
        {
            if (root->left == NULL)
            {
                return 1;
            }
            else
                return searchWord(n, root->left, word);
        }
        else if (c == 0)
        {
            n->first = root->first;
            n->last = root->last;
            n->largest = root->largest;
            n->smallest = root->smallest;
            n->word = word;
            n->count = root->count;
            return 0;
        }
        else
        {
            if (root->right == NULL)
            {

                return 1;
            }
            else
                return searchWord(n, root->right, word);
        }
    }
    return 1;
}

static void free_memory(hashtable *ht) // liberta a memoria d hash table
{

    for (int i = 0; i < ht->size; i++)
    {
        free_tree(ht->table[i]);
    }

    free(ht->table);
}

static void resize(hashtable *ht) // para fazer a hashtable dinamica
{

    printf("Resize........\n");

    node **temp = ht->table; // pontiro para a hash table atual
    int s=ht->size;
    ht->size = (int)(1.5 * ht->size);
    ht->cont=0;
    
    node **newtable = calloc((int)(1.5 * ht->size), sizeof(node));
    ht->table = newtable;
    

    for (int i = 0; i < s ; i++) // percorrer cada index não nulo da hash table
    {
        if ((temp[i]) != NULL)
        {
            depth((temp[i]), ht, ht->size); // faz um depth first na arvore
        }
    }
    
    free(temp);
}

//
// Main program
//

int main(int argc, char **argv)
{
    // command line options
    int opt = -1;
    if (argc >= 3 && strcmp(argv[1], "-a") == 0)
        opt = 'a'; // all words
    if (argc >= 3 && strcmp(argv[1], "-d") == 0)
        opt = 'd'; // different words
    if (argc >= 3 && strcmp(argv[1], "-l") == 0)
        opt = 'l'; // list words
    if (argc >= 3 && strcmp(argv[1], "-w") == 0)
        opt = 'w';
    if (opt < 0)
    {
        fprintf(stderr, "\e[5;31m"); // blink on (may not work in some text terminals), text in red
        fprintf(stderr, "usage: %s -a text_file ...  # count the number of words\n", argv[0]);
        fprintf(stderr, "       %s -d text_file ...  # count the number of different words\n", argv[0]);
        fprintf(stderr, "       %s -l text_file ...  # list all words\n", argv[0]);
        fprintf(stderr, "\e[0m"); // normal output
        return 1;
    }

    file_data_t f;
    hashtable h;
    int pos;
    char w[64];
 

    for (int i = 2; i < argc; i++) //for(int i = 2;i <= argc;i++) tirar o igual pois o ultimo existe mas é um ponteiro para nulo
    {                              // se passar algo nulo ele vai queixar-se
        // read text file

        open_text_file(argv[i], &f);
        initHashTable(&h, 1000);
        
        while (read_word(&f) != -1)
        {
         
           

            if (h.cont >= (int)(0.8 * h.size)) // se o tamanho atual for 80% do tamnho max faz o resize
                resize(&h);

            remove_non_alpha(&f);
            pos = hash_function(f.word, h.size);
        
            insert(&h, &f, pos);
        }

        close_text_file(&f);
    }

    //     // report
    switch (opt)
    {
    case 'a':


        printf("The file contains %ld words\n",h.WordsCount);

        break;
    case 'd':

        printf("The file contains %ld distinct words\n",h.DifferentWords);
        break;
    case 'l':
        // printf("Contents of the file %s:\n");

        for (int i = 0; i < h.size; i++)
        {
            if (h.table[i] != NULL)
                list_words(h.table[i]);
        }

        break;
    case 'w':

        printf("Insira uma palavra:\n");
        scanf("%s", w);
        node n;
        int index;
        index=hash_function(w,h.size);
       if( searchWord(&n, h.table[index], w)==1)
        {
            printf("Palavra não encontrada!\n");
        }
        else
        {
            printf("cont:%ld \t fist:%ld \t last:%ld\n", n.count, n.first, n.last);
            printf("dist. min:%ld \t dis. max:%ld\n", n.smallest, n.largest);
            printf("average:%ld\n", (n.last - n.first) / (n.count - 1));
        }

        break;
    }
    //     // free memory

    free_memory(&h);

    return 0;
}