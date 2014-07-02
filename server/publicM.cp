#include "publicsrv.h"
extern int errno;

/* 金额转换类 */
/******************************************************************************/
/* name:     Mchartocase 						      */
/* writer:              						      */
/* modi:     zhaohao           						      */
/* date:     2001.11.1       						      */
/* function: 将字符c对应的数字转换成汉字的壹、贰等,然后附加在dest后           */
/******************************************************************************/
void Mchartocase(char *dest,char c)
{
	switch (c) {
		case '1':
			strcat(dest,"壹");
			break;
		case '2':
			strcat(dest,"贰");
			break;
		case '3':
			strcat(dest,"叁");
			break;
		case '4':
			strcat(dest,"肆");
			break;
		case '5':
			strcat(dest,"伍");
			break;
		case '6':
			strcat(dest,"陆");
			break;
		case '7':
			strcat(dest,"柒");
			break;
		case '8':
			strcat(dest,"捌");
			break;
		case '9':
			strcat(dest,"玖");
			break;
		case '0':
			strcat(dest,"零");
			break;
		default :
			break;
	}
	return;
}

/******************************************************************************/
/* name:     Mchgmoney	 						      */
/* writer:              						      */
/* modi:     zhaohao           						      */
/* date:     2001.11.1       						      */
/* function: 将src截取后(去除其中的逗号)，变成双精度数ddest，然后再转换为     */
/*           中文形式cdest  						      */
/******************************************************************************/
void Mchgmoney(char *src,char *cdest,char *ddest)
{
	int i=0, j=0;
	char tmpstr[150];

	memset(tmpstr, 0, sizeof(tmpstr));
	for(i=0;i<strlen(src);i++)
	{
		if (src[i] != ',')
		{
			ddest[j]=src[i];
			j++;
			ddest[j]=0;
		}
	}

	Mdoubletocase(atof(ddest),tmpstr);
	for(i=0;i<strlen(tmpstr);i++)
	{
		cdest[i]=tmpstr[i];
	}
	cdest[i]=0;

	return ;
}


/******************************************************************************/
/* name:     Mdoubletocase						      */
/* writer:   Unknown           						      */
/* modi:     zhaohao           						      */
/* date:     2001.11.1       						      */
/* function: 将双精度的数转换为大写的金额				      */
/******************************************************************************/
int Mdoubletocase(double sum,char *ccase)
{
   int i;
   int count;
   char c;
   char *trace;
   char *tail;
   char s[150];
   char upcase[150];

   sprintf(upcase,"%.2lf",sum);
   s[0]='\0';
   i=-1;
   while (c=upcase[++i])
      if ((c>'9' || c<'0') && c!='\0' && c!='.')
         return -1;

   i=-1;
   while (upcase[++i]=='0');
   if (upcase[i]=='\0' || upcase[i]=='.')
      strcat(s,"零元");

   if (upcase[i]=='\0')
   {
	strcat(s,"整");
 	strcpy(upcase,s);
      	strcpy(ccase,upcase);
      	return 0;
   }
   trace=upcase+i;

   for (i=0;;i++)
       if (trace[i]=='.' || trace[i]=='\0')
          break;

   if (i>8)
      return -2;
   count=i-1;
   tail=s;
   for (i=0;i<=count;i++)
   {
      if (strncmp(tail-2,"零",2)!=0 || trace[i]!='0')
/*This means the tail of the string is not "零"*/
      {

         Mchartocase(s,trace[i]);
         tail+=2;
      }
      if (trace[i]!='0' )
      switch (count-i){
         case 0:strcat(s,"元");
                tail+=2;
                break;
         case 1:strcat(s,"拾");
                tail+=2;
                break;
         case 2:strcat(s,"佰");
                tail+=2;
                break;
         case 3:strcat(s,"仟");
                tail+=2;
                break;
         case 4:strcat(s,"万");
                tail+=2;
                break;
         case 5:strcat(s,"拾");
                tail+=2;
                break;
         case 6:strcat(s,"佰");
		tail+=2;
                break;
         case 7:strcat(s,"仟");
                tail+=2;
                break;
         default:break;
      } else if((count-i)==4)
             {
                if (strncmp(tail-2,"零",2)==0)
                {
                   (*(tail-2))='\0';
                   tail-=2;
                }
                strcat(s,"万");
                tail+=2;
             }
             else if((count-i)==0)
             {
                if (strncmp(tail-2,"零",2)==0)
                {
                   (*(tail-2))='\0';
                   tail-=2;

                }
                strcat(s,"元");
                tail+=2;
             }
   } /* -- of for -- */
   if (trace[count+1]=='\0')
   {
      strcat(s,"整");
      tail+=2;
      strcpy(upcase,s);
      strcpy(ccase,upcase);
      return 0;
   }
   i=count+2;

   if (trace[i]=='0' && trace[i+1]!='0' && trace[i+1]!='\0')
   {
      strcat(s,"零");
      Mchartocase(s,trace[i+1]);
      strcat(s,"分");
      strcpy(upcase,s);
      strcpy(ccase,upcase);
      return 0;
   }
   if (trace[i]=='0' && trace[i+1]=='0')
   {
      strcat(s,"整");
      tail+=2;
      strcpy(upcase,s);
      strcpy(ccase,upcase);
      return 0;
   }
   else {
      if (trace[i]!='0' && trace[i]!='\0')
      {
         if (trace[i-2]=='0')
         strcat(s,"零");
         Mchartocase(s,trace[i]);
         tail+=2;
         strcat(s,"角");
         tail+=2;
         if (trace[i+1]=='0'|| trace[i+1]=='\0')
         {
            strcat(s,"整");
            tail+=2;
            strcpy(upcase,s);
            strcpy(ccase,upcase);
            return 0;
	}
      } else if (trace[i]=='\0')
             {
                strcat(s,"整");
                tail+=2;
                strcpy(upcase,s);
                strcpy(ccase,upcase);
                return 0;
             }
      if (trace[i+1]!='0' && trace[i+1]!='\0')
      {
         Mchartocase(s,trace[i+1]);
         strcat(s,"分");
        /* strcat(s,"整"); */
         tail+=6;
      }
   }
   strcpy (upcase,s);
   strcpy(ccase,upcase);
   return 0;
}



