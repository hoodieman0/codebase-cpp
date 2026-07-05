module;

#include <expected>
#include <system_error>

export module network.result;

export namespace network {

template<typename T>
using Result = std::expected<T, std::error_code>;

} // namespace network
