CXXFLAGS=-W -Wall -Wextra -Werror -O2 -std=c++11

otp-image: otp-image.o ppm-reader.o
	$(CXX) $^ -o $@

otp-image.o: bit-cut-template.ps.rawstring

clean:
	rm -f otp-image otp-image.o ppm-reader.o bit-cut-template.ps.rawstring

# Useful target to create DXF files for laser cutting
%-1.dxf : %.ps
	pstoedit -psarg "-r600x600" -nb -f "dxf_s:-mm -ctl" $^ $*-%PAGENUMBER%.dxf

%.rawstring : %
	echo "static constexpr char k$$(echo $^ | sed 's/[\.-]/_/g')[] = R\"(" > $@
	cat $^ >> $@
	echo ')";' >> $@
