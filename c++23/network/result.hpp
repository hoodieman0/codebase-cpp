#ifndef NETWORK_RESULT
#define NETWORK_RESULT

#include <expected>
#include <system_error>

namespace mok::network {

template<typename T>
using Result = std::expected<T, std::error_code>;

} // namespace network

#endif