/*****时间转换函数**********
***f_time: 界面上的时间值***
***r_year: 返回年值      ***
***r_month:返回月值      ***
***r_day:  返回日值      ***/

void chg_time(f_time,r_year,r_month,r_day)
char* f_time;
char* r_year;
char* r_month;
char* r_day;
{
     int  i;
     char cur_time[20];
     char cur_year[5],cur_month[3],cur_day[3];
   
     r_month[0]=f_time[0];
     r_month[1]=f_time[1];
     r_month[2]=0;
     if (r_month[0] == '0')
         r_month[0]=0x20;

     r_day[0]=f_time[3];
     r_day[1]=f_time[4];
     r_day[2]=0;
     if (r_day[0] == '0')
        r_day[0]=0x20;

     r_year[0]=f_time[6];
     r_year[1]=f_time[7];
     r_year[2]=f_time[8];
     r_year[3]=f_time[9];
     r_year[4]=0;
     
}

void change_password(in,out)
char *in;
char *out;
{
             char s1[10];
             char s2[10];
             unsigned char s3[10];
             int leng,i,j;
             int  change[10]={11,22,9,15,27,18,14,22,13,5};


             strcpy(s1,in);
             leng = strlen(s1);
             for(i=0;i<leng;i++)
             {
                s3[leng-i-1] = s1 [i] + i;
             }
             for(i=0;i<leng;i++)
             {
                s3[i] = s3[i]+change[i]  ;
                if (s3[i] > 58 && s3[i] < 65 ) s3[i] = 8+s3[i]  ;
                if (s3[i] > 90 && s3[i] < 97 ) s3[i] = 8+s3[i]  ;
                if (s3[i] > 122 ) s3[i] = s3[i]-122+97  ;
             }
             strcpy(s2,(char *)s3);
             for(i=0;i<leng;i++)
             {
                s3[leng-i-1] = s2 [i] ;
             }
             s3[leng]=0;
             strcpy(out,(char *)s3);
}

/* 对小数点后num位进行四舍五入 */
double roundN(double source, int num)
{
        char tmpstr[64+1];
        char tmp1[12+1];
	double tmpdouble=0.00;
	double tmp5=0.00;
	int tmpnum=0,i=0;

        memset(tmpstr, 0, sizeof(tmpstr));
        memset(tmp1, 0, sizeof(tmp1));

	if(num>=0){
        	tmp5=1;
        	tmpnum=num+3;
        	for(i=1;i<=tmpnum;i++){
        		tmp5=(tmp5*(0.1));
        	}
        	source=source+tmp5;
	}
        
        sprintf(tmp1, "%%12.%df", num);
        sprintf(tmpstr, tmp1, source);
	tmpdouble = atof(tmpstr);
		
        return tmpdouble;
}
