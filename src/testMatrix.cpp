#include "bml/bml.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace bml::test {

// ---------- logging helpers ----------
#define LOG(msg)  do { std::cout << msg << std::endl; } while(0)
#define SEP()     do { std::cout << "------------------------------------------------------------\n"; } while(0)

struct ScopeTimer {
    const char* label;
    std::chrono::steady_clock::time_point t0;
    explicit ScopeTimer(const char* l) : label(l), t0(std::chrono::steady_clock::now()) {}
    ~ScopeTimer() {
        using namespace std::chrono;
        const auto ms = duration_cast<milliseconds>(steady_clock::now() - t0).count();
        std::cout << "[TIME] " << label << ": " << ms << " ms\n";
    }
};

inline std::string pretty_bytes(unsigned long long b) {
    static constexpr const char* units[] = {"B","KiB","MiB","GiB","TiB"};
    double d = static_cast<double>(b);
    int i = 0;
    while (d >= 1024.0 && i < 4) { d /= 1024.0; ++i; }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << d << ' ' << units[i];
    return oss.str();
}

struct TestFail : std::runtime_error { using std::runtime_error::runtime_error; };

// ---------- pretty type & value printing ----------
template<typename T>
const char* type_name() {
    if constexpr (std::is_same_v<T, std::int8_t>)   return "int8_t";
    if constexpr (std::is_same_v<T, std::uint8_t>)  return "uint8_t";
    if constexpr (std::is_same_v<T, std::int16_t>)  return "int16_t";
    if constexpr (std::is_same_v<T, std::uint16_t>) return "uint16_t";
    if constexpr (std::is_same_v<T, std::int32_t>)  return "int32_t";
    if constexpr (std::is_same_v<T, std::uint32_t>) return "uint32_t";
    if constexpr (std::is_same_v<T, std::int64_t>)  return "int64_t";
    if constexpr (std::is_same_v<T, std::uint64_t>) return "uint64_t";
    if constexpr (std::is_same_v<T, float>)         return "float";
    if constexpr (std::is_same_v<T, double>)        return "double";
    if constexpr (std::is_same_v<T, long double>)   return "long double";
    if constexpr (std::is_same_v<T, char>)          return "char";
    if constexpr (std::is_same_v<T, bool>)          return "bool";
    if constexpr (std::is_same_v<T, std::string>)   return "std::string";
    if constexpr (std::is_same_v<T, void*>)         return "void*";
    return "unknown";
}

template<typename T>
std::string format_value(const T& v) {
    std::ostringstream oss;
    if constexpr (std::is_same_v<T, bool>) {
        oss << (v ? "true" : "false");
    } else if constexpr (std::is_same_v<T, char>) {
        if (std::isprint(static_cast<unsigned char>(v))) {
            oss << "'" << v << "' (int:" << static_cast<int>(v) << ")";
        } else {
            oss << "char(int:" << static_cast<int>(v) << ")";
        }
    } else if constexpr (std::is_integral_v<T>) {
        oss << static_cast<long long>(v);
    } else if constexpr (std::is_floating_point_v<T>) {
        oss << std::setprecision(17) << static_cast<long double>(v);
    } else if constexpr (std::is_same_v<T, std::string>) {
        oss << '"' << v << '"';
    } else {
        oss << v; // fallback if streamable
    }
    return oss.str();
}

template<class A, class B>
[[noreturn]] void fail_eq(const A& a, const B& b, const char* what) {
    std::ostringstream oss;
    oss << "[FAIL] " << what
        << " (got " << format_value(a)
        << ", expected " << format_value(b) << ")";
    throw TestFail(oss.str());
}

template<class A, class B>
void expect_eq(const A& a, const B& b, const char* what) {
    if (!(a == b)) fail_eq(a, b, what);
}

inline void expect_true(bool v, const char* what)  { if (!v) throw TestFail(std::string("[FAIL] expected true: ")  + what); }
inline void expect_false(bool v, const char* what) { if ( v) throw TestFail(std::string("[FAIL] expected false: ") + what); }

template<class T> struct is_floatish      : std::is_floating_point<T> {};
template<class T> struct is_real_integral : bml_is_math_integral<T> {};
template<class T> struct is_math          : bml_is_math_arithmetic<T> {};

// ---------- small helpers ----------
template<typename T>
void print_type_header(const char* section) {
    SEP();
    std::cout << "[SECTION] " << section << " — type: " << type_name<T>() << "\n";
}

template<typename T>
void fill_sequence(Matrix<T>& m) {
    for (std::uint32_t r = 0; r < m.numRows(); ++r)
        for (std::uint32_t c = 0; c < m.numCols(); ++c)
            m[r][c] = static_cast<T>(r*10 + c);
}

