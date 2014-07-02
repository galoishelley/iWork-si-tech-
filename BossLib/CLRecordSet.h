#ifndef __CLRecordSet_h__
#define __CLRecordSet_h__

class CLCommand;
class CLRecordSet :	public CLDriver
{
public:
	CLRecordSet();
	~CLRecordSet();
	
	//驱动类型定义代码
	virtual TDriverType GetDriverType() const;
public:
	//当前激活的连接
	CLConnection *ActiveConnection;

	//当前激活的连接
	CLCommand *ActiveCommand;
public:
	//当前记录所处记录集的位置。
	ETYPE GetCurrentPosition(ub4* currentPosition) const throw();

	//得到select语句返回的列数。
	ub4 GetColomnCount() throw();

	//得到到目前为止，已经返回的行数。
	ETYPE GetRowCount(ub4* rowCount) const  throw();

	//得到Fetch语句返回的行数。
	ETYPE GetRowFetched(ub4* rowFetched) const  throw();

	//得到select语句返回的列数。内部用。
	ETYPE SetColomnCount() throw();

	//取出下一批记录
	ETYPE FetchNext(ub4 rowCount) throw();

	//得到字段信息
	vector <CLParameter*>& GetFields() throw();

	//得到字段值

	const void* GetField(ub4 columnIndex, ub4 rowIndex) throw();
	const char* GetFieldName(ub4 columnIndex) throw();
private:

	ub4 m_iColumnCount;
};


#endif//__CLRecordSet_h__
