#include "llayerStruct.h"

CLCommand::CLCommand() :
	ActiveConnection(NULL)
{
	CommondText = "";
	CommondType = adCmdUnknown;
	m_sqlType = DB_STMT_SELECT;
	m_iters = 0;

	m_stmthp = (OCIStmt *) NULL;

	memset(&pvsk, 0, sizeof(pvsk));
	memset(&indsk, 0, sizeof(indsk));
	memset(&rlsk, 0, sizeof(rlsk));
	memset(&rcsk, 0, sizeof(rcsk));

	int i;
	for (i = 0; i < MAXBINDS; i++)
	{
		m_bndhp[i] = (OCIBind *) 0;
	}
	for (i = 0; i < MAXCOLS; i++)
	{
		m_defnp[i] = (OCIDefine *) 0;
	}
	recordSet = (CLRecordSet *)NULL;

	m_inParamCount = 0;

	m_outParamCount = 0;
}

CLCommand::~CLCommand()
{
	int i;
	for (i = 0; i < MAXBINDS; i++)
	{
		if (m_bndhp[i])
		{
			/*
			 * 使用OCIBindByPos会生成一个绑定句柄，此句柄的内存由OCI自己管理，不用调用OCIHandleFree释放。
			 * OCIHandleFree((dvoid *) m_bndhp[pos-1], (ub4) OCI_HTYPE_BIND);
			 */
			m_bndhp[i] = (OCIBind *) 0;
		}
	}
	for (i = 0; i < MAXCOLS; i++)
	{
		if (m_defnp[i])
		{
			/*
			 * 使用OCIDefineByPos会生成一个绑定句柄，此句柄的内存由OCI自己管理，不用调用OCIHandleFree释放。
			 * OCIHandleFree((dvoid *) m_defnp[i], (ub4) OCI_HTYPE_DEFINE);
			 */
			m_defnp[i] = NULL;

		}
	}

	if (m_stmthp)
	{
		(void) OCIHandleFree((dvoid *) m_stmthp, (ub4) OCI_HTYPE_STMT);
		m_stmthp = NULL;
	}

	DetachRecordset();
	ClearParameters(0);
}

TDriverType CLCommand::GetDriverType() const
{
	return DRIVER_DATABASE;
}

//根据位置绑定变量值
//pos 从1开始
ETYPE CLCommand::BindInByPos(ub4 pos, void * data, ub4 dataLen, TDataTypeEnum dataType)  throw()
{
	ETYPE iRet = 0;
	ub2 oraDataType;

	DBUG_ENTER("CLCommand::BindInByPos");
	
	if ((pos > MAXBINDS) || (pos <= 0))
	{
		DBUG_RETURN(CLError::E_DB_BIND_MAXIMUM);
	}
	if (m_bndhp[pos-1])
	{
		/*
		 * 使用OCIBindByPos会生成一个绑定句柄，此句柄的内存由OCI自己管理，不用调用OCIHandleFree释放。
		 * OCIHandleFree((dvoid *) m_bndhp[pos-1], (ub4) OCI_HTYPE_BIND);
		 */
		//m_bndhp[pos-1] = NULL;
	}

	if ((iRet = LocalTypeToDbType(dataType, oraDataType)) != 0)
	{
		DBUG_RETURN(iRet);
	}

	if ((iRet = OCIBindByPos(m_stmthp, &m_bndhp[pos-1], ActiveConnection->GetErrhp(), (ub4) pos,
				(dvoid *) data, (sb4)dataLen, (ub2)oraDataType,
				(dvoid *) 0, (ub2 *)0, (ub2 *)0,
				(ub4) 0, (ub4 *) 0, ActiveConnection->GetOciMode())) != 0)
	{
		DBUG_RETURN(CLError::E_DB_SQLALD_BIND);
	}
	pvsk[pos - 1] = dataLen;
	iRet = OCIBindArrayOfStruct(m_bndhp[pos-1], ActiveConnection->GetErrhp(),
		pvsk[pos - 1], indsk[pos-1], rlsk[pos-1], rcsk[pos-1]);

	DBUG_RETURN(iRet);
}

