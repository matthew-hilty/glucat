#ifndef _GLUCAT_MATRIX_MULTI_IMP_H
#define _GLUCAT_MATRIX_MULTI_IMP_H
/***************************************************************************
    GluCat : Generic library of universal Clifford algebra templates
    matrix_multi_imp.h : Implement the matrix representation of a multivector
                             -------------------
    begin                : Sun 2001-12-09
    copyright            : (C) 2001 by Paul C. Leopardi
    email                : leopardi@bigpond.net.au
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *   See http://www.fsf.org/copyleft/lesser.html for details               *
 ***************************************************************************
 This library is based on a prototype written by Arvind Raja and was
 licensed under the LGPL with permission of the author. See Arvind Raja,
 "Object-oriented implementations of Clifford algebras in C++: a prototype",
 in Ablamowicz, Lounesto and Parra (eds.)
 "Clifford algebras with numeric and symbolic computations", Birkhauser, 1996.
 ***************************************************************************
     See also Arvind Raja's original header comments in glucat.h
 ***************************************************************************/

namespace glucat
{
  // References for algorithms:
  // [M]: Scott Meyers, "Effective C++" Second Edition, Addison-Wesley, 1998.
  // [P]: Ian R. Porteous, "Clifford algebras and the classical groups", Cambridge UP, 1995.
  // [L]: Pertti Lounesto, "Clifford algebras and spinors", Cambridge UP, 1997.
  
  /// Class name used in messages
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const std::string
  matrix_multi<Scalar_T,LO,HI>::
  classname()
  { return "matrix_multi"; }

  /// Determine the log2 dim corresponding to signature p, q
  // Reference: [P] Table 15.27, p 133
  inline
  index_t
  offset_level(const index_t p, const index_t q)
  {
    // Offsets between the log2 of the matrix dimension for the current signature
    // and that of the real superalgebra
    static const int offset_log2_dim[] = {0, 1, 0, 1, 1, 2, 1, 1};
    const int bott = pos_mod(p-q, 8);
    return (p+q)/2 + offset_log2_dim[bott];
  }

  /// Determine the matrix dimension of the fold of a subalegbra
  // Reference: [P] Table 15.27, p 133
  template< typename Matrix_Index_T, const index_t LO, const index_t HI >
  const Matrix_Index_T
  folded_dim( const index_set<LO,HI>& sub )
  { return 1 << offset_level(sub.count_pos(), sub.count_neg()); }

  /// Default constructor
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi()
  : m_frame( index_set_t() ),
    m_matrix( matrix_t( 1, 1 ) )
  { }

