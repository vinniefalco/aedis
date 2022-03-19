/* Copyright (c) 2019 Marcelo Zimbres Silva (mzimbres at gmail dot com)
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string_view>

#include <aedis/config.hpp>
#include <aedis/resp3/detail/parser.hpp>
#include <boost/core/ignore_unused.hpp>

namespace aedis {
namespace resp3 {
namespace detail {

#include <boost/asio/yield.hpp>

template <
   class AsyncReadStream,
   class DynamicBuffer,
   class ResponseAdapter>
class parse_op {
private:
   AsyncReadStream& stream_;
   DynamicBuffer buf_;
   parser<ResponseAdapter> parser_;
   std::size_t consumed_;
   std::size_t buffer_size_;
   net::coroutine coro_;

public:
   parse_op(AsyncReadStream& stream, DynamicBuffer buf, ResponseAdapter adapter)
   : stream_ {stream}
   , buf_ {buf}
   , parser_ {adapter}
   , consumed_{0}
   { }

   template <class Self>
   void operator()( Self& self
                  , boost::system::error_code ec = {}
                  , std::size_t n = 0)
   {
      reenter (coro_) for (;;) {
         if (parser_.bulk() == type::invalid) {
            yield
            net::async_read_until(stream_, buf_, "\r\n", std::move(self));

            if (ec) {
               self.complete(ec, 0);
               return;
            }
         } else {
	    // On a bulk read we can't read until delimiter since the
	    // payload may contain the delimiter itself so we have to
	    // read the whole chunk. However if the bulk blob is small
	    // enough it may be already on the buffer (from the last
	    // read), in which case there is no need of initiating
	    // another async op, otherwise we have to read the missing
	    // bytes.
            if (std::size(buf_) < (parser_.bulk_length() + 2)) {
               buffer_size_ = std::size(buf_);
               buf_.grow(parser_.bulk_length() + 2 - buffer_size_);

               yield
               net::async_read(
                  stream_,
                  buf_.data(buffer_size_, parser_.bulk_length() + 2 - buffer_size_),
                  net::transfer_all(),
                  std::move(self));

               if (ec) {
                  self.complete(ec, 0);
                  return;
               }
            }

            n = parser_.bulk_length() + 2;
            assert(std::size(buf_) >= n);
         }

         n = parser_.advance((char const*)buf_.data(0, n).data(), n, ec);
         if (ec) {
            self.complete(ec, 0);
            return;
         }

         buf_.consume(n);
         consumed_ += n;
         if (parser_.done()) {
            self.complete({}, consumed_);
            return;
         }
      }
   }
};

template <class AsyncReadStream, class DynamicBuffer>
class type_op {
private:
   AsyncReadStream& stream_;
   DynamicBuffer buf_;
   net::coroutine coro_;

public:
   type_op(AsyncReadStream& stream, DynamicBuffer buf)
   : stream_ {stream}
   , buf_ {buf}
   { }

   template <class Self>
   void operator()( Self& self
                  , boost::system::error_code ec = {}
                  , std::size_t n = 0)
   {
      reenter (coro_) {

         boost::ignore_unused(n);
         if (std::size(buf_) == 0) {
            yield net::async_read_until(stream_, buf_, "\r\n", std::move(self));
            if (ec) {
               self.complete(ec, type::invalid);
               return;
            }
         }

         auto const* data = (char const*)buf_.data(0, n).data();
         auto const type = to_type(*data);
         self.complete(ec, type);
      }
   }
};

#include <boost/asio/unyield.hpp>

} // detail
} // resp3
} // aedis
