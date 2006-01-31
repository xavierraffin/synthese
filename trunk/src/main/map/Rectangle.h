#ifndef RECTANGLE_H_
#define RECTANGLE_H_

namespace synmap
{

class XYPoint;


class Rectangle
{
private:
	double _x;
	double _y;
	double _width;
	double _height;
	
public:
	Rectangle(double x, double y, double width, double height);
	Rectangle(const Rectangle& ref);
	
	virtual ~Rectangle();
	
	double getX () const { return _x; }
	double getY () const { return _y; }
	double getWidth () const { return _width; }
	double getHeight () const { return _height; }
			
	bool contains (const XYPoint& p) const;
			
	Rectangle& operator= (const Rectangle& rhs);
	bool operator== (const Rectangle& rhs) const;
	
};

}

#endif /*RECTANGLE_H_*/
