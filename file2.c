#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
struct time{ //inode 시간
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;

};

struct superblock {//superblock inode 128 datablock 256비트씩 동적할당 
	unsigned bootblock : 16;
	unsigned *inodeList;
	unsigned *datablockList;
};
struct inode{
	int type;//파일 종류 일반 파일 0 디렉터리 파일 1 
	char *name; //파일 이름 최대 7바이트 
	int size;//파일 크기
	struct time t;//파일 생성 시간
	int *datablockInfo;//datablock 정보 
	int indirectblock;
};
struct directory_datablock{
	char *name;
	int num;
};
struct datablock{//전체 datablock 256byte
	char *data; 
	struct directory_datablock *directory;
	int *indirect;	
};
typedef struct file_name { //자기 참조 구조체, 현재 위치 저장
	char* d;
	int i;
	struct file_name* next;
}file;
typedef file* NAME;
struct superblock sb; 
struct datablock *datablock;
struct inode *inode;
file *pwd1; //
file *pwd;
file *back;
file *root;
NAME file_list(NAME head, char* s, int i); 
NAME next_file(NAME head, char* s, int i);
NAME back_file(NAME head); 
NAME now_directory(NAME head);//현재 위치
NAME back_directory(NAME head);//이전 위치 저장
void print_file(NAME head); //현재 위치 절대경로 출력
void setroot();   //동적할당, 초기화
void setsuperblock();
void setinode();
void setdatablock();
void filesave();
void fileread();
void bit_print(unsigned int a); // 비트열 출력
void superblockprint();
int find_freeinode(); //가용 inode 찾고 1로 변경
int find_freedatablock();//가용 datablock 찾고 1로 변경
void free_inode(int num); //사용한 inode 0으로 변경
void free_datablock(int num);//사용한 datablock 0으로 변경
int check_inode(int num); //특정 inode 사용중인지 확인
int check_datablock(int num); //특정 datablock 사용중인지 확인
int mymkfs(); 
void myls(char* f1);
void myshowfile(char* num2, char* num3, char* f1);
int mycat(char *f1);
int mycpfrom(char *f1, char *f2);
int mycpto(char *f1, char *f2);
void mypwd();
void mycd(char *f1);
void mycp(char* f1, char* f2);
void mymv(char* f1, char* f2);
int mymkdir(char* f1);
int myrmdir(char* f1);
void myrm(char* f1);
void mytouch(char* f1);
void myinode(char *num1);
void mydatablock(char* num1);
void mystate();
void set_time(struct inode *inode); //inode 시간 변경
int sort(const void* p, const void* q); //myls 사전순 출력 위해 정렬



int main(void)
{
	char *tmp;
	setsuperblock();
	setinode();
	setdatablock();
	tmp = (char *)malloc(sizeof(char)*20);
	
	setroot();
	scanf("%s", tmp);
	getchar();
	if(!(strcmp(tmp,"myfs_shell"))){
		if(access("myfs", F_OK) != -1)
		{
			fileread();

		}
		else
		{
			printf("파일시스템이 없습니다. 파일시스템을 만듭니다.\n");
			filesave();
		}
		int s;
		char *c =(char *)malloc(sizeof(char)*30);
		char* c1,*c2,*c3,*c4,*c5;
		c1 =(char *)calloc(30,sizeof(char));
		c2 =(char *)calloc(30, sizeof(char));
		c3 =(char *)calloc(30,sizeof(char));
		c4 =(char *)calloc(30,sizeof(char));
		

		while(1){	
			printf("[");
			print_file(pwd1);
			printf(" ]$ ");
			pwd = now_directory(pwd1); //현재 위치 pwd에 저장
			back = back_directory(pwd1);//이전 위치 back에 저장

			scanf("%[^\n]", c);
			getchar();
			sscanf(c,"%s %s %s %s", c1, c2, c3, c4);		
			if (strcmp(c1, "mymkfs") == 0)
				mymkfs();
			else if (strcmp(c1, "myls") == 0)
				myls(c2);
			else if (strcmp(c1, "mycat") == 0)
				mycat(c2);
			else if (strcmp(c1, "mycpfrom") == 0)
				mycpfrom(c2, c3);
			else if (strcmp(c1, "mycpto") == 0)
				mycpto(c2, c3);
			else if (strcmp(c1, "mypwd") == 0)
				mypwd();
			else if (strcmp(c1, "mycd") == 0)
				mycd(c2);
			else if (strcmp(c1, "mycp") == 0)
				mycp(c2, c3);
			else if (strcmp(c1, "mymv") == 0)
				mymv(c2, c3);
			else if (strcmp(c1, "mymkdir") == 0)
				mymkdir(c2); 
			else if (strcmp(c1, "myrmdir") == 0)
				myrmdir(c2);
			else if (strcmp(c1, "myshowfile") == 0)
				myshowfile(c2, c3, c4);
			else if (strcmp(c1, "myrm") == 0)
				myrm(c2);
			else if (strcmp(c1, "mytouch") == 0)
				mytouch(c2);
			else if (strcmp(c1, "myinode") == 0)
				myinode(c2);
			else if (strcmp(c1, "mydatablock") == 0)
				mydatablock(c2);
			else if (strcmp(c1, "mystate") == 0)
				mystate();
			else if (strcmp(c1, "ls") == 0 || strcmp(c1, "cat") == 0)
				system(c);
			else if (*c1 == '\0')
				printf("\n");
			else if (strcmp(c1, "exit") == 0)
			{
				free(tmp);
				free(c);
				free(c1);
				free(c2);
				free(c3);
				free(c4);

				printf("Bye \n");

				break;
			}
		
		}	
		
	}
	else{
		printf("Bye \n");
	}
}


