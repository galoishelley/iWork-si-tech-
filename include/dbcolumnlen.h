#ifndef __DBCOLUMNLEN_H_
#define __DBCOLUMNLEN_H_


/******sMarkFavRuleRelease******/
#define LEN_SMARKFAVRULERELEASE_FAVOUR_CODE		8+1
#define	LEN_SMARKFAVRULERELEASE_GROUP_ID		10+1
#define	LEN_SMARKFAVRULERELEASE_LOGIN_NO		6+1
#define	LEN_SMARKFAVRULERELEASE_OP_NOTE			60+1

/******sMarkGradeCode******/
#define LEN_SMARKGRADECODE_CLASS_CODE     2+1
#define LEN_SMARKGRADECODE_CLASS_NAME			20+1
#define LEN_SMARKGRADECODE_GROUP_ID       10+1
#define LEN_SMARKGRADECODE_LOGIN_NO				6+1
#define LEN_SMARKGRADECODE_BEGIN_POINT    13+1
#define LEN_SMARKGRADECODE_END_POINT      13+1          
#define LEN_SMARKGRADECODE_OP_TIME        20+1  
#define LEN_SMARKGRADECODE_OP_NOTE				60+1        
#define LEN_SMARKGRADECODE_CUST_POINT     13+1  

/******sGiftRule******/

#define  LEN_SGIFTRULE_GIFT_RULE_CODE         8+1                       
#define  LEN_SGIFTRULE_RULL_NAME              30+1
#define  LEN_SGIFTRULE_GRADE_CODE             2+1                        
#define  LEN_SGIFTRULE_SM_CODE                2+1
#define  LEN_SGIFTRULE_OWNER_LIST             255+1                  
#define  LEN_SGIFTRULE_CLASS_LIST             255+1
#define  LEN_SGIFTRULE_PREPAY_LEAST           13+1
#define  LEN_SGIFTRULE_LEAST                  255+1
#define  LEN_SGIFTRULE_DEPOSIT_LEAST          13+1
#define  LEN_SGIFTRULE_POWER_RIGHT            10+1
#define  LEN_SGIFTRULE_CUST_TYPE_LIST         255+1
#define  LEN_SGIFTRULE_CUST_CLASS_LIST        255+1
#define  LEN_SGIFTRULE_RULL_POINT             13+1
#define  LEN_SGIFTRULE_LOGIN_NO               6+1
#define  LEN_SGIFTRULE_OP_TIME                20+1
#define  LEN_SGIFTRULE_OP_NOTE                60+1


/******sGiftRuleRel******/
#define LEN_SGIFTRULEREL_GIFT_RULE_CODE			8+1
#define LEN_SGIFTRULEREL_GIFT_CODE				8+1

/******sGiftProductClass******/


#define LEN_SGIFTPRODUCTCLASS_GIFT_CLASS_CODE	3+1
#define LEN_SGIFTPRODUCTCLASS_GIFT_CLASS_NAME	255+1
#define LEN_SGIFTPRODUCTCLASS_LOGIN_NO			6+1
#define LEN_SGIFTPRODUCTCLASS_OP_TIME           20+1
#define LEN_SGIFTPRODUCTCLASS_OP_NOTE			60+1

/******sGiftConfig******/
#define LEN_SGIFTCONFIG_GIFT_CODE               8+1                           
#define LEN_SGIFTCONFIG_GIFT_NAME               255+1
#define LEN_SGIFTCONFIG_GIFT_CLASS_CODE         3+1         
#define LEN_SGIFTCONFIG_AWARD_BEGIN             20+1
#define LEN_SGIFTCONFIG_AWARD_END               20+1
#define LEN_SGIFTCONFIG_LIMIT_DAY               10+1
#define LEN_SGIFTCONFIG_WAITE_DAY               10+1
#define LEN_SGIFTCONFIG_FAVOUR_ADDRESS          60+1 
#define LEN_SGIFTCONFIG_FAVOUR_MONEY            13+1
#define LEN_SGIFTCONFIG_LOGIN_NO                6+1
#define LEN_SGIFTCONFIG_OP_TIME                 20+1
#define LEN_SGIFTCONFIG_OP_NOTE                 60+1

/******sMarkType******/
#define LEN_SMARKTYPE_TYPECODE			2+1
#define LEN_SMARKTYPE_OTYPENAME			20+1

/******sMarkCode******/
#define LEN_SMARKCODE_MARKCODE			10+1
#define LEN_SMARKCODE_MARKTYPE			2+1
#define LEN_SMARKCODE_MARKNAME			20+1

/******sMarkCodeDetail******/
#define LEN_SMARKCODEDETAIL_MARKCODE    10+1
#define LEN_SMARKCODEDETAIL_DETAILCODE    10+1
#define LEN_SMARKCODEDETAIL_DETAILNAME   60+1
#define LEN_SMARKCODEDETAIL_IMPORTRIGHT    13+1
#define LEN_SMARKCODEDETAIL_INITVALUE    13+1
#define LEN_SMARKCODEDETAIL_ORDERCODE    10+1

#endif /*__DBCOLUMNLEN_H_ */
