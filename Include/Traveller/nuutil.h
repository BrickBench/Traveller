#pragma once

#define TRAVELLER_EXPAND(X) X
#define TRAVELLER_REGISTER_RAW_FUNCTION(Address, Return, Name, ...) typedef Return (*TRAVELLER_EXPAND(Name)Signature)(__VA_ARGS__);\
    TRAVELLER_EXPAND(Name)Signature Name = (TRAVELLER_EXPAND(Name)Signature)Address

#define TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(Address, CallConvention, Return, Name, ...) typedef Return (CallConvention *TRAVELLER_EXPAND(Name)Signature)(__VA_ARGS__);\
    TRAVELLER_EXPAND(Name)Signature Name = (TRAVELLER_EXPAND(Name)Signature)Address

#define TRAVELLER_REGISTER_RAW_GLOBAL(Address, Type, Name) inline Type* Name = (Type*)Address
