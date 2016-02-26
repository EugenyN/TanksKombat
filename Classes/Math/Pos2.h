#ifndef _POS2_H_
#define _POS2_H_

class Pos2
{
public:
    int x;
    int y;

	Pos2()
		: x(0), y(0)
	{ }

	Pos2(int xx, int yy)
		: x(xx), y(yy)
	{ }

	Pos2(const int* array)
	{
		x = array[0];
		y = array[1];
	}

	Pos2(const Pos2& copy)
	{
		this->x = copy.x;
		this->y = copy.y;
	}

	Pos2(const cocos2d::Point& copy)
	{
		this->x = copy.x;
		this->y = copy.y;
	}

    ~Pos2()
	{ }

	inline const Pos2 operator+(const Pos2& p) const
	{
		Pos2 result(*this);
		result.add(p);
		return result;
	}

	inline Pos2& operator+=(const Pos2& p)
	{
		add(p);
		return *this;
	}

	inline const Pos2 operator-(const Pos2& p) const
	{
		Pos2 result(*this);
		result.subtract(p);
		return result;
	}

    inline Pos2& operator-=(const Pos2& p)
	{
		subtract(p);
		return *this;
	}

	inline const Pos2 operator-() const
	{
		Pos2 result(*this);
		result.negate();
		return result;
	}

	inline bool operator<(const Pos2& p) const
	{
		if (x == p.x)
		{
			return y < p.y;
		}
		return x < p.x;
	}

	inline bool operator>(const Pos2& p) const
	{
		if (x == p.x)
		{
			return y > p.y;
		}
		return x > p.x;
	}

	inline bool operator==(const Pos2& p) const
	{
		return x == p.x && y == p.y;
	}

    inline bool operator!=(const Pos2& p) const
	{
		return x != p.x || y != p.y;
	}

	explicit operator cocos2d::Point() const 
	{ 
		return cocos2d::Point(x, y);
	}

	inline void set(int xx, int yy)
	{
		this->x = xx;
		this->y = yy;
	}

	inline void add(const Pos2& p)
	{
		x += p.x;
		y += p.y;
	}

	inline void subtract(const Pos2& p)
	{
		x -= p.x;
		y -= p.y;
	}

	inline void negate()
	{
		x = -x;
		y = -y;
	}

	inline void normalize()
	{
		x = (0 < x) - (x < 0);
		y = (0 < y) - (y < 0);
	}

	bool equals(const Pos2& p) const
	{
		return x == p.x && y == p.y;
	}

	inline std::ostream& operator<<(std::ostream& ostr)
	{
		return ostr << toString();
	}

	std::string toString() const
	{
		return cocos2d::StringUtils::format("[ %d, %d ]", x, y);
	}
};

#endif // _POS2_H_
