; ModuleID = '/Users/thiagoh/dev/noname-lang/accessory-src/to-be-translated/cpp-src/datastructure.cpp'
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

%struct.datatype_t = type { i32, i8* }

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: ssp uwtable
define { i32, i8* } @_Z20function_conditional10datatype_tS_(i32 %param_a.coerce0, i8* %param_a.coerce1, i32 %param_b.coerce0, i8* %param_b.coerce1) #0 {
entry:
  %retval = alloca %struct.datatype_t, align 8
  %param_a = alloca %struct.datatype_t, align 8
  %param_b = alloca %struct.datatype_t, align 8
  %actual_long_v = alloca i64, align 8
  %actual_double_v = alloca double, align 8
  %0 = bitcast %struct.datatype_t* %param_a to { i32, i8* }*
  %1 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %0, i32 0, i32 0
  store i32 %param_a.coerce0, i32* %1, align 8
  %2 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %0, i32 0, i32 1
  store i8* %param_a.coerce1, i8** %2, align 8
  %3 = bitcast %struct.datatype_t* %param_b to { i32, i8* }*
  %4 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %3, i32 0, i32 0
  store i32 %param_b.coerce0, i32* %4, align 8
  %5 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %3, i32 0, i32 1
  store i8* %param_b.coerce1, i8** %5, align 8
  %type = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %retval, i32 0, i32 0
  store i32 0, i32* %type, align 8
  %void_v = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %retval, i32 0, i32 1
  store i8* null, i8** %void_v, align 8
  %type1 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %param_a, i32 0, i32 0
  %6 = load i32, i32* %type1, align 8
  %cmp = icmp eq i32 %6, 12
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %void_v2 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %param_a, i32 0, i32 1
  %7 = load i8*, i8** %void_v2, align 8
  %8 = bitcast i8* %7 to i64*
  %9 = load i64, i64* %8, align 8
  %add = add nsw i64 %9, 777
  store i64 %add, i64* %actual_long_v, align 8
  %call = call noalias i8* @_Znwm(i64 8) #5
  %10 = bitcast i8* %call to i64*
  %11 = load i64, i64* %actual_long_v, align 8
  store i64 %11, i64* %10, align 8
  %12 = bitcast i64* %10 to i8*
  %void_v3 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %retval, i32 0, i32 1
  store i8* %12, i8** %void_v3, align 8
  br label %if.end11

if.else:                                          ; preds = %entry
  %type4 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %param_a, i32 0, i32 0
  %13 = load i32, i32* %type4, align 8
  %cmp5 = icmp eq i32 %13, 13
  br i1 %cmp5, label %if.then6, label %if.end

if.then6:                                         ; preds = %if.else
  %void_v7 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %param_a, i32 0, i32 1
  %14 = load i8*, i8** %void_v7, align 8
  %15 = bitcast i8* %14 to double*
  %16 = load double, double* %15, align 8
  %add8 = fadd double %16, 7.770000e+02
  store double %add8, double* %actual_double_v, align 8
  %call9 = call noalias i8* @_Znwm(i64 8) #5
  %17 = bitcast i8* %call9 to double*
  %18 = load double, double* %actual_double_v, align 8
  store double %18, double* %17, align 8
  %19 = bitcast double* %17 to i8*
  %void_v10 = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %retval, i32 0, i32 1
  store i8* %19, i8** %void_v10, align 8
  br label %if.end

if.end:                                           ; preds = %if.then6, %if.else
  br label %if.end11

if.end11:                                         ; preds = %if.end, %if.then
  %20 = bitcast %struct.datatype_t* %retval to { i32, i8* }*
  %21 = load { i32, i8* }, { i32, i8* }* %20, align 8
  ret { i32, i8* } %21
}

; Function Attrs: nobuiltin
declare noalias i8* @_Znwm(i64) #1

