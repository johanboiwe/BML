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

    Matrix<int> toCopy(5,5);
    toCopy.fill(5);
    std::cout<<"Matrix to be copied: \n" << toCopy.toString();
    Matrix<int> copied(toCopy);
    std::cout<<"Matrix that has been copied \n" << copied.toString();

    std::cout << "All tests completed successfully!" << std::endl;
}
