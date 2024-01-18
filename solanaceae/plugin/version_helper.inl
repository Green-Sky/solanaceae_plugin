#pragma once

namespace internal {
	template<typename T>
	class g_type_version {
		typedef char yes[1];
		typedef char no [2];

		template<typename C> static yes& test_version(decltype(&C::version));
		template<typename C> static no& test_version(...);

		static bool const has_version = sizeof(test_version<T>(nullptr)) == sizeof(yes);
		static constexpr const char* get_version(void) {
			if constexpr (has_version) {
				return T::version;
			} else {
				return "UNK"; // default version
			}
		}

		public:
			static constexpr const char* version = get_version();
	};
} // internal

