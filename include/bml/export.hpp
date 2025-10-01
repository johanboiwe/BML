#ifndef BML_EXPORT_HPP
#define BML_EXPORT_HPP
#if defined(_WIN32)
  #if defined(BML_BUILDING_DLL)
    #define BML_API __declspec(dllexport)   // building BML as a DLL
  #elif defined(BML_USE_DLL)
    #define BML_API __declspec(dllimport)   // using BML as a DLL
  #else
    #define BML_API                         // static lib or no dllimport needed
  #endif
#else
  #if defined(__GNUC__) || defined(__clang__)
    #if !defined(BML_STATIC)
      #define BML_API __attribute__((visibility("default")))
    #else
      #define BML_API
    #endif
    #define BML_HIDDEN __attribute__((visibility("hidden")))
  #else
    #define BML_API
    #define BML_HIDDEN
  #endif
#endif


#endif //BML_EXPORT_HPP