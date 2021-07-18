//@author hankinzhang

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <mutex>
#include <iostream>

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define DEFER_OBJ CONCAT(__DEFER_,  CONCAT(__func__, __LINE__))
#define defer(expr) const auto &DEFER_OBJ = dry::utils::MakeObject([&](){ expr })

namespace dry {
  namespace utils {
    static std::mutex g_mt;

    static void InnerOutput(const std::string& str) {
      using namespace std;
      
      std::lock_guard<std::mutex> guard(g_mt);
      cout << str << endl;
    }

    template<typename ... Args>
    void string_format( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        auto buf = std::make_unique<char[]>( size );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        // return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        InnerOutput(std::string( buf.get(), buf.get() + size - 1 ));
    };

    template <typename F>
    class Defer {
    public:
        explicit Defer(F &&f) : _f(std::forward<F>(f)) {}
        ~Defer() {
            _f();
        }

        Defer(Defer &&that) : _f(std::move(that._f)) {}

        Defer() = delete;
        Defer(const Defer &) = delete;
        void operator=(const Defer &) = delete;
        void operator=(Defer &&) = delete;

    private:
        F _f;
    };

    template<typename F>
    Defer<F> MakeObject(F &&f) {
        return Defer<F>(std::forward<F>(f));
    };

    inline void TimeConsume(const std::chrono::steady_clock::time_point& time_point) {
      auto now = std::chrono::steady_clock::now();
      string_format("TimeConsume %lld in nanoseconds\n", std::chrono::duration_cast<std::chrono::nanoseconds>(now - time_point));
    };
  }
};