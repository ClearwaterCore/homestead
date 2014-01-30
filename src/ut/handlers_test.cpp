/**
 * @file handlers_test.cpp UT for Handlers module.
 *
 * Project Clearwater - IMS in the Cloud
 * Copyright (C) 2013  Metaswitch Networks Ltd
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version, along with the "Special Exception" for use of
 * the program along with SSL, set forth below. This program is distributed
 * in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * The author can be reached by email at clearwater@metaswitch.com or by
 * post at Metaswitch Networks Ltd, 100 Church St, Enfield EN2 6BQ, UK
 *
 * Special Exception
 * Metaswitch Networks Ltd  grants you permission to copy, modify,
 * propagate, and distribute a work formed by combining OpenSSL with The
 * Software, or a work derivative of such a combination, even if such
 * copying, modification, propagation, or distribution would otherwise
 * violate the terms of the GPL. You must comply with the GPL in all
 * respects for all of the code used other than OpenSSL.
 * "OpenSSL" means OpenSSL toolkit software distributed by the OpenSSL
 * Project and licensed under the OpenSSL Licenses, or a work based on such
 * software and licensed under the OpenSSL Licenses.
 * "OpenSSL Licenses" means the OpenSSL License and Original SSLeay License
 * under which the OpenSSL Project distributes the OpenSSL toolkit software,
 * as those licenses appear in the file LICENSE-OPENSSL.
 */

#define GTEST_HAS_POSIX_RE 0
#include "test_utils.hpp"
#include <curl/curl.h>

#include "mockdiameterstack.hpp"
#include "mockhttpstack.hpp"
#include "mockcache.hpp"
#include "handlers.h"

using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::_;
using ::testing::Invoke;
using ::testing::WithArgs;

static struct msg* _fd_msg;
static Diameter::Transaction* _tsx;

static void store (struct msg* msg, Diameter::Transaction* tsx)
{
  _fd_msg = msg;
  _tsx = tsx;
}

TEST(HandlersTest, SimpleMainline)
{
  MockHttpStack stack;
  MockHttpStack::Request req(&stack, "/", "ping");
  EXPECT_CALL(stack, send_reply(_, 200));
  PingHandler* handler = new PingHandler(req);
  handler->run();
  EXPECT_EQ("OK", req.content());
}

TEST(HandlersTest, RegistrationStatus)
{
  MockHttpStack httpstack;
  MockHttpStack::Request req(&httpstack, "/impi/impi@example.com/", "registration-status", "?impu=sip:impu@example.com");
  MockDiameterStack diameterstack;
  ImpiRegistrationStatusHandler::Config cfg(true);
  ImpiRegistrationStatusHandler* handler = new ImpiRegistrationStatusHandler(req, &cfg);
  EXPECT_CALL(diameterstack, send(_, _, 200))
    .WillOnce(WithArgs<0,1>(Invoke(store)))
    .Times(1);
  handler->run();

  Diameter::Dictionary* dict;
  MockDiameterMessage rsp(dict);
  int i32;
  std::string str;
  EXPECT_CALL(rsp, get_i32_from_avp(_, &i32))
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<1>(2001), Return(true)));
  EXPECT_CALL(rsp, experimental_result_code())
    .Times(1)
    .WillOnce(Return(0));
  EXPECT_CALL(rsp, get_str_from_avp(_, &str))
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<1>("scscf"), Return(true)));
  EXPECT_CALL(httpstack, send_reply(_, 200));
  handler->on_uar_response(rsp);

  // Build the expected JSON response
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartObject();
  writer.String(JSON_RC.c_str());
  writer.Int(2001);
  writer.String(JSON_SCSCF.c_str());
  writer.String("scscf");
  writer.EndObject();
  EXPECT_EQ(sb.GetString(), req.content());
}

TEST(HandlersTest, RegistrationStatusNoHSS)
{
  // Configure no HSS
  MockHttpStack httpstack;
  MockHttpStack::Request req(&httpstack, "/impi/impi@example.com/", "registration-status", "?impu=sip:impu@example.com");
  ImpiRegistrationStatusHandler::Config cfg(false);
  ImpiRegistrationStatusHandler* handler = new ImpiRegistrationStatusHandler(req, &cfg);
  EXPECT_CALL(httpstack, send_reply(_, 200));
  handler->run();

  // Build the expected JSON response
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartObject();
  writer.String(JSON_RC.c_str());
  writer.Int(2001);
  writer.String(JSON_SCSCF.c_str());
  // GDR - default server name
  writer.String("scscf");
  writer.EndObject();
  EXPECT_EQ(sb.GetString(), req.content());
}

#if 0
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::_;
using ::testing::Invoke;
using ::testing::WithArgs;

// Transaction that would be implemented in the handlers.
class ExampleTransaction : public Cache::Transaction
{
  void on_success(Cache::Request* req)
  {
    std::string xml;
    dynamic_cast<Cache::GetIMSSubscription*>(req)->get_result(xml);
    std::cout << "GOT RESULT: " << xml << std::endl;
  }

  void on_failure(Cache::Request* req, Cache::ResultCode rc, std::string& text)
  {
    std::cout << "FAILED:" << std::endl << text << std::endl;
  }
};

// Start of the test code.
TEST(HandlersTest, ExampleTransaction)
{
  MockCache cache;
  MockCache::MockGetIMSSubscription mock_req;

  EXPECT_CALL(cache, create_GetIMSSubscription("kermit"))
    .WillOnce(Return(&mock_req));
  EXPECT_CALL(cache, send(_, &mock_req))
    .WillOnce(WithArgs<0>(Invoke(&mock_req, &Cache::Request::set_trx)));
  EXPECT_CALL(mock_req, get_result(_))
    .WillRepeatedly(SetArgReferee<0>("<some boring xml>"));

  // This would be in the code-under-test.
  ExampleTransaction* tsx = new ExampleTransaction;
  Cache::Request* req = cache.create_GetIMSSubscription("kermit");
  cache.send(tsx, req);

  // Back to the test code.
  Cache::Transaction* t = mock_req.get_trx();
  ASSERT_FALSE(t == NULL);
  t->on_success(&mock_req);
}
#endif
