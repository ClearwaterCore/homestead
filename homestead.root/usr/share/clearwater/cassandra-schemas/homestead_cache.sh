#!/bin/bash
. /etc/clearwater/config

keyspace=$(basename $0|sed -e 's#^\(.*\)[.]sh$#\1#')
. /etc/clearwater/config
if [ ! -z $signaling_namespace ]; then
    if [ $EUID -ne 0 ]; then
        echo "When using multiple networks, schema creation must be run as root"
        exit 2
    fi
    namespace_prefix="ip netns exec $signaling_namespace"
fi

$(dirname $0)/../bin/wait4cassandra ${keyspace}
if [ $? -ne 0 ]; then
    exit 1
 fi

if [[ ! -e /var/lib/cassandra/data/homestead_cache ]];
then
  echo "CREATE KEYSPACE homestead_cache WITH REPLICATION =  {'class': 'SimpleStrategy', 'replication_factor': 2};
        USE homestead_cache;
        CREATE TABLE impi (private_id text PRIMARY KEY, digest_ha1 text, digest_realm text, digest_qop text, known_preferred boolean) WITH COMPACT STORAGE AND read_repair_chance = 1.0;
        CREATE TABLE impu (public_id text PRIMARY KEY, ims_subscription_xml text, is_registered boolean) WITH COMPACT STORAGE AND read_repair_chance = 1.0;" | $namespace_prefix cqlsh
fi

echo "USE ${keyspace};
      ALTER TABLE impu ADD primary_ccf text;
      ALTER TABLE impu ADD secondary_ccf text;
      ALTER TABLE impu ADD primary_ecf text;
      ALTER TABLE impu ADD secondary_ecf text;" | $namespace_prefix cqlsh

if [[ ! -e /var/lib/cassandra/data/homestead_cache/impi_mapping ]];
then
  echo "USE homestead_cache;
        CREATE TABLE impi_mapping (private_id text PRIMARY KEY, unused text) WITH COMPACT STORAGE AND read_repair_chance = 1.0;" | $namespace_prefix cqlsh
fi

if [ -z "$speculative_retry_value" ]
then
  speculative_retry_value="50ms"
fi

echo "USE homestead_cache;
      ALTER TABLE impu WITH speculative_retry = '$speculative_retry_value';
      ALTER TABLE impi WITH speculative_retry = '$speculative_retry_value';" | /usr/share/clearwater/bin/run-in-signaling-namespace cqlsh
