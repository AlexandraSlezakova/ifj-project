PI = 3.14159
print('GEOMETRY CALCULATIONS')
exit = 0

def ProcessShape(shape):
  if (shape == 'square'):
    ProcessSquare()
  else:
    if (shape == 'ractangle'):
      ProcessRectangle()
    else:
      if (shape == 'circle'):
        ProcessCircle()
      else:
        if (shape == 'triangle'):
          ProcessTriangle()
        else:
          return 1
        return 0

def ProcessSquare():
  print('a = ')
  a = inputf()
  o = 4 * a
  s = a * a
  print('o = ', o, '\ns = ', s)

def ProcessRectangle():
  print('a = ')
  a = inputf()
  print('b = ')
  b = inputf()
  o = 2*a + 2*b
  s = a * b
  print('o = ', o, '\ns = ', s)

def ProcessCircle():
  print('r = ')
  r = inputf()

