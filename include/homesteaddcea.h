/**
 * @file homesteaddcea.h  Sprout Craft Log declarations.
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

#ifndef _HOMESTEADDCEA_H__
#define _HOMESTEADDCEA_H__

#include <string>
#include "craft_dcea.h"

// Homestead syslog identities
/**********************************************************
/ log_id
/ severity
/ Description: (formatted)
/ Cause: 
/ Effect:
/ Action:
**********************************************************/
static const PDLog CL_HOMESTEAD_INVALID_SAS_OPTION
(
  PDLogBase::CL_HOMESTEAD_ID + 1,
  PDLOG_ERR,
   "The sas_server option in /etc/clearwater/config is invalid or not configured",
   "The interface to the SAS is not specified.",
   "No call traces will appear in the sas",
   2,
   "Set the fully qualified sas hostname for the sas_server=<hostname>.<zone> option.",
   "Consult the Clearwater Core Installation document."
);
static const PDLog CL_HOMESTEAD_HELP_OPTION_EXIT
(
  PDLogBase::CL_HOMESTEAD_ID + 2,
  PDLOG_ERR,
  "Help(h) option causes program to exit",
  "Homestead was started with the Help(h) option.",
  "Homestead will exit.",
   1,
  "The Help option is not normally used."
);
static const PDLog1<char> CL_HOMESTEAD_INVALID_OPTION_C
(
  PDLogBase::CL_HOMESTEAD_ID + 3,
  PDLOG_ERR,
  "Fatal - Unknown command line option %c.  Run with --help for options.",
  "There was an invalid command line option in /etc/clearwater/config",
  "Homestead will exit",
  1,
  "Correct the /etc/clearwater/config file."
);
static const PDLog1<const char*> CL_HOMESTEAD_CRASH
(
  PDLogBase::CL_HOMESTEAD_ID + 4,
  PDLOG_ERR,
  "Fatal - Homestead has exited or crashed with signal %s",
  "Homestead has encountered a fatal software error or has been terminated",
  "The Homestead application will restart.",
   2,
   "This error can occur if Homestead has been terminated by operator command.",
   "Actual crashes such as abort, segment trap, bus error trap, should be reported as a problem. "
);
static const PDLog CL_HOMESTEAD_STARTED
(
  PDLogBase::CL_HOMESTEAD_ID + 5,
  PDLOG_NOTICE,
  "Homestead started",
  "The Homestead application is starting",
  "Normal",
   1,
  "None"
);
static const PDLog1<int> CL_HOMESTEAD_CASSANDRA_CACHE_INIT_FAIL
(
  PDLogBase::CL_HOMESTEAD_ID + 6,
  PDLOG_ERR,
  "Fatal - Failed to initialize the cache for the CassandraStore - error code %d",
  "The memory cache used to access Cassandra could not be initialized.",
  "Homestead will exit.",
   3,
  "(1). Try restarting Homestead to see if the problem clears.",
  "(2). Try reinstalling Homestead and starting Homestead to see if the problem clears.",
  "(3). If there is still a problem report the issue."
);
static const PDLog2<const char*, int> CL_HOMESTEAD_DIAMETER_INIT_FAIL
(
  PDLogBase::CL_HOMESTEAD_ID + 7,
  PDLOG_ERR,
  "Fatal - Failed to initialize Diameter stack in function %s with error %d",
  "The Diameter interface could not be initialized or encountered an error while running",
  "Homestead will exit",
   3,
  "(1).  Check the configuration for the hss in/etc/clearwater/config.",
  "Consult the Clearwater Core Installation document."
  "(2).  Check the connectivity to the hss using Wireshark."
);
static const PDLog2<const char*, int> CL_HOMESTEAD_HTTP_INIT_FAIL
(
  PDLogBase::CL_HOMESTEAD_ID + 8,
  PDLOG_ERR,
  "Fatal - Failed to initialize HttpStack stack in function %s with error %d",
  "The HTTP interfaces could not be initialized.",
  "This affects the handling of impu and impi requests from Sprout.  Call processing will not work.",
   3,
  "(1). Check the hs_realm setting in /etc/clearwater/config for correctness.",
  "(2). Restart Homestead to see if the problem clears.",
  "(3). Report the issue if the problem cannot be resolved."
);
static const PDLog CL_HOMESTEAD_ENDED
(
  PDLogBase::CL_HOMESTEAD_ID + 9,
  PDLOG_ERR,
  "Fatal - Termination signal received - terminating",
  "Homestead could have been stopped or Homestead could have been restarted by monit due to a timeout.",
  "Homestead will exit.",
   3,
  "(1). For a command initiated stop the monit log will indicate a stop on user request",
  "Consult the Clearwater Core Installation document."
  "(2).  Report the issue if there was a timeout. "
);
static const PDLog2<const char*, int> CL_HOMESTEAD_HTTP_STOP_FAIL
(
  PDLogBase::CL_HOMESTEAD_ID + 10,
  PDLOG_ERR,
  "The HTTP interfaces encountered an error when stopping the HTTP stack in %s with error %d",
  "When Homestead was exiting it encountered an error when shutting down the HTTP stack.",
  "Not critical as Homestead is exiting anyway.",
  1,
  "Report this issue."
);
static const PDLog2<const char*, int> CL_HOMESTEAD_DIAMETER_STOP_FAIL
(
  PDLogBase::CL_HOMESTEAD_ID + 11,
  PDLOG_ERR,
  "Failed to stop Diameter stack in function %s with error %d",
  "The Diameter interface encountered an error when shutting down the Diameter interface.",
  "Not critical as Homestead is exiting anyway.",
   1,
  "Report this issue."
);




#endif
