/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_ARCH_X86_64_QUICK_METHOD_FRAME_INFO_X86_64_H_
#define ART_RUNTIME_ARCH_X86_64_QUICK_METHOD_FRAME_INFO_X86_64_H_

#include <museum/8.0.0/art/runtime/base/bit_utils.h>
#include <museum/8.0.0/art/runtime/quick/quick_method_frame_info.h>
#include <museum/8.0.0/art/runtime/arch/x86_64/registers_x86_64.h>
#include <museum/8.0.0/art/runtime/runtime.h>  // for Runtime::CalleeSaveType.

namespace facebook { namespace museum { namespace MUSEUM_VERSION { namespace art {
namespace x86_64 {

static constexpr uint32_t kX86_64CalleeSaveAlwaysSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::kNumberOfCpuRegisters);  // Fake return address callee save.
static constexpr uint32_t kX86_64CalleeSaveRefSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RBX) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RBP) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R12) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R13) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R14) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R15);
static constexpr uint32_t kX86_64CalleeSaveArgSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RSI) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RDX) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RCX) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R8) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R9);
static constexpr uint32_t kX86_64CalleeSaveEverythingSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RAX) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RCX) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RDX) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RSI) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::RDI) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R8) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R9) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R10) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::R11);

static constexpr uint32_t kX86_64CalleeSaveFpArgSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM0) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM1) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM2) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM3) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM4) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM5) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM6) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM7);
static constexpr uint32_t kX86_64CalleeSaveFpSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM12) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM13) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM14) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM15);
static constexpr uint32_t kX86_64CalleeSaveFpEverythingSpills =
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM0) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM1) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM2) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM3) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM4) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM5) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM6) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM7) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM8) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM9) |
    (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM10) | (1 << facebook::museum::MUSEUM_VERSION::art::x86_64::XMM11);

constexpr uint32_t X86_64CalleeSaveCoreSpills(Runtime::CalleeSaveType type) {
  return kX86_64CalleeSaveAlwaysSpills | kX86_64CalleeSaveRefSpills |
      (type == Runtime::kSaveRefsAndArgs ? kX86_64CalleeSaveArgSpills : 0) |
      (type == Runtime::kSaveEverything ? kX86_64CalleeSaveEverythingSpills : 0);
}

constexpr uint32_t X86_64CalleeSaveFpSpills(Runtime::CalleeSaveType type) {
  return kX86_64CalleeSaveFpSpills |
      (type == Runtime::kSaveRefsAndArgs ? kX86_64CalleeSaveFpArgSpills : 0) |
      (type == Runtime::kSaveEverything ? kX86_64CalleeSaveFpEverythingSpills : 0);
}

constexpr uint32_t X86_64CalleeSaveFrameSize(Runtime::CalleeSaveType type) {
  return RoundUp((POPCOUNT(X86_64CalleeSaveCoreSpills(type)) /* gprs */ +
                  POPCOUNT(X86_64CalleeSaveFpSpills(type)) /* fprs */ +
                  1 /* Method* */) * static_cast<size_t>(kX86_64PointerSize), kStackAlignment);
}

constexpr QuickMethodFrameInfo X86_64CalleeSaveMethodFrameInfo(Runtime::CalleeSaveType type) {
  return QuickMethodFrameInfo(X86_64CalleeSaveFrameSize(type),
                              X86_64CalleeSaveCoreSpills(type),
                              X86_64CalleeSaveFpSpills(type));
}

}  // namespace x86_64
} } } } // namespace facebook::museum::MUSEUM_VERSION::art

#endif  // ART_RUNTIME_ARCH_X86_64_QUICK_METHOD_FRAME_INFO_X86_64_H_
