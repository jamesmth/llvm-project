#ifndef LLVM_IR_DLLPLUGIN_H
#define LLVM_IR_DLLPLUGIN_H

namespace llvm {

#ifdef _WIN32
  #define DLL_EXPORT __declspec(dllexport)
#else
  #define DLL_EXPORT
#endif

#ifdef _WIN32
  #define DLL_IMPORT __declspec(dllimport)
#else
  #define DLL_IMPORT
#endif

#ifdef EXPORT_SYMBOLS
  #define DLL_API DLL_EXPORT
#else
  #define DLL_API DLL_IMPORT
#endif

} // end namespace llvm

#endif // LLVM_IR_DLLPLUGIN_H
