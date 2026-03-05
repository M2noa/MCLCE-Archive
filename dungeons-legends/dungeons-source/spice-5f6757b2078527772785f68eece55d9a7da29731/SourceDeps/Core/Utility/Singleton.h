#pragma once

#include "Core/Platform/ErrorHandling.h"

// Do you *reallllly* need a singleton? Don't forget that it is unsafe to share singletons across multiple threads,
// they carry the same problems as global data. Also, any changes to this file will likely make you rebuild a
// ton of header dependencies

// Just throw this in the CPP file that uses the singleton class
#define DECLARE_SINGLETON_INSTANCE_WITH_TYPE(type) template<> \
	type::PointerType_t mce::Singleton<type>::instance = nullptr;

// Can we use 'template template arguments' instead of template aliasing to make the syntactic sugar that much more
// sweet? Probably a waste of time

namespace mce {
	namespace SingletonInternal {
		template<typename T>
		using DefaultSingletonPointer = std::unique_ptr<T>;

		// Want to use a std::share_ptr? Are you sure that makes sense with singletons?
	}

	template<typename T, typename PointerType = SingletonInternal::DefaultSingletonPointer<T> >
	class Singleton {
	public:

		typedef PointerType PointerType_t;

		static T& getInstance() {
			MCECHECK(exists(), "The singleton instance doesn't exist");

			return *instance;
		}

		static T& createInstance() {
			if (exists()) {
				MCEASSERTMESSAGEONLY("The singleton instance already exists");
				return *instance;
			}
			else {
				return internalCreateInstance();
			}
		}

		static bool exists() {
			return instance != nullptr;
		}

		static void destroyInstance() {
			internalDestroy();
		}

	protected:

		Singleton() {
			MCECHECK(exists() == false, "You attempted to create a singleton where one already exists");
		}

		~Singleton() {
		}

		static PointerType instance;

	private:

		template<typename S = PointerType, typename = typename std::enable_if<std::is_pointer<S>::value, T>::type>
		static T& internalCreateInstance() {
			instance = new T;
			return *instance;
		}

		template<typename S = PointerType, typename A = typename std::enable_if<std::is_same<S, std::unique_ptr<T> >::value, T>::type, typename Dummy = void>
		static T& internalCreateInstance() {
			instance = std::make_unique<T>();
			return *instance;
		}

		template<typename S = PointerType, typename = typename std::enable_if<std::is_pointer<S>::value, T>::type>
		static void internalDestroy() {
			delete instance;
			instance = nullptr;
		}

		template<typename S = PointerType, typename A = typename std::enable_if<std::is_same<S, std::unique_ptr<T> >::value, T>::type, typename Dummy = void>
		static void internalDestroy() {
			instance.reset();
		}

	};

}
