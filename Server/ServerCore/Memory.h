#pragma once
#include "Allocator.h"

template<typename Type>
Type* xnew()//���ڸ� ���� �ʴ� �⺻���� new
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	new(memory)Type();
	return memory;
}

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//������������ �Ķ���Ͱ� 1���̻��� ��� �Ķ�������¿� ���ؼ� �����Ѵ�.
{
	//���������� ���ڰ����� �Լ������ؼ��� �������� �ʴ´� ������ ������...
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	//placement new������ �޸� ��������ġ�� �����ڸ� ȣ���϶�� �����̴�.
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	xrelease(obj);

}