// ---------- tests ----------
template<typename T>
void test_core_shape_iter_verbose() {
    print_type_header<T>("Core/Shape/Iter");

    Matrix<T> m(2,3);
    LOG("Created Matrix<T>(2,3); rows=" << m.numRows() << " cols=" << m.numCols() << " size=" << m.size());
    expect_false(m.empty(), "matrix not empty");
    m.fill(T{});
    fill_sequence(m);
    LOG("Filled with sequence r*10+c");

    auto row0 = m.getRow(0);
    LOG("getRow(0): [" << format_value(row0[0]) << "," << format_value(row0[1]) << "," << format_value(row0[2]) << "]");
    expect_eq(row0.size(), static_cast<std::size_t>(3), "row size");

    auto col1 = m.getColumn(1);
    LOG("getColumn(1): [" << format_value(col1[0]) << "," << format_value(col1[1]) << "]");
    expect_eq(col1[0], static_cast<T>(1),  "col1[0]");
    expect_eq(col1[1], static_cast<T>(11), "col1[1]");

    auto diag = m.getDiagonal();
    LOG("getDiagonal(): count=" << diag.size());
    expect_eq(diag[0], static_cast<T>(0),  "diag[0]");
    expect_eq(diag[1], static_cast<T>(11), "diag[1]");

    auto adiag = m.getAntiDiagonal();
    LOG("getAntiDiagonal(): count=" << adiag.size());
    expect_eq(adiag[0], static_cast<T>(2), "adiag[0]");
    expect_eq(adiag[1], m[1][1],           "adiag[1]");

    Matrix<T> sub = m.copy(0,1,2,3);
    LOG("copy(0,1,2,3): " << sub.numRows() << "x" << sub.numCols());
    expect_eq(sub[1][0], static_cast<T>(11), "copy content");

    Matrix<T> dst(2,3);
    dst.fill(static_cast<T>(-1));
    dst.paste(sub,0,1);
    LOG("paste(sub,0,1): dst[0][1]=" << format_value(dst[0][1]) << " dst[1][2]=" << format_value(dst[1][2]));

    std::size_t count = 0;
    {
        ScopeTimer t("Row traversal");
        for (auto it=m.begin(TraversalType::Row); it!=m.end(TraversalType::Row); ++it) ++count;
    }
    expect_eq(count, m.size(), "row iter count");
    count = 0;
    {
        ScopeTimer t("Column traversal");
        for (auto it=m.begin(TraversalType::Column); it!=m.end(TraversalType::Column); ++it) ++count;
    }
    expect_eq(count, m.size(), "col iter count");

    expect_true(m.all([](T x){ return x == x; }), "all(x==x)");
    expect_true(m.any_of([](T x){ return x == static_cast<T>(11); }), "any_of x=11");
    expect_false(m.none_of([](T x){ return x == static_cast<T>(11); }), "none_of false");
    LOG("[OK] Core/Shape/Iter");
}

template<typename T>
void test_pod_bytestream_verbose() {
    if constexpr (std::is_same_v<T, std::string>) return;
    print_type_header<T>("ByteStream (POD)");
    Matrix<T> m(2,2);
    m[0][0]=static_cast<T>(1);
    m[0][1]=static_cast<T>(2);
    m[1][0]=static_cast<T>(3);
    m[1][1]=static_cast<T>(4);

    LOG("Serialising to bytestream…");
    auto bs = m.toByteStream();
    LOG("Byte size: " << bs.size() << " (" << pretty_bytes(bs.size()) << ")");

    Matrix<T> r(2,2);
    r.initFromByteStream(bs);
    expect_true(m == r, "POD bytestream round-trip");
    LOG("[OK] ByteStream (POD)");
}

template<typename T>
void test_arithmetic_verbose() {
    if constexpr (!is_math<T>::value) return;
    print_type_header<T>("Arithmetic (element-wise + scalar)");
    Matrix<T> a(2,2), b(2,2);
    a[0][0]=1; a[0][1]=2; a[1][0]=3; a[1][1]=4;
    b[0][0]=5; b[0][1]=6; b[1][0]=7; b[1][1]=8;

    auto c = a + b; expect_eq(c[1][1], static_cast<T>(12), "a+b");
    c = a - b;      expect_eq(c[0][0], static_cast<T>(-4), "a-b");
    c = a * b;      expect_eq(c[0][1], static_cast<T>(12), "a*b");

    c = a + static_cast<T>(10); expect_eq(c[1][1], static_cast<T>(14), "a+10");
    c = a * static_cast<T>(2);  expect_eq(c[0][1], static_cast<T>(4),  "a*2");

    auto s = a.template sum<T>();
    if constexpr (is_floatish<T>::value) {
        expect_true(std::fabs(static_cast<double>(s - static_cast<T>(10))) < 1e-9, "sum float");
    } else {
        expect_eq(s, static_cast<T>(10), "sum int");
    }
    (void)a.template min<T>();
    (void)a.template max<T>();
    (void)a.template argmin<T>();
    (void)a.template argmax<T>();

    Matrix<T> d = a;
    d += b; expect_eq(d[0][0], static_cast<T>(6), "+=");
    d -= b; expect_eq(d[0][0], static_cast<T>(1), "-=");
    d *= b; expect_eq(d[0][1], static_cast<T>(12), "*=");
    d /= a; expect_eq(d[0][1], static_cast<T>(6),  "/=");
    LOG("[OK] Arithmetic");
}

