#include "bml.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <limits>
#include <stdexcept>
#include <typeinfo>
#include <cctype>
#include <cmath>
#include <utility>

namespace {

// ---------- logging helpers ----------
#define LOG(msg)  do { std::cout << msg << std::endl; } while(0)
#define SEP()     do { std::cout << "------------------------------------------------------------\n"; } while(0)

struct ScopeTimer {
    const char* label;
    std::chrono::steady_clock::time_point t0;
    explicit ScopeTimer(const char* l) : label(l), t0(std::chrono::steady_clock::now()) {}
    ~ScopeTimer(){
        using namespace std::chrono;
        auto ms = duration_cast<milliseconds>(steady_clock::now() - t0).count();
        std::cout << "[TIME] " << label << ": " << ms << " ms\n";
    }
};

inline std::string pretty_bytes(unsigned long long b) {
    const char* units[] = {"B","KiB","MiB","GiB","TiB"};
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

inline void expect_true(bool v, const char* what) {
    if (!v) throw TestFail(std::string("[FAIL] expected true: ") + what);
}
inline void expect_false(bool v, const char* what) {
    if (v) throw TestFail(std::string("[FAIL] expected false: ") + what);
}

template<class T> struct is_floatish : std::is_floating_point<T> {};
template<class T> struct is_real_integral : bml_is_math_integral<T> {};
template<class T> struct is_math : bml_is_math_arithmetic<T> {};
template<class T> struct is_boolish : bml_is_bool<T> {};

// ---------- small helpers ----------
template<typename T>
void print_type_header(const char* section) {
    SEP();
    std::cout << "[SECTION] " << section << " — type: " << type_name<T>() << "\n";
}

template<typename T>
void fill_sequence(Matrix<T>& m) {
    for (std::uint32_t r=0;r<m.numRows();++r)
        for (std::uint32_t c=0;c<m.numCols();++c)
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
    expect_eq(col1[0], static_cast<T>(1), "col1[0]");
    expect_eq(col1[1], static_cast<T>(11), "col1[1]");

    auto diag = m.getDiagonal();
    LOG("getDiagonal(): count=" << diag.size());
    expect_eq(diag[0], static_cast<T>(0), "diag[0]");
    expect_eq(diag[1], static_cast<T>(11), "diag[1]");

    auto adiag = m.getAntiDiagonal();
    LOG("getAntiDiagonal(): count=" << adiag.size());
    expect_eq(adiag[0], static_cast<T>(2), "adiag[0]");
    expect_eq(adiag[1], m[1][1], "adiag[1]");

    Matrix<T> sub = m.copy(0,1,2,3);
    LOG("copy(0,1,2,3): " << sub.numRows() << "x" << sub.numCols());
    expect_eq(sub[1][0], static_cast<T>(11), "copy content");

    Matrix<T> dst(2,3); dst.fill(static_cast<T>(-1));
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
    m[0][0]=static_cast<T>(1); m[0][1]=static_cast<T>(2);
    m[1][0]=static_cast<T>(3); m[1][1]=static_cast<T>(4);

    LOG("Serialising to bytestream…");
    auto bs = m.toByteStream();
    LOG("Byte size: " << bs.size() << " (" << pretty_bytes(bs.size()) << ")");

    Matrix<T> r(2,2);
    r.initFromByteStream(bs);
    expect_true(m == r, "POD bytestream roundtrip");
    LOG("[OK] ByteStream (POD)");
}

template<typename T>
void test_arithmetic_verbose() {
    if constexpr (!is_math<T>::value) return;
    print_type_header<T>("Arithmetic (element-wise + scalar)");
    Matrix<T> a(2,2), b(2,2);
    a[0][0]=1; a[0][1]=2; a[1][0]=3; a[1][1]=4;
    b[0][0]=5; b[0][1]=6; b[1][0]=7; b[1][1]=8;

    auto c = a + b; LOG("a+b -> c[1][1]=" << format_value(c[1][1])); expect_eq(c[1][1], static_cast<T>(12), "a+b");
    c = a - b; LOG("a-b -> c[0][0]=" << format_value(c[0][0])); expect_eq(c[0][0], static_cast<T>(-4), "a-b");
    c = a * b; LOG("a*b -> c[0][1]=" << format_value(c[0][1])); expect_eq(c[0][1], static_cast<T>(12), "a*b");

    c = a + static_cast<T>(10); LOG("a+10 -> c[1][1]=" << format_value(c[1][1])); expect_eq(c[1][1], static_cast<T>(14), "a+10");
    c = a * static_cast<T>(2);  LOG("a*2  -> c[0][1]=" << format_value(c[0][1])); expect_eq(c[0][1], static_cast<T>(4), "a*2");

    auto s = a.template sum<T>();
    if constexpr (is_floatish<T>::value) {
        LOG("sum(float) -> " << format_value(s));
        expect_true(std::fabs(static_cast<double>(s - static_cast<T>(10))) < 1e-9, "sum float");
    } else {
        LOG("sum(int) -> " << format_value(s));
        expect_eq(s, static_cast<T>(10), "sum int");
    }
    LOG("min=" << format_value(a.template min<T>()) << " max=" << format_value(a.template max<T>()));
    auto p = a.template argmin<T>(); auto q = a.template argmax<T>();
    LOG("argmin=(" << p.first << "," << p.second << ") argmax=(" << q.first << "," << q.second << ")");

    Matrix<T> d = a;
    d += b; expect_eq(d[0][0], static_cast<T>(6), "+=");
    d -= b; expect_eq(d[0][0], static_cast<T>(1), "-=");
    d *= b; expect_eq(d[0][1], static_cast<T>(12), "*=");
    d /= a; expect_eq(d[0][1], static_cast<T>(6), "/=");
    LOG("[OK] Arithmetic");
}

template<typename T>
void test_integral_ops_verbose() {
    if constexpr (!is_real_integral<T>::value) return;
    print_type_header<T>("Integral-only ops (&,|,^,%,~,<<,>>)");
    Matrix<T> a(2,2), b(2,2);
    a[0][0]=1; a[0][1]=2; a[1][0]=3; a[1][1]=4;
    b[0][0]=1; b[0][1]=3; b[1][0]=2; b[1][1]=5;

    auto m = a % b; LOG("a%b -> m[1][0]=" << format_value(m[1][0]));
    auto bw = a & b; LOG("a&b -> bw[0][1]=" << format_value(bw[0][1]));
    bw = a | b;     LOG("a|b -> bw[1][1]=" << format_value(bw[1][1]));
    bw = a ^ b;     LOG("a^b -> bw[1][0]=" << format_value(bw[1][0]));

    auto notA = ~a; LOG("~a -> notA[0][0]=" << format_value(notA[0][0]));

    auto shl = a << 1; LOG("a<<1 -> shl[0][1]=" << format_value(shl[0][1]));
    auto shr = a >> 1; LOG("a>>1 -> shr[1][1]=" << format_value(shr[1][1]));

    Matrix<T> c = a; c &= b; c |= b; c ^= b; c <<= 1; c >>= 1;
    LOG("[OK] Integral-only ops");
}

// ====== Thorough bool tests ======
static void test_bool_thorough()
{
    // Compile-time expectations about traits
    static_assert(bml_is_bool<bool>::value, "bool must satisfy bml_is_bool");
    static_assert(!bml_is_math_arithmetic<bool>::value, "bool must NOT be math-arithmetic");
    static_assert(!bml_is_math_integral<bool>::value,   "bool must NOT be math-integral");

    print_type_header<bool>("bool exhaustive");

    // Shapes & emptiness
    {
        Matrix<bool> e00(0,0);
        expect_true(e00.empty(), "0x0 empty");
        expect_eq(e00.size(), static_cast<std::size_t>(0), "0x0 size");
        expect_true(e00.none<bool>(), "0x0 none()==true");
        expect_false(e00.any<bool>(), "0x0 any()==false");
    }
    {
        Matrix<bool> e05(0,5), e50(5,0);
        expect_true(e05.empty(), "0x5 empty");
        expect_true(e50.empty(), "5x0 empty");
    }

    // Fill patterns & reductions
    Matrix<bool> m(4,5); // 20 elems
    m.fill(false);
    expect_eq(m.count_true<bool>(), static_cast<std::size_t>(0), "all false count_true");
    expect_true(m.none<bool>(), "all false none");
    expect_false(m.any<bool>(), "all false any");
    // min/max on bool
    expect_eq(m.template min<bool>(), false, "min false");
    expect_eq(m.template max<bool>(), false, "max false");

    // Checkerboard pattern
    for (std::uint32_t r=0;r<m.numRows();++r)
        for (std::uint32_t c=0;c<m.numCols();++c)
            m[r][c] = ((r ^ c) & 1) != 0;

    // Count = roughly half (for 4x5 -> 10)
    expect_eq(m.count_true<bool>(), static_cast<std::size_t>(10), "checkerboard count_true");
    expect_true(m.any<bool>(), "checkerboard any");
    expect_false(m.none<bool>(), "checkerboard none");
    expect_eq(m.template min<bool>(), false, "checkerboard min");
    expect_eq(m.template max<bool>(), true,  "checkerboard max");

    // Slices
    auto row1 = m.getRow(1);
    std::size_t row1_ct = 0; for (bool v : row1) if (v) ++row1_ct;
    expect_eq(row1.size(), static_cast<std::size_t>(5), "row1 len");
    expect_eq(row1_ct, static_cast<std::size_t>((m.numCols()+1)/2), "row1 true count");

    auto col2 = m.getColumn(2);
    std::size_t col2_ct = 0; for (bool v: col2) if (v) ++col2_ct;
    expect_eq(col2.size(), static_cast<std::size_t>(4), "col2 len");

    // Diagonals (min(R,C)=4)
    auto d = m.getDiagonal();
    auto ad = m.getAntiDiagonal();
    expect_eq(d.size(),  static_cast<std::size_t>(4), "diag len");
    expect_eq(ad.size(), static_cast<std::size_t>(4), "adiag len");

    // where() on bool -> still bool matrix
    auto wmask = m.where([](bool v){ return v; }, true, false);
    expect_true(wmask.any<bool>(), "where any");
    expect_eq(wmask.count_true<bool>(), m.count_true<bool>(), "where preserves truth pattern");

    // Logical ops against scalars and another mask
    Matrix<bool> alltrue(4,5); alltrue.fill(true);
    Matrix<bool> allfalse(4,5); allfalse.fill(false);

    auto a = m.logical_and<bool>(alltrue); // should equal m
    auto o = m.logical_or<bool>(false);    // should equal m
    auto x = m.logical_xor<bool>(true);    // inverted

    expect_true(a == m, "and with alltrue equals m");
    expect_true(o == m, "or with false equals m");
    expect_eq(x.count_true<bool>(), m.size() - m.count_true<bool>(), "xor invert counts");

    // Logical ops with scalar overloads
    auto a2 = m.logical_and<bool>(true);   // m
    auto o2 = m.logical_or<bool>(false);   // m
    auto x2 = m.logical_xor<bool>(false);  // m
    expect_true(a2 == m && o2 == m && x2 == m, "scalar logical ops keep equality");

    // Copy / paste
    Matrix<bool> sub = m.copy(1,1,3,4); // (2x3)
    Matrix<bool> z(4,5); z.fill(false);
    z.paste(sub, 0, 2);
    for (std::uint32_t r=0; r<2; ++r)
        for (std::uint32_t c=0; c<3; ++c)
            expect_eq(z[r][c+2], sub[r][c], "paste preserves");

    // Iterators (row/column)
    std::size_t cnt = 0;
    for (auto it = m.begin(TraversalType::Row); it != m.end(TraversalType::Row); ++it) ++cnt;
    expect_eq(cnt, m.size(), "row iterator count");
    cnt = 0;
    for (auto it = m.begin(TraversalType::Column); it != m.end(TraversalType::Column); ++it) ++cnt;
    expect_eq(cnt, m.size(), "column iterator count");

    // Bytestream round-trip (POD path uses uint8_t storage)
    {
        LOG("bool: bytestream roundtrip");
        auto bs = m.toByteStream();
        expect_eq(bs.size(), m.size() * sizeof(storage_of_t<bool>), "bool bytestream size");
        Matrix<bool> r(m.numRows(), m.numCols());
        r.initFromByteStream(bs);
        expect_true(m == r, "bool bytestream roundtrip eq");
    }

    // Negative test: malformed size should throw
    {
        LOG("bool: malformed bytestream size should throw");
        auto bs = m.toByteStream();
        if (!bs.empty()) bs.pop_back(); // truncate by 1 byte
        Matrix<bool> r(m.numRows(), m.numCols());
        bool threw = false;
        try { r.initFromByteStream(bs); } catch (...) { threw = true; }
        expect_true(threw, "initFromByteStream must throw on wrong size");
    }

    LOG("[OK] bool exhaustive");
}

// ---- Deep index & iterator tests (includes mutation through iterators) ----
template<typename T>
void test_index_and_iterators_deep()
{
    print_type_header<T>("Indexing & Iterators (deep)");

    // 3x4 to exercise rectangular shapes
    Matrix<T> m(3,4);
    // Write via [][] and verify
    for (std::uint32_t r=0;r<m.numRows();++r)
        for (std::uint32_t c=0;c<m.numCols();++c)
            m[r][c] = static_cast<T>(r*100 + c);

    // Read back spot checks
    expect_eq(m[0][0], static_cast<T>(0),   "[][] origin");
    expect_eq(m[2][3], static_cast<T>(203), "[][] last");
    expect_eq(m[1][2], static_cast<T>(102), "[][] mid");

    // Mutate via non-const iterator (Row traversal): add 1 to every element
    {
        auto it = m.begin(TraversalType::Row);
        auto ed = m.end(TraversalType::Row);
        for (; it != ed; ++it) {
            auto [r,c,ref] = *it; (void)r; (void)c;
            ref = static_cast<T>(ref + static_cast<T>(1));
        }
    }
    expect_eq(m[0][0], static_cast<T>(1),   "iterator write origin+1");
    expect_eq(m[2][3], static_cast<T>(204), "iterator write last+1");

    // Const iteration (Column traversal): count all items
    {
        const Matrix<T>& cm = m;
        std::size_t count = 0;
        auto it = cm.begin(TraversalType::Column);
        auto ed = cm.end(TraversalType::Column);
        for (; it != ed; ++it) {
            auto [r,c,val] = *it; (void)r; (void)c; (void)val;
            ++count;
        }
        expect_eq(count, cm.size(), "const column iteration count");
    }

    // Diagonal traversal iterator: expect sequence m[i][i]
    {
        const Matrix<T>& cm = m;
        auto it = cm.begin(TraversalType::Diagonal);
        auto ed = cm.end(TraversalType::Diagonal);
        std::uint32_t i = 0, limit = std::min(cm.numRows(), cm.numCols());
        for (; it != ed; ++it, ++i) {
            auto [r,c,val] = *it;
            expect_eq(r, i, "diag r==i");
            expect_eq(c, i, "diag c==i");
            expect_eq(val, cm[i][i], "diag val");
        }
        expect_eq(i, limit, "diag length");
    }

    // AntiDiagonal traversal iterator on rectangular 3x4:
    // indices (0,3) -> (1,2) -> (2,1); length=min(R,C)=3
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
static void test_string_verbose()
{
    print_type_header<std::string>("std::string specialisation");

    Matrix<std::string> m(2,2);
    m[0][0] = "hello";
    m[0][1] = "";
    m[1][0] = "ÅÄÖ";
    m[1][1] = "end";

    LOG("Serialising string matrix to bytestream…");
    auto bs = m.toByteStream();
    LOG("Byte size: " << bs.size() << " (" << pretty_bytes(bs.size()) << ")");

    Matrix<std::string> r(2,2);
    r.initFromByteStream(bs);
    expect_true(m == r, "string roundtrip");

    // all()
    auto all_ok = m.all([](const std::string& s){ return s.size() >= 0; });
    expect_true(all_ok, "string all() non-negative lengths");

    // where(): empty -> "X", else -> "-"
    auto ww = m.where(
        [](const std::string& s){ return s.empty(); },
        std::string("X"),
        std::string("-")
    );

    LOG("where(empty->X,else -): [0,0]="
        << format_value(ww[0][0]) << " [0,1]=" << format_value(ww[0][1]));

    // Basic content checks
    expect_eq(ww[0][0], std::string("-"),  "where on 'hello'");
    expect_eq(ww[0][1], std::string("X"),  "where on empty");
    expect_eq(ww[1][0], std::string("-"),  "where on 'ÅÄÖ'");
    expect_eq(ww[1][1], std::string("-"),  "where on 'end'");

    // copy/paste sanity for strings
    Matrix<std::string> sub = m.copy(0,0,2,1); // 2x1: {"hello","ÅÄÖ"}
    Matrix<std::string> z(2,2); z.fill(std::string(".."));
    z.paste(sub, 0, 1);
    expect_eq(z[0][1], std::string("hello"), "string paste 0,1");
    expect_eq(z[1][1], std::string("ÅÄÖ"),   "string paste 1,1");

    LOG("[OK] strings");
}

// ---------- stress tests ----------
template<typename T>
void stress_numeric(const char* label, std::uint32_t R, std::uint32_t C, int repeats = 2)
{
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
            for (std::uint32_t r=0;r<R;++r)
                for (std::uint32_t c=0;c<C;++c)
                    m[r][c] = static_cast<T>(dist(rng));
        } else if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (std::uint32_t r=0;r<R;++r)
                for (std::uint32_t c=0;c<C;++c)
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
            Matrix<T> k(R,C); k.fill(static_cast<T>(1));
            for (int i=0;i<repeats;++i) {
                auto x = m + k;
                auto y = x - k;
                auto z = y * k;
                auto w = z / k;
                (void)w;
            }
        }
        if constexpr (is_real_integral<T>::value) {
            ScopeTimer t3("stress bitwise &|^~ << >>");
            Matrix<T> k(R,C); k.fill(static_cast<T>(0x0F));
            auto a = m & k;
            a |= k; a ^= k;
            a = ~a; a = (a << 1); a = (a >> 1);
            (void)a;
        }

        if constexpr (!std::is_same_v<T, std::string>) {
            ScopeTimer t4("stress bytestream roundtrip (POD)");
            auto bs = m.toByteStream();
            Matrix<T> r(R,C);
            r.initFromByteStream(bs);
            expect_true(m == r, "stress roundtrip POD");
        }

        LOG("[OK] stress " << label);
    }
    catch (const std::bad_alloc&) {
        LOG("[SKIP] stress " << label << " — std::bad_alloc (too large for this machine)");
    }
    catch (const std::exception& e) {
        LOG(std::string("[FAIL] stress ") + label + ": " + e.what());
        throw;
    }
}

