# Dokumentacni retezec jako parametr prikazu volani funkce
def vypisOseknuty(s, n):
  """ Funkce vypise zadany retezec s oseknuty jen na prvnich n znaku, 
      negativni hodnota vypise prazdny retezec a prilis velka hodnota je
      zastropovana delkou vstupniho retezce. """
  oseknuty = ''
  i = 0
  delka = len(s)
  if n < delka:
    if n < 0:
      n = 0
    else:
      pass
  else:
    n = delka
 while i < n:
    oseknuty = oseknuty + s[i]
    i = i + 1
  print(oseknuty)

# Hlavni telo programu
vypisOseknuty("""a = inputi()
if a < 0.0:  # pred porovnanim probehne implicitni konverze int na float
  print('Faktorial nelze spocitat')
else:
  vysl = factorial(a)
  print('Vysledek je:', vysl)""", 100)