template<typename T>
void test_integral_ops_verbose() {
    if constexpr (!is_real_integral<T>::value) return;
    print_type_header<T>("Integral-only ops (&,|,^,%,~,<<,>>)");
    Matrix<T> a(2,2), b(2,2);
    a[0][0]=1; a[0][1]=2; a[1][0]=3; a[1][1]=4;
    b[0][0]=1; b[0][1]=3; b[1][0]=2; b[1][1]=5;

    auto m = a % b; (void)m;
    auto bw = a & b; (void)bw;
    bw = a | b; bw = a ^ b;

    auto notA = ~a; (void)notA;
    auto shl = a << 1; (void)shl;
    auto shr = a >> 1; (void)shr;

    Matrix<T> c = a;
    c &= b; c |= b; c ^= b; c <<= 1; c >>= 1;
    LOG("[OK] Integral-only ops");
}

// ====== Thorough bool tests ======
static void test_bool_thorough() {
    static_assert(bml_is_bool<bool>::value, "bool must satisfy bml_is_bool");
    static_assert(!bml_is_math_arithmetic<bool>::value, "bool must NOT be math-arithmetic");
    static_assert(!bml_is_math_integral<bool>::value,   "bool must NOT be math-integral");

    print_type_header<bool>("bool exhaustive");

    {
        Matrix<bool> e00(0,0);
        expect_true(e00.empty(), "0x0 empty");
        expect_eq(e00.size(), static_cast<std::size_t>(0), "0x0 size");
        expect_true(e00.none<bool>(),  "0x0 none()==true");
        expect_false(e00.any<bool>(),  "0x0 any()==false");
    }
    {
        Matrix<bool> e05(0,5), e50(5,0);
        expect_true(e05.empty(), "0x5 empty");
        expect_true(e50.empty(), "5x0 empty");
    }

    Matrix<bool> m(4,5);
    m.fill(false);
    expect_eq(m.count_true<bool>(), static_cast<std::size_t>(0), "all false count_true");
    expect_true(m.none<bool>(),  "all false none");
    expect_false(m.any<bool>(),  "all false any");
    expect_eq(m.template min<bool>(), false, "min false");
    expect_eq(m.template max<bool>(), false, "max false");

    for (std::uint32_t r=0; r<m.numRows(); ++r)
        for (std::uint32_t c=0; c<m.numCols(); ++c)
            m[r][c] = ((r ^ c) & 1) != 0;

    expect_eq(m.count_true<bool>(), static_cast<std::size_t>(10), "checkerboard count_true");
    expect_true(m.any<bool>(), "checkerboard any");
    expect_false(m.none<bool>(), "checkerboard none");
    expect_eq(m.template min<bool>(), false, "checkerboard min");
    expect_eq(m.template max<bool>(), true,  "checkerboard max");

    auto row1 = m.getRow(1);
    std::size_t row1_ct = 0;
    for (bool v : row1) if (v) ++row1_ct;
    expect_eq(row1.size(), static_cast<std::size_t>(5), "row1 len");
    expect_eq(row1_ct, static_cast<std::size_t>((m.numCols()+1)/2), "row1 true count");

    auto col2 = m.getColumn(2);
    std::size_t col2_ct = 0;
    for (bool v: col2) if (v) ++col2_ct;
    expect_eq(col2.size(), static_cast<std::size_t>(4), "col2 len");

    auto d  = m.getDiagonal();
    auto ad = m.getAntiDiagonal();
    expect_eq(d.size(),  static_cast<std::size_t>(4), "diag len");
    expect_eq(ad.size(), static_cast<std::size_t>(4), "adiag len");

    auto wmask = m.where([](bool v){ return v; }, true, false);
    expect_true(wmask.any<bool>(), "where any");
    expect_eq(wmask.count_true<bool>(), m.count_true<bool>(), "where preserves truth pattern");

    Matrix<bool> alltrue(4,5);  alltrue.fill(true);
    Matrix<bool> allfalse(4,5); allfalse.fill(false);

    auto a = m.logical_and<bool>(alltrue);
    auto o = m.logical_or <bool>(false);
    auto x = m.logical_xor<bool>(true);
    expect_true(a == m, "and with alltrue equals m");
    expect_true(o == m, "or with false equals m");
    expect_eq(x.count_true<bool>(), m.size() - m.count_true<bool>(), "xor invert counts");

    auto a2 = m.logical_and<bool>(true);
    auto o2 = m.logical_or <bool>(false);
    auto x2 = m.logical_xor<bool>(false);
    expect_true(a2 == m && o2 == m && x2 == m, "scalar logical ops keep equality");

    Matrix<bool> sub = m.copy(1,1,3,4);
    Matrix<bool> z(4,5);
    z.fill(false);
    z.paste(sub, 0, 2);
    for (std::uint32_t r=0; r<2; ++r)
        for (std::uint32_t c=0; c<3; ++c)
            expect_eq(z[r][c+2], sub[r][c], "paste preserves");

    std::size_t cnt = 0;
    for (auto it = m.begin(TraversalType::Row); it != m.end(TraversalType::Row); ++it) ++cnt;
    expect_eq(cnt, m.size(), "row iterator count");
    cnt = 0;
    for (auto it = m.begin(TraversalType::Column); it != m.end(TraversalType::Column); ++it) ++cnt;
    expect_eq(cnt, m.size(), "column iterator count");

    {
        LOG("bool: bytestream round-trip");
        auto bs = m.toByteStream();
        Matrix<bool> r(m.numRows(), m.numCols());
        r.initFromByteStream(bs);
        expect_true(m == r, "bool bytestream round-trip eq");
    }
    {
        LOG("bool: malformed bytestream size should throw");
        auto bs = m.toByteStream();
        if (!bs.empty()) bs.pop_back();
        Matrix<bool> r(m.numRows(), m.numCols());
        bool threw = false;
        try { r.initFromByteStream(bs); } catch (...) { threw = true; }
        expect_true(threw, "initFromByteStream must throw on wrong size");
    }

    LOG("[OK] bool exhaustive");
}