int mymkfs()
{
	char c;
	if (access("myfs", F_OK) != -1)
	{
		printf("파일시스템이 있습니다. 다시 만들겠습니까? (y/n) ");
		scanf("%c", &c);
		getchar();
		if (c == 'y')
		{
			remove("myfs");
			printf("\n--\n");

			for (int i = 0; i < 128; i++) {

				free((inode + i)->name);
				(inode + i)->name = NULL;
				free((inode + i)->datablockInfo);
				(inode + i)->datablockInfo = NULL;
				printf("\n--\n");

			}
			free(inode);
			inode = NULL;
			printf("\n--\n");

			free(sb.inodeList);
			sb.inodeList = NULL;

			free(sb.datablockList);
			sb.datablockList = NULL;

			free(pwd1);
			pwd1 = NULL;

			
			for (int i = 0; i < 256; i++) 
			{
				if ((datablock + i)->data != NULL)
				{
					free((datablock + i)->data);
					(datablock + i)->data = NULL;
				}
				else if ((datablock + i)->directory != NULL)
				{
					int t = 0;
					while (((datablock + i)->directory + t)->name != NULL)
					{
						free(((datablock + i)->directory + t)->name);
						((datablock + i)->directory + t)->name = NULL;
					}
					free((datablock + i)->directory);
					(datablock + i)->directory = NULL;
				}
				else if ((datablock + i)->indirect != NULL)
				{ 
					free((datablock + i)->indirect);
					(datablock + i)->indirect = NULL;
				}
			}
			free(datablock);
			datablock = NULL;
				
			//setsuperblock();
			//setinode();
			//setdatablock();
			//setroot();
			filesave();
			printf("파일시스템을 다시 만들었습니다.\n");
			putchar(' ');
			return 0;
		}
		else if (c == 'n')
			return 0;
	}
}
void myls(char* f1)
{
	int in, in1, da, da1, num;
	
		in = pwd->i;
		in1 = back->i;
		da = *((inode + in)->datablockInfo);
		da1 = *((inode + in1)->datablockInfo);
		int count = 0;
		while ((((datablock + da) -> directory + count) ->name) != NULL)
		{
			count++;
		}
		num = in;
		printf("%d/%d/%d %d:%d:%d ", (inode + num)->t.year, (inode + num)->t.mon, (inode + num)->t.day, (inode + num)->t.hour, (inode + num)->t.min, (inode + num)->t.sec);
		printf(" directory ");
		printf(" %d ", in +1);
		printf(" %4d byte ", (inode + num)->size);
		printf(" . ");
		printf("\n");
		num = in1;
		printf("%d/%d/%d %d:%d:%d ", (inode + num)->t.year, (inode + num)->t.mon, (inode + num)->t.day, (inode + num)->t.hour, (inode + num)->t.min, (inode + num)->t.sec);
		printf(" directory ");
		printf(" %d ", in1+1);
		printf(" %4d byte ", (inode + num)->size);
		printf(" .. ");
		printf("\n");

		char* tmp;
		tmp = (char*)malloc(sizeof(char) * 10);
		
		qsort((datablock + da)->directory, count, sizeof(struct directory_datablock), sort);
		for (int i = 0; i < count; i++) {
			num = ((datablock + da) -> directory + i)->num;
			if (((datablock + da)->directory + i)->name != NULL)
			{
				printf("%d/%d/%d %d:%d:%d ", (inode + num)->t.year, (inode + num)->t.mon, (inode + num)->t.day, (inode + num)->t.hour, (inode + num)->t.min, (inode + num)->t.sec);
				if ((inode + num)->type == 0)
					printf(" file      ");
				else if ((inode + num)->type == 1)
					printf(" directory ");
				printf(" %d ", num + 1);
				printf(" %4d byte ", (inode + num)->size);
				printf(" %s ", (inode + num)->name);
				printf("\n");
			}

		}
		free(tmp);

}
void myshowfile(char* num2, char* num3, char* f1)
{
	if (*num2 == '\0' || *num3 == '\0' || *f1 == '\0')
	{
		printf("인자를 입력해주세요\n");
		return;
	}
	int num, num1;
	num = atoi(num2);
	num1 = atoi(num3);
	int in ,da, in1, size , block, block_left;

	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0, count = 0;
	while ((((datablock + da)->directory + i)->name) != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			in1 = ((datablock + da)->directory + i)->num;
			count++;
		}
		i++;
	}
	if (count == 0)
	{
		printf("myshowfile: %s: no such file\n", f1);
		return;
	}
	size = (inode + in1)->size;
	block = size / 256;
	block_left = size % 256;
	char* tmp, *tmp1;
	tmp = (char*)malloc(sizeof(char) * (size + 1));
	tmp1 = (char*)malloc(sizeof(char) * (num1 - num +2));
	*tmp = '\0';
	*tmp1 = '\0';
	if (block == 0)
	{
		strcat(tmp, (datablock + *((inode + in1)->datablockInfo))->data);


	}
	else if ((block > 0 && block < 8) || (block == 8 && block_left == 0))
	{
		for (int i = 0; i < block; i++)
		{
			strcat(tmp, (datablock + *((inode + in1)->datablockInfo + i))->data);
		}
		if (block_left != 0)
		{
			strcat(tmp, (datablock + *((inode + in1)->datablockInfo + block))->data);
		}

	}
	else if (block > 8 || (block == 8 && block_left != 0))
	{
		int indir = (inode + in1)->indirectblock;
		for (int i = 0; i < 8; i++)
		{
			strcat(tmp, (datablock + *((inode + in1)->datablockInfo + i))->data);
		}
		for (int i = 0; i < block - 8; i++)
		{
			strcat(tmp, (datablock + *((datablock + indir)->indirect + i))->data);
		}
		if (block_left != 0)
		{
			strcat(tmp, (datablock + *((datablock + indir)->indirect + block -8))->data);
		}
	}

	strncpy(tmp1, tmp + num -1, num1 - num + 1);
	*(tmp1 + num1 - num + 1) = '\0';
	printf("\n%s\n", tmp1);

}
int mycat(char *f1)
{
	if(*f1 == '\0')
	{
		printf("파일 이름을 입력해주세요\n");
		return 0;
	}
	int in, da, n, size, block, block_left, count = 0;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			n = ((datablock + da)->directory + i)->num;
			count++;
			break;
		}
		i++;
	}
	if (count != 0)
	{
		size = (inode + n)->size;
		block = size / 256;
		block_left = size % 256;
		if (block == 0)
		{
			printf("\n%s\n", (datablock + *((inode + n)->datablockInfo))->data);
			return 0;
		}
		else if ((block > 0 && block < 8) || (block == 8 && block_left == 0))
		{
			for (int q = 0; q < block; q++)
			{
				printf("%s", (datablock + *((inode + n)->datablockInfo + q))->data);

			}
			if (block_left != 0)
				printf("%s", (datablock + *((inode + n)->datablockInfo + block))->data);
			printf("\n");
			return 0;
		}
		else if (block >= 8 && block_left != 0)
		{
			int indir = (inode + n)->indirectblock;

			for (int q = 0; q < 8; q++)
			{
				printf("%s", (datablock + *((inode + n)->datablockInfo + q))->data);
			}
			for (int q = 0; q < block - 8; q++)
			{
				printf("%s", (datablock + *((datablock + indir)->indirect + q))->data);

			}
			printf("%s", (datablock + *((datablock + indir)->indirect + block -8))->data);

			printf("\n");
			return 0;

		}



	}
	else {
		printf("mycat: %s: no such file\n", f1);
		return 0;
	}
	
}

