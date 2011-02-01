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
    rbRetVal = rb_funcall( rb_const_get( mModule, rb_intern( "Sequence" ) ),
                           rb_intern( "import" ), 3, rbTypecode, rbDest, rbSize );
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

VALUE Node::wrapRFFT( VALUE rbSelf, VALUE rbForward )
{
  VALUE rbRetVal = Qnil;
  try {
    // ...
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
}

