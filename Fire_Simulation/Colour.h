class Colour {
public:
	Colour() {};
	Colour(unsigned _r, unsigned _g, unsigned _b, unsigned _a):r(_r),g(_g),b(_b),a(_a) {};
	void operator=(const Colour& that) {
		this->r = that.r;
		this->g = that.g;
		this->b = that.b;
		this->a = that.a;
	}
	unsigned char r, g, b, a;
};
const Colour Orange(255, 100, 0, 255);
const Colour Amber(255, 191, 0, 255);