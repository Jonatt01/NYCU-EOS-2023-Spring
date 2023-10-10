// change to function pointer
# include<stdio.h>

int gcd_recursive(int a, int b){
    return b==0?a:gcd_recursive(b,a%b);
}

int gcd_iteration(int a, int b){
    while(b){
        int tmp = b;
        b=a%b;
        a = tmp;
    }
    return a;
}
int main(){
    int a;
    int b;
    int ans1, ans2;
    printf("enter two number with space between them: ");
    scanf("%d",&a);
    scanf("%d",&b);
    ans1 = gcd_recursive(a,b);
    ans2 = gcd_iteration(a,b);
    printf("%d\t%d\n",ans1,ans2);
    return 0;
}