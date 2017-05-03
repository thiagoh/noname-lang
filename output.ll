; ModuleID = 'output.bc'
source_filename = "My JIT module1"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

define void @func_foo() {
entry:
  ret void
}
