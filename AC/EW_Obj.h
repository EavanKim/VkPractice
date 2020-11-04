#ifndef EWOBJ_H__
#define EWOBJ_H__

#include "EW_Enum.h"

class EW_Obj
{
public:
	EW_Obj() {}
	virtual ~EW_Obj() {}

	EW_TYPE_FLAG Get_DllTypeFlag() { return DllTypeFlag; }

protected:
	EW_TYPE_FLAG DllTypeFlag = EW_TYPE_FLAG::EW_NONE;
};

class VK_Obj : public EW_Obj
{

};

class SW_Obj : public EW_Obj
{

};

#endif // ! EWOBJ_H__
