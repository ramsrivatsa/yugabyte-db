// Copyright (c) YugaByte, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.
//

#ifndef YB_MASTER_UTIL_YQL_VTABLE_HELPERS_H
#define YB_MASTER_UTIL_YQL_VTABLE_HELPERS_H

#include "yb/common/ql_value.h"
#include "yb/master/master.pb.h"
#include "yb/util/net/inetaddress.h"

namespace yb {
namespace master {
namespace util {

template<class T> struct GetValueHelper;

// In some cases we need to preserve the QLValuePB.
template<> struct GetValueHelper<QLValuePB> {

  static const QLValuePB& Apply(const QLValuePB& value_pb, const DataType data_type) {
    return value_pb;
  }
};

template<> struct GetValueHelper<std::string> {

  static QLValuePB Apply(const std::string& strval, const DataType data_type) {
    QLValuePB value_pb;
    switch (data_type) {
      case STRING:
        QLValue::set_string_value(strval, &value_pb);
        break;
      case BINARY:
        QLValue::set_binary_value(strval, &value_pb);
        break;
      default:
        LOG(ERROR) << "unexpected string type " << data_type;
        break;
    }
    return value_pb;
  }
};

// Need specialization for char[N] to handle strings literals.
template<std::size_t N> struct GetValueHelper<char[N]> {

  static QLValuePB Apply(const char* strval, const DataType data_type) {
    return GetValueHelper<std::string>::Apply(strval, data_type);
  }
};

template<> struct GetValueHelper<int32_t> {

  static QLValuePB Apply(const int32_t intval, const DataType data_type) {
    QLValuePB value_pb;
    QLValue::set_int32_value(intval, &value_pb);
    return value_pb;
  }
};

template<> struct GetValueHelper<InetAddress> {

  static QLValuePB Apply(const InetAddress& inet_val, const DataType data_type) {
    QLValuePB value_pb;
    QLValue::set_inetaddress_value(inet_val, &value_pb);
    return value_pb;
  }
};

template<> struct GetValueHelper<Uuid> {

  static QLValuePB Apply(const Uuid& uuid_val, const DataType data_type) {
    QLValuePB value_pb;
    QLValue::set_uuid_value(uuid_val, &value_pb);
    return value_pb;
  }
};

template<> struct GetValueHelper<bool> {

  static QLValuePB Apply(const bool bool_val, const DataType data_type) {
    QLValuePB value_pb;
    QLValue::set_bool_value(bool_val, &value_pb);
    return value_pb;
  }
};

template<class T>
QLValuePB GetValue(const T& t, DataType data_type) {
  typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type CleanedT;
  return GetValueHelper<CleanedT>::Apply(t, data_type);
}

QLValuePB GetTokensValue(size_t index, size_t node_count);

QLValuePB GetReplicationValue(int replication_factor);

bool RemoteEndpointMatchesTServer(const TSInformationPB& ts_info,
                                  const InetAddress& remote_endpoint);

}  // namespace util
}  // namespace master
}  // namespace yb

#endif // YB_MASTER_UTIL_YQL_VTABLE_HELPERS_H