// ---- Deep index & iterator tests (includes mutation through iterators) ----
template<typename T>
void test_index_and_iterators_deep() {
    print_type_header<T>("Indexing & Iterators (deep)");

    Matrix<T> m(3,4);
    for (std::uint32_t r=0; r<m.numRows(); ++r)
        for (std::uint32_t c=0; c<m.numCols(); ++c)
            m[r][c] = static_cast<T>(r*100 + c);

    expect_eq(m[0][0], static_cast<T>(0),   "[][] origin");
    expect_eq(m[2][3], static_cast<T>(203), "[][] last");
    expect_eq(m[1][2], static_cast<T>(102), "[][] mid");

    if constexpr (std::is_same_v<T, bool>) {
        auto it = m.begin(TraversalType::Row);
        auto ed = m.end(TraversalType::Row);
        for (; it != ed; ++it) {
            auto&& [r, c, ref] = *it;  // bind by reference to the proxy
            (void)r; (void)c;
            ref = true;  // "+1" for bool saturates to true

        }
    } else {
        auto it = m.begin(TraversalType::Row);
        auto ed = m.end(TraversalType::Row);
        for (; it != ed; ++it) {
            auto&& [r, c, ref] = *it;  // bind by reference
            (void)r; (void)c;
            ref = static_cast<T>( static_cast<T>(ref) + static_cast<T>(1) ); // increment
        }
    }
    expect_eq(m[0][0], static_cast<T>(1),   "iterator write origin+1");
    expect_eq(m[2][3], static_cast<T>(204), "iterator write last+1");

    {
        const Matrix<T>& cm = m;
        std::size_t count = 0;
        auto it = cm.begin(TraversalType::Column);
        auto ed = cm.end(TraversalType::Column);
        for (; it != ed; ++it) { auto [r,c,val] = *it; (void)r; (void)c; (void)val; ++count; }
        expect_eq(count, cm.size(), "const column iteration count");
    }

    {
        const Matrix<T>& cm = m;
        auto it = cm.begin(TraversalType::Diagonal);
        auto ed = cm.end(TraversalType::Diagonal);
        std::uint32_t i = 0, limit = std::min(cm.numRows(), cm.numCols());
        for (; it != ed; ++it, ++i) {
            auto [r,c,val] = *it; (void)val;
            expect_eq(r, i, "diag r==i");
            expect_eq(c, i, "diag c==i");
            expect_eq(val, cm[i][i], "diag val");
        }
        expect_eq(i, limit, "diag length");
    }

    {
        const Matrix<T>& cm = m;
        auto it = cm.begin(TraversalType::AntiDiagonal);
        auto ed = cm.end(TraversalType::AntiDiagonal);
        std::vector<std::pair<std::uint32_t,std::uint32_t>> seen;
        for (; it != ed; ++it) {
            auto [r,c,val] = *it; (void)val;
            seen.emplace_back(r,c);
        }
        expect_eq(seen.size(), static_cast<std::size_t>(std::min(cm.numRows(), cm.numCols())), "anti-diag length");
        expect_eq(seen[0].first, 0u,  "adiag[0].r");
        expect_eq(seen[0].second, cm.numCols()-1, "adiag[0].c");
        if (seen.size() >= 3) {
            expect_eq(seen[1].first, 1u, "adiag[1].r");
            expect_eq(seen[1].second, cm.numCols()-2, "adiag[1].c");
            expect_eq(seen[2].first, 2u, "adiag[2].r");
            expect_eq(seen[2].second, cm.numCols()-3, "adiag[2].c");
        }
    }

    LOG("[OK] Indexing & Iterators (deep)");
}

