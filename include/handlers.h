/**
 * @file handlers.cpp handlers for homestead
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

#ifndef HANDLERS_H__
#define HANDLERS_H__

#include "cx.h"
#include "diameterstack.h"
#include "httpstack.h"

class PingHandler : public HttpStack::Handler
{
public:
  PingHandler() : HttpStack::Handler("^/ping$") {};
  void handle(HttpStack::Request& req);
};

class DiameterHttpHandler : public HttpStack::Handler
{
public:
  DiameterHttpHandler(const std::string& path,
                      Diameter::Stack* diameter_stack,
                      const std::string& dest_realm,
                      const std::string& dest_host,
                      const std::string& server_name) :
                      HttpStack::Handler(path),
                      _diameter_stack(diameter_stack),
                      _dest_realm(dest_realm),
                      _dest_host(dest_host),
                      _server_name(server_name) {};

  class Transaction : public Diameter::Transaction
  {
  public:
    Transaction(Cx::Dictionary* dict, HttpStack::Request& req) : Diameter::Transaction(dict), _req(req) {};
    void on_timeout();

    HttpStack::Request _req;
  };

  Diameter::Stack* _diameter_stack;
  std::string _dest_realm;
  std::string _dest_host;
  std::string _server_name;
  Cx::Dictionary _dict;
};

class ImpiDigestHandler : public DiameterHttpHandler
{
public:
  ImpiDigestHandler(Diameter::Stack* diameter_stack,
                    const std::string& dest_realm,
                    const std::string& dest_host,
                    const std::string& server_name) :
                    DiameterHttpHandler("^/impi/[^/]*/digest$",
                                        diameter_stack,
                                        dest_realm,
                                        dest_host,
                                        server_name) {};
  void handle(HttpStack::Request& req);

  class Transaction : public DiameterHttpHandler::Transaction
  {
  public:
    Transaction(Cx::Dictionary* dict, HttpStack::Request& req) : DiameterHttpHandler::Transaction(dict, req) {};
    void on_response(Diameter::Message& rsp);
  };
};

class ImpiAvHandler : public DiameterHttpHandler
{
public:
  ImpiAvHandler(Diameter::Stack* diameter_stack,
                const std::string& dest_realm,
                const std::string& dest_host,
                const std::string& server_name) :
                DiameterHttpHandler("^/impi/[^/]*/av",
                                    diameter_stack,
                                    dest_realm,
                                    dest_host,
                                    server_name) {};
  void handle(HttpStack::Request& req);

  class Transaction : public DiameterHttpHandler::Transaction
  {
  public:
    Transaction(Cx::Dictionary* dict, HttpStack::Request& req) : DiameterHttpHandler::Transaction(dict, req) {};
    void on_response(Diameter::Message& rsp);
  };
};

class ImpuIMSSubscriptionHandler : public DiameterHttpHandler
{
public:
  ImpuIMSSubscriptionHandler(Diameter::Stack* diameter_stack,
                             const std::string& dest_host,
                             const std::string& dest_realm,
                             const std::string& server_name) :
                             DiameterHttpHandler("/impu/",
                                                 diameter_stack,
                                                 dest_realm,
                                                 dest_host,
                                                 server_name) {};
  void handle(HttpStack::Request& req);

  class Transaction : public DiameterHttpHandler::Transaction
  {
  public:
    Transaction(Cx::Dictionary* dict, HttpStack::Request& req) : DiameterHttpHandler::Transaction(dict, req) {};
    void on_response(Diameter::Message& rsp);
  };
};


#endif