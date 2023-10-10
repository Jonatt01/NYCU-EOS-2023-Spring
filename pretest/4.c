// find number of ways to get sum 'x' with 'n' dice where every dice has 'm' faces
// reference : https://www.geeksforgeeks.org/dice-throw-dp-30/

// time complexity = spce complexity = O(n*x)
#include <stdio.h>
#include <string.h>

int findWays(int f, int n, int x)
{
    int table[n + 1][x + 1];
    memset(table, 0, sizeof(table)); // Initialize all entries as 0
 
    // Table entries for only one dice
    for (int j = 1; j <= f && j <= x; j++)
        table[1][j] = 1;
 
    // i: number of dice, j: sum
    for (int i = 2; i <= n; i++)
        for (int j = 1; j <= x; j++){
            table[i][j] = table[i][j - 1] + table[i - 1][j - 1];
            if (j - f - 1 >= 0)
                table[i][j] -= table[i - 1][j - f - 1];
        }

    return table[n][x];
}
 
int main()
{
    printf("%d",findWays(20, 6, 72));
    return 0;
}