; Function Attrs: ssp uwtable
define { i32, i8* } @_Z11__anon_exprv() #0 {
entry:

  ; var_a
  %var_a = alloca %struct.datatype_t, align 8
  %type_var_a = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %var_a, i32 0, i32 0
  %void_v_var_a = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %var_a, i32 0, i32 1

  ; store var_a
  store i32 12, i32* %type_var_a, align 8
  %call_var_a = call noalias i8* @_Znwm(i64 8) #5
  %0 = bitcast i8* %call_var_a to i64*
  store i64 321, i64* %0, align 8
  %1 = bitcast i64* %0 to i8*
  store i8* %1, i8** %void_v_var_a, align 8

  ; var_b
  %var_b = alloca %struct.datatype_t, align 8
  %type_var_b = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %var_b, i32 0, i32 0
  %void_v_var_b = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %var_b, i32 0, i32 1
  
  ; store var_b
  store i32 12, i32* %type_var_b, align 8
  %call_var_b = call noalias i8* @_Znwm(i64 8) #5
  %2 = bitcast i8* %call_var_b to i64*
  store i64 321, i64* %2, align 8
  %3 = bitcast i64* %2 to i8*
  store i8* %3, i8** %void_v_var_b, align 8

  ; copying var_a to aggregator
  %agg_tmp_var_a = alloca %struct.datatype_t, align 8
  %4 = bitcast %struct.datatype_t* %agg_tmp_var_a to i8*
  %5 = bitcast %struct.datatype_t* %var_a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %4, i8* %5, i64 16, i32 8, i1 false)

  ; copying var_b to aggregator
  %agg_tmp_var_b = alloca %struct.datatype_t, align 8
  %6 = bitcast %struct.datatype_t* %agg_tmp_var_b to i8*
  %7 = bitcast %struct.datatype_t* %var_b to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %6, i8* %7, i64 16, i32 8, i1 false)
  
  %8 = bitcast %struct.datatype_t* %agg_tmp_var_a to { i32, i8* }*
  %9 = getelementptr { i32, i8* }, { i32, i8* }* %8, i32 0, i32 0
  %10 = load i32, i32* %9, align 8
  %11 = getelementptr { i32, i8* }, { i32, i8* }* %8, i32 0, i32 1
  %12 = load i8*, i8** %11, align 8

  %13 = bitcast %struct.datatype_t* %agg_tmp_var_b to { i32, i8* }*
  %14 = getelementptr { i32, i8* }, { i32, i8* }* %13, i32 0, i32 0
  %15 = load i32, i32* %14, align 8
  %16 = getelementptr { i32, i8* }, { i32, i8* }* %13, i32 0, i32 1
  %17 = load i8*, i8** %16, align 8

  %call5 = call { i32, i8* } @_Z20function_conditional10datatype_tS_(i32 %10, i8* %12, i32 %15, i8* %17)

  %datatype_t_out = alloca %struct.datatype_t, align 8
  %18 = bitcast %struct.datatype_t* %datatype_t_out to { i32, i8* }*
  %19 = getelementptr { i32, i8* }, { i32, i8* }* %18, i32 0, i32 0
  %20 = getelementptr { i32, i8* }, { i32, i8* }* %18, i32 0, i32 1
  
  %21 = extractvalue { i32, i8* } %call5, 0
  store i32 %21, i32* %19, align 8

  %22 = extractvalue { i32, i8* } %call5, 1
  store i8* %22, i8** %20, align 8
  
  %retval = alloca %struct.datatype_t, align 8
  %type_retval = getelementptr %struct.datatype_t, %struct.datatype_t* %retval, i32 0, i32 0
  store i32 333, i32* %type_retval, align 8
  %23 = bitcast %struct.datatype_t* %retval to { i32, i8* }*
  %24 = load { i32, i8* }, { i32, i8* }* %23, align 8
  ret { i32, i8* } %24
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: norecurse ssp uwtable
define i32 @main() #3 {
entry:
  %retval = alloca i32, align 4
  %d = alloca %struct.datatype_t, align 8
  store i32 0, i32* %retval, align 4
  %call = call { i32, i8* } @_Z11__anon_exprv()
  %0 = bitcast %struct.datatype_t* %d to { i32, i8* }*
  %1 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %0, i32 0, i32 0
  %2 = extractvalue { i32, i8* } %call, 0
  store i32 %2, i32* %1, align 8
  %3 = getelementptr inbounds { i32, i8* }, { i32, i8* }* %0, i32 0, i32 1
  %4 = extractvalue { i32, i8* } %call, 1
  store i8* %4, i8** %3, align 8
  %type = getelementptr inbounds %struct.datatype_t, %struct.datatype_t* %d, i32 0, i32 0
  %5 = load i32, i32* %type, align 8
  %call1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %5)
  ret i32 0
}

declare i32 @printf(i8*, ...) #4

attributes #0 = { ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nobuiltin "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { norecurse ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="core2" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+ssse3" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { builtin }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 3.8.1 (http://llvm.org/git/clang.git 07a6361e0f32f699d47c124106e7911b584974d4) (http://llvm.org/git/llvm.git 051e787f26dbfdc26cf61a57bc82ca00dcb812e8)"}