static void stress_bool(std::uint32_t R, std::uint32_t C)
{
    print_type_header<bool>("stress bool");
    try {
        Matrix<bool> m(R,C);
        // Deterministic pattern + toggles
        for (std::uint32_t r=0;r<R;++r)
            for (std::uint32_t c=0;c<C;++c)
                m[r][c] = ((r*1315423911u + c*2654435761u) & 1u) != 0;

        {
            ScopeTimer t("bool logical ops & reductions");
            Matrix<bool> n(R,C); n.fill(true);
            auto a = m.logical_and<bool>(n);
            auto o = m.logical_or<bool>(false);
            auto x = m.logical_xor<bool>(true);
            auto notm = m.logical_not<bool>();
            (void)a; (void)o; (void)x; (void)notm;
            (void)m.count_true<bool>();
            (void)m.any<bool>();
            (void)m.none<bool>();
        }
        {
            ScopeTimer t("bool bytestream roundtrip");
            auto bs = m.toByteStream();
            Matrix<bool> r(R,C);
            r.initFromByteStream(bs);
            if (!(m == r)) throw TestFail("[FAIL] bool stress roundtrip mismatch");
        }
        LOG("[OK] stress bool");
    } catch (const std::bad_alloc&) {
        LOG("[SKIP] stress bool — std::bad_alloc");
    }
}

