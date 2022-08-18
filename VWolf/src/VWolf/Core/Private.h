#pragma once
#define DECLARE_PRIVATE(Type) namespace Private { class Type; }

#define BEGIN_DEFINE_PRIVATE namespace Private {
#define END_DEFINE_PRIVATE }