#include <iface_reg/ns.hpp>
#include <iface_reg/type_list.hpp>

static_assert(irg::detail::find<int, int, double, int *>() == 0);
static_assert(irg::detail::find<double, int, double, int *>() == 1);
static_assert(irg::detail::find<int *, int, double, int *>() == 2);
static_assert(irg::detail::find<char, int, double, int *>() == 3);
