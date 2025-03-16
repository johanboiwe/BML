#include "bml.hpp"
#include <iostream>
#include <typeinfo>

// Function to get a human-readable representation of the data type
template<typename T>
std::string dtypeToString() {
    if (std::is_same<T, char>::value) {
        return "char";
    } else if (std::is_same<T, unsigned char>::value) {
        return "unsigned char";
    } else if (std::is_same<T, signed char>::value) {
        return "signed char";
    } else if (std::is_same<T, wchar_t>::value) {
        return "wchar_t";
    } else if (std::is_same<T, char16_t>::value) {
        return "char16_t";
    } else if (std::is_same<T, char32_t>::value) {
        return "char32_t";
    } else if (std::is_same<T, int>::value) {
        return "int";
    } else if (std::is_same<T, short>::value) {
        return "short";
    } else if (std::is_same<T, long>::value) {
        return "long";
    } else if (std::is_same<T, long long>::value) {
        return "long long";
    } else if (std::is_same<T, unsigned int>::value) {
        return "unsigned int";
    } else if (std::is_same<T, unsigned short>::value) {
        return "unsigned short";
    } else if (std::is_same<T, unsigned long>::value) {
        return "unsigned long";
    } else if (std::is_same<T, unsigned long long>::value) {
        return "unsigned long long";
    } else if (std::is_same<T, float>::value) {
        return "float";
    } else if (std::is_same<T, double>::value) {
        return "double";
    } else if (std::is_same<T, long double>::value) {
        return "long double";
    } else if (std::is_same<T, bool>::value) {
        return "bool";
    }
    return "unknown type";
}

// Helper function for printing a matrix
template<typename T>
void printMatrix(const Matrix<T>& matrix) {
    std::cout << matrix.toString() << std::endl;
}

// Test for integral types
template<typename T>
void testIntegralMatrix() {
    std::cout << "Testing integral matrix of type: " << typeid(T).name() << std::endl;

    Matrix<T> mat1(3, 3);
    Matrix<T> mat2(3, 3);

    mat1.fill(5);
    mat2.fill(3);

    // Test addition, subtraction, multiplication, division, and modulus
    auto addMat = mat1 + mat2;
    auto subMat = mat1 - mat2;
    auto mulMat = mat1 * mat2;
    auto divMat = mat1 / mat2;
    auto modMat = mat1 % mat2;

    std::cout << "Addition result:" << std::endl;
    printMatrix(addMat);

    std::cout << "Subtraction result:" << std::endl;
    printMatrix(subMat);

    std::cout << "Multiplication result:" << std::endl;
    printMatrix(mulMat);

    std::cout << "Division result:" << std::endl;
    printMatrix(divMat);

    std::cout << "Modulus result:" << std::endl;
    printMatrix(modMat);

    // Scalar operations
    auto scalarAdd = mat1 + 2;
    auto scalarSub = mat1 - 1;
    auto scalarMul = mat1 * 4;
    auto scalarDiv = mat1 / 5;

    std::cout << "Scalar addition result:" << std::endl;
    printMatrix(scalarAdd);

    std::cout << "Scalar subtraction result:" << std::endl;
    printMatrix(scalarSub);

    std::cout << "Scalar multiplication result:" << std::endl;
    printMatrix(scalarMul);

    std::cout << "Scalar division result:" << std::endl;
    printMatrix(scalarDiv);
}

// Test for floating-point types
template<typename T>
void testFloatingPointMatrix() {
    std::cout << "Testing floating-point matrix of type: " << typeid(T).name() << std::endl;

    Matrix<T> mat1(3, 3);
    Matrix<T> mat2(3, 3);

    mat1.fill(5.5);
    mat2.fill(3.3);

    // Test addition, subtraction, multiplication, and division
    auto addMat = mat1 + mat2;
    auto subMat = mat1 - mat2;
    auto mulMat = mat1 * mat2;
    auto divMat = mat1 / mat2;

    std::cout << "Addition result:" << std::endl;
    printMatrix(addMat);

    std::cout << "Subtraction result:" << std::endl;
    printMatrix(subMat);

    std::cout << "Multiplication result:" << std::endl;
    printMatrix(mulMat);

    std::cout << "Division result:" << std::endl;
    printMatrix(divMat);

    // Scalar operations
    auto scalarAdd = mat1 + 2.5;
    auto scalarSub = mat1 - 1.5;
    auto scalarMul = mat1 * 4.0;
    auto scalarDiv = mat1 / 5.0;

    std::cout << "Scalar addition result:" << std::endl;
    printMatrix(scalarAdd);

    std::cout << "Scalar subtraction result:" << std::endl;
    printMatrix(scalarSub);

    std::cout << "Scalar multiplication result:" << std::endl;
    printMatrix(scalarMul);

    std::cout << "Scalar division result:" << std::endl;
    printMatrix(scalarDiv);
}

// Test for character types
template<typename T>
void testCharacterMatrix() {
    std::cout << "Testing character matrix of type: " << dtypeToString<T>() << std::endl; // Call with the template parameter T

    Matrix<T> mat1(3, 3);
    Matrix<T> mat2(3, 3);

    mat1.fill('0');
    mat2.fill('1');
    printMatrix(mat1);
    printMatrix(mat2);

    // Test addition (should give ASCII values)
    Matrix<T>  addMat = mat1 + mat2;
    std::cout << "Addition result (ASCII values):" << std::endl;
    printMatrix(addMat);
}

// Test for boolean type
void testBooleanMatrix() {
    std::cout << "Testing boolean matrix." << std::endl;

    Matrix<bool> mat1(3, 3);
    Matrix<bool> mat2(3, 3);

    mat1.fill(true);
    mat2.fill(false);

    // Test scalar operations
    auto orMat = mat1 + true;
    auto andMat = mat1 * false;

    std::cout << "OR with true result:" << std::endl;
    printMatrix(orMat);

    std::cout << "AND with false result:" << std::endl;
    printMatrix(andMat);
}