int mycpfrom(char *f1, char *f2)
{
	FILE *f;
	char *tmp;
	int size, num1, num2, block, block_left, in, da;
	if(*f1 == '\0' || *f2 == '\0')
	{
		printf("파일 이름을 입력해주세요\n ");
		return 0;

	}
	if((f = fopen(f1, "r")) == NULL)
	{
		printf("파일이 없습니다");
		return -1;
	}
	else{
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		rewind(f);
		tmp = (char *)malloc(sizeof(char)*256);
		block = size / 256;
		block_left = size % 256;
		in = pwd->i;
		da = *((inode + in) -> datablockInfo);
		int i = 0;
		while (((datablock +da) -> directory + i) ->name != NULL)
		{
			if (strcmp(((datablock + da)->directory + i)->name, f2) == 0)
			{
				printf("이미 있는 이름입니다.\n");
				return 0;
			}
			i++;
		}
		num1 = find_freeinode();
		(inode + num1) -> type = 0;
		strcpy((inode + num1) -> name, f2);
		(inode + num1) -> size = size;
		set_time(inode + num1);
		
		(inode + in)->size += 8;

		((datablock + da) -> directory + i)->name = (char*)malloc(sizeof(char) * 7);
		strcpy(((datablock + da) -> directory + i)->name, f2);
		((datablock + da)->directory + i + 1)->name = NULL;
		((datablock + da) -> directory + i)->num = num1;
		if(block == 0)
		{
			fread(tmp,size, 1, f);
			num2 = find_freedatablock();
			*((inode + num1) -> datablockInfo) = num2;
			(datablock + num2) -> data = (char *)malloc(sizeof(char)*256);
			strcpy((datablock + num2) -> data, tmp);
		}
		else if((block >0 && block < 8) || (block == 8 && block_left == 0))
		{

			for(int i = 0; i<block; i++)
			{
				fread(tmp, 256, 1, f);
				num2 = find_freedatablock();
				*(((inode + num1) -> datablockInfo) + i) = num2;
				(datablock + num2) -> data = (char *)malloc(sizeof(char)*256);
				strcpy((datablock + num2) -> data, tmp);
			}
			if(block_left != 0)
			{
				

				fread(tmp, block_left, 1, f);
				num2 = find_freedatablock();
				*(((inode + num1) -> datablockInfo) + block ) = num2;
				(datablock + num2) -> data = (char *)malloc(sizeof(char)* 256);
				strncpy((datablock + num2) -> data, tmp, block_left);
			}
		}
		else if (block > 8 || (block == 8 && block_left != 0))
		{

			int indir = find_freedatablock();
			int t = 0;
			(inode + num1) -> indirectblock = indir;
			(datablock + indir) -> indirect = (int *)malloc(sizeof(int)*(block -8));
			for(int i = 0; i<block; i++)
			{
				if(i < 8){
					fread(tmp, 256, 1, f);
					num2 = find_freedatablock();
					*((inode + num1) -> datablockInfo + i) = num2;
					(datablock + num2) -> data = (char *)malloc(sizeof(char)*256);
					strcpy((datablock + num2) -> data, tmp);
				}
				else
				{	
					fread(tmp, 256, 1, f);
					num2 = find_freedatablock();
					*((datablock + indir)->indirect + t) = num2;
					(datablock + num2)->data = (char*)malloc(sizeof(char) * 256);
					strcpy((datablock + num2)->data, tmp);
					t++;
				}
			}
			if(block_left != 0)
			{
				
				fread(tmp, block_left, 1, f);
				num2 = find_freedatablock();
				*((datablock + indir)->indirect + t) = num2;
				(datablock + num2)->data = (char*)malloc(sizeof(char) * 256);
				strncpy((datablock + num2)->data, tmp, block_left);
			}	
		}		
		fclose(f);
	}
}
int mycpto(char *f1, char *f2)
{
	FILE *f;
	int block, size, block_left, in, da, n;
	if(*f1 == '\0' || *f2 == '\0')
	{
		printf("파일 이름을 입력해주세요\n ");
		return 0;

	}
	if(access(f2,F_OK) != -1)
	{
		printf("이미 있는 파일입니다.\n");
		return 0;
	}
	else{
		in = pwd->i;
		da = *((inode + in)->datablockInfo);
		int i = 0,count = 0;
		while (((datablock + da)->directory + i)->name != NULL)
		{
			if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
			{
				n = ((datablock + da)->directory + i)->num;
				count++;
			}
			i++;
		}
		if (count == 0)
		{
			printf("mycpto: %s: no such file\n", f1);
			return 0;
		}

		else 
		{
			f = fopen(f2, "w");
			size = (inode + n)->size;
			block = size / 256;
			block_left = size % 256;
			if (block == 0)
			{
				fwrite((datablock + *((inode + n)->datablockInfo))->data, size, 1, f);
				fclose(f);
				return 0;
			}
			else if ((block > 0 && block < 8) || (block == 8 && block_left == 0))
			{
				for (int q = 0; q < block; q++)
				{
					fwrite((datablock + *(((inode + n)->datablockInfo) + q))->data, 256, 1, f);

				}
				if (block_left != 0)
					fwrite((datablock + *(((inode + n)->datablockInfo) + block))->data, block_left, 1, f);

				fclose(f);
				return 0;


			}
			else if (block > 8 || (block == 8 && block_left != 0))
			{
				int indir = (inode + n)->indirectblock;

				for (int q = 0; q < 8; q++)
				{
					fwrite((datablock + *(((inode + n)->datablockInfo) + q))->data, 256, 1, f);
				}
				for (int q = 0; q < block - 8; q++)
				{
					fwrite((datablock + *((datablock + indir)->indirect + q))->data, 256, 1, f);

				}
				if (block_left != 0)
					fwrite((datablock + *((datablock + indir)->indirect + block - 8))->data, block_left, 1, f);

				fclose(f);
				return 0;
			}




		}
	}
}
void mypwd()
{
	print_file(pwd);
	printf("\n");
}
void mycd(char* f1)
{
	if (strcmp(f1, ".." ) == 0)
	{
		pwd1 = back_file(pwd1);
	}
	else 
	{
		int in, da, tmp, count;
		count = 0;
		in = pwd->i;
		da = *((inode + in)->datablockInfo);
		int i = 0;
		while (((datablock + da)->directory + i)->name != NULL)
		{
			if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
			{
				tmp = ((datablock + da)->directory + i)->num;
				count++;
			}
			i++;
		}
		if (count == 0)
		{
			printf("mycd: %s: no such directory \n", f1);
			return;
		}
		
		pwd1 = next_file(pwd1, f1, tmp);
		
		
	}
}
void mycp(char* f1, char* f2)
{
	if (*f1 == '\0' || *f2 == '\0')
	{
		printf("error: name missing\n");
		return;
	}
	int in, da, tmp, count;
	count = 0;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			tmp = ((datablock + da)->directory + i)->num;
			count++;
		}
		i++;
	}
	if (count == 0)
	{
		printf("mycp: %s: no such directory \n", f1);
		return;
	}
	int size, block, block_left, num2;
	size = (inode + tmp)->size;
	block = size / 256;
	block_left = size % 256;
	int num = find_freeinode();
	(inode + num)->type = 0;
	(inode + num)->size = size;
	strcpy((inode + num)->name, f2);
	set_time(inode + num);
	if ( block < 8 || (block == 8 && block_left == 0))
	{
		for (int q = 0; q < block; q++)
		{
			num2 = find_freedatablock();
			(datablock + num2)->data = (char*)malloc(sizeof(char) * 256);
			*((inode + num)->datablockInfo + q) = num2;
			strcpy((datablock + num2)->data, (datablock + *((inode + tmp)->datablockInfo +q))->data);
		}
		if (block_left != 0)
		{
			num2 = find_freedatablock();
			(datablock + num2)->data = (char*)malloc(sizeof(char) * block_left);
			*((inode + num)->datablockInfo + block) = num2;
			strcpy((datablock + num2)->data, (datablock + *((inode + tmp)->datablockInfo + block))->data);
		}

	}
	else if (block > 8 || (block == 8 && block_left != 0))
	{
		int indir = find_freedatablock();
		int indir1 = (inode + tmp)->indirectblock;
		(inode + num)->indirectblock = indir;
		for (int q = 0; q < 8; q++)
		{
			num2 = find_freedatablock();
			(datablock + num2)->data = (char*)malloc(sizeof(char) * 256);
			*((inode + num)->datablockInfo + q) = num2;
			strcpy((datablock + num2)->data, (datablock + *((inode + tmp)->datablockInfo + q))->data);
		}
		(datablock + indir)->indirect = (int*)malloc(sizeof(int) * (block - 7));
		for (int q = 0; q < block - 8; q++)
		{
			num2 = find_freedatablock();
			(datablock + num2)-> data = (char*)malloc(sizeof(char) * 256);
			*((datablock + indir)->indirect + q) = num2;
			strcpy((datablock + num2)->data, (datablock + *((datablock + indir1) -> indirect + q))->data);
		}
		if (block_left != 0)
		{
			num2 = find_freedatablock();
			(datablock + num2)->data = (char*)malloc(sizeof(char) * 256);
			*((datablock + indir)->indirect + block - 8) = num2;
			strncpy((datablock + num2)->data, (datablock + *((datablock + indir1)->indirect + block-8))->data, block_left);
		}
		
	}	
	(inode + in)->size += 8;
	((datablock + da)->directory + i)->name = (char*)malloc(sizeof(char) * 7);
	strcpy(((datablock + da)->directory + i)->name, f2);
	((datablock + da)->directory + i + 1)->name = NULL;
	((datablock + da)->directory + i)->num = num;
	
}
void mymv(char* f1, char* f2)
{
	if (*f1 == '\0' || *f2 == '\0')
	{
		printf("error: name missing\n");
		return;
	}
	int in, da, tmp, count;
	count = 0;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0, t;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			tmp = ((datablock + da)->directory + i)->num;
			t = i;
			count++;
		}
		i++;
	}
	if (count == 0)
	{
		printf("mymv: %s: no such directory \n", f1);
		return;
	}
	strcpy((inode + tmp)->name, f2);
	strcpy(((datablock + da)->directory + t)->name, f2);

}
int mymkdir(char* f1)
{
	if (*f1 == '\0')
	{
		printf("디렉터리 이릅을 입력해주세요.\n");
		return 0;
	}
	int in, da, num1, num2;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			printf("이미 있는 이름입니다.\n");
			return 0;
		}
		i++;
	}
	num1 = find_freeinode();
	num2 = find_freedatablock();
	(inode + num1)->type = 1;
	strcpy((inode + num1)->name, f1);
	(inode + num1)->size = 16;
	set_time(inode + num1);
	*((inode + num1)->datablockInfo) = num2;
	(datablock + num2)-> directory = (struct directory_datablock*)malloc(sizeof(struct directory_datablock) * 16);
	
	(datablock + num2)->directory->name = NULL;
	(inode + in)->size += 8;
	
	((datablock + da)->directory + i)->name = (char *)malloc(sizeof(char) *7);
	strcpy(((datablock + da)->directory + i)->name, f1);
	

	((datablock + da)->directory + i)->num = num1;
	return 0;
}
int myrmdir(char* f1)
{
	if (*f1 == '\0')
	{
		printf("디렉터리 이릅을 입력해주세요.\n");
		return 0;
	}
	int i = 0;
	int t = 0, count = 0;
	int in, da, num1, num2;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			count++;
		}
		i++;
	}
	if (count == 0)
	{
		printf("해당 디렉터리가 없습니다.\n");
		return 0;
	}
	for (int q = 0; q<i;q++)
	{
		if (strcmp(((datablock + da)->directory + q)->name, f1) == 0)
		{
			num1 = ((datablock + da)->directory + q)->num;
			num2 = *((inode + num1)->datablockInfo);
			t = q;
		}
	}
	if ((datablock + num2)->directory->name != NULL)
	{
		printf(" %s 디렉터리 안에 파일이 있습니다.\n", f1);
		return 0;
	}

	if ((t == 0 && i == 1) || t == i - 1)
	{
		free(((datablock + da)->directory + t)->name);
		((datablock + da)->directory + t)->name = NULL;
		((datablock + da)->directory + t)->num = -1;
	}
	else if(t>0 && t < i){
		strcpy(((datablock + da)->directory + t)->name, ((datablock + da)->directory + i - 1)->name);
		((datablock + da)->directory + t)->num = ((datablock + da)->directory + i - 1)->num;
		free(((datablock + da)->directory + i-1)->name);
		((datablock + da)->directory + i -1)->name = NULL;

		((datablock + da)->directory + i-1)->num = -1;
	}
	(inode + in)->size -= 8;

	for (int j = 0; j < 8; j++)
		*((inode + num1)->datablockInfo + j) = -1;
	(inode + num1)->indirectblock = 0;
	free((datablock + num2)->directory);
	free_inode(num1);
	free_datablock(num2);
}
void myrm(char *f1)
{
	if (*f1 == '\0')
	{
		printf("파일 이릅을 입력해주세요.\n");
		return ;
	}
	int count = 0;
	int in, da, num1, num2;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0, t=0;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			count++;
		}
		i++;
	}
	if (count == 0)
	{
		printf("해당 파일이 없습니다.\n");
		return ;
	}
	for (int q = 0; q < i; q++)
	{
		if (strcmp(((datablock + da)->directory + q)->name, f1) == 0)
		{
			num1 = ((datablock + da)->directory + q)->num;
			t = q;
		}
	}
	if ((t == 0 && i == 1) || t == i - 1)
	{
		free(((datablock + da)->directory + t)->name);
		((datablock + da)->directory + t)->name = NULL;
		((datablock + da)->directory + t)->num = -1;
	}
	else if (t > 0 && t < i) {
		strcpy(((datablock + da)->directory + t)->name, ((datablock + da)->directory + i - 1)->name);
		((datablock + da)->directory + t)->num = ((datablock + da)->directory + i - 1)->num;
		free(((datablock + da)->directory + i - 1)->name);
		((datablock + da)->directory + i - 1)->name = NULL;
		((datablock + da)->directory + i - 1)->num = -1;
	}
	int size,block, block_left;
	size = (inode + num1)->size;
	block = size / 256;
	block_left = size % 256;
	if (block < 8 || (block == 8 && block_left == 0))
	{
		for (int i = 0; i < block; i++)
		{
			num2 = *((inode + num1)->datablockInfo + i);
			free((datablock + num2)->data);
			free_datablock(num2);

		}
		if (block_left != 0)
		{
			num2 = *((inode + num1)->datablockInfo + block);
			free((datablock + num2)->data);
			free_datablock(num2);
		}
	}
	else if(block > 8 || (block == 8 && block_left != 0))
	{
		int indir = (inode + num1)->indirectblock;
			for (int i = 0; i < 8; i++)
		{
			num2 = *((inode + num1)->datablockInfo + i);
			free((datablock + num2)->data);
			free_datablock(num2);
		}
		for (int i = 0; i < block - 8; i++)
		{
			num2 = *((datablock + indir)->indirect + i);
			free((datablock + num2)->data);
			free_datablock(num2);
		}
		if (block_left != 0)
		{
			num2 = *((datablock + indir)->indirect + block - 8);
			free((datablock + num2)->data);
			free_datablock(num2);
		}
		free_datablock(indir);
	}
	free_inode(num1);
	for (int j = 0; j < 8; j++)
		*((inode + num1)->datablockInfo + j) = -1;
	(inode + num1)->indirectblock = 0;
	(inode + in)->size -= 8;

}
void mytouch(char* f1)
{
	if (*f1 == '\0')
	{
		printf("이름을 입력해주세요.\n");
		return ;
	}
	int count = 0;
	int in, da, num1, num2;
	in = pwd->i;
	da = *((inode + in)->datablockInfo);
	int i = 0;
	while (((datablock + da)->directory + i)->name != NULL)
	{
		if (strcmp(((datablock + da)->directory + i)->name, f1) == 0)
		{
			num1 = ((datablock + da)->directory + i)->num;
			count++;
		}
		i++;
	}
	if (count == 0)
	{

		num1 = find_freeinode();
		num2 = find_freedatablock();
		(inode + num1)->type = 0;
		strcpy((inode + num1)->name, f1);
		(inode + num1)->size = 0;
		set_time(inode + num1);
		*((inode + num1)->datablockInfo) = num2;
		((datablock + da)->directory + i)->name = (char*)malloc(sizeof(char) * 7);
		strcpy(((datablock + da)->directory + i)->name, f1);
		((datablock + da)->directory + i)->num = num1;
		(inode + in)->size += 8;
	}
	else {
		set_time(inode + num1);
	}
	

}
void mydatablock(char *num1)
{
	int num = atoi(num1);
	num--;
	if ((datablock + num)->data != NULL)
	{
		printf("%s\n", (datablock + num)->data);
	}
	else if ((datablock + num)->directory != NULL)
	{
		int t = 0;
		while (((datablock + num)->directory + t)->name != NULL)
		{
			printf( "Inode : %d name : %s\n", ((datablock + num)->directory + t)->num +1, ((datablock + num)->directory + t)->name);
			t++;
		}
	}
	else if ((datablock + num)->indirect != NULL)
	{
		int i = 0;
		if (((datablock + num)->indirect + i) != NULL)
		{
			printf("datablock : %d\n", *((datablock + num)->indirect +i) +1);
			i++;
		}
	}
}

