/*
 * Copyright 2019 IBM Corp. and others
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
 */

#include "em-module.hpp"

#include <time.h>
#include <sys/uio.h>

#include <tuple>
#include <stdexcept>

using namespace wabt;
using interp::Environment;
using interp::Memory;
using interp::HostModule;
using interp::HostFunc;
using interp::FuncSignature;
using interp::TypedValue;
using interp::TypedValues;
using interp::Value;

void AppendEmscriptenModule(Environment* env) {
  HostModule* module = env->AppendHostModule("env");

  Memory* memory = nullptr;
  Index memIndex = 0;
  std::tie(memory, memIndex) = module->AppendMemoryExport("memory", Limits{256, 256});

  module->on_unknown_func_export=
        [](Environment* env, HostModule* host_module, string_view name, Index sig_index) -> Index {
            printf("Importing unimplemented host function '%s' from '%s' module\n", name.to_string().c_str(), host_module->name.c_str());
            auto name_s = name.to_string(); // cached copy of name to avoid reading bad values from string_view
            auto callback = host_module->AppendFuncExport(
                    name,
                    sig_index,
                    [=](const HostFunc* func, const FuncSignature* sig, const TypedValues& args, TypedValues& results) -> interp::Result {
                        printf("Call to unimplemented host function '%s' from '%s' module!!!\n", name_s.c_str(), host_module->name.c_str());
                        return interp::Result::TrapUnreachable;
                    });
            return callback.second;
        };

  module->AppendFuncExport("_time"
                          ,{{Type::I32}, {Type::I32}}
                          ,[=](const HostFunc* func, const FuncSignature* sig, const TypedValues& args, TypedValues& results) -> interp::Result {
                             auto t = static_cast<uint32_t>(time(nullptr));
                             auto addr = args.at(0).get_i32();
                             if (addr != 0) {
                                memcpy(memory->data.data() + addr, &t, sizeof(uint32_t));
                             }
                             results.at(0).set_i32(t);
                             return interp::Result::Ok;
                          });
}
