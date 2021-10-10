/* Copyright (c) 2019 - 2021 Marcelo Zimbres Silva (mzimbres at gmail dot com)
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <queue>
#include <vector>
#include <string>
#include <string_view>
#include <utility>

#include <aedis/command.hpp>
#include <aedis/resp3/detail/assemble.hpp>

namespace aedis {
namespace resp3 {

/** A class to compose redis requests
 *  
 *  A request is composed of one or more redis commands and is
 *  refered to in the redis documentation as a pipeline, see
 *  https://redis.io/topics/pipelining.
 *
 *  The protocol version suported is RESP3, see
 *  https://github.com/antirez/RESP3/blob/74adea588783e463c7e84793b325b088fe6edd1c/spec.md
 */
class request {
public:
   std::string payload;
   std::queue<std::pair<command, std::string>> ids;
   bool sent = false;

public:
   /// Return the size of the pipeline. i.e. how many commands it
   /// contains.
   auto size() const noexcept
      { return std::size(ids); }

   bool empty() const noexcept
      { return std::empty(payload); };

   /// Clears the request.
   void clear()
   {
      payload.clear();
      ids = {};
   }

   void ping()
   {
      detail::assemble(payload, "PING");
      ids.push(std::make_pair(command::ping, std::string{}));
   }

   void quit()
   {
      detail::assemble(payload, "QUIT");
      ids.push(std::make_pair(command::quit, std::string{}));
   }

   void multi()
   {
      detail::assemble(payload, "MULTI");
      ids.push(std::make_pair(command::multi, std::string{}));
   }

   void exec()
   {
      detail::assemble(payload, "EXEC");
      ids.push(std::make_pair(command::exec, std::string{}));
   }

   void incr(std::string_view key)
   {
      detail::assemble(payload, "INCR", key);
      ids.push(std::make_pair(command::incr, std::string{key}));
   }

   /// Adds auth to the request, see https://redis.io/commands/bgrewriteaof
   void auth(std::string_view pwd)
   {
      detail::assemble(payload, "AUTH", pwd);
      ids.push(std::make_pair(command::auth, std::string{}));
   }

   /// Adds bgrewriteaof to the request, see https://redis.io/commands/bgrewriteaof
   void bgrewriteaof()
   {
      detail::assemble(payload, "BGREWRITEAOF");
      ids.push(std::make_pair(command::bgrewriteaof, std::string{}));
   }

   /// Adds role to the request, see https://redis.io/commands/role
   void role()
   {
      detail::assemble(payload, "ROLE");
      ids.push(std::make_pair(command::role, std::string{}));
   }

   /// Adds bgsave to the request, see //https://redis.io/commands/bgsave
   void bgsave()
   {
      detail::assemble(payload, "BGSAVE");
      ids.push(std::make_pair(command::bgsave, std::string{}));
   }

   /// Adds ping to the request, see https://redis.io/commands/flushall
   void flushall()
   {
      detail::assemble(payload, "FLUSHALL");
      ids.push(std::make_pair(command::flushall, std::string{}));
   }