void myinode(char *num1)
{
	if (*num1 == '\0')
		printf("번호를 입력해주세요.");
	else 
	{
		int num = atoi(num1);
		num--;
		if (check_inode(num) == 0)
		{
			printf("사용하지 않는 inode 입니다.\n");
			return;
		}
		if ((inode + num)->type == 0)
			printf("종류 : 파일\n");
		else if ((inode + num)->type == 1)
			printf("종류 : 디렉터리\n");
		printf("생성일자 : %d/%d/%d %d:%d:%d\n", (inode + num)->t.year, (inode + num)->t.mon, (inode + num)->t.day, (inode + num)->t.hour, (inode + num)->t.min, (inode + num)->t.sec);
		printf("크기 : %d\n", (inode + num)->size);
		printf("직접 블록 목록 : \n");
		int numb = 1;
		for (int i = 0; i < 8; i++) {
			if (*((inode + num)->datablockInfo + i) >=0 && *((inode + num)->datablockInfo + i) <= 256) {
				printf("  #%d 직접 데이터 블록 : %d\n", numb, *((inode + num)->datablockInfo + i)+1);
				numb++;
			}
		}
		printf("간접 블록 번호 : ");
		if ((inode + num)->indirectblock != 0)
			printf("%d\n", (inode + num)->indirectblock +1);
	}
	printf("\n");
}
void mystate(){
	int usedinode=0;
	int useddatablock=0;
	printf("Inode state :\n");
	printf("Total : 128\n");
	
		unsigned int tmp = 1;
		tmp <<= 31;
		for (int i = 0; i < 4; i++) {
			unsigned int t = *(sb.inodeList + i);
			for (int j = 0; j < 32; j++) {
				if ((t & tmp) != 0)
				{
					t <<= 1;
					usedinode++;
				}
			}

		}

		printf("Used : %d\n",usedinode);
		printf("Available : %d\n",128-usedinode);
		printf("inode Map : ");
		for (int i = 0; i < 4; i++)
			bit_print(*(sb.inodeList + i));
	
	printf("\n\n");


	printf("Data Block state :\n");
	printf("Total : 256 blocks / 65536 byte\n");
	tmp = 1;
	tmp <<= 31;
	for (int i = 0; i < 8; i++) {
		unsigned int t = *(sb.datablockList + i);
		for (int j = 0; j < 32; j++) {
			if ((t & tmp) != 0)
			{
				t <<= 1;
				useddatablock++;
			}
		}

	}
	printf("Used : %d / %d byte\n",useddatablock,useddatablock*256);
	printf("Available : %d / %d byte\n",256-useddatablock,useddatablock*256);

	
	printf("datablock Map : ");
	for (int i = 0; i < 8; i++)
		bit_print(*(sb.datablockList + i));
	printf("\n");
}
void set_time(struct inode *inode)
{
	time_t tim;
	struct tm *t;

	tim = time(NULL);
	t = localtime(&tim);
	inode -> t.year = t -> tm_year + 1900;
	inode -> t.mon = t -> tm_mon + 1;
	inode -> t.day = t -> tm_mday;
	inode -> t.hour = t -> tm_hour;
	inode -> t.min = t -> tm_min;
	inode -> t.sec = t -> tm_sec;

}
void bit_print(unsigned int a)