//根据位置绑定变量值
//pos 从1开始
ETYPE CLCommand::BindInByName(const char* bindName, ub4 pos, void * data, ub4 dataLen, TDataTypeEnum dataType)  throw()
{
	ETYPE iRet = 0;
	ub2 oraDataType;

	DBUG_ENTER("CLCommand::BindInByName");
	
	if ((pos > MAXBINDS) || (pos <= 0))
	{
		DBUG_RETURN(CLError::E_DB_BIND_MAXIMUM);
	}
	if (m_bndhp[pos-1])
	{
		/*
		 * 使用OCIBindByName会生成一个绑定句柄，此句柄的内存由OCI自己管理，不用调用OCIHandleFree释放。
		 * OCIHandleFree((dvoid *) m_bndhp[pos-1], (ub4) OCI_HTYPE_BIND);
		 */
		//m_bndhp[pos-1] = NULL;
	}

	if ((iRet = LocalTypeToDbType(dataType, oraDataType)) != 0)
	{
		DBUG_RETURN(iRet);
	}

	if ((iRet= OCIBindByName(m_stmthp, &m_bndhp[pos-1], ActiveConnection->GetErrhp(),
					(text *) bindName, (sb4) strlen((char *) bindName),
					(dvoid *) data, (sb4) dataLen, (ub2)oraDataType,
					(dvoid *) 0, (ub2 *)0, (ub2 *)0,
					(ub4) 0, (ub4 *) 0, (ub4) ActiveConnection->GetOciMode()) ) != 0)
	{
		DBUG_RETURN(CLError::E_DB_SQLALD_BIND);
	}
	pvsk[pos - 1] = dataLen;
	iRet = OCIBindArrayOfStruct(m_bndhp[pos-1], ActiveConnection->GetErrhp(),
		pvsk[pos - 1], indsk[pos-1], rlsk[pos-1], rcsk[pos-1]);

	DBUG_RETURN(iRet);
}

//根据位置绑定变量值
//pos 从1开始
ETYPE CLCommand::BindOutByPos(ub4 pos, void * data, ub4 dataLen, TDataTypeEnum dataType)  throw()
{
	ETYPE iRet = 0;
	ub2 oraDataType;

	DBUG_ENTER("CLCommand::BindOutByPos");
	
	if ((pos > MAXCOLS) || (pos <= 0))
	{
		DBUG_RETURN(CLError::E_DB_SELECT_MAXIMUM);
	}
	if (m_defnp[pos-1])
	{
		/*
		 * 使用OCIDefineByPos会生成一个绑定句柄，此句柄的内存由OCI自己管理，不用调用OCIHandleFree释放。
		 * OCIHandleFree((dvoid *) m_defnp[pos-1], (ub4) OCI_HTYPE_DEFINE);
		 */
		//m_defnp[pos-1] = NULL;
	}

	if ((iRet = LocalTypeToDbType(dataType, oraDataType)) != 0)
	{
		DBUG_RETURN(iRet);
	}

	if ((iRet = OCIDefineByPos(m_stmthp, &m_defnp[pos-1], ActiveConnection->GetErrhp(),
						(ub4) pos, (dvoid *) data,(sb4) dataLen, (ub2)oraDataType,
						(dvoid *) 0, (ub2 *)0,(ub2 *)0, (ub4) ActiveConnection->GetOciMode())) != 0)
	{
		DBUG_RETURN(CLError::E_DB_SQL_DEFINE);
	}
	pvsk[pos - 1] = dataLen;
	iRet = OCIDefineArrayOfStruct(m_defnp[pos-1], ActiveConnection->GetErrhp(),
		pvsk[pos - 1], indsk[pos-1], rlsk[pos-1], rcsk[pos-1]);

	DBUG_RETURN(iRet);
}

//生成一个输入参数
//Size	1、参数一行的最大字节位数，Value指向的内存大小＝Size * 每次取出的行数。
//      2、对于查询语句中传入的字符串，使用strlen得到字符串的长度。
//Value	参数的数组值.Value.p1Val[0], Value.p1Val[1], ...
void CLCommand::SetInParameter (TDataTypeEnum Type, long Size, void *Value) throw()
{
	ETYPE iRet=0;
	CLParameter * param=NULL;

	DBUG_ENTER("CLCommand::SetInParameter");
	param = inParams.at(m_inParamCount);
	param->Type = Type;
	param->Size = Size;
	param->Value.pVal = Value;

	m_inParamCount ++;
	DBUG_VOID_RETURN;
}

