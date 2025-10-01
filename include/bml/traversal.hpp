//
// Created by johan on 2025-09-25.
//

#ifndef BML_TRAVERSAL_HPP
namespace bml {
    // Fixed underlying type makes forward-decls possible too
    enum class TraversalType : unsigned {
        Row, Column, Diagonal, AntiDiagonal
      };
}
#define BML_TRAVERSAL_HPP

#endif //BML_TRAVERSAL_HPP