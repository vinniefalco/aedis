/* Copyright (c) 2018 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/assert.hpp>
#include <aedis/resp3/error.hpp>

namespace aedis {
namespace resp3 {
namespace detail {

struct error_category_impl : boost::system::error_category {

   char const* name() const noexcept override
   {
      return "aedis.resp3";
   }

   std::string message(int ev) const override
   {
      switch(static_cast<error>(ev)) {
	 case error::invalid_type: return "Invalid resp3 type.";
	 case error::not_a_number: return "Can't convert string to number.";
	 case error::unexpected_read_size: return "Unexpected read size.";
	 case error::exceeeds_max_nested_depth: return "Exceeds the maximum number of nested responses.";
	 case error::unexpected_bool_value: return "Unexpected bool value.";
	 case error::empty_field: return "Expected field value is empty.";
	 default: BOOST_ASSERT(false);
      }
   }
};

boost::system::error_category const& category()
{
  static error_category_impl instance;
  return instance;
}

} // detail

boost::system::error_code make_error_code(error e)
{
    return boost::system::error_code{static_cast<int>(e), detail::category()};
}

boost::system::error_condition make_error_condition(error e)
{
  return boost::system::error_condition(static_cast<int>(e), detail::category());
}

} // resp3
} // aedis