// ---- std::string tests ----
static void test_string_verbose() {
    print_type_header<std::string>("std::string full coverage");

    // Construct & shape
    Matrix<std::string> m(2,2);
    expect_eq(m.numRows(), 2u, "rows");
    expect_eq(m.numCols(), 2u, "cols");
    expect_false(m.empty(), "not empty");
    expect_eq(m.size(), static_cast<std::size_t>(4), "size");

    // operator[] write/read
    m[0][0] = "hello";
    m[0][1] = "";
    m[1][0] = "ÅÄÖ";
    m[1][1] = "end";

    // toString() — basic sanity (don’t over-spec trailing spaces/newlines)
    {
        const auto s = m.toString();
        expect_true(s.find("hello") != std::string::npos, "toString contains hello");
        expect_true(s.find("ÅÄÖ")   != std::string::npos, "toString contains ÅÄÖ");
        expect_true(s.find("end")   != std::string::npos, "toString contains end");
    }

    // getRow / getColumn (full)
    {
        auto row0 = m.getRow(0);
        expect_eq(row0.size(), static_cast<std::size_t>(2), "row0 size");
        expect_eq(row0[0], "hello", "row0[0]");
        expect_eq(row0[1], "",       "row0[1]");

        auto col0 = m.getColumn(0);
        expect_eq(col0.size(), static_cast<std::size_t>(2), "col0 size");
        expect_eq(col0[0], "hello", "col0[0]");
        expect_eq(col0[1], "ÅÄÖ",   "col0[1]");
    }

    // Ranged getRow / getColumn (uses -1 sentinel)
    {
        auto row0_0_1 = m.getRow(0, /*startCol*/0, /*endCol*/1);
        expect_eq(row0_0_1.size(), static_cast<std::size_t>(1), "row0 [0,1) size");
        expect_eq(row0_0_1[0], "hello", "row0 [0,1) val");

        auto col1_1_end = m.getColumn(1, /*startRow*/1, /*endRow*/-1);
        expect_eq(col1_1_end.size(), static_cast<std::size_t>(1), "col1 [1,end) size");
        expect_eq(col1_1_end[0], "end", "col1 [1,end) val");
    }

    // Diagonals
    {
        auto d  = m.getDiagonal();
        auto ad = m.getAntiDiagonal();
        expect_eq(d.size(),  static_cast<std::size_t>(2), "diag size");
        expect_eq(ad.size(), static_cast<std::size_t>(2), "adiag size");
        expect_eq(d[0],  "hello", "diag[0]");
        expect_eq(d[1],  "end",   "diag[1]");
        expect_eq(ad[0], "",      "adiag[0]");
        expect_eq(ad[1], "ÅÄÖ",   "adiag[1]");
    }

    // Algorithms: all / any_of / none_of / where
    {
        expect_true(m.all([](const std::string& s){ return s.size() >= 0; }),
                    "all non-negative length");
        expect_true(m.any_of([](const std::string& s){ return s.empty(); }),
                    "any_of finds empty");
        expect_false(m.none_of([](const std::string& s){ return s == "end"; }),
                     "none_of false when 'end' exists");

        auto ww = m.where(
            [](const std::string& s){ return s.empty(); },
            std::string("X"),
            std::string("-")
        );
        expect_eq(ww[0][0], "-", "where non-empty");
        expect_eq(ww[0][1], "X", "where empty");
        expect_eq(ww[1][0], "-", "where non-empty ÅÄÖ");
        expect_eq(ww[1][1], "-", "where non-empty end");
    }

    // fill()
    {
        Matrix<std::string> f(2,2);
        f.fill(std::string(".."));
        for (std::uint32_t r=0; r<2; ++r)
            for (std::uint32_t c=0; c<2; ++c)
                expect_eq(f[r][c], std::string(".."), "fill() cell");
    }

    // copy() / paste() (incl. -1 sentinel full-copy)
    {
        Matrix<std::string> full = m.copy(0,0,-1,-1);
        expect_true(full == m, "copy full equals original");

        Matrix<std::string> sub = m.copy(0,0,2,1); // first column
        Matrix<std::string> z(2,2); z.fill(std::string(".."));
        z.paste(sub, 0, 1);
        expect_eq(z[0][1], "hello", "paste col[0]");
        expect_eq(z[1][1], "ÅÄÖ",   "paste col[1]");
    }

    // Iterators: counts, const iteration, and mutation via non-const iterator
    {
        std::size_t cnt = 0;
        for (auto it = m.begin(TraversalType::Row); it != m.end(TraversalType::Row); ++it) ++cnt;
        expect_eq(cnt, m.size(), "row iterator count");

        const Matrix<std::string>& cm = m;
        cnt = 0;
        for (auto it = cm.begin(TraversalType::Column); it != cm.end(TraversalType::Column); ++it) ++cnt;
        expect_eq(cnt, cm.size(), "const column iterator count");

        // Mutate through iterator (non-const)
        auto it = m.begin(TraversalType::Row);
        auto ed = m.end(TraversalType::Row);
        for (; it != ed; ++it) {
            auto&& [r,c,ref] = *it;
            if (r == 0 && c == 0) { ref += "!"; break; }
        }
        expect_eq(m[0][0], "hello!", "iterator write");
        m[0][0] = "hello"; // revert
    }

    // Reductions available for strings: min/max/argmin/argmax (non-pointer types)
    {
        expect_eq(m.template min<std::string>(), std::string(""),   "min (lexicographic) empty");
        expect_eq(m.template max<std::string>(), std::string("ÅÄÖ"), "max (lexicographic) ÅÄÖ");
        auto mn = m.template argmin<std::string>();
        auto mx = m.template argmax<std::string>();
        expect_eq(mn.first,  0u, "argmin r");
        expect_eq(mn.second, 1u, "argmin c");
        expect_eq(mx.first,  1u, "argmax r");
        expect_eq(mx.second, 0u, "argmax c");
    }

    // Bytestream round-trips: vector<> overload and (ptr,size) overload
    {
        auto bs = m.toByteStream();
        Matrix<std::string> r2(2,2); r2.initFromByteStream(bs);
        expect_true(m == r2, "string round-trip (vector)");
        Matrix<std::string> r3(2,2); r3.initFromByteStream(bs.data(), bs.size());
        expect_true(m == r3, "string round-trip (ptr,size)");

        // Malformed bytestream should throw (e.g., truncated)
        if (!bs.empty()) {
            auto bad = bs; bad.pop_back();
            bool threw = false;
            try { Matrix<std::string> t(2,2); t.initFromByteStream(bad); }
            catch (...) { threw = true; }
            expect_true(threw, "initFromByteStream throws on malformed/truncated input");
        }
    }

    // Equality & relational operators
    {
        Matrix<std::string> a(1,2), b(1,2);
        a[0][0] = "a"; a[0][1] = "x";
        b[0][0] = "b"; b[0][1] = "x";
        expect_true(a != b, "operator!=");
        expect_true(a == a, "operator== self");
        expect_true(a <  b, "operator<  lexicographic");
        expect_true(b >  a, "operator>  lexicographic");
        expect_true(a <= b, "operator<=");
        expect_true(b >= a, "operator>=");
    }

    LOG("[OK] strings (full coverage)");
}


