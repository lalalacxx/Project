#include<stdio.h>

#define N 4
//   a b为第一个数字的行和列
//   c d为最后一个数的行和列
void rotate(int arr[N][N],int a,int b,int c,int d)
{
    int i = 0;
    for(i = 0;i < d-b;i++)
    {
        int tmp = arr[a][i];
        arr[a][i] = arr[c][b-i];
        arr[c][b-i] = arr[c][d-i];
        arr[c][d-i] = arr[a][d-i];
        arr[a][d-i] = tmp;
    }
}
void rotateArr(int arr[N][N])
{
    int a = 0;
    int b = 0;
    int c = N-1;
    int d = N-1;
    while(a < c)
    {
        rotate(arr,a++,b++,c--,d--);
    }
}
void show(int arr[N][N])
{
    int i = 0;
    int j = 0;
    for(i = 0;i < N;i++)
    {
        for(j = 0;j < N;j++)
        {
            printf("%-3d",arr[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int arr[N][N];
    int num = 1;
    int i = 0;
    int j = 0;
    for(i = 0;i < N;i++)
    {
        for(j = 0;j < N;j++)
        {
            arr[i][j] = num++;
        }
    }
    show(arr);
    rotateArr(arr);
    printf("\n");
    show(arr);
}
