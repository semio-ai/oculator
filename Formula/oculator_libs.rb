require "formula"

class Vin < Formula
  desc ""
  homepage ""
  # url "https://example.com/foo-0.1.tar.gz"
  # sha256 "85cc828a96735bdafcf29eb6291ca91bac846579bcef7308536e0c875d6c81d7"
  license "MIT"

  # depends_on "cmake" => :build

  def install
    # ENV.deparallelize
    # system "./configure", *std_configure_args, "--disable-silent-rules"
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    # system "make", "install"
  end

  test do
    system "false"
  end
end
