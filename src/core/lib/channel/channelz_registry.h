/*
 *
 * Copyright 2017 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GRPC_CORE_LIB_CHANNEL_CHANNELz_REGISTRY_H
#define GRPC_CORE_LIB_CHANNEL_CHANNELz_REGISTRY_H

#include <grpc/impl/codegen/port_platform.h>

#include "src/core/lib/avl/avl.h"
#include "src/core/lib/channel/channel_trace.h"

#include <stdint.h>

namespace grpc_core {

// singleton registry object to track all objects that are needed to support
// channelz bookkeeping. All objects share globally distributed uuids.
class ChannelzRegistry {
 public:
  // These functions ensure singleton like behavior. We cannot use the normal
  // pattern of a get functions with a static function variable due to build
  // complications.

  // To be called in grpc_init()
  static void Init();

  // To be callen in grpc_shutdown();
  static void Shutdown();

  // Returned the singleton instance of ChannelzRegistry;
  static ChannelzRegistry* Default();

  // globally registers a channelz Object. Returns its unique uuid
  template <typename Object>
  intptr_t Register(Object* object) {
    intptr_t prior = gpr_atm_no_barrier_fetch_add(&uuid_, 1);
    gpr_mu_lock(&mu_);
    avl_ = grpc_avl_add(avl_, (void*)prior, object, nullptr);
    gpr_mu_unlock(&mu_);
    return prior;
  }

  // globally unregisters the object that is associated to uuid.
  void Unregister(intptr_t uuid);

  // if object with uuid has previously been registered, returns the
  // Object associated with that uuid. Else returns nullptr.
  template <typename Object>
  Object* Get(intptr_t uuid) {
    gpr_mu_lock(&mu_);
    Object* ret =
        static_cast<Object*>(grpc_avl_get(avl_, (void*)uuid, nullptr));
    gpr_mu_unlock(&mu_);
    return ret;
  }

 private:
  GPRC_ALLOW_CLASS_TO_USE_NON_PUBLIC_NEW
  GPRC_ALLOW_CLASS_TO_USE_NON_PUBLIC_DELETE

  gpr_mu mu_;
  grpc_avl avl_;
  gpr_atm uuid_;

  ChannelzRegistry();
  ~ChannelzRegistry();
};

}  // namespace grpc_core

#endif /* GRPC_CORE_LIB_CHANNEL_CHANNELz_REGISTRY_H */
