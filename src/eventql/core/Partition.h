/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_TSDB_STREAMCHUNK_H
#define _FNORD_TSDB_STREAMCHUNK_H
#include <eventql/util/stdtypes.h>
#include <eventql/util/option.h>
#include <eventql/util/UnixTime.h>
#include <eventql/util/protobuf/MessageSchema.h>
#include <eventql/util/http/httpconnectionpool.h>
#include <eventql/core/ServerConfig.h>
#include <eventql/core/Table.h>
#include <eventql/core/RecordRef.h>
#include <eventql/core/PartitionInfo.pb.h>
#include <eventql/core/PartitionSnapshot.h>
#include <eventql/core/PartitionWriter.h>
#include <eventql/core/PartitionReader.h>
#include <eventql/core/ReplicationScheme.h>
#include <cstable/CSTableReader.h>

using namespace stx;

namespace zbase {
class Table;
class PartitionReplication;

using PartitionKey =
    std::tuple<
        String,     // namespace
        String,     // table
        SHA1Hash>;  // partition

class Partition : public RefCounted {
public:

  static RefPtr<Partition> create(
      const String& tsdb_namespace,
      RefPtr<Table> table,
      const SHA1Hash& partition_key,
      ServerConfig* cfg);

  static RefPtr<Partition> reopen(
      const String& tsdb_namespace,
      RefPtr<Table> table,
      const SHA1Hash& partition_key,
      ServerConfig* cfg);

  Partition(
      ServerConfig* cfg,
      RefPtr<PartitionSnapshot> snap,
      RefPtr<Table> table);

  ~Partition();

  SHA1Hash uuid() const;

  RefPtr<PartitionWriter> getWriter();
  RefPtr<PartitionReader> getReader();
  RefPtr<PartitionSnapshot> getSnapshot();
  RefPtr<Table> getTable();
  PartitionInfo getInfo() const;

  RefPtr<PartitionReplication> getReplicationStrategy(
      RefPtr<ReplicationScheme> repl_scheme,
      http::HTTPConnectionPool* http);

  String getRelativePath() const;
  String getAbsolutePath() const;

protected:

  bool upgradeToLSMv2() const;

  ServerConfig* cfg_;
  PartitionSnapshotRef head_;
  RefPtr<Table> table_;
  RefPtr<PartitionWriter> writer_;
  std::mutex writer_lock_;
};

}
#endif