// --- Rule-of-Five test -------------------------------------------------------
template<typename T>
static inline T tv(int n) {
    if constexpr (std::is_same_v<T, std::string>) return std::to_string(n);
    else if constexpr (std::is_same_v<T, bool>)   return n != 0;
    else                                          return static_cast<T>(n);
}

template<typename T>
void test_rule_of_five_core() {
    print_type_header<T>("Rule-of-Five");

    static_assert(std::is_copy_constructible_v<Matrix<T>>);
    static_assert(std::is_copy_assignable_v<Matrix<T>>);
    static_assert(std::is_move_constructible_v<Matrix<T>>);
    static_assert(std::is_move_assignable_v<Matrix<T>>);
    static_assert(std::is_nothrow_move_constructible_v<Matrix<T>>);
    static_assert(std::is_nothrow_move_assignable_v<Matrix<T>>);

    Matrix<T> a(2,2);
    a[0][0] = tv<T>(1); a[0][1] = tv<T>(2);
    a[1][0] = tv<T>(3); a[1][1] = tv<T>(4);

    Matrix<T> b(a);
    a[0][0] = tv<T>(10);
    expect_eq(b[0][0], tv<T>(1), "copy-ctor deep copy");

    Matrix<T> c(2,2);
    c = a;
    a[0][1] = tv<T>(20);
    expect_eq(c[0][1], tv<T>(2), "copy-assign deep copy");
    Matrix<T>* alias = &c;
    c = *alias; // self-assign
    expect_eq(c[1][1], tv<T>(4), "copy self-assignment");

    Matrix<T> m(std::move(a));
    expect_true(m.numRows() == 2 && m.numCols() == 2, "move-ctor shape");
    expect_eq(a.size(), static_cast<std::size_t>(0), "move-ctor: moved-from empty");

    Matrix<T> d(1,1);
    d = std::move(b);
    expect_true(d.numRows() == 2 && d.numCols() == 2, "move-assign shape");
    expect_eq(b.size(), static_cast<std::size_t>(0), "move-assign: moved-from empty");

    d = std::move(d);
    expect_true(d.numRows() == 2 && d.numCols() == 2, "move self-assignment unchanged");

    LOG("[OK] Rule-of-Five");
}