{
	int n = sizeof(int) * 8;
	unsigned mask = 1 << (n - 1);
	for (int i = 1; i <= n; ++i) {
		putchar(((a & mask) == 0) ? '0' : '1');
		mask >>= 1;
		if (i % 8 == 0 && i < n)
			putchar(' ');

	}
	printf("\n");
	putchar(' ');
}
NAME file_list(NAME head, char* s, int i) {
	if (*s == '\0')
		return NULL;
	else {
		head = (file *)malloc(sizeof(file));
		head->d = (char *)malloc(sizeof(30));
		strcpy(head->d, s);
		head->i = i;
		head->next == NULL;
	}
	return head;
}
NAME next_file(NAME head, char* s, int i) {
	if (head->next == NULL)
		head->next = file_list(head, s, i);
	else
		next_file(head->next, s, i);
	return head;
}
NAME back_file(NAME head) {
	
	if ((head ->next == NULL && head -> i == 0) || head->next->next == NULL)
	{
		free(head->next->d);
		head->next = NULL;
	}
	else
		back_file(head->next);
	return head;
}
NAME back_directory(NAME head) {
	if ((head->next == NULL && head->i == 0) )
	{
		return head;
	}
	else if (head->next->next == NULL)
	{
		return head;
	}
	else
		back_directory(head->next);
	
}
NAME now_directory(NAME head)
{
	if (head->next == NULL)
		return head;
	else
		now_directory(head->next);
	
}
void print_file(NAME head) {
	if (head == NULL)
		return;
	else {
		if (*head->d =='/')
		{
			if(head -> next != NULL)
				print_file(head->next);
			else {
				printf("%s", head->d);
				print_file(head->next);
			}
		}
		else {
			printf("/%s", head->d);
			print_file(head->next);
		}

	}
}

