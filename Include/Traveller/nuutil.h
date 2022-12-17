#pragma once

#define TRAVELLER_EXPAND(X) X
#define TRAVELLER_REGISTER_RAW_FUNCTION(Address, Return, Name, ...)            \
  typedef Return (*Name##Signature)(__VA_ARGS__);                              \
  Name##Signature Name = (Name##Signature)Address

#define TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(                     \
    Address, CallConvention, Return, Name, ...)                                \
  typedef Return(CallConvention *Name##Signature)(__VA_ARGS__);                \
  Name##Signature Name = (Name##Signature)Address

#define TRAVELLER_REGISTER_RAW_GLOBAL(Address, Type, Name)                     \
  inline Type *Name = (Type *)Address
