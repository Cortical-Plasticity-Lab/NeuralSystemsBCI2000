////////////////////////////////////////////////////////////////////////////////
// $Id$
// Author: juergen.mellinger@uni-tuebingen.de
// Description: A number of GraphObjects representing geometric shapes.
//
// $BEGIN_BCI2000_LICENSE$
// 
// This file is part of BCI2000, a platform for real-time bio-signal research.
// [ Copyright (C) 2000-2012: BCI2000 team and many external contributors ]
// 
// BCI2000 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// BCI2000 is distributed in the hope that it will be useful, but
//                         WITHOUT ANY WARRANTY
// - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// $END_BCI2000_LICENSE$
////////////////////////////////////////////////////////////////////////////////
#ifndef SHAPES_H
#define SHAPES_H

#include "GraphObject.h"

#include "Color.h"

class Shape : public GUI::GraphObject
{
 public:
  Shape( GUI::GraphDisplay& display, int kind, int zOrder = ShapeZOrder );
  virtual ~Shape();
  // Properties
  Shape&     SetCenter( const GUI::Point& );
  GUI::Point Center() const;
  Shape&     SetLineWidth( float );
  float      LineWidth() const;
  Shape&     SetFillColor( RGBColor );
  RGBColor   FillColor() const;
  Shape&     SetColor( RGBColor );
  RGBColor   Color() const;

  // For correcting the invalidation rect
  virtual void OnMove( GUI::DrawContext& ioDC );

  // Intersection testing
 public:
  enum TestResult { false_ = 0, true_ = 1, undetermined };
  virtual TestResult Contains( const GUI::Point& ) const
        { return undetermined; };

 protected:
  virtual TestResult IntersectsArea( const Shape& ) const
        { return undetermined; };

 public:
  static bool AreaIntersection( const Shape&, const Shape& );

 protected:
  int Kind() const;
  Shape& SetKind( int );

  const GUI::Rect& Rect() const
    { return mRect; }

  enum { None, Rectangle, Ellipse, Pie, Linear };
  struct ShapeDef
  {
    int kind;
    RGBColor color, fillColor;
    float lineWidth, startAngle, endAngle;

    ShapeDef( int inKind = None )
     : kind( inKind ),
       color( RGBColor::White ),
       fillColor( RGBColor::NullColor ),
       lineWidth( 0 ),
       startAngle( 0 ),
       endAngle( 360 )
     {}
  };
  static void Draw( const GUI::DrawContext&, const ShapeDef& );

  // GraphObject event handlers
  virtual void OnPaint( const GUI::DrawContext& );

 private:
  GUI::Rect mRect;
  ShapeDef mDef;
};

class RectangularShape : public Shape
{
 public:
  RectangularShape( GUI::GraphDisplay& display, int zOrder = ShapeZOrder )
    : Shape( display, Rectangle, zOrder )
    {}
  virtual ~RectangularShape()
    {}
  virtual TestResult Contains( const GUI::Point& ) const;

 protected:
  virtual TestResult IntersectsArea( const Shape& ) const;
};

class EllipticShape : public Shape
{
 public:
  EllipticShape( GUI::GraphDisplay& display, int zOrder = ShapeZOrder )
    : Shape( display, Ellipse, zOrder )
    {}
  virtual ~EllipticShape()
    {}
  virtual TestResult Contains( const GUI::Point& ) const;

 protected:
  virtual TestResult IntersectsArea( const Shape& ) const;
};

class LineShape : public Shape
{
 public:
  LineShape( GUI::GraphDisplay& display, int zOrder = ShapeZOrder )
    : Shape( display, Linear, zOrder ), flipped(false)
    {}
  virtual ~LineShape()
    {}
  virtual TestResult Contains( const GUI::Point& ) const;
  
  // Intercept these methods so that the line can 
  // be draw with similar methods to the other shapes
  GraphObject& SetObjectRect( const GUI::Rect& );
  virtual void OnPaint( const GUI::DrawContext& );

 protected:
  virtual TestResult IntersectsArea( const Shape& ) const;
  bool flipped;
};

#endif // SHAPES_H