void setroot() {
	
	char* ch = "/";
	pwd = (file *)malloc(sizeof(file));
	pwd1 = file_list(pwd1, ch, 0);

	root = pwd1;
	inode->type = 1;
	strcpy(inode->name, "/");
	inode->size = 16;
	set_time(inode);
	*(inode->datablockInfo) = 0;



}
void filesave() {
	FILE* f;
	f = fopen("myfs", "wb");
	fwrite(sb.inodeList, sizeof(unsigned int) * 4, 1, f);
	fwrite(sb.datablockList, sizeof(unsigned int) * 8, 1, f);

	fclose(f);
}
void fileread() {
	FILE* f;
	f = fopen("myfs", "rb");

	fread(sb.inodeList, sizeof(unsigned int) * 4, 1, f);
	fread(sb.datablockList, sizeof(unsigned int) * 8, 1, f);
	fclose(f);
}
void setsuperblock()
{
	sb.inodeList = (unsigned*)malloc(sizeof(unsigned int) * 4);
	sb.datablockList = (unsigned*)malloc(sizeof(unsigned int) * 8);
	unsigned mask = 1;
	mask <<= 31;
	*sb.inodeList = mask;
	*sb.datablockList = mask;
	for (int i = 1; i < 4; i++) {
		*(sb.inodeList + i) = 0;
	}
	for (int i = 1; i < 8; i++) {
		*(sb.datablockList + i) = 0;
	}

}
void setinode() {

	inode = (struct inode*)malloc(sizeof(inode) * 128);
	for (int i = 0; i < 128; i++) {
		(inode + i)->name = (char*)malloc(sizeof(char) * 7);
		(inode + i)->datablockInfo = (int*)malloc(sizeof(int) * 8);
		for (int j = 0; j < 8; j++)
			*((inode + i)->datablockInfo + j) = -1;
	}
	
}
void setdatablock() {

	datablock = (struct datablock*)malloc(sizeof(struct datablock) * 256);
	for (int i = 0; i < 256; i++)
	{
		(datablock + i)->data = NULL;
		(datablock + i)->directory = NULL;
		(datablock + i)->indirect = NULL;
	}
	datablock->directory = (struct directory_datablock*)malloc(sizeof(struct directory_datablock) * 16);

}
void superblockprint() {
	printf("inode List : ");
	for (int i = 0; i < 4; i++)
		bit_print(*(sb.inodeList + i));
	printf("\n\ndatablock List : ");
	for (int i = 0; i < 8; i++)
		bit_print(*(sb.datablockList + i));

}
int find_freeinode() {
	unsigned int tmp = 1;
	tmp <<= 31;
	int n = 0;
	for (int i = 0; i < 4; i++) {
		unsigned int t = *(sb.inodeList + i);
		for (int j = 0; j < 32; j++) {
			if ((t & tmp) == 0)
			{
				tmp >>= n;
				*(sb.inodeList + i) |= tmp;
				return n;
			}
			else {

				t <<= 1;
				n++;
			}
		}

	}
	return -1;


}
int find_freedatablock() {
	unsigned int tmp = 1;
	tmp <<= 31;
	int n = 0;
	for (int i = 0; i < 8; i++) {
		unsigned int t = *(sb.datablockList + i);
		for (int j = 0; j < 32; j++) {
			if ((t & tmp) == 0)
			{
				tmp >>= n;
				*(sb.datablockList + i) |= tmp;
				return n;
			}
			else {

				t <<= 1;
				n++;
			}
		}

	}
	return -1;


}
void free_inode(int num) {
	unsigned int tmp = 1;
	tmp <<= 31;
	int block;
	int left;
	block = num / 32;
	left = num % 32;
	tmp >>= left;
	*(sb.inodeList + block) ^= tmp;
	
}
void free_datablock(int num) {
	unsigned int tmp = 1;
	tmp <<= 31;
	int block;
	int left;
	block = num / 32;
	left = num % 32;
	tmp >>= left;
	*(sb.datablockList + block) ^= tmp;
}
int check_inode(int num) {
	unsigned int tmp = 1;
	tmp <<= 31;
	int block;
	int left;
	block = num / 32;
	left = num % 32;
	
	unsigned int t = *(sb.inodeList + block);
	t <<= left;
	if ((t & tmp) == 0)
		return 0;
	else
		return 1;
}
int check_datablock(int num) {
	unsigned int tmp = 1;
	tmp <<= 31;
	int block;
	int left;
	block = num / 32;
	left = num % 32;

	unsigned int t = *(sb.datablockList + block);
	t <<= left;
	if ((t & tmp) == 0)
		return 0;
	else
		return 1;
}
int sort(const void* p, const void* q)
{
	const struct directory_datablock* a = (const struct directory_datablock*)p;
	const struct directory_datablock* b = (const struct directory_datablock*)q;
	return strcmp(a -> name, b-> name);
}
