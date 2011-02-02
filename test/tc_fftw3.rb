require 'test/unit'
begin
  require 'rubygems'
rescue LoadError
end
Kernel::require 'hornetseye_fftw3'

class TC_FFTW3 < Test::Unit::TestCase

  SC = Hornetseye::SCOMPLEX
  DC = Hornetseye::DCOMPLEX

  F = Hornetseye::SFLOAT
  D = Hornetseye::DFLOAT

  def S( *args )
    Hornetseye::Sequence *args
  end

  def M( *args )
    Hornetseye::MultiArray *args
  end

  def X( *args )
    Complex *args
  end

  I = Complex::I

  def setup
  end

  def teardown
  end

  def test_fft
    [ SC, DC ].each do |t|
      s = S( t, 3 )[ 1, I, 3 ]
      u = s.fft.ifft
      s.to_a.zip( u.to_a ).each do |a,b|
        assert_in_delta a.real, b.real, 1.0e-5
        assert_in_delta a.imag, b.imag, 1.0e-5
      end
    end
  end

  def test_rfft
    [ F, D ].each do |t|
      s = S( t, 4 )[ 2, 3, 5, 7 ]
      u = s.rfft.irfft
      s.to_a.zip( u.to_a ).each do |a,b|
        assert_in_delta a, b, 1.0e-5
      end
    end
  end

 end

