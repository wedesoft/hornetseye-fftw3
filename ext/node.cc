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
#include "node.hh"

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
    // ...
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
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