//生成一个输出参数
//Size	参数一行的最大字节位数，Value指向的内存大小＝Size * 每次取出的行数。
//Value	参数的数组值.Value.p1Val[0], Value.p1Val[1], ...
void CLCommand::SetOutParameter (TDataTypeEnum Type, long Size, void *Value) throw()
{
	ETYPE iRet=0;
	CLParameter * param=NULL;

	DBUG_ENTER("CLCommand::SetOutParameter");
	param = outParams.at(m_outParamCount);
	param->Type = Type;
	param->Size = Size;
	param->Value.pVal = Value;

	m_outParamCount ++;
	DBUG_VOID_RETURN;
}

//清空输入输出参数列表
ETYPE CLCommand::InitParameters (int initOptions) throw()
{
	ETYPE iRet=0;
	CLParameter * param=NULL;
	ub4 i = 0;

	DBUG_ENTER("CLCommand::InitParameters");
   
	for ( i = 0; i < MAXBINDS; i ++ )
	{
		param = new CLParameter(DT_PUB1, adParamInput, 0, NULL);
		if (param == (CLParameter *)NULL)
		{
			DBUG_RETURN(CLError::E_MEMORY_LACK);
		}
		inParams.push_back(param);
	}

	for ( i = 0; i < MAXCOLS; i ++ )
	{
		param = new CLParameter(DT_PUB1, adParamOutput, 0, NULL);
		if (param == (CLParameter *)NULL)
		{
			DBUG_RETURN(CLError::E_MEMORY_LACK);
		}
		outParams.push_back(param);
	}

	DBUG_RETURN(iRet);
}


//清空输入输出参数列表
void CLCommand::ClearParameters (int clearOptions) throw()
{
	ETYPE iRet=0;
	ub4 i = 0;

	DBUG_ENTER("CLCommand::ClearParameter");
   
	for ( i = 0; i < inParams.size(); i ++ )
	{
		delete inParams.at(i);
	}

	for ( i = 0; i < outParams.size(); i ++ )
	{
		delete outParams.at(i);
	}

	inParams.clear();
	outParams.clear();
	DBUG_VOID_RETURN;
}

ETYPE CLCommand::Execute(ub4 RecordsAffected, long Options) throw()
{
	ETYPE iRet=0;

	DBUG_ENTER("CLCommand::Execute");

	m_iters = RecordsAffected;
	if ( (m_sqlType != DB_STMT_SELECT) && (m_iters <=0 || m_iters>MAXITER))
	{
		DBUG_RETURN(CLError::E_DB_SQL_EXECUTE_ITERS);
	}

	if (!ActiveConnection)
	{
		DBUG_RETURN(CLError::E_DB_SQL_NOCONNECT);
	}

	if (!ActiveConnection->IsLoggedOn())
	{
		DBUG_RETURN(CLError::E_DB_SQL_NOCONNECT);
	}

	if ((iRet = PrepareSql(CommondText.c_str(), 0)) != 0)
	{
		DBUG_RETURN(iRet);
	}

	for (ub4 i = 0; i < m_inParamCount; i ++)
	{
		if ((iRet = BindInByPos(i+1, inParams.at(i)->Value.pVal, 
			inParams.at(i)->Size,
			inParams.at(i)->Type)) != 0)
		{
			DBUG_RETURN(iRet);
		}
#ifndef DBUG_OFF
		switch (inParams.at(i)->Type)
		{
		case DT_UB1:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%d]", i, *(inParams.at(i)->Value.p1Val)));
			break;
		case DT_UB2:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%d]", i, *(inParams.at(i)->Value.p2Val)));
			break;
		case DT_UB4:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%d]", i, *(inParams.at(i)->Value.p4Val)));
			break;
		case DT_UB8:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%ld]", i, *(inParams.at(i)->Value.p8Val)));
			break;
		case DT_FLOAT:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%f]", i, *(inParams.at(i)->Value.pfVal)));
			break;
		case DT_DOUBLE:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%lf]", i, *(inParams.at(i)->Value.pdVal)));
			break;
		case DT_PUB1:
			DBUG_PRINT("Execute", ("CLCommand::Execute InParam[%d][%s]", i, inParams.at(i)->Value.p1Val));
			break;
		}
