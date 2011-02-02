/* HornetsEye - Computer Vision with Ruby
   Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011   Jan Wedekind

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */
#include <iostream>

#include <boost/shared_array.hpp>
#include <fftw3.h>
#include "error.hh"
#include "node.hh"

using namespace boost;
using namespace std;

VALUE Node::cRubyClass = Qnil;

VALUE Node::mModule = Qnil;

VALUE Node::registerRubyClass( VALUE module )
{
  mModule = module;
  cRubyClass = rb_define_class_under( module, "Node", rb_cObject );
  rb_define_method( cRubyClass, "fft", RUBY_METHOD_FUNC( wrapFFT ), 1 );
  rb_define_method( cRubyClass, "rfft", RUBY_METHOD_FUNC( wrapRFFT ), 1 );
  return cRubyClass;
}

VALUE Node::wrapFFT( VALUE rbSelf, VALUE rbForward )
{
  VALUE rbRetVal = Qnil;
  try {
    int sign = rbForward == Qtrue ? FFTW_FORWARD : FFTW_BACKWARD;
    VALUE rbMalloc = rb_funcall( rbSelf, rb_intern( "memory" ), 0 );
    VALUE rbTypecode = rb_funcall( rbSelf, rb_intern( "typecode" ), 0 );
    VALUE rbSize = rb_funcall( rbSelf, rb_intern( "size" ), 0 );
    int size = NUM2INT( rbSize );
    VALUE rbShape = rb_funcall( rbSelf, rb_intern( "shape" ), 0 );
    int rank = RARRAY_LEN( rbShape );
    shared_array< int > n( new int[ rank ] );
    for ( int i=0; i<rank; i++ )
      n[ rank - 1 - i ] = NUM2INT( RARRAY_PTR( rbShape )[i] );
    VALUE rbDest = Qnil;
    if ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                     rb_const_get( mModule, rb_intern( "SCOMPLEX" ) ) ) == Qtrue ) {
      assert( sizeof( fftwf_complex ) == sizeof( float ) * 2 );
      fftwf_complex *source;
      Data_Get_Struct( rbMalloc, fftwf_complex, source );
      fftwf_complex *dest = ALLOC_N( fftwf_complex, size );
      rbDest = Data_Wrap_Struct( rb_const_get( mModule, rb_intern( "Malloc" ) ), 0, xfree,
                                 (void *)dest );
      rb_ivar_set( rbDest, rb_intern( "@size" ), INT2NUM( size * sizeof( fftwf_complex ) ) );
      fftwf_plan plan =
        fftwf_plan_dft( rank, n.get(), source, dest, sign,
                        FFTW_ESTIMATE | FFTW_PRESERVE_INPUT );
      ERRORMACRO( plan != NULL, Error, , "Error creating FFTW plan" );
      fftwf_execute( plan );
      fftwf_destroy_plan( plan );
    } else if ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                            rb_const_get( mModule, rb_intern( "DCOMPLEX" ) ) ) == Qtrue ) {
      assert( sizeof( fftw_complex ) == sizeof( double ) * 2 );
      fftw_complex *source;
      Data_Get_Struct( rbMalloc, fftw_complex, source );
      fftw_complex *dest = ALLOC_N( fftw_complex, size );
      rbDest = Data_Wrap_Struct( rb_const_get( mModule, rb_intern( "Malloc" ) ), 0, xfree,
                                 (void *)dest );
      rb_ivar_set( rbDest, rb_intern( "@size" ), INT2NUM( size * sizeof( fftw_complex ) ) );
      fftw_plan plan =
        fftw_plan_dft( rank, n.get(), source, dest, sign,
                       FFTW_ESTIMATE | FFTW_PRESERVE_INPUT );
      ERRORMACRO( plan != NULL, Error, , "Error creating FFTW plan" );
      fftw_execute( plan );
      fftw_destroy_plan( plan );
    } else {
      ERRORMACRO( false, Error, ,
                  "This datatype is not supported for the selected transform" );
    };
    rbRetVal = rb_funcall2( rb_funcall( rb_const_get( mModule, rb_intern( "Sequence" ) ),
                                        rb_intern( "import" ), 3,
                                        rbTypecode, rbDest, rbSize ),
                            rb_intern( "reshape" ), rank, RARRAY_PTR(rbShape) );
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

