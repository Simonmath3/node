// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_EXECUTION_S390_FRAME_CONSTANTS_S390_H_
#define V8_EXECUTION_S390_FRAME_CONSTANTS_S390_H_

#include "src/base/bits.h"
#include "src/base/macros.h"
#include "src/execution/frame-constants.h"

namespace v8 {
namespace internal {

class EntryFrameConstants : public AllStatic {
 public:
  static constexpr int kCallerFPOffset =
      -(StandardFrameConstants::kFixedFrameSizeFromFp + kSystemPointerSize);
  // Stack offsets for arguments passed to JSEntry.
  static constexpr int kArgvOffset = 20 * kSystemPointerSize;
};

class WasmCompileLazyFrameConstants : public TypedFrameConstants {
 public:
  static constexpr int kNumberOfSavedGpParamRegs = 4;
#ifdef V8_TARGET_ARCH_S390X
  static constexpr int kNumberOfSavedFpParamRegs = 4;
#else
  static constexpr int kNumberOfSavedFpParamRegs = 2;
#endif

  // FP-relative.
  static constexpr int kWasmInstanceOffset = TYPED_FRAME_PUSHED_VALUE_OFFSET(0);
  static constexpr int kFixedFrameSizeFromFp =
      TypedFrameConstants::kFixedFrameSizeFromFp +
      kNumberOfSavedGpParamRegs * kSystemPointerSize +
      kNumberOfSavedFpParamRegs * kDoubleSize;
};

// Frame constructed by the {WasmDebugBreak} builtin.
// After pushing the frame type marker, the builtin pushes all Liftoff cache
// registers (see liftoff-assembler-defs.h).
class WasmDebugBreakFrameConstants : public TypedFrameConstants {
 public:
  // {r2, r3, r4, r5, r6, r7, r8}
  static constexpr uint32_t kPushedGpRegs = 0b111111100;
  // {d0 .. d12}
  static constexpr uint32_t kPushedFpRegs = 0b1111111111111;

  static constexpr int kNumPushedGpRegisters =
      base::bits::CountPopulation(kPushedGpRegs);
  static constexpr int kNumPushedFpRegisters =
      base::bits::CountPopulation(kPushedFpRegs);

  static constexpr int kLastPushedGpRegisterOffset =
      -TypedFrameConstants::kFixedFrameSizeFromFp -
      kSystemPointerSize * kNumPushedGpRegisters;
  static constexpr int kLastPushedFpRegisterOffset =
      kLastPushedGpRegisterOffset - kDoubleSize * kNumPushedFpRegisters;

  // Offsets are fp-relative.
  static int GetPushedGpRegisterOffset(int reg_code) {
    DCHECK_NE(0, kPushedGpRegs & (1 << reg_code));
    uint32_t lower_regs = kPushedGpRegs & ((uint32_t{1} << reg_code) - 1);
    return kLastPushedGpRegisterOffset +
           base::bits::CountPopulation(lower_regs) * kSystemPointerSize;
  }

  static int GetPushedFpRegisterOffset(int reg_code) {
    DCHECK_NE(0, kPushedFpRegs & (1 << reg_code));
    uint32_t lower_regs = kPushedFpRegs & ((uint32_t{1} << reg_code) - 1);
    return kLastPushedFpRegisterOffset +
           base::bits::CountPopulation(lower_regs) * kDoubleSize;
  }
};

}  // namespace internal
}  // namespace v8

#endif  // V8_EXECUTION_S390_FRAME_CONSTANTS_S390_H_
