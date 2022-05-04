#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#define MAX_NAME_LEN            30
#define MAX_EXTEN_LEN           10
#define CREATION_DATE_LEN       25
#define DEGREE                  30
#define PATH_SEP                '/'
#define BUFF_SIZE               200


typedef struct node{
    int num_keys;
    struct key *keys[DEGREE - 1];
    struct node *children[DEGREE];
    struct node *parent;
    bool is_leaf;

    // ##############METADATE##############
    char creation_date[CREATION_DATE_LEN + 1];
    char name[MAX_NAME_LEN + 1];
    
}node;

typedef struct key{
    char name[MAX_NAME_LEN + 1];
    char extension[MAX_EXTEN_LEN + 1];
    char creation_date[CREATION_DATE_LEN + 1];
    node* directory;
}key;

typedef struct b_tree{
  node *root;
  unsigned long long num_of_elem;  
  short degree;
}b_tree;

node *root = NULL; // Корень дерева
node *curr_node = NULL;

char* get_curr_date();
void init_tree();
bool push_node(node*, char*);
bool push_key(node*, char*, char*);
bool delete_elem(node*, char*, char*, bool);
bool delete_key(node*, int);
bool delete_node(node*);
void print_shell();
void print_elems();
bool read_command();

int main(){
    /**
     * @brief Создана корневая папка системы(глобальная переменная root), текущий узел теперь корень
     * 
     */
    init_tree();
    while(1){
        print_shell();
        read_command();
        
    }


    return 0;
}


//---------------ФУНКЦИИ ДЕРЕВА---------------

void init_tree(){
    root = (node*)malloc(sizeof(node));
    /**
     * @brief Проверка выделилась ли память на дерево
     * 
     */
    if(root){
        root->is_leaf = 0;
        root->num_keys = 0;
        root->parent = NULL;
        for(int i = 0;i < DEGREE; i++){
		    root->children[i] = NULL;
	    }
        strcpy(root->name, "/");
        strcpy(root->creation_date, get_curr_date());
        curr_node = root;
        
    }
    else{
        printf("Out of memory!\n");
        exit(EXIT_FAILURE);
    }
}

bool push_node(node* dir, char* name){
    int i = 0;
    bool flag = 0;
    //###############ПРОВЕРКА НА СУЩЕСТВОВАНИЯ КАТАЛОГА С ТАКИМ ИМЕНЕМ###############
    for(i = 0; i < DEGREE; i++){
        if(dir->children[i] == NULL){
            break;
        }
        if(!strcmp(dir->children[i]->name, name)){
            printf("mkdir: невозможно создать каталог \"%s\": Файл уже существует\n", name);
            return 0;
        }
    }
    //###############СОЗДАНИЕ НОВОГО УЗЛА###############
    node* new_node;
    new_node = (node*)malloc(sizeof(node));
    if(!new_node){
        printf("Out of memmory!\n");
        exit(EXIT_FAILURE);
    }
    new_node->is_leaf = 1;
    new_node->parent = dir;
    new_node->num_keys = 0;
    // strcpy(new_node->children[0]->name, "..");
    // new_node->children[0] = dir;
    for(i = 1;i < DEGREE; i++){
		new_node->children[i] = NULL;
	}
    for(i = 0;i < DEGREE - 1; i++){
		new_node->keys[i] = NULL;
	}
    strcpy(new_node->name, name);
    strcpy(new_node->creation_date, get_curr_date());
    //###############ИЗМЕНЕНИЕ ТЕКУЩЕГО КАТАЛОГА###############
    for(i = 0; i < DEGREE;i++){
        if(dir->children[i] == NULL){
            dir->children[i] = new_node;
            flag = 1;
            break;
        }
    }
    if(!flag) return 1;
    dir->is_leaf = 0;
    return 0;
}