VALUE Node::wrapRFFT( VALUE rbSelf, VALUE rbForward )
{
  VALUE rbRetVal = Qnil;
  try {
    int sign = rbForward == Qtrue ? FFTW_FORWARD : FFTW_BACKWARD;
    VALUE rbMalloc = rb_funcall( rbSelf, rb_intern( "memory" ), 0 );
    VALUE rbTypecode = rb_funcall( rbSelf, rb_intern( "typecode" ), 0 );
    VALUE rbInSize = rb_funcall( rbSelf, rb_intern( "size" ), 0 );
    int inSize = NUM2INT( rbInSize );
    VALUE rbShape = rb_funcall( rbSelf, rb_intern( "shape" ), 0 );
    int rank = RARRAY_LEN( rbShape );
    shared_array< int > n( new int[ rank ] );
    for ( int i=0; i<rank; i++ )
      n[ rank - 1 - i ] = NUM2INT( RARRAY_PTR( rbShape )[i] );
    VALUE rbDest = Qnil;
    VALUE rbRetType = Qnil;
    int size = 0;
    if ( rank > 0 ) {
      if ( sign == FFTW_FORWARD ) {
        ERRORMACRO( n[ rank - 1 ] % 2 == 0, Error, ,
                    "First dimension of array must be even for discrete "
                    "fourier transform of real data" );
        int half = n[ rank - 1 ] / 2 + 1;
        size = inSize / n[ rank - 1 ] * half;
        RARRAY_PTR(rbShape)[0] = INT2NUM(half);
      } else {
        int twice = ( n[ rank - 1 ] - 1 ) * 2;
        size = inSize / n[ rank - 1 ] * twice;
        RARRAY_PTR(rbShape)[0] = INT2NUM(twice);
        n[ rank - 1 ] = twice;
      };
    };
    if ( ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                       rb_const_get( mModule,
                                     rb_intern( "SFLOAT" ) ) ) ==
           Qtrue &&
           sign == FFTW_FORWARD ) ||
         ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                       rb_const_get( mModule,
                                     rb_intern( "SCOMPLEX" ) ) ) ==
           Qtrue &&
           sign == FFTW_BACKWARD ) ) {
      assert( sizeof( fftwf_complex ) == sizeof( float ) * 2 );
      fftwf_complex *source;
      Data_Get_Struct( rbMalloc, fftwf_complex, source );
      void *dest;
      fftwf_plan plan;
      shared_array< fftwf_complex > copy;
      int retTypeSize;
      if ( sign == FFTW_FORWARD ) {
        dest = ALLOC_N( fftwf_complex, size );
        retTypeSize = sizeof( fftwf_complex );
        plan = fftwf_plan_dft_r2c( rank, n.get(), (float *)source,
                                   (fftwf_complex *)dest,
                                   FFTW_ESTIMATE | FFTW_PRESERVE_INPUT );
        rbRetType = rb_const_get( mModule, rb_intern( "SCOMPLEX" ) );
      } else {
        dest = ALLOC_N( float, size );
        retTypeSize = sizeof( float );
        // FFTW_PRESERVE_INPUT not supported in this case.
        copy = shared_array< fftwf_complex >( new fftwf_complex[ inSize ] );
        memcpy( copy.get(), source, inSize * sizeof( fftwf_complex ) );
        plan = fftwf_plan_dft_c2r( rank, n.get(), (fftwf_complex *)copy.get(),
                                   (float *)dest, FFTW_ESTIMATE );
        rbRetType = rb_const_get( mModule, rb_intern( "SFLOAT" ) );
      };
      rbDest = Data_Wrap_Struct( rb_const_get( mModule, rb_intern( "Malloc" ) ), 0, xfree,
                                 (void *)dest );
      rb_ivar_set( rbDest, rb_intern( "@size" ), INT2NUM( size * retTypeSize ) );
      ERRORMACRO( plan != NULL, Error, , "Error creating FFTW plan" );
      fftwf_execute( plan );
      fftwf_destroy_plan( plan );
    } else if ( ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                              rb_const_get( mModule,
                                            rb_intern( "DFLOAT" ) ) ) ==
                  Qtrue &&
                  sign == FFTW_FORWARD ) ||
                ( rb_funcall( rbTypecode, rb_intern( "==" ), 1,
                              rb_const_get( mModule,
                                            rb_intern( "DCOMPLEX" ) ) ) ==
                  Qtrue &&
                  sign == FFTW_BACKWARD ) ) {
      assert( sizeof( fftw_complex ) == sizeof( double ) * 2 );
      fftw_complex *source;
      Data_Get_Struct( rbMalloc, fftw_complex, source );
      void *dest;
      fftw_plan plan;
      shared_array< fftw_complex > copy;
      int retTypeSize;
      if ( sign == FFTW_FORWARD ) {
        dest = ALLOC_N( fftw_complex, size );
        retTypeSize = sizeof( fftw_complex );
        plan = fftw_plan_dft_r2c( rank, n.get(), (double *)source,
                                  (fftw_complex *)dest,
                                  FFTW_ESTIMATE | FFTW_PRESERVE_INPUT );
        rbRetType = rb_const_get( mModule, rb_intern( "DCOMPLEX" ) );
      } else {
        dest = ALLOC_N( double, size );
        retTypeSize = sizeof( double );
        // FFTW_PRESERVE_INPUT not supported in this case.
        copy = shared_array< fftw_complex >( new fftw_complex[ inSize ] );
        memcpy( copy.get(), source, inSize * sizeof( fftw_complex ) );
        plan = fftw_plan_dft_c2r( rank, n.get(), (fftw_complex *)copy.get(),
                                  (double *)dest, FFTW_ESTIMATE );
        rbRetType = rb_const_get( mModule, rb_intern( "DFLOAT" ) );
      };
      rbDest = Data_Wrap_Struct( rb_const_get( mModule, rb_intern( "Malloc" ) ), 0, xfree,
                                 (void *)dest );
      rb_ivar_set( rbDest, rb_intern( "@size" ), INT2NUM( size * retTypeSize ) );
      ERRORMACRO( plan != NULL, Error, , "Error creating FFTW plan" );
      fftw_execute( plan );
      fftw_destroy_plan( plan );
    } else {
      ERRORMACRO( false, Error, ,
                  "This datatype is not supported for the selected transform" );
    };
    rbRetVal = rb_funcall2( rb_funcall( rb_const_get( mModule, rb_intern( "Sequence" ) ),
                                        rb_intern( "import" ), 3,
                                        rbRetType, rbDest, INT2NUM( size ) ),
                            rb_intern( "reshape" ), rank, RARRAY_PTR(rbShape) );
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