// Main testing function
void testMatrix(void) {
    // Test all integral types
    testIntegralMatrix<int>();
    testIntegralMatrix<short>();
    testIntegralMatrix<long>();
    testIntegralMatrix<long long>();

    // Test all unsigned integral types
    testIntegralMatrix<unsigned int>();
    testIntegralMatrix<unsigned short>();
    testIntegralMatrix<unsigned long>();
    testIntegralMatrix<unsigned long long>();

    // Test all floating-point types
    testFloatingPointMatrix<float>();
    testFloatingPointMatrix<double>();
    testFloatingPointMatrix<long double>();

    // Test all character types
    testCharacterMatrix<char>();
    testCharacterMatrix<unsigned char>();
    testCharacterMatrix<signed char>();
    testCharacterMatrix<wchar_t>();
    testCharacterMatrix<char16_t>();
    testCharacterMatrix<char32_t>();

    // Test boolean type
    testBooleanMatrix();

    Matrix<int> toCopy(3,3);
    toCopy.fill(12345);
    std::cout<<"Matrix to be copied: \n" << toCopy.toString();
    Matrix<int> copied(toCopy);
    std::cout<<"Matrix that has been copied \n" << copied.toString();

     // ============================
    //  NEW: Test the Iterators
    // ============================
    std::cout << "\n--- Testing Iterators (non-const and const) ---\n";

    // 1) Non-const Iterator (row-wise)
    {
        Matrix<int> m(3,3);
        m.fill(0);

        // We'll fill the matrix with consecutive values using row-major iteration
        int value = 1;
        for (auto it = m.begin(TraversalType::Row); it != m.end(TraversalType::Row); ++it)
        {
            auto [r, c, currentVal] = *it;
            // We can modify the underlying matrix because it's non-const
            m[r][c] = value++;
        }

        std::cout << "Matrix filled row-wise using non-const iterator:\n";
        std::cout << m.toString() << std::endl;
    }

    // 2) Const Iterator (column-wise)
    {
        Matrix<int> m(3,3);
        // Fill the matrix with something we can read back
        // e.g., row i, col j => value = (i+1)*10 + (j+1) for clarity
        for (unsigned int i = 0; i < m.numRows(); ++i)
        {
            for (unsigned int j = 0; j < m.numCols(); ++j)
            {
                m[i][j] = static_cast<int>((i + 1) * 10 + (j + 1));
            }
        }

        // Now treat 'm' as a const reference
        const Matrix<int>& cm = m;
        std::cout << "Matrix for const iteration:\n" << cm.toString() << std::endl;

        std::cout << "Reading elements column-wise via const iterator (debug):\n";
for (auto cit = cm.begin(TraversalType::Column); cit != cm.end(TraversalType::Column); ++cit)
{
    // Extract row, col, and value
    auto triple = *cit;
    auto r = std::get<0>(triple);
    auto c = std::get<1>(triple);
    // 'valRef' is a const T& if your operator*() returns const T&
    auto &valRef = std::get<2>(triple);

    // Print row, col, the value, and the address
    std::cout << " row=" << r
              << " col=" << c
              << " value=" << valRef
              << " &value=" << static_cast<const void*>(&valRef)
              << "\n";
}
std::cout << std::endl;

        std::cout << std::endl;
    }

    // Comparison tests for Matrix<int>
{
    std::cout << "\n--- Testing Comparison Operators ---\n";

    // Create three 2x2 matrices
    Matrix<int> m1(2, 2);
    Matrix<int> m2(2, 2);
    Matrix<int> m3(2, 2);

    // Fill m1 with:
    // [ 1 2 ]
    // [ 3 4 ]
    m1[0][0] = 1; m1[0][1] = 2;
    m1[1][0] = 3; m1[1][1] = 4;

    // Fill m2 with the same values as m1
    m2[0][0] = 1; m2[0][1] = 2;
    m2[1][0] = 3; m2[1][1] = 4;

    // Fill m3 with a slight difference:
    // [ 1 2 ]
    // [ 3 5 ]
    m3[0][0] = 1; m3[0][1] = 2;
    m3[1][0] = 3; m3[1][1] = 5;

    // Test equality and inequality
    std::cout << "m1 == m2: " << (m1 == m2 ? "true" : "false") << std::endl;
    std::cout << "m1 == m3: " << (m1 == m3 ? "true" : "false") << std::endl;
    std::cout << "m1 != m3: " << (m1 != m3 ? "true" : "false") << std::endl;

    // Test less-than operator
    std::cout << "m1 < m3: " << (m1 < m3 ? "true" : "false") << std::endl;
    std::cout << "m3 < m1: " << (m3 < m1 ? "true" : "false") << std::endl;

    // Test greater-than operator
    std::cout << "m1 > m3: " << (m1 > m3 ? "true" : "false") << std::endl;
    std::cout << "m3 > m1: " << (m3 > m1 ? "true" : "false") << std::endl;

    // Test less-than or equal-to operator
    std::cout << "m1 <= m2: " << (m1 <= m2 ? "true" : "false") << std::endl;
    std::cout << "m1 <= m3: " << (m1 <= m3 ? "true" : "false") << std::endl;

    // Test greater-than or equal-to operator
    std::cout << "m1 >= m2: " << (m1 >= m2 ? "true" : "false") << std::endl;
    std::cout << "m1 >= m3: " << (m1 >= m3 ? "true" : "false") << std::endl;
}


    std::cout << "\nAll tests completed successfully!\n" << std::endl;
}

