// reference : https://www.geeksforgeeks.org/multiply-large-numbers-represented-as-strings/
# include <stdio.h>
# define MAX 10

void large_mul(int* result, int* a, int* b){
	// a and b are arrays of int
	int sum=0;
	int pos_a = 0;
	int pos_b = 0;
	int carry = 0;

	for(int j=0;j<5;j++){
		if(a[j]==0){
			pos_a++;
			continue;
		}

		else{
			pos_b=0;////don't forget////
			for(int k=0;k<5;k++){
				sum=a[j]*b[k]+result[pos_a+pos_b]+carry;
				// carry for next iteration
				carry=sum/10;
				// store result
				result[pos_a+pos_b]=sum%10;
				pos_b++;
			}
		}
		pos_a++;
	}
}
void large_add(int* sumofsquare,int*result){
	int carry = 0;
	for(int j=0;j<20;j++){
		int tmp = sumofsquare[j]+ result[j]+carry;
		sumofsquare[j]=tmp%10;
		carry=tmp/10;
		result[j]=0; // reset the result of the square after computing add
	}
}
int main(){
	int result[20]={0}; // result of the square
	int sumofsquare[20]={0}; // final ans
	int a[5]={0};
	int b[5]={0};
	int count=1;
	for(int i = 0;i<MAX;i++){
		count=i+1; // the number need to square now

		// save integers to array
		for(int j=0;(j<5) && (count!=0);j++){
			a[j]=count%10;

			count = count/10;
		}
		// copy a to b
		for(int j=0;j<5;j++)
			b[j]=a[j];

		large_mul(result,a,b);
		large_add(sumofsquare,result);
	}
	// output ans to screen
	for(int i = 19;i>=0;i--)
		printf("%d",sumofsquare[i]);
	printf("\n");

	return 0;
}