   /// Adds ping to the request, see https://redis.io/commands/lpop
   void lpop(std::string_view key, int count = 1)
   {
      //if (count == 1) {
	 detail::assemble(payload, "LPOP", key);
      //} else {
      //auto par = {std::to_string(count)};
      //detail::assemble(
      //   payload,
      //   "LPOP",
      //   {key},
      //   std::cbegin(par),
      //   std::cend(par));
      //}

      ids.push(std::make_pair(command::lpop, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/subscribe
   void subscribe(std::string_view key)
   {
      // The response to this command is a push.
      detail::assemble(payload, "SUBSCRIBE", key);
   }

   /// Adds ping to the request, see https://redis.io/commands/unsubscribe
   void unsubscribe(std::string_view key)
   {
      // The response to this command is a push.
      detail::assemble(payload, "UNSUBSCRIBE", key);
   }

   /// Adds ping to the request, see https://redis.io/commands/get
   void get(std::string_view key)
   {
      detail::assemble(payload, "GET", key);
      ids.push(std::make_pair(command::get, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/keys
   void keys(std::string_view pattern)
   {
      detail::assemble(payload, "KEYS", pattern);
      ids.push(std::make_pair(command::keys, std::string{}));
   }

   /// Adds ping to the request, see https://redis.io/commands/hello
   void hello(std::string_view version = "3")
   {
      detail::assemble(payload, "HELLO", version);
      ids.push(std::make_pair(command::hello, std::string{}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/sentinel
   void sentinel(std::string_view arg, std::string_view name)
   {
      auto par = {name};
      detail::assemble(payload, "SENTINEL", {arg}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::sentinel, std::string{}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/append
   void append(std::string_view key, std::string_view msg)
   {
      auto par = {msg};
      detail::assemble(payload, "APPEND", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::append, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/bitcount
   void bitcount(std::string_view key, int start = 0, int end = -1)
   {
      auto const start_str = std::to_string(start);
      auto const end_str = std::to_string(end);
      std::initializer_list<std::string_view> par {start_str, end_str};

      detail::assemble( payload
   	            , "BITCOUNT"
   		    , {key}
   		    , std::cbegin(par)
   		    , std::cend(par));
      ids.push(std::make_pair(command::bitcount, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/rpush
   template <class Iter>
   void rpush(std::string_view key, Iter begin, Iter end)
   {
      detail::assemble(payload, "RPUSH", {key}, begin, end);
      ids.push(std::make_pair(command::rpush, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/rpush
   template <class T>
   void rpush(std::string_view key, std::initializer_list<T> v)
   {
      return rpush(key, std::cbegin(v), std::cend(v));
   }

   /// Adds ping to the request, see https://redis.io/commands/rpush
   template <class Range>
   void rpush(std::string_view key, Range const& v)
   {
      using std::cbegin;
      using std::cend;
      rpush(key, cbegin(v), cend(v));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/lpush
   template <class Iter>
   void lpush(std::string_view key, Iter begin, Iter end)
   {
      detail::assemble(payload, "LPUSH", {key}, begin, end);
      ids.push(std::make_pair(command::lpush, std::string{key}));
   }
   
   void psubscribe( std::initializer_list<std::string_view> l)
   {
      std::initializer_list<std::string_view> dummy = {};
      detail::assemble(payload, "PSUBSCRIBE", l, std::cbegin(dummy), std::cend(dummy));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/publish
   void publish(std::string_view key, std::string_view msg)
   {
      auto par = {msg};
      detail::assemble(payload, "PUBLISH", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::publish, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/set
   void set(std::string_view key,
            std::initializer_list<std::string_view> args)
   {
      detail::assemble(payload, "SET", {key}, std::cbegin(args), std::cend(args));
      ids.push(std::make_pair(command::set, std::string{key}));
   }

   // TODO: Find a way to assert the value type is a pair.
   /// Adds ping to the request, see https://redis.io/commands/hset
   template <class Range>
   void hset(std::string_view key, Range const& r)
   {
      //Note: Requires an std::pair as value type, otherwise gets
      //error: ERR Protocol error: expected '$', got '*'
      using std::cbegin;
      using std::cend;
      detail::assemble(payload, "HSET", {key}, std::cbegin(r), std::cend(r), 2);
      ids.push(std::make_pair(command::hset, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hincrby
   void hincrby(std::string_view key, std::string_view field, int by)
   {
      auto by_str = std::to_string(by);
      std::initializer_list<std::string_view> par {field, by_str};
      detail::assemble(payload, "HINCRBY", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::hincrby, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hkeys
   void hkeys(std::string_view key)
   {
      auto par = {""};
      detail::assemble(payload, "HKEYS", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::hkeys, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hlen
   void hlen(std::string_view key)
   {
      detail::assemble(payload, "HLEN", {key});
      ids.push(std::make_pair(command::hlen, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/hgetall
   void hgetall(std::string_view key)
   {
      detail::assemble(payload, "HGETALL", {key});
      ids.push(std::make_pair(command::hgetall, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/hvals
   void hvals( std::string_view key)
   {
      detail::assemble(payload, "HVALS", {key});
      ids.push(std::make_pair(command::hvals, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hget
   void hget(std::string_view key, std::string_view field)
   {
      auto par = {field};
      detail::assemble(payload, "HGET", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::hget, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hmget
   void hmget(
      std::string_view key,
      std::initializer_list<std::string_view> fields)
   {
      detail::assemble( payload
   	            , "HMGET"
   		    , {key}
   		    , std::cbegin(fields)
   		    , std::cend(fields));

      ids.push(std::make_pair(command::hmget, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/hdel
   void
   hdel(std::string_view key,
        std::initializer_list<std::string_view> fields)
   {
      detail::assemble(
	 payload,
	 "HDEL",
	 {key},
	 std::cbegin(fields),
	 std::cend(fields));

      ids.push(std::make_pair(command::hdel, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/expire
   void expire(std::string_view key, int secs)
   {
      auto const str = std::to_string(secs);
      std::initializer_list<std::string_view> par {str};
      detail::assemble(payload, "EXPIRE", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::expire, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/zadd
   void zadd(std::string_view key, int score, std::string_view value)
   {
      auto const score_str = std::to_string(score);
      std::initializer_list<std::string_view> par = {score_str, value};
      detail::assemble(payload, "ZADD", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::zadd, std::string{key}));
   }
   
   /// Adds zadd to the request, see https://redis.io/commands/zadd
   template <class Range>
   void zadd(std::initializer_list<std::string_view> key, Range const& r)
   {
      detail::assemble(payload, "ZADD", key, std::cbegin(r), std::cend(r), 2);
      ids.push(std::make_pair(command::zadd, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/zrange
   void zrange(std::string_view key, int min = 0, int max = -1)
   {
      auto const min_str = std::to_string(min);
      auto const max_str = std::to_string(max);
      std::initializer_list<std::string_view> par {min_str, max_str};

      detail::assemble(payload, "ZRANGE", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::zrange, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/zrangebyscore
   void zrangebyscore(std::string_view key, int min, int max)
   {
      auto max_str = std::string {"inf"};
      if (max != -1)
         max_str = std::to_string(max);
   
      auto const min_str = std::to_string(min);
      auto par = {min_str , max_str};
      detail::assemble(payload, "ZRANGEBYSCORE", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::zrangebyscore, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/zremrangebyscore
   void
   zremrangebyscore(
      std::string_view key,
      std::string_view min,
      std::string_view max)
   {
      auto par = {min, max};
      detail::assemble(payload, "ZREMRANGEBYSCORE", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::zremrangebyscore, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/lrange
   void lrange(std::string_view key, int min = 0, int max = -1)
   {
      auto const min_str = std::to_string(min);
      auto const max_str = std::to_string(max);
      std::initializer_list<std::string_view> par {min_str, max_str};
      detail::assemble(payload, "LRANGE", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::lrange, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/ltrim
   void ltrim(std::string_view key, int min = 0, int max = -1)
   {
      auto const min_str = std::to_string(min);
      auto const max_str = std::to_string(max);
      std::initializer_list<std::string_view> par {min_str, max_str};
      detail::assemble(payload, "LTRIM", {key}, std::cbegin(par), std::cend(par));
      ids.push(std::make_pair(command::ltrim, std::string{key}));
   }
   
   // TODO: Overload for vector del.
   /// Adds ping to the request, see https://redis.io/commands/del
   void del(std::string_view key)
   {
      detail::assemble(payload, "DEL", key);
      ids.push(std::make_pair(command::del, std::string{key}));
   }
   
   /// Adds ping to the request, see https://redis.io/commands/llen
   void llen(std::string_view key)
   {
      detail::assemble(payload, "LLEN", key);
      ids.push(std::make_pair(command::llen, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/sadd
   template <class Iter>
   void sadd(std::string_view key, Iter begin, Iter end)
   {
      detail::assemble(payload, "SADD", {key}, begin, end);
      ids.push(std::make_pair(command::sadd, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/sadd
   template <class Range>
   void sadd(std::string_view key, Range const& r)
   {
      using std::cbegin;
      using std::cend;
      sadd(key, cbegin(r), cend(r));
   }

   /// Adds ping to the request, see https://redis.io/commands/smembers
   void smembers(std::string_view key)
   {
      detail::assemble(payload, "SMEMBERS", key);
      ids.push(std::make_pair(command::smembers, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/scard
   void scard(std::string_view key)
   {
      detail::assemble(payload, "SCARD", key);
      ids.push(std::make_pair(command::scard, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/scard
   void scard(std::string_view key, std::initializer_list<std::string_view> l)
   {
      detail::assemble(payload, "SDIFF", {key}, std::cbegin(l), std::cend(l));
      ids.push(std::make_pair(command::sdiff, std::string{key}));
   }

   /// Adds ping to the request, see https://redis.io/commands/client_id
   void client_id(std::string_view parameters)
   {
      detail::assemble(payload, "CLIENT ID", {parameters});
      ids.push(std::make_pair(command::client_id, std::string{}));
   }
};

/** Prepares the back of a queue to receive further commands and
 *  returns true if a write is possible.
 */
bool prepare_next(std::queue<request>& reqs);

} // resp3
} // aedis
