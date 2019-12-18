// A = 8
// n1 = 5, n2 = 6, n3 = 3, n4 = 1
// Table
// |||||||||||||||
// ||  |   |   ||
// ||  | 0 | * ||
// ||  | * |   ||
// |||||||||||||||
// 

#pragma comment(lib, "bdd.lib")

#include <bdd.h>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;
std::ofstream out;

const int SIDE_LEN = 3;
const int COUNT_OF_OBJECTS = SIDE_LEN * SIDE_LEN;//число объектов 
const int COUNT_OF_PROPERITIES = 4;//число свойств
const int LOG_N = 4;
const int COUNT_OF_VARIABLES = COUNT_OF_OBJECTS * LOG_N * COUNT_OF_PROPERITIES; 
const int K = 24;
// p[k][i][j] - свойство к у объекта i имеет значение = j
bdd p[COUNT_OF_PROPERITIES][COUNT_OF_OBJECTS][COUNT_OF_OBJECTS];
// ф-ция для вывода решений
void fun(char *varset, int size);

unsigned int isRightReachable(int i){
    return (i + 1) % SIDE_LEN != 0;
}

unsigned int isLeftReachable(int i){
    return i % SIDE_LEN != 0;
}

unsigned int isBottomReachable(int i){
    return i + SIDE_LEN < COUNT_OF_OBJECTS;
}

unsigned int isTopReachable(int i){
    return i - SIDE_LEN >= 0;
}

unsigned int getRight(int i){
    return i + 1;
}

unsigned int getBottom(int i){
    return i + SIDE_LEN;
}

bdd rightNeighbors(int k1, int v1, int kCntr, int vCntr){
    bdd tmp = bddtrue;
    for (auto i = 0; i < COUNT_OF_OBJECTS; ++i){
        if(isRightReachable(i))
            tmp &= !(p[kCntr][i][vCntr] ^ p[k1][getRight(i)][v1]);

        if (!isRightReachable(i))
            tmp &= !p[kCntr][i][vCntr];
        if(!isLeftReachable(i))
            tmp &= !p[k1][i][v1];
    }
    return tmp;
}

bdd downNeighbors(int k1, int v1, int kCntr, int vCntr){
    bdd tmp = bddtrue;
    for (auto i = 0; i < COUNT_OF_OBJECTS; ++i){
        if(isBottomReachable(i))
            tmp &= !(p[kCntr][i][vCntr] ^ p[k1][getBottom(i)][v1]);
        
        if(!isBottomReachable(i))
            tmp &= !p[kCntr][i][vCntr];
        if(!isTopReachable(i))
            tmp &= !p[k1][i][v1];
    }
    return tmp;
}

bdd neibghors(int k1, int v1, int kCntr, int vCntr){
    bdd tmp = bddtrue;
    bdd tmp2 = bddtrue;
    for (auto i = 0; i < COUNT_OF_OBJECTS; ++i){
        if(isRightReachable(i))
            tmp &= !(p[kCntr][i][vCntr] ^ p[k1][getRight(i)][v1]);
        if(isBottomReachable(i))
            tmp2 &= !(p[kCntr][i][vCntr] ^ p[k1][getBottom(i)][v1]);
        if(!isTopReachable(i) && !isLeftReachable(i))
            tmp &= !p[k1][i][v1];
        if(!isRightReachable(i) && !isBottomReachable(i))
            tmp &= !p[kCntr][i][vCntr];
    }

    return tmp | tmp2;    
}

