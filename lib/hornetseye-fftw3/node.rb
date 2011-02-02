# hornetseye-fftw3 - Fourier transforms
# Copyright (C) 2011 Jan Wedekind
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
module Hornetseye

  class Node

    alias_method :orig_fft, :fft

    def fft( forward = true )
      if forward
        to_type( Hornetseye::COMPLEX( basetype.float ) ).memorise.orig_fft true
      else
        to_type( Hornetseye::COMPLEX( basetype.float ) ).memorise.orig_fft( false ) / size
      end
    end

    def ifft
      fft false
    end

    alias_method :orig_rfft, :rfft

    def rfft( forward = true )
      if forward
        to_type( typecode.float ).memorise.orig_rfft true
      else
        result = to_type( Hornetseye::COMPLEX( basetype.float ) ).memorise.orig_rfft false
        result / result.size
      end
    end

    def irfft
      rfft false
    end

  end

end

