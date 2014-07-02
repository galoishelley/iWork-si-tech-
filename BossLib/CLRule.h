#ifndef __CLRule_h__
#define __CLRule_h__

class CLRule
{
public:
	CLRule(CLCommand *cmd);
	~CLRule();
private:
	CLCommand *m_cmd;
};

#endif//__CLBase_h__
