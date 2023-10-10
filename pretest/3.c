// find number of ways to get sum 'x' with 'n' dice where every dice has 'm' faces
// reference : https://www.geeksforgeeks.org/dice-throw-dp-30/

// time complexity = O(m*n*x) ； space complexity = O(n*x)
#include <stdio.h>
#include <string.h>

int findWays(int m, int n, int x)
{
    int table[n + 1][x + 1];
    memset(table, 0, sizeof(table)); // Initialize all entries as 0
 
    // Table entries for only one dice
    for (int j = 1; j <= m && j <= x; j++)
        table[1][j] = 1;
 
    // i: number of dice, j: sum
    for (int i = 2; i <= n; i++)
        for (int j = 1; j <= x; j++)
            for (int k = 1; k <= m && k < j; k++)
                table[i][j] += table[i-1][j-k];
 
    return table[n][x];
}
 
int main()
{
    printf("%d",findWays(6, 5, 15)); 
    return 0;
}