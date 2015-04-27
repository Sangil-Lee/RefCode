///////////////////////////////////////////////////////////
//  ControllerInterface.h
//  Implementation of the Interface ControllerInterface
//  Created on:      09-4-2013 ���� 7:24:23
//  Original author: ysw
///////////////////////////////////////////////////////////

#if !defined(EA_DF3E3BA3_BEB4_4056_AB14_95A3350599F9__INCLUDED_)
#define EA_DF3E3BA3_BEB4_4056_AB14_95A3350599F9__INCLUDED_

class ControllerInterface
{

public:
	virtual int close() =0;
	virtual bool isOpened() =0;
	virtual int open() =0;
	virtual int read(void * buf, const int size) =0;
	virtual int write(void * buf, const int size) =0;

private:
	bool m_bOpend;
	int m_iRetStatus;

};
#endif // !defined(EA_DF3E3BA3_BEB4_4056_AB14_95A3350599F9__INCLUDED_)