#endif
	}

	// Execute the Insert statement
	if (OCIStmtExecute(ActiveConnection->GetSvchp(), m_stmthp, ActiveConnection->GetErrhp(), (ub4) m_iters, (ub4) 0,
		(CONST OCISnapshot*) 0, (OCISnapshot*) 0,
		(ub4) ActiveConnection->GetOciMode()))
	{
		DBUG_RETURN(CLError::E_DB_SQL_EXECUTE);
	}

	if (m_sqlType == DB_STMT_SELECT)
	{
		if (recordSet != (CLRecordSet*)NULL)
		{
			sb4 paramStatus;
			ub2 dtype;
			text         *colName;
			ub4          colNameLen;
			OCIParam     *paramInfo = (OCIParam *) 0;

			if ((iRet = recordSet->SetColomnCount()) != 0)
			{
				DBUG_RETURN(CLError::E_DB_SQL_EXECUTE);
			}
			if (recordSet->GetColomnCount() >= MAXCOLS)
			{
				DBUG_RETURN(CLError::E_DB_SELECT_MAXIMUM);
			}
			for (ub4 i = 0; i < recordSet->GetColomnCount() && i < outParams.size(); i ++)
			{
				if ((iRet = BindOutByPos(i+1, (outParams.at(i))->Value.pVal, (outParams.at(i))->Size,
					(outParams.at(i))->Type)) != 0)
				{
					DBUG_RETURN(iRet);
				}
				paramStatus = OCIParamGet((dvoid *)m_stmthp, OCI_HTYPE_STMT, ActiveConnection->GetErrhp(),
							   (dvoid **)&paramInfo, (ub4) i+1);
				if (paramStatus != OCI_SUCCESS)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
				if (OCIAttrGet((dvoid*) paramInfo, (ub4) OCI_DTYPE_PARAM, (dvoid*) &dtype,
					(ub4 *) 0, (ub4) OCI_ATTR_DATA_TYPE,  ActiveConnection->GetErrhp()) != 0)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
				if (OCIAttrGet((dvoid*) paramInfo, (ub4) OCI_DTYPE_PARAM, &colName,
					(ub4 *) &colNameLen, (ub4) OCI_ATTR_NAME,  ActiveConnection->GetErrhp()) != 0)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
				strncpy((outParams.at(i))->Name,(const char*)colName, MAX_COL_LENGTH);
				(outParams.at(i))->Name[MAX_COL_LENGTH] = '\0';
				if (OCIAttrGet((dvoid*) paramInfo, (ub4) OCI_DTYPE_PARAM, (dvoid*) &(outParams.at(i))->Precision,
					(ub4 *) 0, (ub4) OCI_ATTR_PRECISION,  ActiveConnection->GetErrhp()) != 0)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
				if (OCIAttrGet((dvoid*) paramInfo, (ub4) OCI_DTYPE_PARAM, (dvoid*) &(outParams.at(i))->NumericScale,
					(ub4 *) 0, (ub4) OCI_ATTR_SCALE,  ActiveConnection->GetErrhp()) != 0)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
				paramStatus=OCIDescriptorFree(paramInfo, (ub4)OCI_DTYPE_PARAM);
				if (paramStatus != OCI_SUCCESS)
				{
					DBUG_RETURN(CLError::E_DB_GET_COL_ATTR);
				}
			}
		}
	}
	else
	{
		this->Close();
	}

	//设置初始输入参数数量
	m_inParamCount = 0;
	m_outParamCount = 0;

	DBUG_RETURN(iRet);
}

//关闭语句
void CLCommand::Close() throw()
{
	if (m_stmthp)
	{
		(void) OCIHandleFree((dvoid *) m_stmthp, (ub4) OCI_HTYPE_STMT);
		m_stmthp = NULL;
	}
}

void CLCommand::GetOraError(int *errorCode, char* errMsg) throw()
{ 
	text  msgbuf[512];

	(void) OCIErrorGet((dvoid *)ActiveConnection->GetErrhp(), (ub4) 1, (text *) NULL, errorCode,
			msgbuf, (ub4) sizeof(msgbuf), (ub4) OCI_HTYPE_ERROR);
	strcpy(errMsg, (const char *)msgbuf);
	return;
}

void CLCommand::DetachRecordset() throw()
{
	if (recordSet)
	{
		delete recordSet;
	}
	return;
}


CLRecordSet * CLCommand::BindToRecordset() throw()
{
	if (!recordSet)
	{
		recordSet = new CLRecordSet();
	}
	recordSet->ActiveConnection = this->ActiveConnection;
	recordSet->ActiveCommand = this;
	return recordSet;
}