// ---------- stress tests ----------
template<typename T>
void stress_numeric(const char* label, std::uint32_t R, std::uint32_t C, int repeats = 2) {
    print_type_header<T>(label);
    const std::size_t n = static_cast<std::size_t>(R) * static_cast<std::size_t>(C);
    const unsigned long long bytes = n * sizeof(storage_of_t<T>);
    LOG("Attempting " << R << "x" << C << " elements=" << n
        << " bytes=" << bytes << " (" << pretty_bytes(bytes) << ")");

    try {
        ScopeTimer alloc_t("stress alloc/fill");
        Matrix<T> m(R,C);
        std::mt19937_64 rng(1234567);
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<long long> dist(0, 1000);
            for (std::uint32_t r=0; r<R; ++r)
                for (std::uint32_t c=0; c<C; ++c)
                    m[r][c] = static_cast<T>(dist(rng));
        } else if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (std::uint32_t r=0; r<R; ++r)
                for (std::uint32_t c=0; c<C; ++c)
                    m[r][c] = static_cast<T>(dist(rng));
        } else {
            m.fill(T{});
        }

        {
            ScopeTimer t("stress sum/min/max/arg*");
            (void)m.template sum<T>();
            (void)m.template min<T>();
            (void)m.template max<T>();
            (void)m.template argmin<T>();
            (void)m.template argmax<T>();
        }

        if constexpr (is_math<T>::value) {
            ScopeTimer t2("stress arithmetic + scalar");
            Matrix<T> k(R,C);
            k.fill(static_cast<T>(1));
            for (int i=0; i<repeats; ++i) {
                auto x = m + k;
                auto y = x - k;
                auto z = y * k;
                auto w = z / k;
                (void)w;
            }
        }
        if constexpr (is_real_integral<T>::value) {
            ScopeTimer t3("stress bitwise &|^~ << >>");
            Matrix<T> k(R,C);
            k.fill(static_cast<T>(0x0F));
            auto a = m & k;
            a |= k; a ^= k; a = ~a; a = (a << 1); a = (a >> 1);
            (void)a;
        }

        if constexpr (!std::is_same_v<T, std::string>) {
            ScopeTimer t4("stress bytestream round-trip (POD)");
            auto bs = m.toByteStream();
            Matrix<T> r(R,C);
            r.initFromByteStream(bs);
            expect_true(m == r, "stress round-trip POD");
        }

        LOG("[OK] stress " << label);
    } catch (const std::bad_alloc&) {
        LOG("[SKIP] stress " << label << " — std::bad_alloc (too large for this machine)");
    } catch (const std::exception& e) {
        LOG(std::string("[FAIL] stress ") + label + ": " + e.what());
        throw;
    }
}

static void stress_bool(std::uint32_t R, std::uint32_t C) {
    print_type_header<bool>("stress bool");
    try {
        Matrix<bool> m(R,C);
        for (std::uint32_t r=0; r<R; ++r)
            for (std::uint32_t c=0; c<C; ++c)
                m[r][c] = ((r*1315423911u + c*2654435761u) & 1u) != 0;

        {
            ScopeTimer t("bool logical ops & reductions");
            Matrix<bool> n(R,C); n.fill(true);
            auto a = m.logical_and<bool>(n);
            auto o = m.logical_or <bool>(false);
            auto x = m.logical_xor<bool>(true);
            auto notm = m.logical_not<bool>();
            (void)a; (void)o; (void)x; (void)notm;
            (void)m.count_true<bool>();
            (void)m.any<bool>();
            (void)m.none<bool>();
        }
        {
            ScopeTimer t("bool bytestream round-trip");
            auto bs = m.toByteStream();
            Matrix<bool> r(R,C);
            r.initFromByteStream(bs);
            if (!(m == r)) throw TestFail("[FAIL] bool stress round-trip mismatch");
        }
        LOG("[OK] stress bool");
    } catch (const std::bad_alloc&) {
        LOG("[SKIP] stress bool — std::bad_alloc");
    }
}

