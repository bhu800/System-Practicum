#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getRemainder(char divisor[],char data[],int N,int dataLen,char remainder[]){
	char dividend[N+1];dividend[N]='\0';

	for(int i=0;i<N;i++)
		dividend[i]=data[i];
	// divisor,dividend -> remainder
	if(dividend[0]=='1'){
		for(int j=0;j<N;j++){
			if(dividend[j]-divisor[j])
				remainder[j]='1';
			else
				remainder[j]='0';
		}
	}
	else{
		for(int j=0;j<N;j++)
			remainder[j]=dividend[j];
	}
	// printf("%s\n", dividend);
	// printf("%s\n\n", remainder);


	for(int i=N;i<dataLen;i++){
		for(int j=1;j<N;j++)
			dividend[j-1]=remainder[j];
		dividend[N-1]=data[i];

		// divisor,dividend -> remainder
		if(dividend[0]=='1'){
			for(int j=0;j<N;j++){
				if(dividend[j]-divisor[j])
					remainder[j]='1';
				else
					remainder[j]='0';
			}
		}
		else{
			for(int j=0;j<N;j++)
				remainder[j]=dividend[j];
		}

		// printf("%s\n", dividend);
		// printf("%s\n\n", remainder);
	}

	return ;
}

int main(){
	// divisor
	char divisor[]="111";
	int N=3;

	// data/dividend
	char data[80];
  	fgets(data,80,stdin);
	int dataLen=strlen(data);
	printf("%d\n", dataLen);
	printf("%d\n", data[dataLen-1]);

	// append extra bits for CRC
	int modifiedDataLen=dataLen + N-1;
	char* modifiedData=(char*)(malloc(sizeof(char)*(modifiedDataLen+1)));	// DELETE

	for(int i=0;i<dataLen;i++)
		modifiedData[i]=data[i];
	for(int i=dataLen;i<modifiedDataLen;i++)
		modifiedData[i]='0';
	modifiedData[modifiedDataLen]='\0';

	// printf("%s\n", modifiedData);

	// get CRC/Remainder, modify CRC bits
	char remainder[N+1];remainder[N]='\0';
	getRemainder(divisor,modifiedData,N,modifiedDataLen,remainder);
	for(int i=dataLen;i<modifiedDataLen;i++)
		modifiedData[i]=remainder[i-dataLen+1];

	// printf("%s\n", remainder);
	printf("%s\n",modifiedData);

	// validation
	char remainder2[N+1];remainder2[N]='\0';
	getRemainder(divisor,modifiedData,N,modifiedDataLen,remainder2);
	printf("%s\n", remainder2);

	return 0;
}