//内部执行SQL语句
ETYPE CLCommand::PrepareSql(const char* sqlContext, int preFlags)
{
	ETYPE iRet;

	DBUG_ENTER("CLCommand::PrepareSql");

	if (!ActiveConnection->IsLoggedOn())
	{
		DBUG_RETURN(CLError::E_DB_SQL_NOCONNECT);
	}

	//对于没有关闭的SQL语句进行关闭
	Close();

	// Allocate a statement handle
	if (OCIHandleAlloc((dvoid *)ActiveConnection->GetEnvhp(), (dvoid **) &m_stmthp,
		(ub4)OCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0))
	{
		DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_STMT);
	}

	// Prepare the statement
	if (OCIStmtPrepare(m_stmthp, ActiveConnection->GetErrhp(), (const OraText *)sqlContext, (ub4)strlen((char *)sqlContext),
		(ub4) OCI_NTV_SYNTAX, (ub4)ActiveConnection->GetOciMode()))
	{
		DBUG_RETURN(CLError::E_DB_SQL_PREPARE);
	}

	if ((iRet = OCIAttrGet(m_stmthp, (ub4)OCI_HTYPE_STMT, (ub2 *)&m_sqlType,
		(ub4*) 0, OCI_ATTR_STMT_TYPE, ActiveConnection->GetErrhp())) != 0)
	{
		int oraErrCode =0;
		char oraErrMsg[512];
		GetOraError(&oraErrCode, oraErrMsg);
		DBUG_PRINT("CLCommand::PrepareSql",("OCIAttrGet iRet=[%ld], oraErrCode=[%ld], oraErrMsg=[%s]",
			iRet, oraErrCode, oraErrMsg));
		DBUG_RETURN(CLError::E_DB_OCI_ATTR_GET);
	}

	switch(m_sqlType)
	{
	case OCI_STMT_SELECT :
		{
			m_sqlType = DB_STMT_SELECT;
			break;
		}
	case OCI_STMT_UPDATE :
		{
			m_sqlType = DB_STMT_UPDATE;
			break;
		}
	case OCI_STMT_DELETE :
		{
			m_sqlType = DB_STMT_DELETE;
			break;
		}
	case OCI_STMT_INSERT :
		{
			m_sqlType = DB_STMT_INSERT;
			break;
		}
	case OCI_STMT_CREATE :
		{
			m_sqlType = DB_STMT_CREATE;
			break;
		}
	case OCI_STMT_DROP   :
		{
			m_sqlType = DB_STMT_DROP;
			break;
		}
	case OCI_STMT_ALTER  :
		{
			m_sqlType = DB_STMT_ALTER;
			break;
		}
	case OCI_STMT_BEGIN  :
		{
			m_sqlType = DB_STMT_BEGIN;
			break;
		}
	case OCI_STMT_DECLARE:
		{
			m_sqlType = DB_STMT_DECLARE;
			break;
		}
	default:
		{
			DBUG_PRINT("CLCommand::PrepareSql",("OCI_ATTR_STMT_TYPE[%d]", m_sqlType));
			DBUG_RETURN(CLError::E_DB_SQL_UNKNOWN);
		}
	}
	DBUG_PRINT("CLCommand::PrepareSql",("OCI_ATTR_STMT_TYPE[%d][%s]", m_sqlType, sqlContext));

	DBUG_RETURN(0);
}


CLCommand* CLCommand::I(const char* data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const char*");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_PUB1, dataLength, (void*)data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const char& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const char&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_UB1, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const ub2& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const ub2&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_UB2, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const ub4& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const ub4&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_UB4, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const ub8& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const ub8&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_UB8, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const float& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const float&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_FLOAT, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::I(const double& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::I const double&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetInParameter (DT_DOUBLE, dataLength, (void*)&data);
	DBUG_RETURN(this);
}


CLCommand* CLCommand::O(char* data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const char*");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_PUB1, dataLength, (void*)data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(char& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const char&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_UB1, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(ub2& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const ub2&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_UB2, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(ub4& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const ub4&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_UB4, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(ub8& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const ub8&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_UB8, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(float& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const float&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_FLOAT, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

CLCommand* CLCommand::O(double& data, ub4 dataLength) throw(CLException)
{
	DBUG_ENTER("CLCommand::O const double&");
	if ((m_inParamCount > MAXBINDS ) )
	{
		throw CLException(CLError::E_DB_BIND_MAXIMUM);
	}
	SetOutParameter (DT_DOUBLE, dataLength, (void*)&data);
	DBUG_RETURN(this);
}

