#pragma once

#include <algorithm>
#include <tuple>

namespace noice {
template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...)->overload<Ts...>;

template <class Cont, class Pred>
void erase_if(Cont&& c, Pred&& p) {
	c.erase(std::remove_if(begin(c), end(c), p), end(c));
}

template <class Cont, class V>
void erase(Cont&& c, V&& value) {
	c.erase(std::remove(c.begin(), c.end(), value), c.end());
}

template<class F>
auto curry(F&& f) {
  return[cap = std::forward<F>(f)](auto&& x) {
    return std::apply(cap, std::forward<decltype(x)>(x));
  };
}

}