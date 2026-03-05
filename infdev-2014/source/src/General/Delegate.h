#pragma once

#include "DelegateBase.h"

#include <type_traits>
#include <memory>

namespace cppu
{
	template <typename T> class delegate;
	template <typename T> class multicast_delegate;

	template<typename RET, typename ...PARAMS>
	class delegate<RET(PARAMS...)> final : private impl::delegate_base<RET(PARAMS...)>
	{
		friend class multicast_delegate<RET(PARAMS...)>;
	private:
		typename impl::delegate_base<RET(PARAMS...)>::InvocationElement invocation;
		unsigned short lambda_object_size = 0;

	public:

		delegate() = default;
		~delegate()
		{
			if (lambda_object_size)
				free(invocation.object);
		}

		bool isNull() const { return invocation.stub == nullptr; }
		operator bool() const { return invocation.stub != nullptr; }

		bool operator ==(void* ptr) const { return (ptr == nullptr) && this->isNull(); } //operator ==
		bool operator !=(void* ptr) const { return (ptr != nullptr) || (!this->isNull()); } //operator !=

		delegate(const delegate& copy)
		{
			invocation.stub = copy.invocation.stub;
			if (lambda_object_size)
				reinterpret_cast<void(*)(void*)>(reinterpret_cast<uintptr_t>(invocation.object) + lambda_object_size)(invocation.object);

			if (lambda_object_size = copy.lambda_object_size)
			{
				invocation.object = malloc(lambda_object_size + sizeof(void(*)(void*)));
				memcpy(invocation.object, copy.invocation.object, lambda_object_size + sizeof(void(*)(void*)));
			}
			else
				invocation.object = copy.invocation.object;
		}

		delegate(delegate&& move)
		{
			std::swap(lambda_object_size, move.lambda_object_size);
			std::swap(invocation.stub, move.invocation.stub);
			std::swap(invocation.object, move.invocation.object);
		}

		template <typename LAMBDA>
		delegate(const LAMBDA& lambda)
		{
			if constexpr (sizeof(LAMBDA) < sizeof(invocation.object))
			{
				lambda_object_size = 0;
				memcpy(invocation.object, &lambda, sizeof(LAMBDA));
				invocation.stub = lambda_local_stub<LAMBDA>;
			}
			else // we need to store the lambda's state
			{
				lambda_object_size = sizeof(LAMBDA);
				void* l = malloc(sizeof(LAMBDA));
				memcpy(l, &lambda, sizeof(LAMBDA));
				assign(l, lambda_stub<LAMBDA>);
			}
		}

		delegate(RET(* method)(PARAMS...))
		{
			assign(nullptr, method);
		}

		delegate& operator =(const delegate& copy)
		{
			invocation.stub = copy.invocation.stub;
			if (lambda_object_size)
				reinterpret_cast<void(*)(void*)>(reinterpret_cast<uintptr_t>(invocation.object) + lambda_object_size)(invocation.object);

			if (lambda_object_size = copy.lambda_object_size)
			{
				invocation.object = malloc(lambda_object_size + sizeof(void(*)(void*)));
				memcpy(invocation.object, copy.invocation.object, lambda_object_size + sizeof(void(*)(void*)));
			}
			else
				invocation.object = copy.invocation.object;

			return *this;
		}

		delegate& operator =(delegate&& move)
		{
			std::swap(lambda_object_size, move.lambda_object_size);
			std::swap(invocation.stub, move.invocation.stub);
			std::swap(invocation.object, move.invocation.object);

			return *this;
		}

		template <typename LAMBDA> // template instantiation is not needed, will be deduced (inferred):
		delegate& operator =(const LAMBDA& lambda)
		{
			if (lambda_object_size)
				reinterpret_cast<void(*)(void*)>(reinterpret_cast<uintptr_t>(invocation.object) + lambda_object_size)(invocation.object);

			if constexpr (sizeof(LAMBDA) < sizeof(invocation.object))
			{
				lambda_object_size = 0;
				memcpy(invocation.object, &lambda, sizeof(LAMBDA));
				invocation.stub = lambda_local_stub<LAMBDA>;
			}
			else // we need to store the lambda's state
			{
				lambda_object_size = sizeof(LAMBDA);
				void* l = malloc(sizeof(LAMBDA));
				memcpy(l, &lambda, sizeof(LAMBDA));
				assign(l, lambda_stub<LAMBDA>);
			}
			return *this;
		}

		bool operator == (const delegate& another) const { return invocation == another.invocation; }
		bool operator != (const delegate& another) const { return invocation != another.invocation; }

		bool operator ==(const multicast_delegate<RET(PARAMS...)>& another) const { return another == (*this); }
		bool operator !=(const multicast_delegate<RET(PARAMS...)>& another) const { return another != (*this); }

		template <class T, RET(T::* TMethod)(PARAMS...)>
		static delegate create(T* instance)
		{
			return delegate(instance, method_stub<T, TMethod>);
		}