int main(int argc, char **argv){
    bdd_init(10000000, 1000000);
    bdd_setvarnum(COUNT_OF_VARIABLES);
    for (int k = 0; k < COUNT_OF_PROPERITIES; ++k){
        int delta = 0;
        // по объектам
        for (auto i = 0; i < COUNT_OF_OBJECTS; ++i){
            // цикл значений свойств
            for (auto j = 0; j < COUNT_OF_OBJECTS; ++j){
                p[k][i][j] = bddtrue;
                for (auto l = 0; l < LOG_N; ++l){
                    if (j >> l & 1)
                        p[k][i][j] &= bdd_ithvar(delta + LOG_N * k + l);
                    else
                        p[k][i][j] &= bdd_nithvar(delta + LOG_N * k + l);
                }
            }
            delta += LOG_N * COUNT_OF_PROPERITIES;
        }
    }

    bdd task = bddtrue;

    cout << bdd_satcount(task) << " solution(s) after start\n"<< endl;

    {
        for (int k = 0; k < COUNT_OF_PROPERITIES; ++k){
            for (int i = 0; i < COUNT_OF_OBJECTS; ++i){
                bdd temp = bddfalse;
                for (int j = 0; j < COUNT_OF_OBJECTS; ++j)
                    temp |= p[k][i][j];
                task &= temp;
            }
        }
    }
    cout << bdd_satcount(task) << " solution(s) after adding default constaints\n"<< endl;

    // установка начальных ограничений (по заданию)
    // n1(5)
    {
        task &= p[1][2][1];
        task &= p[1][3][3];
        task &= p[2][4][6];
        task &= p[0][1][0];
        task &= p[3][8][5];

        task &= p[0][8][7];
        task &= p[3][1][3];
        task &= p[2][6][7];
        task &= p[3][2][2];
        task &= p[0][5][3];
        task &= p[1][5][4];
        task &= p[2][5][3];
        task &= p[3][5][1];
        task &= p[1][0][5];
        task &= p[2][0][0];
        task &= p[3][7][8];
        task &= p[2][7][1];
        task &= p[3][3][0];
        task &= p[2][2][8];
        task &= p[1][1][2];
        task &= p[0][0][8];

    }
    cout << bdd_satcount(task) << " solution(s) after adding constaints n1\n"<< endl; 
    
    // n2(6)
    {
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[1][i][7] ^ p[0][i][7]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[2][i][4] ^ p[3][i][5]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[0][i][0] ^ p[3][i][3]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[0][i][4] ^ p[1][i][1]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[0][i][6] ^ p[3][i][7]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[0][i][2] ^ p[1][i][8]);

        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[3][i][0] ^ p[2][i][5]);
        for (int i = 0; i < COUNT_OF_OBJECTS; ++i)
            task &= !(p[0][i][8] ^ p[3][i][4]);
    }
    cout << bdd_satcount(task) << " solution(s) after adding constaints n2\n"<< endl;

    // n3(3)
    {
        task &= rightNeighbors(0,2,0,5);
        task &= downNeighbors(0,1,2,6);
        task &= rightNeighbors(1,7,1,6);
    }
    cout << bdd_satcount(task) << " solution(s) after adding constaints n3\n"<< endl; 

    // n4(1)
    {
        task &= neibghors(0,0,0,8);
    }
    cout << bdd_satcount(task) << " solution(s) after adding constaints n4\n"<< endl;

    // n7(1)
    {
        bdd temp;
        for (int i = 1; i < COUNT_OF_OBJECTS; i++){
            temp = bddtrue;
            for (int j1 = 0; j1 < COUNT_OF_OBJECTS; j1++){
                for (int j2 = 0; j2 < COUNT_OF_OBJECTS; j2++)
                    for (int j3 = 0; j3 < COUNT_OF_OBJECTS; j3++)
                        for (int j4 = 0; j4 < COUNT_OF_OBJECTS; j4++)
                            if (j1 + j2 + j3 + j4 > K)
                                temp &= !(p[0][i][j1] & p[1][i][j2] & p[2][i][j3] & p[3][i][j4]);
            }
            task &= temp;
        }
    }
    cout << bdd_satcount(task) << " solution(s) after adding constaints n7\n"<< endl;


    {
        for (int k = 0; k < COUNT_OF_PROPERITIES; ++k)
            for (int i = 0; i < COUNT_OF_OBJECTS - 1; ++i)
                for (int j = 0; j < COUNT_OF_OBJECTS; ++j)
                    for (int l = i + 1; l < COUNT_OF_OBJECTS; ++l)
                        task &= p[k][i][j] >> !p[k][l][j];
    }
    cout << bdd_satcount(task) << " solution(s) after adding default constraints\n"<< endl;
    bdd_allsat(task, fun);
    bdd_done();
    return 0;    
}

char var[COUNT_OF_VARIABLES];

void print()
{
    const int columSize1 = 6;
    const int columSize2 = 4;
    const string mesT1 = "Obj";
    const string mesT2 = "#";
    const string div = " |";

    cout << setfill('-');
    cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * COUNT_OF_PROPERITIES) << '-' << endl;

    cout << setfill(' ');
    cout << setw(columSize1) << mesT1 << div;
    for (int i = 0; i < COUNT_OF_PROPERITIES; ++i)
    {
        int numCount = 0;
        int p = i + 1;
        while (p > 0)
        {
            p = p / 10;
            ++numCount;
        }

        cout << setw(columSize2 - numCount - 1) << mesT2 << " " << i + 1 << div;
    }
    cout << endl;

    for (int i = 0; i < COUNT_OF_OBJECTS; i++)
    {
        cout << setfill('-');
        cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * COUNT_OF_PROPERITIES) << '-' << endl;

        cout << setfill(' ');
        cout << setw(columSize1) << i << div;
        for (int j = 0; j < COUNT_OF_PROPERITIES; j++)
        {
            int J = i * COUNT_OF_PROPERITIES * LOG_N + j * LOG_N;
            int num = 0;
            for (unsigned k = 0; k < LOG_N; k++)
                num += (int)(var[J + k] << k);
            cout << setw(columSize2) << num << div;
        }

        cout << endl;
    }
    cout << setfill('-');
    cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * COUNT_OF_PROPERITIES) << '-' << endl;
    cout << endl;
}

void build(char *varset, unsigned n, unsigned I)
{
    if (I == n - 1)
    {
        if (varset[I] >= 0)
        {
            var[I] = varset[I];
            print();
            return;
        }
        var[I] = 0;
        print();
        var[I] = 1;
        print();
        return;
    }
    if (varset[I] >= 0)
    {
        var[I] = varset[I];
        build(varset, n, I + 1);
        return;
    }
    var[I] = 0;
    build(varset, n, I + 1);
    var[I] = 1;
    build(varset, n, I + 1);
}

void fun(char *varset, int size)
{
    // freopen("output", "w", stdout);
    build(varset, size, 0);
}