static void stress_strings(std::uint32_t R, std::uint32_t C) {
    print_type_header<std::string>("stress strings");
    try {
        Matrix<std::string> m(R,C);
        for (std::uint32_t r=0; r<R; ++r)
            for (std::uint32_t c=0; c<C; ++c)
                m[r][c] = (r%7==0) ? "X" : (r%5==0) ? "" : "abcdefghijklmnopqrstuvwxyz";
        ScopeTimer t("string bytestream round-trip");
        auto bs = m.toByteStream();
        Matrix<std::string> r2(R,C);
        r2.initFromByteStream(bs);
        expect_true(m == r2, "string stress round-trip");
        LOG("[OK] stress strings");
    } catch (const std::bad_alloc&) {
        LOG("[SKIP] stress strings — std::bad_alloc");
    }
}

// ---------- type packs ----------
#define FOR_EACH_INT_T(OP) \
    OP(std::int8_t)  OP(std::uint8_t) \
    OP(std::int16_t) OP(std::uint16_t) \
    OP(std::int32_t) OP(std::uint32_t) \
    OP(std::int64_t) OP(std::uint64_t)

#define FOR_EACH_FP_T(OP) \
    OP(float) OP(double) OP(long double)

#define FOR_EACH_POD_T(OP) \
    FOR_EACH_INT_T(OP) FOR_EACH_FP_T(OP) OP(char)

} // namespace bml::test

// ============ PUBLIC ENTRY ============
int testMatrix() {
    using namespace bml::test;
    try {
        LOG("[BML TEST] Starting…");
        SEP();

        // --- verbose per-type functional tests ---
        // Core/iter + POD bytestream
        #define CALL_CORE_AND_POD(T) \
            test_core_shape_iter_verbose<T>(); \
            test_pod_bytestream_verbose<T>();
        FOR_EACH_POD_T(CALL_CORE_AND_POD)
        #undef CALL_CORE_AND_POD

        // Thorough bool and its bytestream path
        test_core_shape_iter_verbose<bool>();
        test_pod_bytestream_verbose<bool>();
        test_bool_thorough();

        // Deep index & iterator tests
        test_index_and_iterators_deep<std::int32_t>();
        test_index_and_iterators_deep<double>();
        test_index_and_iterators_deep<bool>();

        // Arithmetic families
        #define CALL_ARITH(T) test_arithmetic_verbose<T>();
        FOR_EACH_INT_T(CALL_ARITH)
        FOR_EACH_FP_T(CALL_ARITH)
        #undef CALL_ARITH

        // Integral-only ops
        #define CALL_INT_ONLY(T) test_integral_ops_verbose<T>();
        FOR_EACH_INT_T(CALL_INT_ONLY)
        #undef CALL_INT_ONLY

        // Strings
        test_string_verbose();

        SEP();
        LOG("[BML TEST] Functional tests passed. Beginning stress…");
        SEP();

        test_rule_of_five_core<std::int32_t>();
        test_rule_of_five_core<std::string>();
        test_rule_of_five_core<double>();
        test_rule_of_five_core<bool>(); // if Matrix<bool> meets the semantics

        // --- Stress sizes (adaptive)
        struct Attempt { std::uint32_t r,c; };
        const Attempt big_attempts[] = { {4096,4096}, {2048,2048} };

        for (auto [r,c] : big_attempts) {
            stress_numeric<std::uint32_t>("stress u32", r,c);
            stress_numeric<float>("stress float", r,c);
            stress_numeric<std::uint64_t>("stress u64", r,c, /*repeats*/1);
            stress_bool(std::min<std::uint32_t>(r, 8192), std::min<std::uint32_t>(c, 8192));
            stress_strings(std::min<std::uint32_t>(r, 1024), std::min<std::uint32_t>(c, 1024));
            break; // do one tier; remove to cascade
        }

        SEP();
        LOG("[BML TEST] All tests completed successfully.");
        return 0;
    } catch (const TestFail& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "[BML TEST] FAILED.\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[UNEXPECTED EXCEPTION] " << e.what() << std::endl;
        std::cerr << "[BML TEST] FAILED.\n";
        return 1;
    }
}