bool push_key(node* dir, char* name, char* extension){
    int i = 0;
    bool flag = 0;
    //###############СОЗДАНИЕ НОВОГО КЛЮЧА###############
    key* new_key;
    new_key = (key*)malloc(sizeof(key));
    if(!new_key){
        printf("Out of memmory!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_key->creation_date, get_curr_date());
    strcpy(new_key->name, name);
    strcpy(new_key->extension, extension);
    //###############ИЗМЕНЕНИЕ ТЕКУЩЕГО КАТАЛОГА###############
    for(i = 0; i < DEGREE - 1;i++){
        if(dir->keys[i] == NULL || (!strcmp(dir->keys[i]->name, name) && !strcmp(dir->keys[i]->extension, extension))){
            dir->keys[i] = new_key;
            flag = 1;
            break;
        }
    }
    if(!flag) return 1;
    return 0;
}

void print_elems(){
    int i = 0;
    while(curr_node->keys[i] != NULL){
        printf("%s%s ", curr_node->keys[i]->name, curr_node->keys[i]->extension);
        i++;
    }
    i = 0;
    while(curr_node->children[i] != NULL){
        printf("%s/ ", curr_node->children[i]->name);
        i++;
    }
    puts(" ");
}

bool delete_elem(node* dir, char* name, char* extension,bool is_req){
    int i = 0;
    bool flag = 0;
    for(i = 0; i < DEGREE - 1; i++){
        if(dir->keys[i] == NULL){
            break;
        }
        if(!strcmp(dir->keys[i]->name, name) && !strcmp(dir->keys[i]->extension, extension)){
            delete_key(dir, i);
            return 0;
        }
    }
    for(i = 0; i < DEGREE ; i++){
        if(dir->children[i] == NULL){
            break;
        }
        if(!strcmp(dir->children[i]->name, name)){
            if(!is_req){
                printf("rm: невозможно удалить \"%s\": Это каталог\n", name);
                return 1;
            }
            delete_node(dir->children[i]);
            return 0;
        }
    }
    printf("rm: невозможно удалить \"%s\": Нет такого файла или каталога\n", name);
    return 1;

}

bool delete_node(node* dir){
    int i = 0;
    bool flag = 0;
    for(i = 0; i < DEGREE - 1; i++){
        if(dir->keys[i] == NULL){
            break;
        }
        free(dir->keys[i]);
        dir->keys[i] = NULL;
    }
    for(i = 0; i < DEGREE; i++){
        if(dir->children[i] == NULL){
            break;
        }
        delete_node(dir->children[i]);
    }
    for(i = 0; i < DEGREE; i++){
        if((dir->parent)->children[i] == dir){
            free((dir->parent)->children[i]);
            (dir->parent)->children[i] = NULL;
            break;
        }
    }
    return 0;
}

bool delete_key(node* dir, int i){
    int j;
    for(j = 0; j < DEGREE - 1; j++){
        // printf("%d", i);
        if(dir->keys[j] == NULL){
            break;
        }
        if(j < i){
            continue;
        }
        if(j >= i){
            dir->keys[j-1] = dir->keys[j];
            free(dir->keys[j]);
            dir->keys[j] = NULL;
            continue;
        }
    //     else{
            
    //     }
    // }
    // if(dir->keys[j] == NULL){
    //     free(dir->keys[j-1]);
    //     dir->keys[j-1] = NULL;
    // }
    }
    return 0;
}
//---------------ФУНКЦИИ ВВОДА---------------

bool read_command(){
    const enum comands{MKDIR = 'm',TOUCH = 't',RM = 'r',FIND = 'f',CD = 'c',LS = 'l'};
    char extension[MAX_EXTEN_LEN] = {'\0'};
    char line[BUFF_SIZE] = {'0'};
    char name[MAX_NAME_LEN] = {'\0'};
    char command[6] = {'\0'};
    int count = 0;
    bool is_req = 0;
    int i = 0;
    fgets(line, sizeof(line),stdin);
    while(line[count] == ' '){
        count++;
    }
    switch (line[count]){
    case MKDIR:
        for(i = 0; i < 5; i++){
            command[i] = line[count + i];
        }
        if(strcmp(command, "mkdir")){
            puts("Команда не найдена");
            break;
        }
        count += i + 1;
        while(line[count] == ' '){
            count++;
        }   
        i = 0;
        while(line[i+count] != '\n' && line[i+count] != ' ' && i+count < BUFF_SIZE){
            name[i] = line[i+count];
            i++;
        }
        if(!strcmp(name,"\n") || !strcmp(name,"\0")){
            puts("mkdir: пропущен операнд");
            break;
        }
        push_node(curr_node, name);
        break;

    case TOUCH:
        for(i = 0; i < 5; i++){
            command[i] = line[count + i];
        }
        if(strcmp(command, "touch")){
            puts("Команда не найдена");
            break;
        }
        count += i + 1;
        while(line[count] == ' '){
            count++;
        }   
        i = 0;
        while(line[i+count] != '.' && line[i+count] != '\n' && line[i+count] != ' ' && i+count < BUFF_SIZE){
            name[i] = line[i+count];
            i++;
        }
        if(!strcmp(name,"\n") || !strcmp(name,"\0")){
            puts("touch: пропущен операнд");
            break;
        }
        count += i;
        i = 0;
        while(line[i+count] != '\n' && line[i+count] != ' ' && line[i+count] != '\0' && i + count < BUFF_SIZE){
            extension[i] = line[i+count];
            i++;
        }
        push_key(curr_node, name, extension);
        break;
    case RM:
        //###############СЧИТЫВАНИЕ КОМАНДЫ ДО КОНЦА###############
        for(i = 0; i < 2; i++){
            command[i] = line[count + i];
        }
        if(strcmp(command, "rm")){
            puts("Команда не найдена");
            break;
        }
        //###############ОБРАБОТКА ПРОБЕЛОВ МЕЖДУ КОМАНДОЙ И ОПЕРАНДАМИ###############
        count += i + 1;
        while(line[count] == ' '){
            count++;
        }
        //###############СЧИТЫВАНИЕ КЛЮЧЕЙ ДО ОПЕРНДОВ###############
        if(line[count] == '-'){
            if(line[count+1] == 'r'){
                is_req = 1;
                count+=2;
            }
            else{
                printf("rm: неверный ключ — \"%c\"", line[count+1]);
                break;
            }
        }
        //###############ОБРАБОТКА ПРОБЕЛОВ МЕЖДУ КЛЮЧАМИ И ОПЕРАНДАМИ###############
        while(line[count] == ' '){
            count++;
        }
        i = 0;
        //###############ОБРАБОТКА ИМЕНИ ОПЕРАНДА###############
        while(line[i+count] != '.' && line[i+count] != '\n' && line[i+count] != ' ' && i+count < BUFF_SIZE){
            name[i] = line[i+count];
            i++;
        }
        //###############ПРОВЕРКА НА НЕНУЛЕВОЕ ИМЯ###############
        if(!strcmp(name,"\n") || !strcmp(name,"\0")){
            puts("rm: пропущен операнд");
            break;
        }
        count += i;
        i = 0;
        //###############СЧИТЫВАНИЕ РАСШИРЕНИЯ ДЛЯ ФАЙЛА###############
        while(line[i+count] != '\n' && line[i+count] != ' ' && line[i+count] != '\0' && i + count < BUFF_SIZE){
            extension[i] = line[i+count];
            i++;
        }
        count += i;
        //###############ОБРАБОТКА ПРОБЕЛОВ МЕЖДУ ОПЕРНАДОМ И ВОЗМОЖНЫМ КЛЮЧОМ###############
        while(line[count] == ' ' && count < BUFF_SIZE){
            count++;
        }
        if(line[count] == '-'){
            if(line[count+1] == 'r'){
                is_req = 1;
                count+=2;
            }
            else{
                printf("rm: неверный ключ — \"%c\"", line[count+1]);
                break;
            }
        }
        delete_elem(curr_node, name, extension, is_req);
        break;

    case FIND:
        break;

    case CD:
        //###############СЧИТЫВАНИЕ КОМАНДЫ ДО КОНЦА###############
        for(i = 0; i < 2; i++){
            command[i] = line[count + i];
        }
        if(strcmp(command, "cd")){
            puts("Команда не найдена");
            break;
        }
        //###############ОБРАБОТКА ПРОБЕЛОВ МЕЖДУ КОМАНДОЙ И ОПЕРАНДАМИ###############
        count += i + 1;
        while(line[count] == ' '){
            count++;
        }
        i = 0;
        //###############ОБРАБОТКА ИМЕНИ ОПЕРАНДА###############
        while(line[i+count] != '\n' && line[i+count] != ' ' && i+count < BUFF_SIZE){
            name[i] = line[i+count];
            i++;
        }
        //###############ПРОВЕРКА НА НЕНУЛЕВОЕ ИМЯ###############
        if(!strcmp(name,"\n") || !strcmp(name,"\0")){
            curr_node = root;
            break;
        }
        if(!strcmp(name,"..")){
            curr_node = curr_node->parent;
            break;
        }
        for(i = 0; i < DEGREE; i++){
            if(curr_node->children[i] == NULL){
                break;
            }
            if(!strcmp(name, curr_node->children[i]->name)){
                curr_node = curr_node->children[i];
                return 0;
            }
        }
        printf("cd: Каталога \"%s\" не существует\n", name);
        break;

    case LS:
        for(i = 0; i < 2; i++){
            command[i] = line[count + i];
        }
        if(strcmp(command, "ls")){
            puts("Команда не найдена");
            break;
        }
        print_elems();
        break;
    default:
        puts("Команда не найдена");
        break;
    }
    return 1;
}

//---------------ФУНКЦИИ ВЫВОДА---------------

void print_shell(){
    printf("%s >> ", curr_node->name);
}

//---------------СЛУЖЕБНЫЕ ФУНКЦИИ---------------


char* get_curr_date(){
    time_t now = time(0);
    return asctime(localtime(&now));
}
