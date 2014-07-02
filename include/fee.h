#ifndef __FEE_DEFINE_H__
#define __FEE_DEFINE_H__

typedef struct 
{
	int	       return_code;
	char 	       phone_no[16];
	char           bill_type[2];
	char           fee_code[3];
	int            should_pay;
	int            favour_fee;
	int            talk_fee;
	int            times;
	int            duration;
} dUnBill;

typedef struct {
	int	       return_code;
	char           phone_no[16];
	int            should_pay;
	int            favour_fee;
	int            talk_fee;
	int            times;
	int            duration;
	int            bill_begin;
	int            bill_end;
} dUnBillTotal;

typedef struct 
{
	int	return_code;
	char	phone_no[16];
	int	total_fee; 
} dUnBillTotal2;

typedef struct 
{
	char           fee_code[3];
	int            total_fee;
} dUnBill_Item;

typedef struct
{
	int	return_code;
	char	phone_no[16];
	int     total_fee; /** 用户总未出帐话费 **/
	dUnBill_Item   items[50];
} dUnBillFee;

int call_smds_opr(char *sOpCode,dUnBillTotal * unBillFee);

#endif /* __FEE_DEFINE_H__ */

