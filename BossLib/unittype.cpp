#include "llayerStruct.h"


int u_errno=0;


/*不成功返回NULL*/
utype* uInit(int capacity)
{
	int i,noMem=0;
	utype *pu;

	u_errno=0;

	if(capacity<1)
		capacity=1;

	pu=(utype*)malloc(sizeof(utype));
	if (!pu)
	{
		u_errno=U_ERR_NOMEM;
		return NULL;
	}
	memset(pu,0,sizeof(utype));
	pu->capacity=capacity;
	pu->cnt=0;
	pu->ppe=(ue**)malloc(sizeof(ue*)*capacity);
	if (!(pu->ppe))
	{
		free(pu);
		u_errno=U_ERR_NOMEM;
		pu = NULL;
	}
	memset(pu->ppe, 0, sizeof(sizeof(ue*)*capacity));

	for(i=0;i<capacity;i++)
	{
		pu->ppe[i]=(ue*)malloc(sizeof(ue));
		if (pu->ppe[i] == NULL)
		{
			noMem = i+1;
			break;
		}
		memset(pu->ppe[i],0,sizeof(ue));
	}
	if (noMem != 0)
	{
		for(i=0;i<noMem-1;i++)
		{
			free(pu->ppe[i]);
		}
		free(pu->ppe);
		free(pu);
		u_errno=U_ERR_NOMEM;
		pu = NULL;
	}

	return pu;
}

/*不成功返回NULL*/
utype* uAlloc(utype *pu,int occ)
{
	int i,noMem=0,new_cap;
	ue** tmp;

	u_errno=0;
	if(occ<pu->cnt)
	{
		ueFree(pu->ppe[occ]);
	}
	else if(occ>=pu->cnt && occ<pu->capacity)
	{
		pu->cnt=occ+1;
	}
	else if(occ>=pu->capacity)
	{
		if(occ>pu->capacity+pu->capacity/2)
			new_cap=occ+1;
		else 
			new_cap=pu->capacity+pu->capacity/2+1;

		tmp =(ue**)realloc(pu->ppe,sizeof(ue*)*new_cap);
		if (!tmp)
		{
			u_errno=U_ERR_NOMEM;
			return NULL;
		}
		pu->ppe=tmp;
		memset(pu->ppe+pu->capacity,0,sizeof(ue*)*(new_cap-pu->capacity));
		for(i=0;i<new_cap-pu->capacity;i++)
		{
			pu->ppe[pu->capacity+i]=(ue*)malloc(sizeof(ue));
			if (!(pu->ppe[pu->capacity+i]))
			{
				noMem = i+1;
				break;
			}
			memset(pu->ppe[pu->capacity+i],0,sizeof(ue));
		}

		if (noMem != 0)
		{
			for(i=0;i<noMem-1;i++)
			{
				free(pu->ppe[pu->capacity+i]);
			}
			u_errno=U_ERR_NOMEM;
			return NULL;
		}
		pu->capacity=new_cap;
		pu->cnt=occ+1;
	}
	return pu;
}

/*不成功返回NULL*/
utype * uSetInt(utype *pu,int occ,int iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_INT;
	(*(pu->ppe+occ))->value.v_int=iv;
	return pu;
}

/*不成功返回NULL*/
utype * uSetLong(utype *pu,int occ,long iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_LONG;
	(*(pu->ppe+occ))->value.v_long=iv;
	return pu;
}

/*不成功返回NULL*/
utype * uSetDouble(utype *pu,int occ,double iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_DOUBLE;
	(*(pu->ppe+occ))->value.v_double=iv;
	return pu;
}

int uGetInt(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_INT)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.v_int);
}

long uGetLong(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_LONG)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.v_long);
}

double uGetDouble(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_DOUBLE)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.v_double);
}

/*不成功返回NULL*/
utype *uSetStr(utype *pu,int occ,const char * iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_STRING;
	(*(pu->ppe+occ))->value.p_char=(char*)malloc(strlen(iv)+1);
	if ((*(pu->ppe+occ))->value.p_char == NULL)
	{
		return NULL;
	}
	strcpy((*(pu->ppe+occ))->value.p_char,iv);
	return pu;
}

char * uGetStr(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_STRING)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.p_char);
}

/*不成功返回NULL*/
utype * uSetArray(utype *pu,int occ,utype * iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_ARRAY;
	(*(pu->ppe+occ))->value.p_utype=iv;
	return pu;
}

utype* uGetArray(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_ARRAY)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.p_utype);
}

/*不成功返回NULL*/
utype * uSetStruct(utype *pu,int occ,utype * iv)
{
	u_errno=0;
	if (uAlloc(pu,occ) == NULL)
	{
		return NULL;
	}
	(*(pu->ppe+occ))->type=U_STRUCT;
	(*(pu->ppe+occ))->value.p_utype=iv;
	return pu;
}

utype* uGetStruct(utype *pu,int occ)
{
	u_errno=0;
	if(occ>=pu->cnt)
		u_errno=U_ERR_NOOCC;
	else if(pu->ppe[occ]->type!=U_STRUCT)
		u_errno=U_ERR_WRONGTYPE;
	else if(pu->ppe[occ]->type==U_EMPTY)
		u_errno=U_ERR_EMPTY;
	return(pu->ppe[occ]->value.p_utype);
}

void uFree(utype *pu)
{
	int i;

	u_errno=0;
	for(i=0;i<pu->capacity;i++){
		if( pu->ppe[i]->type==U_STRING){
			free(pu->ppe[i]->value.p_char);
		}
		else if( pu->ppe[i]->type==U_ARRAY || pu->ppe[i]->type==U_STRUCT ){
			uFree(pu->ppe[i]->value.p_utype);
		}
		free(pu->ppe[i]);
	}
	free(pu->ppe);
	free(pu);
}

void ueFree(ue * pue)
{
	u_errno=0;
	if(pue->type==U_STRING)
		free(pue->value.p_char);
	else if(pue->type==U_ARRAY || pue->type==U_STRUCT)
		uFree(pue->value.p_utype);
}

