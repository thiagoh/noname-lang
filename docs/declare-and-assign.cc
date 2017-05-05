/**
  * user level untyped variable
  * declare the "untyped"  variable that will ultimately point to the value
  */
AllocaInst* ptr_my_voidp_variable_not_defined = new AllocaInst(PointerTy_4, "my_voidp_variable_not_defined", label_entry);
ptr_my_voidp_variable_not_defined->setAlignment(8);
// create the actual Constant value
ConstantInt* const_int32_18 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("100"), 10));
// alocate the "typed" variable that will handle the Constant value
AllocaInst* ptr_int_v = new AllocaInst(IntegerType::get(mod->getContext(), 32), "int_v", label_entry);
ptr_int_v->setAlignment(4);
// store the Constant into the allocated "typed" variable
StoreInst* void_34 = new StoreInst(const_int32_18, ptr_int_v, false, label_entry);
void_34->setAlignment(4);
// Cast the the "typed" variable to the "untyped" variable
CastInst* ptr_40 = new BitCastInst(ptr_int_v, PointerTy_4, "", label_entry);
// Store the address of the 
StoreInst* void_41 = new StoreInst(ptr_40, ptr_my_voidp_variable_not_defined, false, label_entry);
void_41->setAlignment(8);