  /// Construct a multivector, within a given frame, from a given multivector
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const multivector_t& val, const index_set_t& frm, const bool prechecked)
  : m_frame( frm )
  {
    if (!prechecked && (val.frame() | frm) != frm)
      throw error_t("multivector_t(val,frm): cannot initialize with value outside of frame");
    *this = multivector_t(framed_multi_t(val), frm, true);
  }

  /// Construct a multivector from an index set and a scalar coordinate
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const index_set_t& ist, const Scalar_T& crd)
  : m_frame( ist )
  {
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);
    *this += pair_t(ist, crd);
  }

  /// Construct a multivector, within a given frame, from an index set and a scalar coordinate
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const index_set_t& ist, const Scalar_T& crd, const index_set_t& frm, const bool prechecked)
  : m_frame( frm )
  {
    if (!prechecked && (ist | frm) != frm)
      throw error_t("multivector_t(ist,crd,frm): cannot initialize with value outside of frame");
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);
    *this += pair_t(ist, crd);
  }

  /// Construct a multivector from a scalar (within a frame, if given)
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const Scalar_T& scr, const index_set_t& frm)
  : m_frame( frm )
  {
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);
    *this += pair_t(index_set_t(), scr);
  }

  /// Construct a multivector from an int (within a frame, if given)
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const int scr, const index_set_t& frm)
  { *this = multivector_t(Scalar_T(scr), frm); }

  /// Construct a multivector, within a given frame, from a given vector
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const vector_t& vec,
               const index_set_t& frm, const bool prechecked)
  : m_frame( frm )
  {
    if (!prechecked && index_t(vec.size()) != frm.count())
      throw error_t("multivector_t(vec,frm): cannot initialize with vector not matching frame");
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);
    typename vector_t::const_iterator scvec = vec.begin();
    const index_t begin_index = m_frame.min();
    const index_t end_index = m_frame.max()+1;

    for (index_t idx = begin_index; idx != end_index; ++idx)
      if (m_frame[idx])
      {
        *this += pair_t(index_set_t(idx), *scvec);
        ++scvec;
      }
  }

  /// Construct a multivector from a string: eg: "3+2{1,2}-6.1e-2{2,3}"
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const std::string& str)
  { *this = framed_multi_t(str); }

  /// Construct a multivector, within a given frame, from a string: eg: "3+2{1,2}-6.1e-2{2,3}"
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const std::string& str, const index_set_t& frm, const bool prechecked)
  { *this = multivector_t(framed_multi_t(str), frm, prechecked); }

  /// Construct a multivector from a framed_multi_t
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const framed_multi_t& val)
  : m_frame( val.frame() )
  {
    if (val.size() >= Tune_P::fast_size_threshold)
      try
      {
        *this = val.fast_matrix_multi(m_frame);
        return;
      }
      catch (const glucat_error& e)
      { }
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);

    for(typename framed_multi_t::const_iterator
      scan = val.begin(); scan != val.end(); ++scan)
      *this += *scan;
  }

  /// Construct a multivector, within a given frame, from a framed_multi_t
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const framed_multi_t& val, const index_set_t& frm, const bool prechecked)
  : m_frame( frm )
  {
    if (!prechecked && (val.frame() | frm) != frm)
      throw error_t("multivector_t(val,frm): cannot initialize with value outside of frame");
    if (val.size() >= Tune_P::fast_size_threshold)
      try
      {
        *this = val.fast_matrix_multi(m_frame);
        return;
      }
      catch (const glucat_error& e)
      { }
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);

    for(typename framed_multi_t::const_iterator
      scan = val.begin(); scan != val.end(); ++scan)
      *this += *scan;
  }

  /// Construct a multivector within a given frame from a given matrix
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>::
  matrix_multi(const matrix_t& mtx, const index_set_t& frm)
  : m_frame( frm )
  {
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);
    m_matrix.resize(dim, dim);
    m_matrix = mtx;
  }

  /// Assignment operator
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator= (const matrix_multi_t& rhs)
  {
    // Check for assignment to self
    if (this == &rhs)
      return *this;
    m_frame = rhs.m_frame;
    m_matrix.resize( rhs.m_matrix.size1(), rhs.m_matrix.size2() );
    m_matrix = rhs.m_matrix;
    return *this;
  }

  /// Test for equality of multivectors
  template< typename Scalar_T, const index_t LO, const index_t HI >
  bool
  matrix_multi<Scalar_T,LO,HI>::
  operator==  (const multivector_t& rhs) const
  {
    // Compare only within a common frame
    if (m_frame != rhs.m_frame)
      return framed_multi_t(*this) == framed_multi_t(rhs);
    else
      return ublas::equals(m_matrix, rhs.m_matrix);
  }

  // Test for equality of multivector and scalar
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  bool
  matrix_multi<Scalar_T,LO,HI>::
  operator==  (const Scalar_T& scr) const
  { return (*this) == multivector_t(framed_multi_t(scr), m_frame, true); }

  /// Geometric sum of multivector and scalar
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator+= (const Scalar_T& scr)
  { return *this += pair_t(index_set_t(), scr); }

  /// Geometric sum
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator+= (const multivector_t& rhs)
  {
    // Operate only within a common frame
    const index_set_t our_frame = m_frame | rhs.m_frame;
    if (m_frame != our_frame)
      // Represent *this in our_frame via conversion through framed_multi_t
      *this = multivector_t(framed_multi_t(*this), our_frame, true);
    if (rhs.m_frame == our_frame)
      m_matrix += rhs.m_matrix;
    else
    { // Represent rhs in our_frame via conversion through framed_multi_t
      const multivector_t& our_rhs = multivector_t(framed_multi_t(rhs), our_frame, true);
      m_matrix += our_rhs.m_matrix;
    }
    return *this;
  }

  /// Geometric difference
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator-= (const multivector_t& rhs)
  {
    // Operate only within a common frame
    const index_set_t our_frame = m_frame | rhs.m_frame;
    if (m_frame != our_frame)
      // Represent *this in our_frame via conversion through framed_multi_t
      *this = multivector_t(framed_multi_t(*this), our_frame, true);
    if (rhs.m_frame == our_frame)
      m_matrix -= rhs.m_matrix;
    else
    { // Represent rhs in our_frame via conversion through framed_multi_t
      const multivector_t& our_rhs = multivector_t(framed_multi_t(rhs), our_frame, true);
      m_matrix -= our_rhs.m_matrix;
    }
    return *this;
  }

  /// Unary -
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  operator- () const
  { return multivector_t(-m_matrix, m_frame); }

  /// Product of multivector and scalar
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator*= (const Scalar_T& scr)
  { // multiply coordinates of all terms by scalar
    if (scr == Scalar_T(0))
      *this = 0;
    else
      m_matrix *= scr;
    return *this;
  }

  /// Geometric product
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator*= (const multivector_t& rhs)
  {
    typedef typename multivector_t::matrix_t matrix_t;

    // Operate only within a common frame
    const index_set_t our_frame = m_frame | rhs.m_frame;
    if (m_frame != our_frame)
      // Represent *this in our_frame via conversion through framed_multi_t
      *this = multivector_t(framed_multi_t(*this), our_frame, true);
    if (rhs.m_frame == our_frame)
      m_matrix = matrix::sparse_prod(m_matrix, rhs.m_matrix);
    else
      // Represent rhs in our_frame via conversion through framed_multi_t
      m_matrix = matrix::sparse_prod(m_matrix,
                                         multivector_t(framed_multi_t(rhs), our_frame, true).m_matrix);
    return *this;
  }

  /// Contraction
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator%= (const multivector_t& rhs)
  {
    framed_multi_t lhs(*this);
    return *this = lhs %= framed_multi_t(rhs);
  }

  /// Inner product
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator&= (const multivector_t& rhs)
  { // Arvind Raja's original reference:
    // "old clical, innerproduct(p,q:pterm):pterm in file compmod.pas"
    framed_multi_t lhs(*this);
    return *this = lhs &= framed_multi_t(rhs);
  }

  /// Outer product
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator^= (const multivector_t& rhs)
  { // Arvind Raja's original reference:
    // "old clical, outerproduct(p,q:pterm):pterm in file compmod.pas"
    framed_multi_t lhs(*this);
    return *this = lhs ^= framed_multi_t(rhs);
  }

  /// Quotient of multivector and scalar
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator/= (const Scalar_T& scr)
  { return *this *= Scalar_T(1)/scr; }

  // Geometric quotient
  template< typename Scalar_T, const index_t LO, const index_t HI >
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator/= (const multivector_t& rhs)
  {
    if( m_frame != rhs.m_frame )
      return *this = framed_multi_t(*this) /
                     framed_multi_t(rhs);
    else
    { // Solve result == *this/rhs <=> result*rhs == *this
      // We now solve X == B/A
      // (where X == result, B == m_matrix and A == rhs.m_matrix)
      // X == B/A <=> X*A == B <=> AT*XT == BT
      // So, we solve AT*XT == BT

      const matrix_t& AT = ublas::trans(rhs.m_matrix);
      matrix_t LU = AT;
      typedef ublas::permutation_matrix<matrix_index_t> permutation_t;
      permutation_t pvector(AT.size1());
      if (! ublas::lu_factorize(LU, pvector))
      {
        matrix_t XT = ublas::trans(m_matrix);
        ublas::lu_substitute(LU, pvector, XT);
        // Iterative refinement.
        // Reference: Nicholas J. Higham, "Accuracy and Stability of Numerical Algorithms",
        // SIAM, 1996, ISBN 0-89871-355-2, Chapter 11
        if (Tune_P::div_max_steps > 0)
        {
          const matrix_t& BT = ublas::trans(m_matrix);
          matrix_t R = matrix::sparse_prod(AT, XT) - BT;
          Scalar_T nr = ublas::norm_inf(R);
          // (nr == nr) below is to exclude NaN
          if ( nr != Scalar_T(0) && nr == nr )
          {
            matrix_t XTnew = XT;
            Scalar_T nrold = nr + Scalar_T(1);
            for (int step = 0;
                 step != Tune_P::div_max_steps &&
                 nr < nrold &&
                 nr != Scalar_T(0) &&
                 nr == nr;
                  ++step )
            {
              nrold = nr;
              if (step != 0)
                XT = XTnew;
              matrix_t& D = R;
              ublas::lu_substitute(LU, pvector, D);
              XTnew -= D;
              R = matrix::sparse_prod(AT, XTnew) - BT;
              nr = ublas::norm_inf(R);
            }
          }
        }
        m_matrix = ublas::trans(XT);
      }
      else
        // AT is singular. Return IEEE NaN or -Inf
        *this = std::numeric_limits<Scalar_T>::has_quiet_NaN ?
          std::numeric_limits<Scalar_T>::quiet_NaN() : Scalar_T(std::log(0.0));
      return *this;
    }
  }

  /// Clifford multiplicative inverse
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  inv() const
  {
    multivector_t result(1, m_frame);
    return result /= *this;
  }

  /// Subscripting: map from index set to scalar coordinate
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  Scalar_T
  matrix_multi<Scalar_T,LO,HI>::
  operator[] (const index_set_t& ist) const
  { return matrix::inner<Scalar_T>(basis_element<Scalar_T>(ist, m_frame), m_matrix); }

  /// Main involution, each {i} is replaced by -{i} in each term
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  involute() const
  { return framed_multi_t(*this).involute(); }

  /// Reversion, order of {i} is reversed in each term
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  reverse() const
  { return framed_multi_t(*this).reverse(); }

  /// Conjugation, conj == reverse o involute == involute o reverse
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  conj() const
  { return framed_multi_t(*this).conj(); }

  /// Quadratic form := scalar part of rev(x)*x
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  Scalar_T
  matrix_multi<Scalar_T,LO,HI>::
  quad() const
  { // scalar(conj(x)*x) = 2*quad(even(x)) - quad(x)
    // Arvind Raja ref: "old clical: quadfunction(p:pter):pterm in file compmod.pas"
    return framed_multi_t(*this).quad();
  }

  /// Scalar_T norm squared= sum of norm squared of coordinates
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  Scalar_T
  matrix_multi<Scalar_T,LO,HI>::
  norm() const
  { return matrix::inner<Scalar_T>(m_matrix, m_matrix); }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  pow(int m) const
  {
    multivector_t a;
    if (m < 0)
    {
      m = -m;
      a = (*this).inv();
    }
    else
      a = *this;
    multivector_t result = 1;
    for (; m != 0; m >>= 1)
    {
      if (m & 1)
        result *= a;
      a *= a;
    }
    return result;
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  outer_pow(int m) const
  { // outer product power
    if (m < 0)
      throw error_t("outer_pow(m): negative exponent");
    multivector_t result = 1;
    multivector_t a = *this;
    for (; m != 0; m >>= 1)
    {
      if (m & 1)
        result ^= a;
      a ^= a;
    }
    return result;
  }

  /// Grading: part where each term is a grade-vector
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  operator() (index_t grade) const
  {
    if ((grade < 0) || (grade > HI-LO))
      return 0;
    else
      return (framed_multi_t(*this))(grade);
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  even() const
  { // even part of x, sum of the pure(count) with even count
    return framed_multi_t(*this).even();
  }

  /// Vector part of multivector, as a vector_t
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const typename matrix_multi<Scalar_T,LO,HI>::vector_t
  matrix_multi<Scalar_T,LO,HI>::
  vector_part() const
  {
    vector_t result;
    const index_t begin_index = m_frame.min();
    const index_t end_index = m_frame.max()+1;
    for (index_t idx = begin_index; idx != end_index; ++idx)
      if (m_frame[idx])
        // Frame may contain indices which do not correspond to a grade 1 term but
        // frame cannot omit any index corresponding to a grade 1 term
        result.push_back(
          matrix::inner<Scalar_T>(
            basis_element<Scalar_T>(index_set_t(idx), m_frame),
            m_matrix));
    return result;
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  std::ostream&
  operator<< (std::ostream& os, const matrix_multi<Scalar_T,LO,HI>& val)
  {
    os << matrix_multi<Scalar_T,LO,HI>::framed_multi_t(val);
    return os;
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  std::istream&
  operator>> (std::istream& s, matrix_multi<Scalar_T,LO,HI>& val)
  { // Input looks like 1.0-2.0{1,2}+3.2{3,4}
    framed_multi<Scalar_T,LO,HI> local;
    s >> local;
    // If s.bad() then we have a corrupt input
    // otherwise we are fine and can copy the resulting matrix_multi
    if (!s.bad())
      val = local;
    return s;
  }

  /// Write out multivector
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  void
  matrix_multi<Scalar_T,LO,HI>::
  write(const std::string& msg) const
  { framed_multi_t(*this).write(msg); }

  /// Write out multivector to file
  template< typename Scalar_T, const index_t LO, const index_t HI >
  void
  matrix_multi<Scalar_T,LO,HI>::
  write(std::ofstream& ofile, const std::string& msg) const
  {
    if (!ofile)
      throw error_t("write(ofile,msg): cannot write to output file");
    framed_multi_t(*this).write(ofile, msg);
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  Scalar_T
  matrix_multi<Scalar_T,LO,HI>::
  max_abs() const
  { return framed_multi_t(*this).max_abs(); }

  /// Check if a multivector contains any IEEE NaN values
  template< typename Scalar_T, const index_t LO, const index_t HI >
  bool
  matrix_multi<Scalar_T,LO,HI>::
  isnan() const
  { // The distinguishing feature is that NaN != NaN
    for (typename matrix_t::const_iterator1 i = m_matrix.begin1(); i != m_matrix.end1(); ++i)
      for (typename matrix_t::const_iterator2 j = i.begin(); j != i.end(); ++j)
        if (*j != *j)
          return true;
    return false;
  }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  truncated(const Scalar_T& limit) const
  { return framed_multi_t(*this).truncated(limit); }

  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  const index_set<LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  frame() const
  { return m_frame; }

  /// Add a term, if non-zero
  template< typename Scalar_T, const index_t LO, const index_t HI >
  inline
  matrix_multi<Scalar_T,LO,HI>&
  matrix_multi<Scalar_T,LO,HI>::
  operator+= (const pair_t& term)
  {
    if (term.second != Scalar_T(0))
      m_matrix += basis_element<Scalar_T>(term.first, m_frame) * term.second;
    return *this;
  }

  /// Inverse generalized Fast Fourier Transform
  template< typename Multivector_T, typename Matrix_T >
  Multivector_T
  fast(const Matrix_T& M, index_t level)
  {
    typedef Multivector_T framed_multi_t;
    if (level == 0)
      return framed_multi_t(M(0,0));

    typedef Matrix_T matrix_t;
    typedef typename framed_multi_t::index_set_t index_set_t;
    const matrix_t I = matrix::unit<matrix_t>(2);
    matrix_t J(2,2);
    J(0,1) =    -1;
    J(1,0) = 1;
    matrix_t K(2,2);
    K(0,1) =     1;
    K(1,0) = 1;
    matrix_t JK(2,2);
    JK(0,0) = -1;
    JK(1,1) =    1;

    typedef typename framed_multi_t::index_set_t index_set_t;
    typedef typename framed_multi_t::scalar_t scalar_t;
    const framed_multi_t mn = framed_multi_t(index_set_t(-level), scalar_t(1));
    const framed_multi_t pn = framed_multi_t(index_set_t(level), scalar_t(1));
    const bool mono = true;
    if (level == 1)
    {
      const framed_multi_t& I_M  = (matrix::nork(I,  M, mono))(0, 0);
      const framed_multi_t& J_M  = (matrix::nork(J,  M, mono))(0, 0);
      const framed_multi_t& K_M  = (matrix::nork(K,  M, mono))(0, 0);
      const framed_multi_t& JK_M = (matrix::nork(JK, M, mono))(0, 0);
      return    I_M
        + mn * (JK_M * pn + J_M)
        +       K_M * pn;
    }
    const framed_multi_t& I_M  =
      fast<framed_multi_t, matrix_t>(matrix::nork(I,  M, mono), level-1);
    const framed_multi_t& J_M  =
      fast<framed_multi_t, matrix_t>(matrix::nork(J,  M, mono), level-1);
    const framed_multi_t& K_M  =
      fast<framed_multi_t, matrix_t>(matrix::nork(K,  M, mono), level-1);
    const framed_multi_t& JK_M =
      fast<framed_multi_t, matrix_t>(matrix::nork(JK, M, mono), level-1);
    const framed_multi_t& ev_I_M = even(I_M);
    const framed_multi_t& od_I_M = I_M - ev_I_M;
    const framed_multi_t& ev_J_M = even(J_M);
    const framed_multi_t& od_J_M = J_M - ev_J_M;
    const framed_multi_t& ev_K_M = even(K_M);
    const framed_multi_t& od_K_M = K_M - ev_K_M;
    const framed_multi_t& ev_JK_M = even(JK_M);
    const framed_multi_t& od_JK_M = JK_M - ev_JK_M;
    return (ev_I_M  - od_JK_M)
     + mn*((ev_JK_M + od_I_M)*pn
     +     (ev_J_M  + od_K_M))
     +     (ev_K_M  - od_J_M)*pn;
  }

  /// Use generalized FFT to construct a matrix_multi_t
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const matrix_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  fast_matrix_multi(const index_set_t& frm) const
  {
    if (m_frame == frm)
      return *this;
    else
      return (this->fast_framed_multi()).fast_matrix_multi(frm);
  }

  /// Use inverse generalized FFT to construct a framed_multi_t
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const framed_multi<Scalar_T,LO,HI>
  matrix_multi<Scalar_T,LO,HI>::
  fast_framed_multi() const
  {
    // Determine the amount of off-centering needed
    int p = m_frame.count_pos();
    int q = m_frame.count_neg();

    const index_t bott = pos_mod(p-q, 8);
    p += std::max(gen::offset_to_super[bott],0);
    q -= std::min(gen::offset_to_super[bott],0);

    const int orig_p = p;
    const int orig_q = q;
    while (p-q > 4)
      { p -= 4; q += 4; }
    while (p-q < -3)
      { p += 4; q -= 4; }
    if (p-q > 1)
    {
      int old_p = p;
      p = q+1;
      q = old_p-1;
    }
    const index_t level = (p+q)/2;
    framed_multi_t val = fast<framed_multi_t, matrix_t>(m_matrix, level);

    // Off-centre val
    switch (pos_mod(orig_p-orig_q, 8))
    {
    case 2:
    case 3:
    case 4:
      val.centre_qp1_pm1(p, q);
      break;
    default:
      break;
    }
    if (orig_p-orig_q > 4)
      while (p != orig_p)
        val.centre_pp4_qm4(p, q);
    if (orig_p-orig_q < -3)
      while (p != orig_p)
        val.centre_pm4_qp4(p, q);
    return val.unfold(m_frame);
  }

  /// Table of basis elements used as a cache by basis_element()
  template< typename Scalar_T, const index_t LO, const index_t HI >
  class basis_table :
  public std::map< std::pair< const index_set<LO,HI>, const index_set<LO,HI> >,
                   typename matrix_multi<Scalar_T, LO, HI>::matrix_t >
  {
  public:
    /// Single instance of basis table
    static basis_table& basis() { static basis_table b; return b;}
  private:
    // Enforce singleton
    // Reference: A. Alexandrescu, "Modern C++ Design", Chapter 6
    basis_table() {}
    ~basis_table() {}
    basis_table(const basis_table&);
    basis_table& operator= (const basis_table&);

    /// Friend declaration to avoid compiler warning:
    /// "... only defines a private destructor and has no friends"
    /// Ref: Carlos O'Ryan, ACE http://doc.ece.uci.edu
    friend class friend_for_private_destructor;
  };

  /// Create a basis element matrix within a frame
  template< typename Scalar_T, const index_t LO, const index_t HI >
  const typename matrix_multi<Scalar_T,LO,HI>::matrix_t
  basis_element(const index_set<LO,HI>& ist, const index_set<LO,HI>& m_frame)
  {
    typedef matrix_multi<Scalar_T,LO,HI>           multivector_t;
    typedef typename multivector_t::matrix_t       matrix_t;
    typedef typename multivector_t::matrix_index_t matrix_index_t;
    typedef index_set<LO,HI>                       index_set_t;

    const index_set_t folded_set = ist.fold(m_frame);
    const index_set_t folded_frame = m_frame.fold();
    const index_t folded_max = folded_frame.max();
    const index_t folded_min = folded_frame.min();
    const matrix_index_t dim = folded_dim<matrix_index_t>(m_frame);

    const int p = std::max( int(folded_max), 0);
    const int q = std::max(-int(folded_min), 0);

    typedef basis_table<Scalar_T,LO,HI>                     basis_table_t;
    typedef std::pair<const index_set_t, const index_set_t> index_set_pair_t;
    typedef std::pair<const index_set_pair_t, matrix_t>     basis_pair_t;
    typedef typename basis_table_t::const_iterator          basis_const_iterator_t;
    basis_table_t& basis_cache = basis_table_t::basis();
    const index_set_pair_t& folded_pair = index_set_pair_t(folded_set, folded_frame);
    if (p+q <= Tune_P::basis_max_count)
    {
      const basis_const_iterator_t basis_it = basis_cache.find(folded_pair);
      if (basis_it != basis_cache.end())
        return basis_it->second;
    }
    const matrix_t* e = (gen::generator_table<matrix_t>::generator())(p,q);
    matrix_t result = matrix::unit<matrix_t>(dim);
    for (index_t k = folded_min; k <= folded_max; ++k)
      if (folded_set[k])
        result = matrix::mono_prod(result, e[k]);
    if (p+q <= Tune_P::basis_max_count)
      basis_cache.insert(basis_pair_t(folded_pair, result));
    return result;
  }

}
#endif  // _GLUCAT_MATRIX_MULTI_IMP_H
