/********************************************
* 家族谱管理系统 vCPP.D.0.S
********************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <assert.h>

using namespace std;

// 字符串最大长度
#define STR_LEN 128
// 最大孩子数
#define CHILD_LEN 64
// 缓存容量
#define BUFFER_SIZE 1024

// 家谱成员信息
struct Ren {
    char xm[STR_LEN];   // 姓名
    int sn;             // 生年
    int cn;             // 卒年
};

// 家谱树节点
struct Node {
    Ren ren;                // 成员信息
    Node* fu;               // 父节点
    Node* ha[CHILD_LEN];    // 孩子节点
    int sl;                 // 孩子数量
    int cj;                 // 节点层级
};

// 创建树节点
Node* createNode(Node* parent, Ren* ren) {
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node) {
        node->ren = *ren;
        if (parent) {
            node->fu = parent;
            node->cj = parent->cj + 1;
            parent->ha[parent->sl++] = node;
        }
    }
    return node;
}

// 删除树节点
void removeNode(Node* cursor) {
    if (cursor) {
        if (cursor->fu) {
            Node* parent = cursor->fu;
            int position = -1;
            for (int index = 0; index < parent->sl; ++index) {
                if (parent->ha[index] == cursor) {
                    position = index;
                    break;
                }
            }
            if (position != -1) {
                for (int index = position + 1; index < parent->sl; ++index) {
                    parent->ha[index - 1] = parent->ha[index];
                }
                --parent->sl;
            }
        }
    }
}

// 计算成员层级
int countRenLevel(const char content[]) {
    int level = 0;
    const char* cursor = content;
    while (*cursor == ' ') {
        ++level;
        ++cursor;
    }
    return level;
}

// 编辑家谱成员信息
void editFamilyRen(Ren* ren) {
    cout << "-----------------------------------" << endl;
    cout << "     编辑家谱成员信息 " << endl;
    if (strlen(ren->xm)) {
        printf(" 姓名：%s\n", ren->xm);
    }
    else {
        cout << " 姓名：";
        cin >> ren->xm;
    }
    cout << " 生年：";
    cin >> ren->sn;
    cout << " 卒年：";
    cin >> ren->cn;
    cout << "-----------------------------------" << endl;
}

// 输出成员标题
void showFamilyRenTitle() {
    printf(" %-10s", "姓名");
    printf(" %-10s", "生年");
    printf(" %-10s", "卒年");
    printf(" %-10s", "享年");
    printf("\n");
}

// 输出成员信息
void showFamilyRen(Ren* ren, int newline) {
    printf(" %-10s", ren->xm);
    printf(" %-10d", ren->sn);
    printf(" %-10d", ren->cn);
    printf(" %-10d", ren->cn - ren->sn);
    if (newline) {
        printf("\n");
    }
}

// 递归遍历显示家谱信息
void recursiveFamilyNodeShow(Node* cursor, int brother_line[], int flag, int all) {
    if (cursor) {
        char generation[STR_LEN] = { 0 };
        if (all) {
            showFamilyRen(&cursor->ren, 1);
            for (int index = 0; index < cursor->sl; ++index) {
                Node* child = cursor->ha[index];
                recursiveFamilyNodeShow(child, brother_line, flag, all);
            }
        }
        else {
            sprintf(generation, "【%d世】", cursor->cj + 1);
            printf("%10s", generation);
            if (cursor->cj > 0) {
                for (int index = 0; index < cursor->cj - 1; ++index) {
                    if (brother_line[index]) {
                        printf("   │");
                    }
                    else {
                        printf("    ");
                    }
                }
                if (flag) {
                    printf("   ├─>");
                }
                else {
                    printf("   └─>");
                }
            }
            printf(" %s\n", cursor->ren.xm);
            for (int index = 0; index < cursor->sl; ++index) {
                int flag = (index < cursor->sl - 1);
                if (flag) {
                    brother_line[cursor->cj] = 1;
                }
                else {
                    brother_line[cursor->cj] = 0;
                }
                Node* child = cursor->ha[index];
                recursiveFamilyNodeShow(child, brother_line, flag, all);
            }
            brother_line[cursor->cj] = 0;
        }
    }
}

// 递归遍历存储家谱信息
void recursiveFamilyNodeSave(Node* cursor, FILE* output) {
    if (cursor) {
        if (cursor->cj > 0) {
            char format[STR_LEN] = { 0 };
            sprintf(format, "%%%ds", cursor->cj);
            fprintf(output, format, " ");
        }
        fprintf(output, "%s ", cursor->ren.xm);
        fprintf(output, "%d ", cursor->ren.sn);
        fprintf(output, "%d ", cursor->ren.cn);
        fprintf(output, "\n");
        for (int index = 0; index < cursor->sl; ++index) {
            Node* child = cursor->ha[index];
            recursiveFamilyNodeSave(child, output);
        }
    }
}

// 递归遍历查询家谱信息
Node* recursiveFamilyNodeFind(Node* cursor, const char name[]) {
    if (cursor) {
        if (strcmp(cursor->ren.xm, name) == 0) {
            return cursor;
        }
        for (int index = 0; index < cursor->sl; ++index) {
            Node* child = cursor->ha[index];
            Node* result = recursiveFamilyNodeFind(child, name);
            if (result) {
                return result;
            }
        }
    }
    return NULL;
}

// 递归遍历查询家谱信息
void recursiveFamilyNodeFindByLevel(Node* cursor, int level) {
    if (cursor) {
        if (cursor->cj == level) {
            showFamilyRen(&cursor->ren, 1);
        }
        for (int index = 0; index < cursor->sl; ++index) {
            Node* child = cursor->ha[index];
            recursiveFamilyNodeFindByLevel(child, level);
        }
    }
}

// 递归遍历计算家谱成员数量信息
int recursiveFamilyNodeCount(Node* cursor) {
    int count = 0;
    if (cursor) {
        count = 1;
        for (int index = 0; index < cursor->sl; ++index) {
            Node* child = cursor->ha[index];
            count += recursiveFamilyNodeCount(child);
        }
    }
    return count;
}

// 递归遍历清空家谱信息
void recursiveFamilyNodeClear(Node* cursor) {
    if (cursor) {
        for (int index = 0; index < cursor->sl; ++index) {
            Node* child = cursor->ha[index];
            recursiveFamilyNodeClear(child);
        }
        free(cursor);
    }
}

// 从文件中加载家谱信息
Node* loadFamilyTree() {
    Node* root = NULL;
    FILE* input = fopen("familytree.txt", "r");
    if (input) {
        Node* cursor = NULL;
        char buffer[BUFFER_SIZE] = { 0 };
        while (fgets(buffer, sizeof(buffer), input)) {
            Ren ren = { 0 };
            if (sscanf(buffer, "%s %d %d", ren.xm, &ren.sn, &ren.cn) == 3) {
                int level = countRenLevel(buffer);
                if (level == 0) {
                    assert(root == NULL);
                    root = createNode(NULL, &ren);
                    cursor = root;
                }
                else {
                    int step = level - cursor->cj;
                    assert(step <= 1);
                    if (step != 1) {
                        int count = 0 - step + 1;
                        while (count) {
                            cursor = cursor->fu;
                            --count;
                        }
                    }
                    cursor = createNode(cursor, &ren);
                }
            }
        }
        fclose(input);
    }
    return root;
}

// 将家谱信息存储到文件
void saveFamilyTree(Node* root) {
    FILE* output = fopen("familytree.txt", "w");
    if (output) {
        recursiveFamilyNodeSave(root, output);
        fclose(output);
    }
}

// 图形模式展现家族谱
void showFamilyTreeGraph(Node* root) {
    cout << "-----------------------------------" << endl;
    cout << "     图形模式展现家族谱 " << endl;
    int* brother_line = (int*)calloc(1024, sizeof(int));
    recursiveFamilyNodeShow(root, brother_line, 0, 0);
    free(brother_line);
    cout << "-----------------------------------" << endl;
}

// 列表模式展现家族谱
void showFamilyTreeList(Node* root) {
    cout << "-----------------------------------" << endl;
    cout << "     列表模式展现家族谱 " << endl;
    int* brother_line = (int*)calloc(1024, sizeof(int));
    showFamilyRenTitle();
    recursiveFamilyNodeShow(root, brother_line, 0, 1);
    free(brother_line);
    cout << "-----------------------------------" << endl;
}

// 添加家谱成员
void addFamilyTree(Node** root) {
    char name[STR_LEN] = { 0 };
    cout << "-----------------------------------" << endl;
    cout << "     添加家谱成员 " << endl;
    cout << "  输入新成员姓名：";
    cin >> name;
    if (*root) {
        if (!recursiveFamilyNodeFind(*root, name)) {
            Node* target = NULL;
            char parentname[STR_LEN] = { 0 };
            printf("  指定双亲：");
            cin >> parentname;
            target = recursiveFamilyNodeFind(*root, parentname);
            if (target) {
                Ren ren = { 0 };
                strcpy(ren.xm, name);
                editFamilyRen(&ren);
                createNode(target, &ren);
                saveFamilyTree(*root);
                showFamilyRenTitle();
                showFamilyRen(&ren, 1);
                printf("----------------\n");
                printf("成功添加以上家谱成员！\n");
            }
            else {
                printf("添加失败，家谱中未找到该名字！\n");
            }
        }
        else {
            printf("添加失败，该成员名称已经存在！\n");
        }
    }
    else {
        Ren ren = { 0 };
        strcpy(ren.xm, name);
        editFamilyRen(&ren);
        *root = createNode(NULL, &ren);
        saveFamilyTree(*root);
        showFamilyRenTitle();
        showFamilyRen(&ren, 1);
        printf("----------------\n");
        printf("成功添加以上家谱成员！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 删除家谱成员
void removeFamilyTree(Node** root) {
    Node* target = NULL;
    char name[STR_LEN] = { 0 };
    cout << "-----------------------------------" << endl;
    cout << "     删除家谱成员 " << endl;
    cout << "  输入姓名：";
    cin >> name;
    target = recursiveFamilyNodeFind(*root, name);
    if (target) {
        showFamilyRenTitle();
        showFamilyRen(&target->ren, 1);
        removeNode(target);
        recursiveFamilyNodeClear(target);
        if (target == *root) {
            *root = NULL;
        }
        saveFamilyTree(*root);
        printf("----------------\n");
        printf("成功删除以上家谱成员！\n");
    }
    else {
        printf("  没有找到相关信息！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 按姓名查找
void findFamilyTreeByName(Node* root) {
    Node* target = NULL;
    char name[STR_LEN] = { 0 };
    cout << "-----------------------------------" << endl;
    cout << "     按姓名查找 " << endl;
    cout << "  输入姓名：" << endl;
    cin >> name;
    target = recursiveFamilyNodeFind(root, name);
    if (target) {
        int* brother_line = (int*)calloc(1024, sizeof(int));
        printf("---------------\n");
        showFamilyRenTitle();
        showFamilyRen(&target->ren, 1);
        printf("---------------\n");
        printf("【本人和子孙】\n");
        recursiveFamilyNodeShow(target, brother_line, 0, 0);
        free(brother_line);
        if (target->fu) {
            printf("【祖先】\n");
            showFamilyRenTitle();
            while (target->fu) {
                target = target->fu;
                showFamilyRen(&target->ren, 1);
            }
        }
    }
    else {
        printf("  没有找到相关信息！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 按世代查找
void findFamilyTreeByLevel(Node* root) {
    int level;
    cout << "-----------------------------------" << endl;
    cout << "              $ 按世代查找 $" << endl;
    cout << "  输入世代（数字1~N）：";
    cin >> level;
    showFamilyRenTitle();
    recursiveFamilyNodeFindByLevel(root, level - 1);
    cout << "-----------------------------------" << endl;
}

// 查询家族谱成员关系
void relationFamilyTree(Node* root) {
    Node* target1 = NULL;
    Node* target2 = NULL;
    char name[STR_LEN] = { 0 };
    cout << "-----------------------------------" << endl;
    printf("     查询家族谱成员关系 \n");
    cout << "  输入第一人姓名：" << endl;
    cin >> name;
    target1 = recursiveFamilyNodeFind(root, name);
    cout << "  输入第二人姓名：";
    cin >> name;
    target2 = recursiveFamilyNodeFind(root, name);
    if (target1 && target2) {
        Node* ancestor = target1->cj < target2->cj ? target1 : target2;
        Node* progeny = ancestor == target1 ? target2 : target1;
        int diff = progeny->cj - ancestor->cj;
        switch (diff) {
        case 0:
            if (progeny->fu == ancestor->fu) {
                printf("  关系：[同胞兄弟]\n");
            }
            else {
                printf("  关系：[堂兄弟]\n");
            }
            break;
        case 1:
            if (progeny->fu == ancestor) {
                printf("  关系：[父子]\n");
            }
            else {
                printf("  关系：[叔侄]\n");
            }
            break;
        case 2:
            printf("  关系：[爷孙]\n");
            break;
        case 3:
            printf("  关系：[曾祖孙]\n");
            break;
        case 4:
            printf("  关系：[高祖孙]\n");
            break;
        }
    }
    else {
        printf("  没有找到相关信息！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 查找家谱成员
void findFamilyTree(Node* root) {
    while (1) {
        int option = 0;
        cout << "-----------------------------------\n";
        cout << "      查询家族谱信息 " << endl;
        cout << "  1 # 按姓名查找成员" << endl;
        cout << "  2 # 按世代查找成员" << endl;
        cout << "  3 # 查询家族谱成员关系" << endl;
        cout << "  0 # 返回" << endl;
        cout << "-----------------------------------" << endl;
        cout << "     请选择：";
        cin >> option;
        if (option == 0) break;
        switch (option) {
        case 1:
            findFamilyTreeByName(root);
            break;
        case 2:
            findFamilyTreeByLevel(root);
            break;
        case 3:
            relationFamilyTree(root);
            break;
        }
    }
}

// 修改家谱成员
void modifyFamilyTree(Node* root) {
    Node* target = NULL;
    char name[STR_LEN] = { 0 };
    cout << "-----------------------------------" << endl;
    cout << "     修改家谱成员 " << endl;
    cout << "  输入姓名：";
    cin >> name;
    target = recursiveFamilyNodeFind(root, name);
    if (target) {
        showFamilyRenTitle();
        showFamilyRen(&target->ren, 1);
        printf("----------------\n");
        editFamilyRen(&target->ren);
        printf("----------------\n");
        saveFamilyTree(root);
        printf("成功修改以上家谱成员！\n");
    }
    else {
        printf("  没有找到相关信息！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 导入家族谱
void importFamilyTree(Node** root) {
    char filename[STR_LEN] = { 0 };
    FILE* input = NULL;
    cout << "-----------------------------------" << endl;
    cout << "     导入家族谱 " << endl;
    cout << "  输入文件名：" << endl;
    cin >> filename;
    input = fopen(filename, "r");
    if (input) {
        showFamilyRenTitle();
        while (1) {
            char buffer[1024] = { 0 };
            char parent[STR_LEN] = { 0 };
            Ren ren = { 0 };
            if (!fgets(buffer, sizeof(buffer), input)) break;
            sscanf(buffer, "%s %d %d %s", ren.xm, &ren.sn, &ren.cn, parent);
            if (!recursiveFamilyNodeFind(*root, ren.xm)) {
                if (strlen(parent)) {
                    Node* target = NULL;
                    target = recursiveFamilyNodeFind(*root, parent);
                    if (target) {
                        createNode(target, &ren);
                        showFamilyRen(&ren, 1);
                    }
                    else {
                        printf("%s 添加失败，%s未找到。\n", ren.xm, parent);
                    }
                }
                else {
                    *root = createNode(NULL, &ren);
                    showFamilyRen(&ren, 1);
                }
            }
            else {
                printf("添加失败，%s已经存在！\n", ren.xm);
            }
        }
        saveFamilyTree(*root);
        fclose(input);
        printf("导入完成。\n");
    }
    else {
        printf("文件读取失败！\n");
    }
    cout << "-----------------------------------" << endl;
}

// 家族谱管理
void manageOptions(Node** root) {
    while (1) {
        int option = 0;
        cout << "-----------------------------------" << endl;
        cout << "      家族谱管理系统" << endl;
        cout << "  1 # 添加家谱成员" << endl;
        cout << "  2 # 删除家谱成员" << endl;
        cout << "  3 # 修改家谱成员" << endl;
        cout << "  4 # 导入家族谱" << endl;
        cout << "  0 # 返回" << endl;
        cout << "-----------------------------------" << endl;
        cout << "     请选择：";
        cin >> option;
        if (option == 0) break;
        switch (option) {
        case 1:
            addFamilyTree(root);
            break;
        case 2:
            removeFamilyTree(root);
            break;
        case 3:
            modifyFamilyTree(*root);
            break;
        case 4:
            importFamilyTree(root);
            break;
        }
    }
}

// 菜单操作
void menuOptions(Node* root) {
    system("title 家族谱管理系统");
    while (1) {
        int option = 0;
        cout << "-----------------------------------" << endl;;
        cout << "      家族谱管理系统 " << endl;
        cout << "  1 # 图形展现家族谱" << endl;
        cout << "  2 # 列表展现家族谱" << endl;
        cout << "  3 # 查询家族谱信息" << endl;
        cout << "  4 # 管理族谱信息" << endl;
        cout << "  0 # 退出程序" << endl;
        cout << "-----------------------------------" << endl;
        cout << "     请选择：";
        cin >> option;
        if (option == 0) break;
        switch (option) {
        case 1:
            showFamilyTreeGraph(root);
            break;
        case 2:
            showFamilyTreeList(root);
            break;
        case 3:
            findFamilyTree(root);
            break;
        case 4:
            manageOptions(&root);
            break;
        }
    }
}

// 主函数
int main() {
    // 从文件加载家谱信息
    Node* root = loadFamilyTree();
    // 进入操作菜单
    menuOptions(root);
    // 释放家谱树
    recursiveFamilyNodeClear(root);
    return 0;
}