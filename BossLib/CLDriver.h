#ifndef __CLDriver_h__
#define __CLDriver_h__

//�������Ͷ���
enum TDriverType
{
	//���ݿ���������
	DRIVER_DATABASE		= 0,
	
	//�ļ���������
	DRIVER_FILE			= 1,
	DRIVER_SHM			= 2
};

/*
 ��������Ļ��ඨ�壬Ϊ�����࣬���붨�����࣬ʵ�������ĸ��ֲ�����
 */
class CLDriver :public CLLayer
{
public:
	virtual TLayerNo GetLayerNo() const;

	//�������Ͷ������
	virtual TDriverType GetDriverType() const = 0;
};

#endif //__CLDriver_h__
