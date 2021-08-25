#pragma once
#include "Allocator.h"

template<typename Type>
Type* xnew()//인자를 받지 않는 기본생성 new
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	new(memory)Type();
	return memory;
}

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//보편참조인자 파라미터가 1개이상인 모든 파라미터형태에 대해서 대응한다.
{
	//보편참조는 인자가없는 함수에대해서는 대응하지 않는다 컴파일 에러남...
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	//placement new지정된 메모리 포인터위치에 생성자를 호출하라는 문법이다.
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	xrelease(obj);

}