void stress_strings(std::uint32_t R, std::uint32_t C)
{
    print_type_header<std::string>("stress strings");
    try {
        Matrix<std::string> m(R,C);
        for (std::uint32_t r=0;r<R;++r)
            for (std::uint32_t c=0;c<C;++c)
                m[r][c] = (r%7==0) ? "X" : (r%5==0) ? "" : "abcdefghijklmnopqrstuvwxyz";
        ScopeTimer t("string bytestream roundtrip");
        auto bs = m.toByteStream();
        Matrix<std::string> r2(R,C);
        r2.initFromByteStream(bs);
        expect_true(m == r2, "string stress roundtrip");
        LOG("[OK] stress strings");
    } catch (const std::bad_alloc&) {
        LOG("[SKIP] stress strings — std::bad_alloc");
    }
}

} // namespace

// ============ PUBLIC ENTRY ============
int testMatrix()
{
    try {
        LOG("[BML TEST] Starting…");
        SEP();

        // --- verbose per-type functional tests ---
        // Core/iter + POD bytestream
        (test_core_shape_iter_verbose<std::int8_t>(),   test_pod_bytestream_verbose<std::int8_t>());
        (test_core_shape_iter_verbose<std::uint8_t>(),  test_pod_bytestream_verbose<std::uint8_t>());
        (test_core_shape_iter_verbose<std::int16_t>(),  test_pod_bytestream_verbose<std::int16_t>());
        (test_core_shape_iter_verbose<std::uint16_t>(), test_pod_bytestream_verbose<std::uint16_t>());
        (test_core_shape_iter_verbose<std::int32_t>(),  test_pod_bytestream_verbose<std::int32_t>());
        (test_core_shape_iter_verbose<std::uint32_t>(), test_pod_bytestream_verbose<std::uint32_t>());
        (test_core_shape_iter_verbose<std::int64_t>(),  test_pod_bytestream_verbose<std::int64_t>());
        (test_core_shape_iter_verbose<std::uint64_t>(), test_pod_bytestream_verbose<std::uint64_t>());
        (test_core_shape_iter_verbose<float>(),         test_pod_bytestream_verbose<float>());
        (test_core_shape_iter_verbose<double>(),        test_pod_bytestream_verbose<double>());
        (test_core_shape_iter_verbose<long double>(),   test_pod_bytestream_verbose<long double>());

        // Thorough bool before others finish
        test_core_shape_iter_verbose<bool>();
        test_pod_bytestream_verbose<bool>();   // ensure bytestream path for bool is tested
        test_bool_thorough();                  // exhaustive bool tests

        // Deep index & iterator tests (include bool)
        test_index_and_iterators_deep<int32_t>();
        test_index_and_iterators_deep<double>();
        test_index_and_iterators_deep<bool>();

        // Also cover char
        test_core_shape_iter_verbose<char>();
        test_pod_bytestream_verbose<char>();

        // Arithmetic families
        test_arithmetic_verbose<std::int8_t>();
        test_arithmetic_verbose<std::uint8_t>();
        test_arithmetic_verbose<std::int16_t>();
        test_arithmetic_verbose<std::uint16_t>();
        test_arithmetic_verbose<std::int32_t>();
        test_arithmetic_verbose<std::uint32_t>();
        test_arithmetic_verbose<std::int64_t>();
        test_arithmetic_verbose<std::uint64_t>();
        test_arithmetic_verbose<float>();
        test_arithmetic_verbose<double>();
        test_arithmetic_verbose<long double>();

        // Integral-only ops
        test_integral_ops_verbose<std::int8_t>();
        test_integral_ops_verbose<std::uint8_t>();
        test_integral_ops_verbose<std::int16_t>();
        test_integral_ops_verbose<std::uint16_t>();
        test_integral_ops_verbose<std::int32_t>();
        test_integral_ops_verbose<std::uint32_t>();
        test_integral_ops_verbose<std::int64_t>();
        test_integral_ops_verbose<std::uint64_t>();

        // Strings
        test_string_verbose();

        SEP();
        LOG("[BML TEST] Functional tests passed. Beginning stress…");
        SEP();

        // --- Stress sizes (adaptive)
        struct Attempt { std::uint32_t r,c; };
        const Attempt big_attempts[] = { {8192,8192}, {6144,6144}, {4096,4096}, {2048,2048} };

        for (auto [r,c] : big_attempts) {
            stress_numeric<std::uint32_t>("stress u32", r,c);
            stress_numeric<float>("stress float", r,c);
            stress_numeric<std::uint64_t>("stress u64", r,c, /*repeats*/1);
            stress_bool(std::min<std::uint32_t>(r, 8192), std::min<std::uint32_t>(c, 8192)); // dedicated bool stress
            stress_strings(std::min<std::uint32_t>(r, 1024), std::min<std::uint32_t>(c, 1024));
            break; // do one tier; remove to cascade
        }

        SEP();
        LOG("[BML TEST] All tests completed successfully.");
        return 1;
    }
    catch (const TestFail& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "[BML TEST] FAILED.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[UNEXPECTED EXCEPTION] " << e.what() << std::endl;
        std::cerr << "[BML TEST] FAILED.\n";
        return 0;
    }
}