		template <class T, RET(T::* TMethod)(PARAMS...) const>
		static delegate create(T const* instance)
		{
			return delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
		}

		template <RET(*TMethod)(PARAMS...)>
		static delegate create()
		{
			return delegate(nullptr, function_stub<TMethod>);
		}

		template <typename LAMBDA>
		static delegate create(const LAMBDA& lambda)
		{
			if constexpr (sizeof(LAMBDA) < sizeof(invocation.object))
			{
				lambda_object_size = 0;
				memcpy(invocation.object, &lambda, sizeof(LAMBDA));
				invocation.stub = lambda_local_stub<LAMBDA>;
			}
			else // we need to store the lambda's state
			{
				lambda_object_size = sizeof(LAMBDA);
				void* l = malloc(sizeof(LAMBDA));
				memcpy(l, &lambda, sizeof(LAMBDA));
				return delegate(l, lambda_stub<LAMBDA>);
			}
		}

		RET operator()(PARAMS... arg) const
		{
			return (*invocation.stub)(invocation.object, arg...);
		}

	private:

		delegate(void* anObject, typename impl::delegate_base<RET(PARAMS...)>::stub_type aStub)
		{
			invocation.object = anObject;
			invocation.stub = aStub;
		}

		void assign(void* anObject, typename impl::delegate_base<RET(PARAMS...)>::stub_type aStub)
		{
			this->invocation.object = anObject;
			this->invocation.stub = aStub;
		}

		template <class T, RET(T::* TMethod)(PARAMS...)>
		static RET method_stub(void* this_ptr, PARAMS... params)
		{
			T* p = static_cast<T*>(this_ptr);
			return (p->*TMethod)(params...);
		}

		template <class T, RET(T::* TMethod)(PARAMS...) const>
		static RET const_method_stub(void* this_ptr, PARAMS... params)
		{
			const T* p = static_cast<T*>(this_ptr);
			return (p->*TMethod)(params...);
		}

		template <RET(*TMethod)(PARAMS...)>
		static RET function_stub(void* this_ptr, PARAMS... params)
		{
			return (TMethod)(params...);
		}

		template <typename LAMBDA>
		static RET lambda_stub(void* this_ptr, PARAMS... params)
		{
			LAMBDA* p = static_cast<LAMBDA*>(this_ptr);
			return (p->operator())(params...);
		}

		template <typename LAMBDA>
		static RET lambda_local_stub(void* this_ptr, PARAMS... params)
		{
			LAMBDA& p = static_cast<LAMBDA&>(this_ptr);
			return (p.operator())(params...);
		}

	};

	/**
	* A delegateFactory is used to create a delegate for a certain method call.
	* It takes care of setting up the function that will cast the object that is stored
	* inside the delegate back to the correct type.
	*/
	template<auto F>
	struct DelegateFactory
	{	
		static decltype(F) Create() {}
		static decltype(F) Create(void*) {}
	};

	template<typename T, typename return_type, typename... params, return_type(T::* F)(params...)>
	struct DelegateFactory<F>
	{
		static delegate<return_type(params...)> Create(T* ptr)
		{
			return cppu::delegate<return_type(params...)>::template create<T, F>(ptr);
		}
	};

	template<typename T, typename return_type, typename... params, return_type(T::* F)(params...) const>
	struct DelegateFactory<F>
	{
		static delegate<return_type(params...)> Create(T* ptr)
		{
			return cppu::delegate<return_type(params...)>::template create<T, F>(ptr);
		}
	};

	template<typename return_type, typename... params, return_type(*F)(params...)>
	struct DelegateFactory<F>
	{
		static delegate<return_type(params...)> Create()
		{
			return cppu::delegate<return_type(params...)>::template create<F>();
		}
		
		
	};

	template <typename ReturnType, typename... Args>
	auto CallIt(ReturnType(*method)(Args...)) -> ReturnType(*)(Args...)
	{
		return method;
	}

	template<typename Lambda>
	auto CreateLambda(const Lambda& func)
	{
		typedef delegate<decltype(cppu::CallIt<decltype(&Lambda::operator())>)> dele;
		return dele::create(func);
	}

	/*template<typename lambda>
	struct DelegateFactory<1.f>
	{
		static auto Create(const lambda& F)
		{
			return cppu::delegate<decltype(&decltype(F)::operator())>::create(F);
		}
	};*/

#define DELEGATE(func, ...) DelegateFactory<func>::Create(__VA_ARGS__)
#define DELEGATE_LAMBDA(func) CreateLambda(func)
//#define DELEGATE(func, thisPrtRef) Makedelegate(func, thisPrtRef)

	//#define DELEGATE(func, thisPrtRef) (DelegateFactory(func).Create<func>(&thisPrtRef))
	//#define DELEGATE_CONST(func, thisPrtRef) (MakedelegateC(func).CreateC<func>(&thisPrtRef))
//#define DELEGATE(func) (MakedelegateFree(func).CreateForFunction<func>())
}

#ifdef CPPU_USE_NAMESPACE
using namespace cppu;
#endif